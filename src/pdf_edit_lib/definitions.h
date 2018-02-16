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

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <string>
#include <list>

enum class HAlignment {
    Left,
    Center,
    Right
};

enum class VAlignment {
    Top,
    Center,
    Bottom
};

struct NupSettings {
    double  page_width;
    double  page_height;

    int rows;
    int columns;

    int rotation;

    HAlignment h_alignment;
    VAlignment v_alignment;

    double margin_top;
    double margin_bottom;
    double margin_left;
    double margin_right;
    double spacing;
};

enum class Backend {
    PoDoFo
};

struct IntervalIssue {
    enum Name {
        error_invalid_interval,
        error_invalid_char,
        error_page_out_of_range,
        warning_overlapping_interval
    };

    IntervalIssue(Name name, const std::string &data) : name(name), data(data) {}
    Name name;
    std::string data;
};

#endif // DEFINITIONS_H
