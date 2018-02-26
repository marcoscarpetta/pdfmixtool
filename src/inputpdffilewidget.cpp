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

#include "inputpdffilewidget.h"

#include <QGridLayout>
#include <QFocusEvent>
#include <QPainter>

double draw_preview_page(QPainter *painter,
                       int max_width, int max_height,
                       double page_width, double page_height,
                       HAlignment h_alignment,
                       VAlignment v_alignment,
                       const QString &text)
{
    double scale = std::min(max_width / page_width, max_height / page_height);

    int w = page_width * scale;
    int h = page_height * scale;
    int dx, dy;

    switch (h_alignment)
    {
    case HAlignment::Left:
        dx = - max_width / 2;
        break;
    case HAlignment::Center:
        dx = - w / 2;
        break;
    case HAlignment::Right:
        dx = max_width / 2 - w;
    }

    switch (v_alignment)
    {
    case VAlignment::Top:
        dy = - max_height / 2;
        break;
    case VAlignment::Center:
        dy = - h / 2;
        break;
    case VAlignment::Bottom:
        dy = max_height / 2 - h;
    }

    painter->drawRect(dx, dy, w, h);

    if (text.size() > 0)
    {
        QFont font = painter->font();
        font.setFamily("Serif");
        font.setPixelSize(h / 5 * 4);
        painter->setFont(font);

        painter->drawText(dx + w / 2 - painter->fontMetrics().width(text) / 2,
                          dy + h / 2 + font.pixelSize() / 2,
                          text);
    }

    return scale;
}

void draw_preview(QPainter *painter, const QRect &rect,
                  double source_width, double source_height,
                  int rotation, int multipage_default_index)
{
    painter->save();

    painter->fillRect(rect, painter->background());
    painter->setPen(QColor(10, 10, 10));

    painter->translate(rect.x() + rect.width() / 2, rect.y() + rect.height() / 2);
    painter->rotate(rotation);

    if (multipage_default_index == -1)
        draw_preview_page(painter, rect.width() - 4, rect.height() - 4,
                          source_width, source_height,
                          HAlignment::Center, VAlignment::Center,
                          "1");
    else
    {
        const Multipage &m = multipage_defaults[multipage_default_index];

        double scale = draw_preview_page(painter, rect.width() - 4, rect.height() - 4,
                                         m.page_width, m.page_height,
                                         HAlignment::Center, VAlignment::Center,
                                         "");

        painter->rotate(m.rotation);

        int rows = m.rotation == 90 ? m.columns : m.rows;
        int columns = m.rotation == 90 ? m.rows : m.columns;

        int margin_left = (m.rotation == 90 ? m.margin_top : m.margin_left) * scale;
        int margin_right = (m.rotation == 90 ? m.margin_bottom : m.margin_right) * scale;
        int margin_top = (m.rotation == 90 ? m.margin_left : m.margin_top) * scale;
        int margin_bottom = (m.rotation == 90 ? m. margin_right : m.margin_bottom) * scale;
        int spacing = m.spacing * scale;

        int page_width = (m.rotation == 90 ? m.page_height : m.page_width) * scale;
        int page_height = (m.rotation == 90 ? m.page_width : m.page_height) * scale;

        int subpage_width = (page_width - margin_left - margin_right - (columns - 1) * spacing) / columns;
        int subpage_height = (page_height - margin_top - margin_bottom - (rows - 1) * spacing) / rows;

        int page_number = 1;

        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                painter->translate(
                            margin_left - page_width / 2 + j * (spacing + subpage_width) + subpage_width / 2,
                            margin_top - page_height / 2 + i * (spacing + subpage_height) + subpage_height / 2
                            );

                draw_preview_page(painter, subpage_width, subpage_height,
                                  source_width, source_height,
                                  HAlignment::Center, VAlignment::Center,
                                  QString::number(page_number));

                painter->translate(
                            - (margin_left - page_width / 2 + j * (spacing + subpage_width) + subpage_width / 2),
                            - (margin_top - page_height / 2 + i * (spacing + subpage_height) + subpage_height / 2)
                            );

                page_number++;
            }
        }

    }

    painter->restore();
}

