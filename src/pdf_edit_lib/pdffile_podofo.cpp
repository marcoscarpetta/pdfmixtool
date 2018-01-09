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

#include "pdffile_podofo.h"

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
}

PoDoFoInputPdfFile::PoDoFoInputPdfFile(InputPdfFile *pdf_file) :
    InputPdfFile(pdf_file)
{
    m_podofo_file = static_cast<PoDoFoInputPdfFile *>(pdf_file)->m_podofo_file;
    m_podofo_file->ref_count++;
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
    PoDoFo::PdfMemDocument *podofo_file = static_cast<PoDoFoOutputPdfFile *>(output_file)->output_file;

    int page_index = podofo_file->GetPageCount();
    int added_pages = 0;

    //add pages to output document from this document
    if (m_filters.size() == 0)
    {
        podofo_file->InsertPages(*(m_podofo_file->file), 0, m_podofo_file->file->GetPageCount());
        added_pages += m_podofo_file->file->GetPageCount();
    }
    else
    {
        std::list<std::pair<int, int>>::iterator it;
        for (it=m_filters.begin(); it != m_filters.end(); ++it)
        {
            int page_count = it->second - it->first + 1;
            podofo_file->InsertPages(*(m_podofo_file->file), it->first - 1, page_count);
            added_pages += page_count;
        }
    }

    //set pages rotation
    if (m_rotation != 0)
    {
        for (int i=0; i < added_pages; i++)
        {
            PoDoFo::PdfPage *page = podofo_file->GetPage(page_index + i);
            int rotation = (page->GetRotation() + m_rotation)%360;
            page->SetRotation(rotation);
        }
    }
}
