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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QListView>
#include <QStandardItemModel>
#include <QSettings>

#include "mouseeventfilter.h"
#include "pdf_edit_lib/pdfeditor.h"

Q_DECLARE_METATYPE(Multipage)

QDataStream &operator<<(QDataStream &out, const Multipage &maltipage);
QDataStream &operator>>(QDataStream &in, Multipage &maltipage);

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(MouseEventFilter *filter, QWidget *parent = nullptr);

signals:

public slots:
    void add_pdf_files();

    void move_up();

    void move_down();

    void remove_pdf_file();

    void edit_menu_activated();

    void item_mouse_pressed(const QModelIndex &index);

    void update_output_page_count();

    void generate_pdf_button_pressed();

    void generate_pdf();

    void closeEvent(QCloseEvent *event);

protected:
    bool eventFilter(QObject *obj, QEvent *ev);

private:
    const QList<int> selected_indexes();

    PdfEditor *m_pdf_editor;

    QSettings *m_settings;

    QLabel *m_output_page_count;
    QProgressBar *m_progress_bar;

    QListView *m_files_list_view;
    QStandardItemModel *m_files_list_model;

    QMenu *m_edit_menu;

    QMap<int, Multipage> m_custom_multipages;
};

#endif // MAINWINDOW_H