InputPdfFileWidget::InputPdfFileWidget(InputPdfFile *pdf_file, int preview_size, QWidget *parent) :
    QWidget(parent),
    m_pdf_file(pdf_file),
    m_preview_size(preview_size),
    m_preview_label(new QLabel(this)),
    m_pages_filter_lineedit(new QLineEdit(this)),
    m_multipage_combobox(new QComboBox(this)),
    m_rotation_combobox(new QComboBox(this))
{
    this->setBackgroundRole(QPalette::AlternateBase);
    this->setAutoFillBackground(true);

    QGridLayout *layout = new QGridLayout();
    this->setLayout(layout);

    m_multipage_combobox->addItem(tr("Disabled"), -1);
    int i = 0;
    for (const Multipage &multipage : multipage_defaults)
    {
        m_multipage_combobox->addItem(QString::fromStdString(multipage.name), i);
        i++;
    }

    m_rotation_combobox->addItem(tr("No rotation"), 0);
    m_rotation_combobox->addItem(tr("90°"), 90);
    m_rotation_combobox->addItem(tr("180°"), 180);
    m_rotation_combobox->addItem(tr("270°"), 270);

    layout->addWidget(m_preview_label, 1, 1, 3, 1);
    layout->addWidget(new QLabel(tr("Pages:"), this), 1, 2);
    layout->addWidget(m_pages_filter_lineedit, 1, 3);
    layout->addWidget(new QLabel(tr("Multipage:"), this), 1, 4);
    layout->addWidget(m_multipage_combobox, 1, 5);
    layout->addWidget(new QLabel(tr("Rotation:"), this), 2, 2);
    layout->addWidget(m_rotation_combobox, 2, 3);

    layout->addItem(new QSpacerItem(0, 0), 1, 6);
    layout->setColumnStretch(3, 10);
    layout->setColumnStretch(5, 10);
    layout->setColumnStretch(6, 90);

    connect(m_multipage_combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(update_preview()));
    connect(m_rotation_combobox, SIGNAL(currentIndexChanged(int)), this, SLOT(update_preview()));

    update_preview();
}

void InputPdfFileWidget::set_data_from_pdf_input_file()
{
    m_pages_filter_lineedit->setText(QString::fromStdString(m_pdf_file->pages_filter_string()));

    m_multipage_combobox->setCurrentIndex(
                m_multipage_combobox->findData(m_pdf_file->multipage_default_index()));

    m_rotation_combobox->setCurrentIndex(m_rotation_combobox->findData(m_pdf_file->rotation()));
}

void InputPdfFileWidget::set_data_to_pdf_input_file()
{
    m_pdf_file->set_pages_filter_from_string(m_pages_filter_lineedit->text().toStdString());

    m_pdf_file->set_multipage_default_index(m_multipage_combobox->currentData().toInt());

    m_pdf_file->set_rotation(m_rotation_combobox->currentData().toInt());
}

void InputPdfFileWidget::mouse_button_pressed(QMouseEvent *event)
{
    QRect multipage_rect = m_multipage_combobox->rect();
    multipage_rect.setHeight(6 * multipage_rect.height());
    multipage_rect.setTop(multipage_rect.top() - 5 * multipage_rect.height());

    QRect rotation_rect = m_rotation_combobox->rect();
    rotation_rect.setHeight(5 * rotation_rect.height());
    rotation_rect.setTop(rotation_rect.top() - 4 * rotation_rect.height());

    if (! this->rect().contains(this->mapFromGlobal(event->globalPos())) &&
            ! multipage_rect.contains(m_multipage_combobox->mapFromGlobal(event->globalPos())) &&
            ! rotation_rect.contains(m_rotation_combobox->mapFromGlobal(event->globalPos()))
            )
        emit focus_out(this);
}

void InputPdfFileWidget::update_preview()
{
    QPixmap pixmap(m_preview_size - layout()->contentsMargins().top() * 2, m_preview_size - layout()->contentsMargins().top() * 2);
    QPainter painter(&pixmap);

    draw_preview(&painter, pixmap.rect(),
                 m_pdf_file->page_width(), m_pdf_file->page_height(),
                 m_rotation_combobox->currentData().toInt(),
                 m_multipage_combobox->currentData().toInt());

    m_preview_label->setPixmap(pixmap);
}
