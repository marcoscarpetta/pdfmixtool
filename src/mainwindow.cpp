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

#include "mainwindow.h"

#include <QApplication>
#include <QTimer>

QDataStream &operator<<(QDataStream &out, const Multipage &multipage)
{
    out << multipage.enabled;

    out << multipage.name.c_str();

    out << multipage.page_width;
    out << multipage.page_height;

    out << multipage.rows;
    out << multipage.columns;

    out << multipage.rotation;

    out << multipage.h_alignment;
    out << multipage.v_alignment;

    out << multipage.margin_left;
    out << multipage.margin_right;
    out << multipage.margin_top;
    out << multipage.margin_bottom;

    out << multipage.spacing;

    return out;
}

QDataStream &operator>>(QDataStream &in, Multipage &multipage)
{
    in >> multipage.enabled;

    char *name;
    in >> name;
    multipage.name = name;

    in >> multipage.page_width;
    in >> multipage.page_height;

    in >> multipage.rows;
    in >> multipage.columns;

    in >> multipage.rotation;

    int h_alignment;
    int v_alignment;
    in >> h_alignment;
    multipage.h_alignment = static_cast<Multipage::Alignment>(h_alignment);
    in >> v_alignment;
    multipage.v_alignment = static_cast<Multipage::Alignment>(v_alignment);

    in >> multipage.margin_left;
    in >> multipage.margin_right;
    in >> multipage.margin_top;
    in >> multipage.margin_bottom;

    in >> multipage.spacing;

    return in;
}

