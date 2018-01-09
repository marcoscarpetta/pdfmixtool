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

#include "lineeditdelegate.h"

#define PDF_FILE_ROLE Qt::UserRole
#define ROTATATION_ROLE Qt::UserRole + 1

Q_DECLARE_METATYPE(InputPdfFile *)

LineEditDelegate::LineEditDelegate(QWidget *parent) :
    QStyledItemDelegate(parent)
{

}

QWidget *LineEditDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return new QLineEdit(parent);
}

void LineEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *lineedit = static_cast<QLineEdit *>(editor);
    lineedit->setText(index.data(Qt::EditRole).toString());
}

void LineEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *lineedit = static_cast<QLineEdit *>(editor);
    InputPdfFile *pdf_file = index.data(PDF_FILE_ROLE).value<InputPdfFile *>();

    QMap<int, QVariant> map;
    map[PDF_FILE_ROLE] = QVariant::fromValue<InputPdfFile *>(pdf_file);

    Problems problems = pdf_file->set_pages_filter_from_string(lineedit->text().toStdString());

    if (problems.count != 0)
    {
        if (problems.errors)
            map[Qt::BackgroundRole] = QBrush(QColor(255, 0, 0, 100));
        else
            map[Qt::BackgroundRole] = QBrush(QColor(255, 255, 0, 100));
    }
    //else
      //  map[Qt::ForegroundRole] = QBrush(QColor(0, 0, 0));

    map[Qt::DisplayRole] = QVariant(lineedit->text());
    map[Qt::EditRole] = QVariant(lineedit->text());

    model->setItemData(index, map);

    emit data_edit();
}
