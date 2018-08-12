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

#ifndef PDFFILE_PODOFO_H
#define PDFFILE_PODOFO_H

#include <podofo/podofo.h>
#include <string>
#include <list>
#include "pdffile.h"

struct PoDoFoFile {
    PoDoFo::PdfMemDocument *file;
    int ref_count;
};

class PoDoFoOutputPdfFile : public OutputPdfFile
{
public:
    PoDoFoOutputPdfFile();

    ~PoDoFoOutputPdfFile();

    void write(const std::string &filename);

    PoDoFo::PdfMemDocument *output_file;
};

class PoDoFoInputPdfFile : public InputPdfFile
{
public:
    PoDoFoInputPdfFile(const std::string &filename);
    PoDoFoInputPdfFile(InputPdfFile *pdf_file);

    ~PoDoFoInputPdfFile();

    int page_count();

    void run(OutputPdfFile *output_file);

private:
    PoDoFoFile *m_podofo_file;
};

#endif // PDFFILE_PODOFO_H