MainWindow::MainWindow(MouseEventFilter *filter, QWidget *parent) :
    QMainWindow(parent),
    m_pdf_editor(new PdfEditor()),
    m_settings(new QSettings(this)),
    m_add_file_button(new QPushButton(QIcon::fromTheme("list-add"), tr("Add PDF file"), this)),
    m_move_up_button(new QPushButton(QIcon::fromTheme("go-up"), tr("Move up"), this)),
    m_move_down_button(new QPushButton(QIcon::fromTheme("go-down"), tr("Move down"), this)),
    m_remove_file_button(new QPushButton(QIcon::fromTheme("list-remove"), tr("Remove file"), this)),
    m_multipage_profiles_button(new QPushButton(QIcon::fromTheme("document-properties"), tr("Multipage profiles…"), this)),
    m_about_button(new QPushButton(QIcon::fromTheme("help-about"), tr("About"), this)),
    m_generate_pdf_button(new QPushButton(QIcon::fromTheme("document-save-as"), tr("Generate PDF"), this)),
    m_output_page_count(new QLabel(this)),
    m_progress_bar(new QProgressBar(this)),
    m_files_list_view(new QListView(this)),
    m_pdfinputfile_delegate(new InputPdfFileDelegate(filter, m_custom_multipages, this)),
    m_files_list_model(new QStandardItemModel(this)),
    m_about_dialog(new AboutDialog(this))
{
    this->setWindowIcon(QIcon(QString("%1/../share/icons/hicolor/48x48/apps/pdfmixtool.png").arg(qApp->applicationDirPath())));
    this->setWindowTitle(tr("PDF Mix Tool"));
    this->restoreGeometry(m_settings->value("main_window_geometry").toByteArray());

    // Load custom multipage profiles
    qRegisterMetaTypeStreamOperators<Multipage>("Multipage");
    m_settings->beginGroup("custom_maltipage_profiles");
    for (QString key : m_settings->childKeys())
         m_custom_multipages[key.toInt()] = m_settings->value(key).value<Multipage>();
    m_settings->endGroup();

    m_multipage_profiles_manager = new MultipageProfilesManager(&m_custom_multipages, m_settings, this);

    m_progress_bar->hide();

    m_files_list_view->setWordWrap(false);
    m_files_list_view->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_files_list_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_files_list_view->setEditTriggers(QAbstractItemView::DoubleClicked |
                                       QAbstractItemView::AnyKeyPressed);
    m_files_list_view->setModel(m_files_list_model);
    m_files_list_view->setItemDelegate(m_pdfinputfile_delegate);
    m_files_list_view->setFocusPolicy(Qt::WheelFocus);
    m_files_list_view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_files_list_view->setSpacing(2);

    m_add_file_button->setDefault(true);
    m_move_up_button->setDefault(true);
    m_move_down_button->setDefault(true);
    m_remove_file_button->setDefault(true);
    m_multipage_profiles_button->setDefault(true);
    m_about_button->setDefault(true);
    m_generate_pdf_button->setAutoDefault(true);

    QWidget::setTabOrder(m_add_file_button, m_move_up_button);
    QWidget::setTabOrder(m_move_up_button, m_move_down_button);
    QWidget::setTabOrder(m_move_down_button, m_remove_file_button);
    QWidget::setTabOrder(m_remove_file_button, m_multipage_profiles_button);
    QWidget::setTabOrder(m_multipage_profiles_button, m_about_button);
    QWidget::setTabOrder(m_about_button, m_generate_pdf_button);

    QVBoxLayout *v_layout = new QVBoxLayout();
    QWidget *central_widget = new QWidget(this);
    central_widget->setLayout(v_layout);
    this->setCentralWidget(central_widget);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(m_add_file_button);
    layout->addWidget(m_move_up_button);
    layout->addWidget(m_move_down_button);
    layout->addWidget(m_remove_file_button);

    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::VLine);
    separator->setLineWidth(1);
    layout->addWidget(separator);

    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    layout->addWidget(m_multipage_profiles_button);
    layout->addWidget(m_about_button);
    v_layout->addLayout(layout);

    v_layout->addWidget(m_files_list_view);

    layout = new QHBoxLayout();
    layout->addWidget(m_output_page_count);
    layout->addWidget(m_progress_bar, 1);
    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    layout->addWidget(m_generate_pdf_button);
    v_layout->addLayout(layout);

    connect(m_add_file_button, SIGNAL(pressed()), this, SLOT(add_pdf_files()));

    connect(m_move_up_button, SIGNAL(pressed()), this, SLOT(move_up()));
    connect(m_move_down_button, SIGNAL(pressed()), this, SLOT(move_down()));
    connect(m_remove_file_button, SIGNAL(pressed()), this, SLOT(remove_pdf_file()));

    connect(m_multipage_profiles_button, SIGNAL(pressed()), m_multipage_profiles_manager, SLOT(show()));

    connect(m_about_button, SIGNAL(pressed()), m_about_dialog, SLOT(show()));

    connect(m_pdfinputfile_delegate, SIGNAL(data_edit()), this, SLOT(update_output_page_count()));

    connect(m_generate_pdf_button, SIGNAL(pressed()), this, SLOT(generate_pdf_button_pressed()));
}

void MainWindow::add_pdf_files()
{
    QStringList selected = QFileDialog::getOpenFileNames(
                            this,
                            tr("Select one or more PDF files to open"),
                            m_settings->value("open_directory", "").toString(),
                            tr("PDF files (*.pdf)"));

    for (int i=0; i<selected.count(); i++)
    {
        // Check if filename is already in the model
        InputPdfFile * pdf_file = NULL;

        for (int j=0; j<m_files_list_model->rowCount(); j++)
        {
            QStandardItem *item = m_files_list_model->item(j);
            InputPdfFile *pdf = item->data(PDF_FILE_ROLE).value<InputPdfFile *>();
            if (pdf->filename() == selected.at(i).toStdString())
                pdf_file = pdf;
        }

        if (pdf_file != NULL)
            pdf_file = m_pdf_editor->new_input_file(pdf_file);
        else
            pdf_file = m_pdf_editor->new_input_file(selected.at(i).toStdString());

        QStandardItem *item = new QStandardItem();
        item->setData(QVariant::fromValue<InputPdfFile *>(pdf_file), PDF_FILE_ROLE);
        m_files_list_model->setItem(m_files_list_model->rowCount(), item);
    }

    if (selected.size() > 0)
    {
        m_settings->setValue("open_directory", QFileInfo(selected.at(0)).dir().absolutePath());
        this->update_output_page_count();
    }
}

