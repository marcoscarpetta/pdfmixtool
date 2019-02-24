/* Copyright (C) 2017-2019 Marco Scarpetta
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

#ifndef PDFEDITOR_H
#define PDFEDITOR_H

#include "podofo_pdffile.h"

class PdfEditor
{
public:
    PdfEditor(Backend backend=Backend::PoDoFo);

    InputPdfFile *new_input_file(const std::string &filename);

    InputPdfFile *new_input_file(InputPdfFile *pdf_file);

    OutputPdfFile *new_output_file();

private:
    Backend m_backend;
};

#endif // PDFEDITOR_H
