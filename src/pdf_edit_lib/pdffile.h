/* Copyright (C) 2017 Marco Scarpetta
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

#ifndef PDFFILE_H
#define PDFFILE_H

#include <string>
#include <list>
#include "definitions.h"

class OutputPdfFile
{
public:
    OutputPdfFile();

    virtual void write(const std::string &filename) = 0;
};

class InputPdfFile
{
public:
    InputPdfFile();
    InputPdfFile(const std::string &filename);
    InputPdfFile(InputPdfFile *pdf_file);

    virtual const std::string &filename();

    virtual int page_count() = 0;

    virtual int output_page_count();

    virtual Problems set_pages_filter_from_string(const std::string &str);

    virtual Problem *add_pages_filter(int from, int to);

    virtual void set_rotation(int rotation);

    virtual void run(OutputPdfFile *output_file) = 0;

protected:
    std::string m_filename;

    std::list<std::pair<int, int>> m_filters;
    int m_rotation;
    int m_output_page_count;
};

#endif // PDFFILE_H