void MainWindow::move_up()
{
    QList<int> indexes = this->selected_indexes();

    if (indexes.size() > 0 && indexes.at(0) > 0)
    {
        QItemSelection sel;

        // Move items up
        for (int i : indexes)
        {
            QList<QStandardItem *> row = m_files_list_model->takeRow(i);
            m_files_list_model->insertRow(i - 1, row);

            sel.push_back(QItemSelectionRange(m_files_list_model->index(i - 1, 0)));
        }

        // Restore selection
        m_files_list_view->selectionModel()->select(sel, QItemSelectionModel::Select);
    }
}

void MainWindow::move_down()
{
    QList<int> indexes = this->selected_indexes();

    if (indexes.size() > 0 && indexes.back() < m_files_list_model->rowCount() - 1)
    {
        QItemSelection sel;

        /* Qt >= 5.6
        // Move items down
        for (QList<int>::reverse_iterator it = indexes.rbegin(); it != indexes.rend(); ++it)
        {
            QList<QStandardItem *> row = m_files_list_model->takeRow(*it);
            m_files_list_model->insertRow(*it + 1, row);

            sel.push_back(QItemSelectionRange(m_files_list_model->index(*it + 1, 0)));
        }*/

        for (int i = indexes.size() - 1; i >= 0; --i)
        {
            QList<QStandardItem *> row = m_files_list_model->takeRow(indexes.at(i));
            m_files_list_model->insertRow(indexes.at(i) + 1, row);

            sel.push_back(QItemSelectionRange(m_files_list_model->index(indexes.at(i) + 1, 0)));
        }

        // Restore selection
        m_files_list_view->selectionModel()->select(sel, QItemSelectionModel::Select);
    }
}

void MainWindow::remove_pdf_file()
{
    QList<int> indexes = this->selected_indexes();

    for (int i=indexes.count() - 1; i >= 0; i--)
    {
        delete m_files_list_model->item(indexes[i])->data(PDF_FILE_ROLE).value<InputPdfFile *>();
        m_files_list_model->removeRow(indexes[i]);
    }

    this->update_output_page_count();
}

void MainWindow::update_output_page_count()
{
    int output_page_count = 0;
    for (int i=0; i<m_files_list_model->rowCount(); i++)
    {
        InputPdfFile *pdf_file = m_files_list_model->item(i)->data(PDF_FILE_ROLE).value<InputPdfFile *>();
        output_page_count += pdf_file->output_page_count();
    }

    m_output_page_count->setText(tr("Output pages: %1").arg(output_page_count));
}

