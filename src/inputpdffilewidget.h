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
#include <QMouseEvent>

#include "pdf_edit_lib/pdffile.h"

double draw_preview_page(QPainter *painter,
                       int max_width, int max_height,
                       double page_width, double page_height,
                       HAlignment h_alignment, VAlignment v_alignment,
                       const QString &text);

void draw_preview(QPainter *painter, const QRect &rect,
                  double source_width, double source_height,
                  int rotation, int multipage_default_index);

class InputPdfFileWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InputPdfFileWidget(InputPdfFile *pdf_file, int preview_size, QWidget *parent = nullptr);

    void set_data_from_pdf_input_file();

    void set_data_to_pdf_input_file();

signals:
    void focus_out(QWidget *editor) const;

public slots:
    void mouse_button_pressed(QMouseEvent *event);

    void update_preview();

private:
    InputPdfFile *m_pdf_file;
    int m_preview_size;
    QLabel *m_preview_label;
    QLineEdit *m_pages_filter_lineedit;
    QComboBox *m_multipage_combobox;
    QComboBox *m_rotation_combobox;
};

#endif // INPUTPDFFILEWIDGET_H
