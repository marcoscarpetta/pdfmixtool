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

#include "pdffile.h"

OutputPdfFile::OutputPdfFile()
{

}

InputPdfFile::InputPdfFile() :
    m_multipage(multipage_defaults[0]),
    m_rotation(0)
{

}

InputPdfFile::InputPdfFile(const std::string &filename) :
    InputPdfFile()
{
    m_filename = filename;
}

InputPdfFile::InputPdfFile(InputPdfFile *pdf_file) :
    InputPdfFile()
{
    m_filename = pdf_file->m_filename;
}

const std::string &InputPdfFile::filename()
{
    return m_filename;
}

float InputPdfFile::page_width()
{
    return m_page_width;
}

float InputPdfFile::page_height()
{
    return m_page_height;
}

const PaperSize &InputPdfFile::paper_size()
{
    return m_paper_size;
}

int InputPdfFile::output_page_count()
{
    int count = 0;

    if (m_filters.size() == 0)
        count = this->page_count();
    else
    {
        std::list<std::pair<int, int>>::iterator it;
        for (it=m_filters.begin(); it != m_filters.end(); ++it)
        {
            count += it->second - it->first + 1;
        }
    }

    if (m_multipage.enabled)
    {
        int multipage = m_multipage.rows * m_multipage.columns;

        if (count % multipage > 0)
            count = count / multipage + 1;
        else
            count = count / multipage;
    }

    return count;
}

void InputPdfFile::set_pages_filter_from_string(const std::string &str)
{
    m_interval_errors.clear();
    m_interval_warnings.clear();

    m_pages_filter_string = str;

    m_filters.clear();

    // Invalid characters
    if (str.find_first_not_of("0123456789- ,") != std::string::npos)
    {
        m_interval_errors.push_back(IntervalIssue(
                              IntervalIssue::error_invalid_char,
                              str.substr(str.find_first_not_of("0123456789- ,"), 1))
                          );
        return;
    }

     // Void string
    if (str.find_first_not_of("- ,") == std::string::npos)
    {
        return;
    }

    // Parse string
    std::string::size_type cursor = str.find_first_not_of(" ,-");
    std::string::size_type interval_end = str.find_first_of(" ,", cursor);

    while (cursor < str.length())
    {
        // Single number
        if (str.find_first_of('-', cursor) >= interval_end)
        {
            std::string page_number = str.substr(cursor, interval_end - cursor);
            int num = std::stoi(page_number);
            this->add_pages_filter(num, num);

            cursor = str.find_first_not_of(" ,-", interval_end);
            interval_end = str.find_first_of(" ,", cursor);
        }
        // Interval
        else
        {
            std::string::size_type first_number_end = str.find_first_of('-', cursor);
            std::string::size_type second_number_start = str.find_first_not_of('-', first_number_end);
            if (
                    // Syntax error: no second number
                    second_number_start >= interval_end ||
                    // Syntax error: more '-' in one interval
                    str.find_first_of('-', second_number_start) < interval_end
                    )
            {
                m_interval_errors.push_back(IntervalIssue(
                                      IntervalIssue::error_invalid_interval,
                                      str.substr(cursor, interval_end - cursor))
                                  );

                cursor = str.find_first_not_of(" ,-", interval_end);
                interval_end = str.find_first_of(" ,", cursor);
            }
            else
            {
                int from = std::stoi(str.substr(cursor, first_number_end - cursor));
                int to = std::stoi(str.substr(second_number_start, interval_end));
                this->add_pages_filter(from, to);

                cursor = str.find_first_not_of(" ,-", interval_end);
                interval_end = str.find_first_of(" ,", cursor);
            }
        }
    }
}

const std::string &InputPdfFile::pages_filter_string()
{
    return m_pages_filter_string;
}

void InputPdfFile::add_pages_filter(int from, int to)
{
    // Check interval boundaries
    if (from < 1 || from > this->page_count() ||
            to < 1 || to > this->page_count())
    {
        m_interval_errors.push_back(IntervalIssue(
                    IntervalIssue::error_page_out_of_range,
                    std::to_string(from) + "-" + std::to_string(to)
                    ));
        return;
    }

    if (from > to)
    {
        m_interval_errors.push_back(IntervalIssue(
                    IntervalIssue::error_invalid_interval,
                    std::to_string(from) + "-" + std::to_string(to)
                    ));
        return;
    }

    // Check if the new interval intersects old ones
    std::list<std::pair<int, int>>::iterator it;
    for (it=m_filters.begin(); it != m_filters.end(); ++it)
    {
        // Intersection found
        if (!(from > it->second || to < it->first))
            m_interval_warnings.push_back(IntervalIssue(
                        IntervalIssue::warning_overlapping_interval,
                        std::to_string(from) + "-" + std::to_string(to)
                        ));
    }

    m_filters.push_back(std::pair<int, int>(from, to));
}

const std::vector<IntervalIssue> &InputPdfFile::pages_filter_errors()
{
    return m_interval_errors;
}

const std::vector<IntervalIssue> &InputPdfFile::pages_filter_warnings()
{
    return m_interval_warnings;
}

void InputPdfFile::set_multipage(const Multipage &multipage)
{
    m_multipage = multipage;
}

const Multipage &InputPdfFile::multipage()
{
    return m_multipage;
}

void InputPdfFile::set_rotation(int rotation)
{
    m_rotation = rotation;
}

int InputPdfFile::rotation()
{
    return m_rotation;
}
