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

#ifndef INPUTPDFFILEWIDGET_H
#define INPUTPDFFILEWIDGET_H

#include <QComboBox>
#include <QLineEdit>
#include <QLabel>

#include "pdf_edit_lib/pdffile.h"

class InputPdfFileWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InputPdfFileWidget(QWidget *parent = nullptr);

    void set_data_from_pdf_input_file(InputPdfFile *pdf_file);

    void set_data_to_pdf_input_file(InputPdfFile *pdf_file);

private:
    QLineEdit *m_pages_filter_lineedit;
    QComboBox *m_multipage_combobox;
    QComboBox *m_rotation_combobox;
};

#endif // INPUTPDFFILEWIDGET_H
