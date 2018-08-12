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
                       Multipage::Alignment h_alignment,
                       Multipage::Alignment v_alignment,
                       const QString &text)
{
    double scale = std::min(max_width / page_width, max_height / page_height);

    int w = page_width * scale;
    int h = page_height * scale;
    int dx, dy;

    switch (h_alignment)
    {
    case Multipage::Left:
        dx = - max_width / 2;
        break;
    case Multipage::Center:
        dx = - w / 2;
        break;
    case Multipage::Right:
        dx = max_width / 2 - w;
        break;
    default:
        dx = - max_width / 2;
    }

    switch (v_alignment)
    {
    case Multipage::Top:
        dy = - max_height / 2;
        break;
    case Multipage::Center:
        dy = - h / 2;
        break;
    case Multipage::Bottom:
        dy = max_height / 2 - h;
        break;
    default:
        dy = - max_height / 2;
    }

    painter->drawRect(dx, dy, w, h);

    if (text.size() > 0)
    {
        QFont font = painter->font();
        font.setFamily("Serif");
        font.setPixelSize(h / 5 * 4);
        painter->setFont(font);

        painter->drawText(dx + w / 2 - painter->fontMetrics().boundingRect(text).width() / 2,
                          dy + h / 2 + font.pixelSize() / 2,
                          text);
    }

    return scale;
}

void draw_preview(QPainter *painter, const QRect &rect,
                  double source_width, double source_height,
                  int rotation, const Multipage &multipage)
{
    painter->save();

    painter->fillRect(rect, painter->background());
    painter->setPen(QColor(10, 10, 10));

    painter->translate(rect.x() + rect.width() / 2, rect.y() + rect.height() / 2);
    painter->rotate(rotation);

    if (! multipage.enabled)
        draw_preview_page(painter, rect.width() - 4, rect.height() - 4,
                          source_width, source_height,
                          Multipage::Center, Multipage::Center,
                          "1");
    else
    {
        double scale = draw_preview_page(painter, rect.width() - 4, rect.height() - 4,
                                         multipage.page_width, multipage.page_height,
                                         Multipage::Center, Multipage::Center,
                                         "");

        painter->rotate(multipage.rotation);

        int rows = multipage.rotation == 90 ? multipage.columns : multipage.rows;
        int columns = multipage.rotation == 90 ? multipage.rows : multipage.columns;

        int margin_left = (multipage.rotation == 90 ? multipage.margin_top : multipage.margin_left) * scale;
        int margin_right = (multipage.rotation == 90 ? multipage.margin_bottom : multipage.margin_right) * scale;
        int margin_top = (multipage.rotation == 90 ? multipage.margin_left : multipage.margin_top) * scale;
        int margin_bottom = (multipage.rotation == 90 ? multipage. margin_right : multipage.margin_bottom) * scale;
        int spacing = multipage.spacing * scale;

        int page_width = (multipage.rotation == 90 ? multipage.page_height : multipage.page_width) * scale;
        int page_height = (multipage.rotation == 90 ? multipage.page_width : multipage.page_height) * scale;

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
                                  multipage.h_alignment, multipage.v_alignment,
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

InputPdfFileWidget::InputPdfFileWidget(InputPdfFile *pdf_file,
                                       const QMap<int, Multipage> &custom_multipages,
                                       int preview_size, QWidget *parent) :
    QWidget(parent),
    m_pdf_file(pdf_file),
    m_custom_multipages(custom_multipages),
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

    m_pages_filter_lineedit->setClearButtonEnabled(true);

    m_multipage_combobox->addItem(tr("Disabled"), 0);
    int i = 0;
    for (const Multipage &multipage : multipage_defaults)
    {
        if (i != 0)
            m_multipage_combobox->addItem(QString::fromStdString(multipage.name), i);
        i++;
    }

    QMap<int, Multipage>::const_iterator it;
    for (it = m_custom_multipages.constBegin(); it != m_custom_multipages.constEnd(); ++it)
        m_multipage_combobox->addItem(QString::fromStdString(it.value().name), it.key());

    m_rotation_combobox->addItem(tr("No rotation"), 0);
    m_rotation_combobox->addItem("90°", 90);
    m_rotation_combobox->addItem("180°", 180);
    m_rotation_combobox->addItem("270°", 270);

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

    int i = m_multipage_combobox->findText(QString::fromStdString(m_pdf_file->multipage().name));
    if (i >= 0)
        m_multipage_combobox->setCurrentIndex(i);
    else
        m_multipage_combobox->setCurrentIndex(0);

    m_rotation_combobox->setCurrentIndex(m_rotation_combobox->findData(m_pdf_file->rotation()));
}

void InputPdfFileWidget::set_data_to_pdf_input_file()
{
    m_pdf_file->set_pages_filter_from_string(m_pages_filter_lineedit->text().toStdString());

    if (m_multipage_combobox->currentData().toInt() < 100)
        m_pdf_file->set_multipage(multipage_defaults[m_multipage_combobox->currentData().toInt()]);
    else
        m_pdf_file->set_multipage(m_custom_multipages[m_multipage_combobox->currentData().toInt()]);

    m_pdf_file->set_rotation(m_rotation_combobox->currentData().toInt());
}

void InputPdfFileWidget::mouse_button_pressed(QMouseEvent *event)
{
    QRect multipage_rect = m_multipage_combobox->rect();
    multipage_rect.setHeight((m_multipage_combobox->count() + 1) * multipage_rect.height());
    multipage_rect.setTop(
                multipage_rect.top() - m_multipage_combobox->count() * multipage_rect.height());

    QRect rotation_rect = m_rotation_combobox->rect();
    rotation_rect.setHeight((m_rotation_combobox->count() + 1) * rotation_rect.height());
    rotation_rect.setTop(rotation_rect.top() - m_rotation_combobox->count() * rotation_rect.height());

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

    if (m_multipage_combobox->currentData().toInt() < 100)
        draw_preview(&painter, pixmap.rect(),
                     m_pdf_file->page_width(), m_pdf_file->page_height(),
                     m_rotation_combobox->currentData().toInt(),
                     multipage_defaults[m_multipage_combobox->currentData().toInt()]);
    else
        draw_preview(&painter, pixmap.rect(),
                     m_pdf_file->page_width(), m_pdf_file->page_height(),
                     m_rotation_combobox->currentData().toInt(),
                     m_custom_multipages[m_multipage_combobox->currentData().toInt()]);

    m_preview_label->setPixmap(pixmap);
}
