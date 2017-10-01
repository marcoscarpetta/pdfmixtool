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

#include "mainwindow.h"

#include <QTimer>
#include "pdffile.h"

#define NAME_COLUMN 0
#define PAGES_FILTER_COLUMN 2
#define ROTATION_COLUMN 3

Q_DECLARE_METATYPE(PdfFile *)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_layout(new QGridLayout(this)),
    m_add_file_button(new QPushButton(QIcon::fromTheme("list-add"), tr("Add PDF file"), this)),
    m_move_up_button(new QPushButton(QIcon::fromTheme("go-up"), tr("Move up"), this)),
    m_move_down_button(new QPushButton(QIcon::fromTheme("go-down"), tr("Move Down"), this)),
    m_remove_file_button(new QPushButton(QIcon::fromTheme("list-remove"), tr("Remove file"), this)),
    m_about_button(new QPushButton(QIcon::fromTheme("help-about"), tr("About"), this)),
    m_dest_file_button(new QPushButton(QIcon::fromTheme("document-save-as"), tr("Generate PDF"), this)),
    m_progress_bar(new QProgressBar(this)),
    m_open_file_dialog(new QFileDialog(this)),
    m_opened_count(0),
    m_dest_file_dialog(new QFileDialog(this)),
    m_files_list_view(new QTableView(this)),
    m_combobox_delegate(new ComboBoxDelegate(this)),
    m_files_list_model(new QStandardItemModel(0, 4, this)),
    m_about_dialog(new AboutDialog(this))
{
    m_progress_bar->hide();

    m_open_file_dialog->setNameFilter("*.pdf");
    m_open_file_dialog->setFilter(QDir::Files);
    m_open_file_dialog->setFileMode(QFileDialog::ExistingFiles);

    m_dest_file_dialog->setNameFilter("*.pdf");
    m_dest_file_dialog->setFilter(QDir::Files);
    m_dest_file_dialog->setAcceptMode(QFileDialog::AcceptSave);

    m_files_list_view->setWordWrap(false);
    m_files_list_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_files_list_view->setEditTriggers(QAbstractItemView::CurrentChanged |
                                       QAbstractItemView::DoubleClicked |
                                       QAbstractItemView::SelectedClicked);
    m_files_list_view->setModel(m_files_list_model);
    m_files_list_model->setHorizontalHeaderItem(0, new QStandardItem(tr("Filename")));
    m_files_list_model->setHorizontalHeaderItem(1, new QStandardItem(tr("Page count")));
    m_files_list_model->setHorizontalHeaderItem(2, new QStandardItem(tr("Pages filter")));
    m_files_list_model->setHorizontalHeaderItem(3, new QStandardItem(tr("Rotation")));
    m_files_list_view->setItemDelegateForColumn(3, m_combobox_delegate);

    QWidget *central_widget = new QWidget(this);
    central_widget->setLayout(m_layout);

    m_layout->addWidget(m_add_file_button, 1, 1);
    m_layout->addWidget(m_move_up_button, 1, 2);
    m_layout->addWidget(m_move_down_button, 1, 3);
    m_layout->addWidget(m_remove_file_button, 1, 4);
    m_layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 1, 5);
    m_layout->addWidget(m_about_button, 1, 6);

    m_layout->addWidget(m_files_list_view, 2, 1, 1, 6);

    m_layout->addWidget(m_progress_bar, 3, 1, 1, 2);
    m_layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 3, 3, 1, 3);
    m_layout->addWidget(m_dest_file_button, 3, 6);

    this->setCentralWidget(central_widget);

    connect(m_add_file_button, SIGNAL(pressed()), m_open_file_dialog, SLOT(exec()));
    connect(m_move_up_button, SIGNAL(pressed()), this, SLOT(move_up()));
    connect(m_move_down_button, SIGNAL(pressed()), this, SLOT(move_down()));
    connect(m_remove_file_button, SIGNAL(pressed()), this, SLOT(remove_pdf_file()));
    connect(m_about_button, SIGNAL(pressed()), m_about_dialog, SLOT(show()));
    connect(m_dest_file_button, SIGNAL(pressed()), m_dest_file_dialog, SLOT(exec()));

    connect(m_open_file_dialog, SIGNAL(filesSelected(QStringList)), this, SLOT(pdf_file_added(QStringList)));

    connect(m_dest_file_dialog, SIGNAL(fileSelected(QString)), this, SLOT(generate_pdf(QString)));
}

