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
#include <QFileInfo>
#include <QDir>

InputPdfFileDelegate::InputPdfFileDelegate(MouseEventFilter *filter,
                                           const QMap<int, Multipage> &custom_multipages,
                                           QWidget *parent) :
    QStyledItemDelegate(parent),
    m_mouse_event_filter(filter),
    m_custom_multipages(custom_multipages)
{

}

void InputPdfFileDelegate::paint(
        QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const
{
    InputPdfFile *pdf_file = index.data(PDF_FILE_ROLE).value<InputPdfFile *>();

    QColor text_color = option.palette.text().color();

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
    border.setTopLeft(border.topLeft() + QPoint(1, 1));

    if (option.state & QStyle::State_Selected)
    {
        painter->fillRect(border, option.palette.highlight());
        text_color = option.palette.highlightedText().color();
    }
    else if (option.state & QStyle::State_MouseOver)
        painter->fillRect(border, option.palette.midlight());

    // Draw preview
    draw_preview(painter,
                 QRect(option.rect.x() + 2,
                       option.rect.y() + 2,
                       option.rect.height() - 4,
                       option.rect.height() - 4),
                 pdf_file->page_width(), pdf_file->page_height(),
                 pdf_file->rotation(), pdf_file->multipage());

    // Draw text
    QFontMetrics fm = painter->fontMetrics();

    int font_height = fm.height();

    int line_height = static_cast<int>(1.2 * font_height);

    int x = option.rect.x() + option.rect.height() + 10;
    int y = option.rect.y() + line_height;

    // Define fonts
    painter->setPen(text_color);
    QFont font = painter->font();
    QFont bold = font;
    bold.setBold(true);

    // First row minimum width
    QString file_infos = QString(" − %1 cm x %2 cm%3 − %5").arg(
                QString::number(pdf_file->page_width()),
                QString::number(pdf_file->page_height()),
                pdf_file->paper_size().name.size() > 0 ?
                        QString(" (") + pdf_file->paper_size().name.c_str() + " " +
                        (pdf_file->paper_size().portrait ? tr("portrait") : tr("landscape")) + ")" : " ",
                tr("%n page(s)", "", pdf_file->page_count())
                );

    int first_row = fm.boundingRect(file_infos).width();

    QFileInfo fileinfo(QString::fromStdString(pdf_file->filename()));
    QFontMetrics fmb(bold);
    first_row += fmb.boundingRect(fileinfo.fileName()).width();

    int path_available_width = option.rect.width() - (x - option.rect.x() + first_row + 30);
    QString path = QDir::separator();
    QStringList path_split = fileinfo.absolutePath().split(QDir::separator());

    int i = path_split.size() - 1;

    while (i > -1)
    {
        if (fm.boundingRect(path_split.at(i) + path).width() > path_available_width)
            break;

        path.prepend(path_split.at(i));
        if (i != 0)
            path.prepend(QDir::separator());

        i--;
    }

    if (i > -1)
        path.prepend("…");

    painter->drawText(x, y, path);
    x += fm.boundingRect(path).width();

    painter->setFont(bold);
    painter->drawText(x, y, fileinfo.fileName());
    x += fmb.boundingRect(fileinfo.fileName()).width();

    painter->setFont(font);
    painter->drawText(x, y, file_infos);

    y += line_height;
    x = option.rect.x() + option.rect.height() + 10 + 30;

    QString pages_filter = QString::fromStdString(pdf_file->pages_filter_string());
    if (pages_filter.size() == 0)
        pages_filter = tr("All");

    QString pages = tr("Pages:") + QString(" %1").arg(pages_filter);
    QString multipage = tr("Multipage:") + " " + (
                pdf_file->multipage().enabled ?
                    QString(" %1").arg(QString::fromStdString(pdf_file->multipage().name)) :
                    tr("Disabled"));
    QString rotation = tr("Rotation:") + QString(" %1°").arg(pdf_file->rotation());

    if (pdf_file->pages_filter_errors().size() > 0)
    {
        //painter->setPen(QPen(QColor(190, 20, 20), 2, Qt::DotLine));
        QRect rect(x - 3, y - font_height + 3, fm.boundingRect(pages).width() + 6, font_height + 5);
        painter->fillRect(rect, QColor(255,150,150));
        painter->setPen(option.palette.text().color());
        painter->drawRect(rect);
    }
    else if (pdf_file->pages_filter_warnings().size() > 0)
    {
        QRect rect(x - 3, y - font_height + 3, fm.boundingRect(pages).width() + 6, font_height + 5);
        painter->fillRect(rect, QColor(255,255,150));
        painter->setPen(option.palette.text().color());
        painter->drawRect(rect);
    }

    painter->drawText(x, y, pages);
    y += line_height;

    painter->setPen(text_color);
    painter->drawText(x, y, rotation);
    y += line_height;

    y = option.rect.y() + 2 * line_height;
    x += std::max(fm.boundingRect(pages).width(), fm.boundingRect(rotation).width()) + 40;
    painter->drawText(x, y, multipage);
    y += line_height;
}

QSize InputPdfFileDelegate::sizeHint(
        const QStyleOptionViewItem &option,
        const QModelIndex &index
        ) const
{
    InputPdfFile *pdf_file = index.data(PDF_FILE_ROLE).value<InputPdfFile *>();

    QFontMetrics fm = option.fontMetrics;

    int font_height = fm.height();

    int line_height = static_cast<int>(1.2 * font_height);

    int h = line_height * 3 + 15;

    int w = h + 20;

    // Second row width
    QString pages_filter = QString::fromStdString(pdf_file->pages_filter_string());
    if (pages_filter.size() == 0)
        pages_filter = tr("All");

    QString pages = tr("Pages:") + QString(" %1").arg(pages_filter);
    QString multipage = tr("Multipage:") + " " + (
                pdf_file->multipage().enabled ?
                    QString(" %1").arg(QString::fromStdString(pdf_file->multipage().name)) :
                    tr("Disabled"));
    QString rotation = tr("Rotation:") + QString(" %1°").arg(pdf_file->rotation());

    int second_row = std::max(
                fm.boundingRect(pages).width(),
                fm.boundingRect(rotation).width()) + 40 + fm.boundingRect(multipage).width();

    // First row minimum width
    QString file_infos = QString(" − %1 cm x %2 cm%3 − %5").arg(
                QString::number(pdf_file->page_width()),
                QString::number(pdf_file->page_height()),
                pdf_file->paper_size().name.size() > 0 ?
                        QString(" (") + pdf_file->paper_size().name.c_str() + " " +
                        (pdf_file->paper_size().portrait ? tr("portrait") : tr("landscape")) + ")" : " ",
                tr("%n page(s)", "", pdf_file->page_count())
                );

    int first_row = fm.boundingRect(file_infos).width();

    QFileInfo fileinfo(QString::fromStdString(pdf_file->filename()));
    QFont font = option.font;
    font.setBold(true);
    QFontMetrics fmb(font);
    first_row += fmb.boundingRect(fileinfo.fileName()).width() + 30;

    w += std::max(first_row, second_row);

    return QSize(w, h);
}

QWidget *InputPdfFileDelegate::createEditor(
        QWidget *parent,
        const QStyleOptionViewItem &option,
        const QModelIndex &index
        ) const
{
    InputPdfFileWidget *editor = new InputPdfFileWidget(
                index.data(PDF_FILE_ROLE).value<InputPdfFile *>(),
                m_custom_multipages,
                option.rect.height(),
                parent);
    connect(m_mouse_event_filter, SIGNAL(mouse_button_pressed(QMouseEvent*)),
            editor, SLOT(mouse_button_pressed(QMouseEvent*)));
    connect(editor, SIGNAL(focus_out(QWidget*)), this, SLOT(end_editing(QWidget*)));
    return editor;
}

void InputPdfFileDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    Q_UNUSED(index)
    InputPdfFileWidget *w = static_cast<InputPdfFileWidget *>(editor);
    w->set_data_from_pdf_input_file();
}

void InputPdfFileDelegate::setModelData(
        QWidget *editor,
        QAbstractItemModel *model,
        const QModelIndex &index
        ) const
{
    Q_UNUSED(editor)
    Q_UNUSED(model)
    Q_UNUSED(index)
    InputPdfFileWidget *w = static_cast<InputPdfFileWidget *>(editor);
    w->set_data_to_pdf_input_file();

    emit data_edit();
}

void InputPdfFileDelegate::end_editing(QWidget *editor)
{
    emit commitData(editor);
    emit closeEditor(editor);
}
