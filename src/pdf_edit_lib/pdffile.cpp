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

std::list<Error *> *InputPdfFile::set_pages_filter_from_string(const std::string &str)
{
    m_filters.clear();

    std::list<Error *> *errors = new std::list<Error *>();

    if (str.find_first_not_of("0123456789- ,") != std::string::npos)
    {
        errors->push_back(new Error(
                              ErrorType::invalid_char,
                              str.substr(str.find_first_not_of("0123456789- ,"), 1))
                          );
        return errors;
    }

    if (str.find_first_not_of("- ,") == std::string::npos)
    {
        delete errors;
        return NULL; //void str
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
            Error *error = this->add_pages_filter(num, num);

            if (error != NULL)
                errors->push_back(error);

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
                errors->push_back(new Error(
                                      ErrorType::invalid_interval,
                                      str.substr(cursor, interval_end - cursor))
                                  );

                cursor = str.find_first_not_of(" ,-", interval_end);
                interval_end = str.find_first_of(" ,", cursor);
            }
            else
            {
                int from = std::stoi(str.substr(cursor, first_number_end - cursor));
                int to = std::stoi(str.substr(second_number_start, interval_end));
                Error *error = this->add_pages_filter(from, to);

                if (error != NULL)
                    errors->push_back(error);

                cursor = str.find_first_not_of(" ,-", interval_end);
                interval_end = str.find_first_of(" ,", cursor);
            }
        }
    }

    if (errors->size() == 0)
    {
        delete errors;
        return NULL;
    }
    else
        return errors;
}

Error *InputPdfFile::add_pages_filter(int from, int to)
{
    //check interval boundaries
    if (from < 1 || from > this->page_count() ||
            to < 1 || to > this->page_count())
        return new Error(
                    ErrorType::page_out_of_range,
                    std::to_string(from) + "-" + std::to_string(to)
                    );

    if (from > to)
        return new Error(
                    ErrorType::invalid_interval,
                    std::to_string(from) + "-" + std::to_string(to)
                    );

    //check if this interval can be pushed back
    if (m_filters.size() == 0 || from > m_filters.back().second)
    {
        m_filters.push_back(std::pair<int, int>(from, to));
        return NULL;
    }

    //check if the new interval doesn't intersect old ones
    std::list<std::pair<int, int>>::iterator it;
    for (it=m_filters.begin(); it != m_filters.end(); ++it)
    {
        //non-intersection found
        if (to < it->first)
        {
            m_filters.insert(it, std::pair<int, int>(from, to));
            return NULL;
        }
        //intersection found
        else if (from >= it->first && from <= it->second || to >= it->first && to <= it->second)
            break;
    }

    //check if the new interval intersect the iterator interval
    if (to >= it->first && to <= it->second)
        it->first = from;
    else if (from >= it->first && from <= it->second)
    {
        std::list<std::pair<int, int>>::iterator it_end;
        for (it_end=it; it_end != m_filters.end(); ++it_end)
        {
            if (to < it_end->first)
                break;

            if (to >= it_end->first && to <= it_end->second)
                to = it_end->second;
        }

        //delete overlying intervals
        for (--it_end; it_end != it; --it_end)
            m_filters.erase(it_end);

        it->second = to;
    }

    return NULL;
}

void InputPdfFile::set_rotation(int rotation)
{
    m_rotation = rotation;
}