void MainWindow::move_up()
{
    if (m_files_list_view->selectionModel()->hasSelection())
    {
        int from = -1;
        int to = -1;

        QModelIndexList selected = m_files_list_view->selectionModel()->selectedRows();
        for (QModelIndexList::const_iterator it=selected.begin(); it<selected.end(); ++it)
        {
            if (from == -1)
                from = (*it).row();
            if (to == -1 || (*it).row() - to == 1)
                to = (*it).row();
            else
                return; //non contiguous rows
        }

        if (from > 0)
        {
            for (int i=from; i<=to; i++)
            {
                QList<QStandardItem *> row = m_files_list_model->takeRow(i);
                m_files_list_model->insertRow(i-1, row);
            }

            QItemSelection sel(m_files_list_model->index(from-1, 0), m_files_list_model->index(to-1, 0));
            m_files_list_view->selectionModel()->select(sel, QItemSelectionModel::Select | QItemSelectionModel::Rows);
        }
    }
}

void MainWindow::move_down()
{
    if (m_files_list_view->selectionModel()->hasSelection())
    {
        int from = -1;
        int to = -1;

        QModelIndexList selected = m_files_list_view->selectionModel()->selectedRows();
        for (QModelIndexList::const_iterator it=selected.begin(); it<selected.end(); ++it)
        {
            if (from == -1)
                from = (*it).row();
            if (to == -1 || (*it).row() - to == 1)
                to = (*it).row();
            else
                return; //non contiguous rows
        }

        if (to < m_files_list_model->rowCount() - 1)
        {
            for (int i=to; i>=from; i--)
            {
                QList<QStandardItem *> row = m_files_list_model->takeRow(i);
                m_files_list_model->insertRow(i+1, row);
            }

            QItemSelection sel(m_files_list_model->index(from+1, 0), m_files_list_model->index(to+1, 0));
            m_files_list_view->selectionModel()->select(sel, QItemSelectionModel::Select | QItemSelectionModel::Rows);
        }
    }
}

void MainWindow::remove_pdf_file()
{
    QModelIndexList selected = m_files_list_view->selectionModel()->selectedRows();
    for (int i=selected.size() - 1; i >= 0; i--)
    {
        delete m_files_list_model->item(selected.at(i).row(), NAME_COLUMN)->data().value<PdfFile *>();
        m_files_list_model->removeRow(selected.at(i).row());
    }
}

void MainWindow::pdf_file_added(const QStringList &selected)
{
    for (int i=m_opened_count; i<selected.count(); i++)
    {
        //check if filename is already in the model
        PdfFile * pdf_file = NULL;

        for (int j=0; j<m_files_list_model->rowCount(); j++)
        {
            QStandardItem *item = m_files_list_model->item(j, NAME_COLUMN);
            if (item->data().value<PdfFile *>()->filename() == selected.at(i).toStdString())
                pdf_file = item->data().value<PdfFile *>();
        }

        if (pdf_file != NULL)
            pdf_file = new PdfFile(*pdf_file);
        else
            pdf_file = new PdfFile(selected.at(i).toStdString());

        //create the row
        QList<QStandardItem *> row_data;

        QStandardItem *column = new QStandardItem(selected.at(i));
        column->setEditable(false);
        column->setData(QVariant::fromValue<PdfFile *>(pdf_file));
        row_data << column;

        column = new QStandardItem(QString::number(pdf_file->page_count()));
        column->setEditable(false);
        row_data << column;

        row_data << new QStandardItem();

        column = new QStandardItem("No rotation");
        column->setData(0, Qt::UserRole);
        row_data << column;

        m_files_list_model->appendRow(row_data);
    }

    m_opened_count = selected.count();
    m_open_file_dialog->setDirectory(m_open_file_dialog->directory());
    m_dest_file_dialog->setDirectory(m_open_file_dialog->directory());

    m_files_list_view->resizeColumnsToContents();
}

void MainWindow::generate_pdf(const QString &file_selected)
{
    m_progress_bar->setValue(0);
    m_progress_bar->show();

    PoDoFo::PdfMemDocument *output_file = new PoDoFo::PdfMemDocument();

    for (int i=0; i<m_files_list_model->rowCount(); i++)
    {
        PdfFile *pdf_file = m_files_list_model->item(i, NAME_COLUMN)->data().value<PdfFile *>();

        pdf_file->set_pages_filter_from_string(
                    m_files_list_model->item(i, PAGES_FILTER_COLUMN)->data(Qt::EditRole).toString().toStdString());
        pdf_file->set_rotation(m_files_list_model->item(i, ROTATION_COLUMN)->data(Qt::UserRole).toInt());

        pdf_file->run(output_file);

        m_progress_bar->setValue((i+1)*100/(m_files_list_model->rowCount()+1));
    }

    output_file->Write(file_selected.toStdString().c_str());

    m_progress_bar->setValue(100);

    QTimer::singleShot(2000, m_progress_bar, SLOT(hide()));
}
