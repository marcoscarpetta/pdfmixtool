/* Copyright (C) 2017-2018 Marco Scarpetta
 *
 * This file is part of PDF Mix Tool.
 *
 * PDF Mix Tool is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PDF Mix Tool is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PDF Mix Tool. If not, see <http://www.gnu.org/licenses/>.
 */

#include "podofo_pdffile.h"
#include "podofo_pdftranslator.h"

PoDoFoOutputPdfFile::PoDoFoOutputPdfFile() :
    OutputPdfFile()
{
    output_file = new PoDoFo::PdfMemDocument();
}

PoDoFoOutputPdfFile::~PoDoFoOutputPdfFile()
{
    delete output_file;
}

void PoDoFoOutputPdfFile::write(const std::string &filename)
{
    output_file->Write(filename.c_str());
    delete output_file;
    output_file = new PoDoFo::PdfMemDocument();
}

PoDoFoInputPdfFile::PoDoFoInputPdfFile(const std::string &filename) :
    InputPdfFile(filename)
{
    m_podofo_file = new PoDoFoFile;
    m_podofo_file->file = new PoDoFo::PdfMemDocument(filename.c_str());
    m_podofo_file->ref_count = 1;

    int page_rotation = m_podofo_file->file->GetPage(0)->GetRotation();

    PoDoFo::PdfRect rect(m_podofo_file->file->GetPage(0)->GetMediaBox());
    if ((page_rotation == 90) || (page_rotation == 270))
    {
        m_page_width = std::round((rect.GetHeight() - rect.GetBottom()) / mm) / 10;
        m_page_height = std::round((rect.GetWidth() - rect.GetLeft()) / mm) / 10;
    }
    else
    {
        m_page_width = std::round((rect.GetWidth() - rect.GetLeft()) / mm) / 10;
        m_page_height = std::round((rect.GetHeight() - rect.GetBottom()) / mm) / 10;
    }

    for (const PaperSize &size : paper_sizes)
    {
        if (m_page_width == size.width && m_page_height == size.height)
        {
            m_paper_size = size;
            return;
        }
        else if (m_page_height == size.width && m_page_width == size.height)
        {
            m_paper_size = size;
            m_paper_size.width = m_page_width;
            m_paper_size.height = m_page_height;
            m_paper_size.portrait = false;
            return;
        }
    }

    m_paper_size = {m_page_width, m_page_height, "", (m_page_height > m_page_width)};
}

PoDoFoInputPdfFile::PoDoFoInputPdfFile(InputPdfFile *pdf_file) :
    InputPdfFile(pdf_file)
{
    m_podofo_file = static_cast<PoDoFoInputPdfFile *>(pdf_file)->m_podofo_file;
    m_podofo_file->ref_count++;

    m_page_width = pdf_file->page_width();
    m_page_height = pdf_file->page_height();
    m_paper_size = pdf_file->paper_size();
}

PoDoFoInputPdfFile::~PoDoFoInputPdfFile()
{
    if (m_podofo_file != NULL)
    {
        if (m_podofo_file->ref_count == 1)
        {
            delete m_podofo_file->file;
            delete m_podofo_file;
        }
        else
            m_podofo_file->ref_count--;
    }
}

int PoDoFoInputPdfFile::page_count()
{
    return m_podofo_file->file->GetPageCount();
}

void PoDoFoInputPdfFile::run(OutputPdfFile *output_file)
{
    PoDoFo::PdfMemDocument *podofo_file;

    // Add pages to a tmp file
    if (m_multipage.enabled)
        podofo_file = new PoDoFo::PdfMemDocument();
    // Add pages to output PDF file
    else
        podofo_file = static_cast<PoDoFoOutputPdfFile *>(output_file)->output_file;

    int page_index = static_cast<PoDoFoOutputPdfFile *>(output_file)->output_file->GetPageCount();
    int added_pages = 0;

    // Add pages to podofo_file from this document
    if (m_filters.size() == 0)
    {
        podofo_file->InsertPages(*(m_podofo_file->file), 0, m_podofo_file->file->GetPageCount());
        added_pages += m_podofo_file->file->GetPageCount();
    }
    else
    {
        std::list<std::pair<int, int>>::iterator it;
        for (it = m_filters.begin(); it != m_filters.end(); ++it)
        {
            int page_count = it->second - it->first + 1;
            podofo_file->InsertPages(*(m_podofo_file->file), it->first - 1, page_count);
            added_pages += page_count;
        }
    }

    // Multipage
    int initial_rotation = 0;
    int multipage_rotation = 0;

    if (m_multipage.enabled)
    {
        initial_rotation = podofo_file->GetPage(0)->GetRotation();
        multipage_rotation = m_multipage.rotation;

        PdfTranslator *translator = new PdfTranslator();
        translator->setSource(podofo_file);
        PoDoFo::PdfMemDocument *tmp = translator->impose(m_multipage);
        PoDoFo::PdfMemDocument *old = podofo_file;

        // Add multipage pages to output file
        podofo_file = static_cast<PoDoFoOutputPdfFile *>(output_file)->output_file;
        added_pages = this->output_page_count();
        podofo_file->InsertPages(*tmp, 0, added_pages);

        delete tmp;
        delete old;
        delete translator;
    }

    // Set pages rotation
    if (m_rotation != 0 || initial_rotation != 0 || multipage_rotation != 0)
    {
        for (int i = 0; i < added_pages; i++)
        {
            PoDoFo::PdfPage *page = podofo_file->GetPage(page_index + i);
            int rotation = (
                        page->GetRotation() + initial_rotation + multipage_rotation + m_rotation
                        ) % 360;
            page->SetRotation(rotation);
        }
    }
}
