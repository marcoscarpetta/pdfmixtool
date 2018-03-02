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

#ifndef EDITMULTIPAGEPROFILEDIALOG_H
#define EDITMULTIPAGEPROFILEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>

#include "pdf_edit_lib/definitions.h"

class EditMultipageProfileDialog : public QDialog
{
    Q_OBJECT
public:
    EditMultipageProfileDialog(QWidget *parent = nullptr);

    void set_multipage(const Multipage &multipage);

    Multipage get_multipage();

    void set_index(int index);

    int get_index();

private slots:
    void custom_page_size_toggled(bool toggled);

    void page_size_changed(int index);

    void page_width_changed(double value);

    void page_height_changed(double value);

private:
    int m_index;
    QLineEdit m_name;
    QComboBox m_page_size;
    QCheckBox m_custom_page_size;
    QDoubleSpinBox m_page_width;
    QDoubleSpinBox m_page_height;
    QSpinBox m_rows;
    QSpinBox m_columns;
    QComboBox m_rotation;
    QComboBox m_h_alignment;
    QComboBox m_v_alignment;
    QDoubleSpinBox m_margin_left;
    QDoubleSpinBox m_margin_right;
    QDoubleSpinBox m_margin_top;
    QDoubleSpinBox m_margin_bottom;
    QDoubleSpinBox m_spacing;
};

#endif // EDITMULTIPAGEPROFILEDIALOG_H
