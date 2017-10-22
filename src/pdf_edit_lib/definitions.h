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

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <string>
#include <list>

enum class Backend {
    PoDoFo
};

enum class ProblemType {
    error_invalid_interval,
    error_invalid_char,
    error_page_out_of_range,
    warning_overlapping_interval
};

struct Problem {
    Problem(ProblemType type, const std::string &data) : type(type), data(data), next(NULL) {}
    ProblemType type;
    std::string data;
    Problem *next;
};


struct Problems {
    Problems() : errors(false), count(0), first(NULL) {}
    void add(Problem *problem) {
        if (first == NULL)
        {
            first = problem;
            last = problem;
        }
        else
        {
            last->next = problem;
            last = problem;
        }

        if (problem->type != ProblemType::warning_overlapping_interval)
            errors = true;

        count++;
    }
    bool errors;
    int count;
    Problem *first;
    Problem *last;
};

#endif // DEFINITIONS_H
