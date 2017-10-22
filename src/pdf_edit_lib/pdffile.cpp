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

#include "pdffile.h"

OutputPdfFile::OutputPdfFile()
{

}

InputPdfFile::InputPdfFile() :
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

Problems InputPdfFile::set_pages_filter_from_string(const std::string &str)
{
    m_filters.clear();

    Problems problems;

    if (str.find_first_not_of("0123456789- ,") != std::string::npos)
    {
        problems.add(new Problem(
                              ProblemType::error_invalid_char,
                              str.substr(str.find_first_not_of("0123456789- ,"), 1))
                          );
        return problems;
    }

    if (str.find_first_not_of("- ,") == std::string::npos)
    {
        return problems; //void str
    }

    //parse str
    std::string::size_type cursor = str.find_first_not_of(" ,-");
    std::string::size_type interval_end = str.find_first_of(" ,", cursor);

    while (cursor < str.length())
    {
        //single number
        if (str.find_first_of('-', cursor) >= interval_end)
        {
            std::string page_number = str.substr(cursor, interval_end - cursor);
            int num = std::stoi(page_number);
            Problem *problem = this->add_pages_filter(num, num);

            if (problem != NULL)
                problems.add(problem);

            cursor = str.find_first_not_of(" ,-", interval_end);
            interval_end = str.find_first_of(" ,", cursor);
        }
        //interval
        else
        {
            std::string::size_type first_number_end = str.find_first_of('-', cursor);
            std::string::size_type second_number_start = str.find_first_not_of('-', first_number_end);
            if (second_number_start >= interval_end || //syntax error: no second number
                    str.find_first_of('-', second_number_start) < interval_end) //syntax error: more '-' in one interval
            {
                problems.add(new Problem(
                                      ProblemType::error_invalid_interval,
                                      str.substr(cursor, interval_end - cursor))
                                  );

                cursor = str.find_first_not_of(" ,-", interval_end);
                interval_end = str.find_first_of(" ,", cursor);
            }
            else
            {
                int from = std::stoi(str.substr(cursor, first_number_end - cursor));
                int to = std::stoi(str.substr(second_number_start, interval_end));
                Problem *problem = this->add_pages_filter(from, to);

                if (problem != NULL)
                    problems.add(problem);

                cursor = str.find_first_not_of(" ,-", interval_end);
                interval_end = str.find_first_of(" ,", cursor);
            }
        }
    }

    return problems;
}

Problem *InputPdfFile::add_pages_filter(int from, int to)
{
    //check interval boundaries
    if (from < 1 || from > this->page_count() ||
            to < 1 || to > this->page_count())
        return new Problem(
                    ProblemType::error_page_out_of_range,
                    std::to_string(from) + "-" + std::to_string(to)
                    );

    if (from > to)
        return new Problem(
                    ProblemType::error_invalid_interval,
                    std::to_string(from) + "-" + std::to_string(to)
                    );

    Problem *problem = NULL;

    //check if the new interval intersects old ones
    std::list<std::pair<int, int>>::iterator it;
    for (it=m_filters.begin(); it != m_filters.end(); ++it)
    {
        //intersection found
        if (!(from > it->second || to < it->first))
            problem = new Problem(
                        ProblemType::warning_overlapping_interval,
                        std::to_string(from) + "-" + std::to_string(to)
                        );
    }

    m_filters.push_back(std::pair<int, int>(from, to));

    return problem;
}

void InputPdfFile::set_rotation(int rotation)
{
    m_rotation = rotation;
}
