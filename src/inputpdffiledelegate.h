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

#ifndef INPUTPDFFILEDELEGATE_H
#define INPUTPDFFILEDELEGATE_H

#include <QStyledItemDelegate>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>

#include "mouseeventfilter.h"
#include "pdf_edit_lib/pdffile.h"

#define PDF_FILE_ROLE Qt::UserRole

Q_DECLARE_METATYPE(InputPdfFile *)

class InputPdfFileDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    InputPdfFileDelegate(MouseEventFilter *filter, QWidget *parent);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;

    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

signals:
    void data_edit() const;

public slots:
    void end_editing(QWidget *editor);

private:
    MouseEventFilter *m_mouse_event_filter;
};

#endif // INPUTPDFFILEDELEGATE_H
