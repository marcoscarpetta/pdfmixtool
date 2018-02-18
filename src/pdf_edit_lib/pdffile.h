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

#ifndef PDFFILE_H
#define PDFFILE_H

#include <string>
#include <list>
#include <vector>
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

    virtual float page_width();

    virtual float page_height();

    virtual const PaperSize &paper_size();

    virtual int page_count() = 0;

    virtual int output_page_count();

    virtual void set_pages_filter_from_string(const std::string &str);

    virtual const std::string &pages_filter_string();

    virtual void add_pages_filter(int from, int to);

    virtual const std::vector<IntervalIssue> &pages_filter_errors();

    virtual const std::vector<IntervalIssue> &pages_filter_warnings();

    virtual void set_default_nup_settings(int i);

    virtual int default_nup_settings();

    virtual void set_rotation(int rotation);

    virtual int rotation();

    virtual void run(OutputPdfFile *output_file) = 0;

protected:
    std::string m_filename;

    float m_page_width;
    float m_page_height;
    PaperSize m_paper_size;

    std::list<std::pair<int, int>> m_filters;
    std::string m_pages_filter_string;
    std::vector<IntervalIssue> m_interval_errors;
    std::vector<IntervalIssue> m_interval_warnings;

    int m_default_nup_settings;

    int m_rotation;
};

#endif // PDFFILE_H