void MainWindow::generate_pdf_button_pressed()
{
    // Check if there is at least one input file
    if (m_files_list_model->rowCount() == 0)
    {
        QMessageBox::critical(this,
                              tr("PDF generation error"),
                              tr("You must add at least one PDF file."));
        return;
    }

    bool errors = false, warnings = false;
    QString errors_list, warnings_list;

    for (int i=0; i<m_files_list_model->rowCount(); i++)
    {
        InputPdfFile *pdf_file = m_files_list_model->item(i)->data(PDF_FILE_ROLE).value<InputPdfFile *>();

        if (pdf_file->pages_filter_errors().size() > 0)
        {
            errors = true;

            for (
                 std::vector<IntervalIssue>::const_iterator it = pdf_file->pages_filter_errors().begin();
                 it != pdf_file->pages_filter_errors().end();
                 ++it
                 )
            {
                if ((*it).name == IntervalIssue::error_invalid_char)
                    errors_list +=
                            tr("<li>Invalid character \"<b>%1</b>\" in pages filter of file \"<b>%2</b>\"</li>")
                            .arg(
                                QString::fromStdString((*it).data),
                                QString::fromStdString(pdf_file->filename()));
                else if ((*it).name == IntervalIssue::error_invalid_interval)
                    errors_list +=
                            tr("<li>Invalid interval \"<b>%1</b>\" in pages filter of file \"<b>%2</b>\"</li>")
                            .arg(
                                QString::fromStdString((*it).data),
                                QString::fromStdString(pdf_file->filename()));
                else if ((*it).name == IntervalIssue::error_page_out_of_range)
                    errors_list +=
                            tr("<li>Boundaries of interval \"<b>%1</b>\" in pages filter of file \"<b>%2</b>\" are out of allowed interval</li>")
                            .arg(
                                QString::fromStdString((*it).data),
                                QString::fromStdString(pdf_file->filename()));
            }
        }

        if (pdf_file->pages_filter_warnings().size() > 0)
        {
            warnings = true;

            for (
                 std::vector<IntervalIssue>::const_iterator it = pdf_file->pages_filter_warnings().begin();
                 it != pdf_file->pages_filter_warnings().end();
                 ++it
                 )
            {
                if ((*it).name == IntervalIssue::warning_overlapping_interval)
                    warnings_list +=
                            tr("<li>Interval \"<b>%1</b>\" in pages filter of file \"<b>%2</b>\" is overlapping with another interval</li>")
                            .arg(
                                QString::fromStdString((*it).data),
                                QString::fromStdString(pdf_file->filename()));
            }
        }
    }

    if (errors)
    {
        QString error_message(tr("<p>The PDF generation failed due to the following errors:</p>"));
        error_message += QString("<ul>") + errors_list + QString("</ul>");
        QMessageBox::critical(this,
                              tr("PDF generation error"),
                              error_message);
    }
    else if (warnings)
    {
        QString warning_message(tr("<p>The following problems were encountered while generating the PDF file:</p>"));
        warning_message += QString("<ul>") + warnings_list + QString("</ul>");
        int ret = QMessageBox::warning(this,
                              "",
                              warning_message,
                             QMessageBox::Cancel | QMessageBox::Ignore,
                             QMessageBox::Ignore);
        if (ret == QMessageBox::Ignore)
            generate_pdf();
    }
    else
        generate_pdf();
}

void MainWindow::generate_pdf()
{
    QString selected_file = QFileDialog::getSaveFileName(this, tr("Save PDF file"),
                               m_settings->value("save_directory", "").toString(),
                               tr("PDF files (*.pdf)"));

    if (! selected_file.isNull())
    {
        m_settings->setValue("save_directory", QFileInfo(selected_file).dir().absolutePath());

        m_progress_bar->setValue(0);
        m_progress_bar->show();

        OutputPdfFile *output_file = m_pdf_editor->new_output_file();

        // Write each file to the output file
        for (int i=0; i<m_files_list_model->rowCount(); i++)
        {
            InputPdfFile *pdf_file = m_files_list_model->item(i)->data(PDF_FILE_ROLE).value<InputPdfFile *>();

            pdf_file->run(output_file);

            m_progress_bar->setValue((i+1)*100/(m_files_list_model->rowCount()+1));
        }

        // Save output file on disk
        output_file->write(selected_file.toStdString());
        delete output_file;

        m_progress_bar->setValue(100);
        QTimer::singleShot(4000, m_progress_bar, SLOT(hide()));
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_settings->setValue("main_window_geometry", this->saveGeometry());

    // Save custom multipage profiles
    m_settings->beginGroup("custom_maltipage_profiles");

    for (QString key : m_settings->childKeys())
         m_settings->remove(key);

    QMap<int, Multipage>::const_iterator it;
    for (it = m_custom_multipages.constBegin(); it != m_custom_multipages.constEnd(); ++it)
        m_settings->setValue(QString::number(it.key()), QVariant::fromValue<Multipage>(it.value()));

    m_settings->endGroup();

    QMainWindow::closeEvent(event);
}

const QList<int> MainWindow::selected_indexes()
{
    QList<int> indexes;
    for(const QModelIndex &index : m_files_list_view->selectionModel()->selectedIndexes())
       indexes.append(index.row());

    qSort(indexes);

    return indexes;
}
