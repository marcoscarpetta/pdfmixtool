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

#include "comboboxdelegate.h"

#define PDF_FILE_ROLE Qt::UserRole
#define ROTATATION_ROLE Qt::UserRole + 1

ComboBoxDelegate::ComboBoxDelegate(QWidget *parent) :
    QStyledItemDelegate(parent)
{

}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return new QComboBox(parent);
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QComboBox *combobox = static_cast<QComboBox *>(editor);
    combobox->addItem(tr("No rotation"), 0);
    combobox->addItem(tr("90°"), 90);
    combobox->addItem(tr("180°"), 180);
    combobox->addItem(tr("270°"), 270);

    int rotation = index.data(ROTATATION_ROLE).toInt();
    combobox->setCurrentIndex(combobox->findData(rotation));
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *combobox = static_cast<QComboBox *>(editor);

    QMap<int, QVariant> map;
    map[Qt::DisplayRole] = QVariant(combobox->currentText());
    map[ROTATATION_ROLE] = QVariant(combobox->currentData());

    model->setItemData(index, map);
}
