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

#include "inputpdffiledelegate.h"

#include <QPainter>

#include "inputpdffilewidget.h"

InputPdfFileDelegate::InputPdfFileDelegate(QWidget *parent) :
    QStyledItemDelegate(parent)
{

}

void InputPdfFileDelegate::paint(
        QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    InputPdfFile *pdf_file = index.data(PDF_FILE_ROLE).value<InputPdfFile *>();

    // Draw border
    QPen pen;
    pen.setBrush(option.palette.mid());
    if (option.state & QStyle::State_MouseOver)
        pen.setBrush(option.palette.highlight());
    painter->setPen(pen);

    QRect border = option.rect;
    border.setBottomRight(border.bottomRight() - QPoint(1, 1));
    painter->drawRect(border);

    // Draw background
    painter->setPen(option.palette.text().color());
    border.setTopLeft(border.topLeft() + QPoint(1, 1));

    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(border, option.palette.highlight());
        painter->setPen(option.palette.highlightedText().color());
    }
    else if (option.state & QStyle::State_MouseOver)
        painter->fillRect(border, option.palette.midlight());

    // Draw text
    QFontMetrics fm = painter->fontMetrics();

    int font_height = fm.height();

    int line_height = (int)(1.2 * font_height);

    int x = option.rect.x() + 5;
    int y = option.rect.y() + line_height;

    QFont font = painter->font();
    font.setBold(true);
    painter->setFont(font);
    QString filename = QString::fromStdString(pdf_file->filename());
    painter->drawText(x, y, filename);
    x += painter->fontMetrics().width(filename);

    font.setBold(false);
    painter->setFont(font);
    QString file_infos = QString(" − %1 cm x %2 cm%3 − %5 %6").arg(
                QString::number(pdf_file->page_width()),
                QString::number(pdf_file->page_height()),
                pdf_file->paper_size().name.size() > 0 ?
                        QString(" (") + pdf_file->paper_size().name.c_str() + " " +
                        (pdf_file->paper_size().portrait ? tr("portrait") : tr("landscape")) + ")" : " ",
                QString::number(pdf_file->page_count()),
                tr("page", "", pdf_file->page_count())
                );

    painter->drawText(x, y, file_infos);

    y += line_height;
    x = option.rect.x() + 5 + 30;

    QString pages_filter = QString::fromStdString(pdf_file->pages_filter_string());
    if (pages_filter.size() == 0)
        pages_filter = tr("All");

    QString pages = tr("Pages:") + QString(" %1").arg(pages_filter);
    QString multipage = tr("Multipage:") + " " + (
                pdf_file->multipage_default_index() < 0 ? tr("Disabled") :
            QString(" %1").arg(QString::fromStdString(
                                   multipage_defaults[pdf_file->multipage_default_index()].name))
            );
    QString rotation = tr("Rotation:") + QString(" %1°").arg(pdf_file->rotation());

    if (pdf_file->pages_filter_errors().size() > 0)
    {
        painter->fillRect(
                    x - 3, y - font_height,
                    fm.width(pages) + 6, font_height + 8,
                    QColor(255, 0, 0, 100));
    }
    else if (pdf_file->pages_filter_warnings().size() > 0)
    {
        painter->fillRect(
                    x - 3, y - font_height,
                    fm.width(pages) + 6, font_height + 8,
                    QColor(255, 255, 0, 100));
    }
    painter->drawText(x, y, pages);
    y += line_height;

    painter->drawText(x, y, rotation);
    y += line_height;

    y = option.rect.y() + 2 * line_height;
    x += std::max(fm.width(pages), fm.width(rotation)) + 40;
    painter->drawText(x, y, multipage);
    y += line_height;
}

QSize InputPdfFileDelegate::sizeHint(
        const QStyleOptionViewItem &option,
        const QModelIndex &index
        ) const
{
    QFontMetrics fm = option.fontMetrics;

    int font_height = fm.height();

    int line_height = (int)(1.2 * font_height);

    int h = line_height * 3 + 15;

    return QSize(300, h);
}

QWidget *InputPdfFileDelegate::createEditor(
        QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index
        ) const
{
    return new InputPdfFileWidget(parent);
}

void InputPdfFileDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    InputPdfFile *pdf_file = index.data(PDF_FILE_ROLE).value<InputPdfFile *>();

    InputPdfFileWidget *w = static_cast<InputPdfFileWidget *>(editor);

    w->set_data_from_pdf_input_file(pdf_file);
}

void InputPdfFileDelegate::setModelData(
        QWidget *editor,
        QAbstractItemModel *model,
        const QModelIndex &index
        ) const
{
    InputPdfFile *pdf_file = index.data(PDF_FILE_ROLE).value<InputPdfFile *>();

    InputPdfFileWidget *w = static_cast<InputPdfFileWidget *>(editor);

    w->set_data_to_pdf_input_file(pdf_file);

    emit data_edit();
}
