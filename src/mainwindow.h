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
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QFileDialog>
#include <QListView>
#include <QStandardItemModel>
#include <QSettings>
#include <QMessageBox>
#include "inputpdffiledelegate.h"
#include "aboutdialog.h"
#include "pdf_edit_lib/pdfeditor.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

signals:

public slots:
    void pdf_file_added(const QStringList &selected);

    void move_up();

    void move_down();

    void remove_pdf_file();

    void update_output_page_count();

    void generate_pdf_button_pressed();

    void generate_pdf(const QString &file_selected);

    void closeEvent(QCloseEvent *event);

private:
    const QList<int> selected_indexes();

    PdfEditor *m_pdf_editor;

    QSettings *m_settings;

    QPushButton *m_add_file_button;
    QPushButton *m_remove_file_button;
    QPushButton *m_move_up_button;
    QPushButton *m_move_down_button;
    QPushButton *m_about_button;
    QPushButton *m_generate_pdf_button;

    QLabel *m_output_page_count;
    QProgressBar *m_progress_bar;

    QFileDialog *m_open_file_dialog;
    int m_opened_count;
    QFileDialog *m_dest_file_dialog;

    QListView *m_files_list_view;
    InputPdfFileDelegate *m_pdfinputfile_delegate;
    QStandardItemModel *m_files_list_model;

    QMessageBox *m_error_dialog;
    QMessageBox *m_warning_dialog;
    AboutDialog *m_about_dialog;
};

#endif // MAINWINDOW_H
