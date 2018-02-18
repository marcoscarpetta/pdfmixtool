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

#include "inputpdffilewidget.h"

#include <QGridLayout>

InputPdfFileWidget::InputPdfFileWidget(QWidget *parent) :
    QWidget(parent),
    m_pages_filter_lineedit(new QLineEdit(this)),
    m_multipage_combobox(new QComboBox(this)),
    m_rotation_combobox(new QComboBox(this))
{
    this->setBackgroundRole(QPalette::AlternateBase);
    this->setAutoFillBackground(true);

    QGridLayout *layout = new QGridLayout();
    this->setLayout(layout);

    m_multipage_combobox->addItem(tr("Disabled"), -1);
    int i = 0;
    for (const NupSettings &ns : nup_settings_defaults)
    {
        m_multipage_combobox->addItem(QString::fromStdString(ns.name), i);
        i++;
    }

    m_rotation_combobox->addItem(tr("No rotation"), 0);
    m_rotation_combobox->addItem(tr("90°"), 90);
    m_rotation_combobox->addItem(tr("180°"), 180);
    m_rotation_combobox->addItem(tr("270°"), 270);

    layout->addWidget(new QLabel(tr("Pages:"), this), 1, 1);
    layout->addWidget(m_pages_filter_lineedit, 1, 2);
    layout->addWidget(new QLabel(tr("Multipage:"), this), 1, 3);
    layout->addWidget(m_multipage_combobox, 1, 4);
    layout->addWidget(new QLabel(tr("Rotation:"), this), 2, 1);
    layout->addWidget(m_rotation_combobox, 2, 2);

    layout->addItem(new QSpacerItem(0, 0), 1, 5);
    layout->setColumnStretch(2, 10);
    layout->setColumnStretch(4, 10);
    layout->setColumnStretch(5, 90);
}

void InputPdfFileWidget::set_data_from_pdf_input_file(InputPdfFile *pdf_file)
{
    m_pages_filter_lineedit->setText(QString::fromStdString(pdf_file->pages_filter_string()));

    m_multipage_combobox->setCurrentIndex(m_multipage_combobox->findData(pdf_file->default_nup_settings()));

    m_rotation_combobox->setCurrentIndex(m_rotation_combobox->findData(pdf_file->rotation()));
}

void InputPdfFileWidget::set_data_to_pdf_input_file(InputPdfFile *pdf_file)
{
    pdf_file->set_pages_filter_from_string(m_pages_filter_lineedit->text().toStdString());

    pdf_file->set_default_nup_settings(m_multipage_combobox->currentData().toInt());

    pdf_file->set_rotation(m_rotation_combobox->currentData().toInt());
}
