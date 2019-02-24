/* Copyright (C) 2017-2019 Marco Scarpetta
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

#include "editpdfentrydialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <math.h>

EditPdfEntryDialog::EditPdfEntryDialog(const QMap<int, Multipage> &custom_multipages,
                                       const QList<InputPdfFile *> &files,
                                       QWidget *parent) :
    QDialog(parent),
    m_custom_multipages(custom_multipages),
    m_input_pdf_files(files)
{
    this->setWindowTitle(tr("Edit PDF files' properties"));
    this->setModal(true);

    m_rotation_combobox.addItem(tr("No rotation"), 0);
    m_rotation_combobox.addItem("90°", 90);
    m_rotation_combobox.addItem("180°", 180);
    m_rotation_combobox.addItem("270°", 270);

    m_multipage_combobox.addItem(tr("Disabled"), 0);
    int i = 0;
    for (const Multipage &multipage : multipage_defaults)
    {
        if (i != 0)
            m_multipage_combobox.addItem(QString::fromStdString(multipage.name), i);
        i++;
    }

    // Set current rotation
    int rotation = m_input_pdf_files[0]->rotation();
    bool is_the_same = true;
    for (int i = 1; i < m_input_pdf_files.count(); i++)
    {
        if (m_input_pdf_files[i]->rotation() != rotation)
        {
            is_the_same = false;
            break;
        }
    }
    if (is_the_same)
        m_rotation_combobox.setCurrentIndex(m_rotation_combobox.findData(rotation));
    else
        m_rotation_combobox.setCurrentIndex(-1);

    // Set current multipage profile
    std::string profile_name = m_input_pdf_files[0]->multipage().name;
    is_the_same = true;
    for (int i = 1; i < m_input_pdf_files.count(); i++)
    {
        if (m_input_pdf_files[i]->multipage().name != profile_name)
        {
            is_the_same = false;
            break;
        }
    }
    if (is_the_same)
    {
        int profile = m_multipage_combobox.findText(QString::fromStdString(m_input_pdf_files[0]->multipage().name));
        if (profile < 0)
            profile = 0;
        m_multipage_combobox.setCurrentIndex(profile);
    }
    else
        m_multipage_combobox.setCurrentIndex(-1);

    QPushButton *ok_button = new QPushButton(QIcon::fromTheme("dialog-ok-apply"), tr("OK"), this);
    ok_button->setDefault(true);

    QPushButton *cancel_button = new QPushButton(QIcon::fromTheme("dialog-cancel"), tr("Cancel"), this);

    QMap<int, Multipage>::const_iterator it;
    for (it = m_custom_multipages.constBegin(); it != m_custom_multipages.constEnd(); ++it)
        m_multipage_combobox.addItem(QString::fromStdString(it.value().name), it.key());

    QGridLayout *layout = new QGridLayout();
    this->setLayout(layout);

    int row = 1;
    layout->addWidget(new QLabel(tr("Multipage:"), this), row, 1);
    layout->addWidget(&m_multipage_combobox, row++, 2);
    layout->addWidget(new QLabel(tr("Rotation:"), this), row, 1);
    layout->addWidget(&m_rotation_combobox, row++, 2);

    QHBoxLayout *h_layout = new QHBoxLayout();
    layout->addLayout(h_layout, row, 1, 1, 2);

    h_layout->addWidget(ok_button);
    h_layout->addWidget(cancel_button);

    connect(cancel_button, SIGNAL(pressed()), this, SLOT(close()));
    connect(ok_button, SIGNAL(pressed()), this, SLOT(accepted()));
}

void EditPdfEntryDialog::accepted()
{
    if (m_rotation_combobox.currentData() != QVariant())
    {
        for (int i = 0; i < m_input_pdf_files.count(); i++)
            m_input_pdf_files[i]->set_rotation(m_rotation_combobox.currentData().toInt());
    }

    if (m_multipage_combobox.currentData() != QVariant())
    {
        for (int i = 0; i < m_input_pdf_files.count(); i++)
            if (m_multipage_combobox.currentData().toInt() < 100)
                m_input_pdf_files[i]->set_multipage(multipage_defaults[m_multipage_combobox.currentData().toInt()]);
            else
                m_input_pdf_files[i]->set_multipage(m_custom_multipages[m_multipage_combobox.currentData().toInt()]);
    }

    this->close();
}
