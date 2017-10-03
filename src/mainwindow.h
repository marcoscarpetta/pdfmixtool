/* Copyright (C) 2017 Marco Scarpetta
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
#include <QGridLayout>
#include <QPushButton>
#include <QProgressBar>
#include <QFileDialog>
#include <QTableView>
#include <QStandardItemModel>
#include <QSettings>
#include "comboboxdelegate.h"
#include "aboutdialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

signals:

public slots:
    void move_up();

    void move_down();

    void remove_pdf_file();

    void pdf_file_added(const QStringList &selected);

    void generate_pdf(const QString &file_selected);

private:
    QSettings *m_settings;

    QGridLayout *m_layout;

    QPushButton *m_add_file_button;
    QPushButton *m_remove_file_button;
    QPushButton *m_move_up_button;
    QPushButton *m_move_down_button;
    QPushButton *m_about_button;
    QPushButton *m_dest_file_button;

    QProgressBar *m_progress_bar;

    QFileDialog *m_open_file_dialog;
    int m_opened_count;
    QFileDialog *m_dest_file_dialog;

    QTableView *m_files_list_view;
    ComboBoxDelegate *m_combobox_delegate;
    QStandardItemModel *m_files_list_model;

    AboutDialog *m_about_dialog;
};

#endif // MAINWINDOW_H
