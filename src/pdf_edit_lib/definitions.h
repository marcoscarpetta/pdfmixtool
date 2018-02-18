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
#include <map>

const double inch = 72;
const double cm = inch / 2.54;
const double mm = cm / 10;

struct PaperSize {
    float width;
    float height;
    std::string name;
    bool portrait;
};

const PaperSize paper_sizes[] = {
    {21.6, 27.9, "US letter", true},
    {84.1, 118.9, "A0", true},
    {59.4, 84.1, "A1", true},
    {42.0, 59.4, "A2", true},
    {29.7, 42.0, "A3", true},
    {21.0, 29.7, "A4", true},
    {14.8, 21.0, "A5", true},
    {10.5, 14.8, "A6", true},
    {7.4, 10.5, "A7", true},
    {5.2, 7.4, "A8", true},
    {3.7, 5.2, "A9", true},
    {2.6, 3.7, "A10", true},
};

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
    std::string name;

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

const NupSettings nup_settings_defaults[] = {
    {
        "2x1, A4, 0°",
        21.0, 29.7,
        2, 1, 0,
        HAlignment::Center, VAlignment::Center,
        1, 1, 1, 1, 1
    },
    {
        "2x1, A4, 90°",
        21.0, 29.7,
        2, 1, 90,
        HAlignment::Center, VAlignment::Center,
        1, 1, 1, 1, 1
    },
    {
        "2x2, A4, 0°",
        21.0, 29.7,
        2, 2, 0,
        HAlignment::Center, VAlignment::Center,
        1, 1, 1, 1, 1
    },
    {
        "2x2, A4, 90°",
        21.0, 29.7,
        2, 2, 90,
        HAlignment::Center, VAlignment::Center,
        1, 1, 1, 1, 1
    }
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
