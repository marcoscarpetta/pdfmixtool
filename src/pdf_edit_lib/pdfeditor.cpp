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

#include "pdfeditor.h"

PdfEditor::PdfEditor(Backend backend) :
    m_backend(backend)
{

}

InputPdfFile *PdfEditor::new_input_file(const std::string &filename)
{
    switch (m_backend) {
    case Backend::PoDoFo:
        return new PoDoFoInputPdfFile(filename);
    default:
        return new PoDoFoInputPdfFile(filename);
    }
}

InputPdfFile *PdfEditor::new_input_file(InputPdfFile *pdf_file)
{
    switch (m_backend) {
    case Backend::PoDoFo:
        return new PoDoFoInputPdfFile(pdf_file);
    default:
        return new PoDoFoInputPdfFile(pdf_file);
    }
}

OutputPdfFile *PdfEditor::new_output_file()
{
    switch (m_backend) {
    case Backend::PoDoFo:
        return new PoDoFoOutputPdfFile();
    default:
        return new PoDoFoOutputPdfFile();
    }
}
