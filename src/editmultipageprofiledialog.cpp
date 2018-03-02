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

#include "editmultipageprofiledialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <math.h>

EditMultipageProfileDialog::EditMultipageProfileDialog(QWidget *parent) :
    QDialog(parent)
{
    this->setWindowTitle(tr("Edit multipage profile"));
    this->setModal(true);

    int i = 0;
    for (PaperSize size : paper_sizes)
    {
        m_page_size.addItem(QString::fromStdString(size.name));
        i++;
    }

    m_page_width.setSuffix(" cm");
    m_page_width.setDecimals(1);
    m_page_width.setSingleStep(0.1);
    m_page_width.setMinimum(1.0);
    m_page_width.setMaximum(1000.0);

    m_page_height.setSuffix(" cm");
    m_page_height.setDecimals(1);
    m_page_height.setSingleStep(0.1);
    m_page_height.setMinimum(1.0);
    m_page_height.setMaximum(1000.0);

    m_rows.setMinimum(1);
    m_rows.setMaximum(10);

    m_columns.setMinimum(1);
    m_columns.setMaximum(10);

    m_rotation.addItem("0°", 0);
    m_rotation.addItem("90°", 90);

    m_h_alignment.addItem(tr("Left"), Multipage::Left);
    m_h_alignment.addItem(tr("Center"), Multipage::Center);
    m_h_alignment.addItem(tr("Right"), Multipage::Right);

    m_v_alignment.addItem(tr("Top"), Multipage::Top);
    m_v_alignment.addItem(tr("Center"), Multipage::Center);
    m_v_alignment.addItem(tr("Bottom"), Multipage::Bottom);

    m_margin_left.setSuffix(" cm");
    m_margin_left.setDecimals(1);
    m_margin_left.setSingleStep(0.1);
    m_margin_left.setMaximum(1000.0);

    m_margin_right.setSuffix(" cm");
    m_margin_right.setDecimals(1);
    m_margin_right.setSingleStep(0.1);
    m_margin_right.setMaximum(1000.0);

    m_margin_top.setSuffix(" cm");
    m_margin_top.setDecimals(1);
    m_margin_top.setSingleStep(0.1);
    m_margin_top.setMaximum(1000.0);

    m_margin_bottom.setSuffix(" cm");
    m_margin_bottom.setDecimals(1);
    m_margin_bottom.setSingleStep(0.1);
    m_margin_bottom.setMaximum(1000.0);

    m_spacing.setSuffix(" cm");
    m_spacing.setDecimals(1);
    m_spacing.setSingleStep(0.1);
    m_spacing.setMaximum(1000.0);

    QGridLayout *layout = new QGridLayout();
    this->setLayout(layout);

    int row = 1;
    layout->addWidget(new QLabel(tr("Name:"), this), row, 1);
    layout->addWidget(&m_name, row++, 2, 1, 3);

    QFrame *separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setLineWidth(1);
    layout->addWidget(separator, row++, 1, 1, 4);

    layout->addWidget(new QLabel(tr("Output page size"), this), row++, 1, 1, 4, Qt::AlignCenter);

    layout->addWidget(new QLabel(tr("Standard size:"), this), row, 1);
    layout->addWidget(&m_page_size, row++, 2);

    layout->addWidget(new QLabel(tr("Custom size:"), this), row, 2);
    layout->addWidget(&m_custom_page_size, row++, 3);

    layout->addWidget(new QLabel(tr("Width:"), this), row, 1);
    layout->addWidget(&m_page_width, row, 2);

    layout->addWidget(new QLabel(tr("Height:"), this), row, 3);
    layout->addWidget(&m_page_height, row++, 4);

    separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setLineWidth(1);
    layout->addWidget(separator, row++, 1, 1, 4);

    layout->addWidget(new QLabel(tr("Pages layout"), this), row++, 1, 1, 4, Qt::AlignCenter);

    layout->addWidget(new QLabel(tr("Rows:"), this), row, 1);
    layout->addWidget(&m_rows, row, 2);

    layout->addWidget(new QLabel(tr("Columns:"), this), row, 3);
    layout->addWidget(&m_columns, row++, 4);

    layout->addWidget(new QLabel(tr("Rotation:"), this), row, 1);
    layout->addWidget(&m_rotation, row, 2);

    layout->addWidget(new QLabel(tr("Spacing:"), this), row, 3);
    layout->addWidget(&m_spacing, row++, 4);

    separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setLineWidth(1);
    layout->addWidget(separator, row++, 1, 1, 4);

    layout->addWidget(new QLabel(tr("Pages alignment"), this), row++, 1, 1, 4, Qt::AlignCenter);

    layout->addWidget(new QLabel(tr("Horizontal:"), this), row, 1);
    layout->addWidget(&m_h_alignment, row, 2);

    layout->addWidget(new QLabel(tr("Vertical:"), this), row, 3);
    layout->addWidget(&m_v_alignment, row++, 4);

    separator = new QFrame(this);
    separator->setFrameShape(QFrame::HLine);
    separator->setLineWidth(1);
    layout->addWidget(separator, row++, 1, 1, 4);

    layout->addWidget(new QLabel(tr("Margins"), this), row++, 1, 1, 4, Qt::AlignCenter);

    layout->addWidget(new QLabel(tr("Left") + ":", this), row, 1);
    layout->addWidget(&m_margin_left, row, 2);
    layout->addWidget(new QLabel(tr("Right") + ":", this), row, 3);
    layout->addWidget(&m_margin_right, row++, 4);
    layout->addWidget(new QLabel(tr("Top") + ":", this), row, 1);
    layout->addWidget(&m_margin_top, row, 2);
    layout->addWidget(new QLabel(tr("Bottom") + ":", this), row, 3);
    layout->addWidget(&m_margin_bottom, row++, 4);

    QPushButton *cancel_button = new QPushButton("Cancel", this);
    QPushButton *save_button = new QPushButton("Save", this);
    cancel_button->setDefault(true);
    save_button->setAutoDefault(true);

    layout->addItem(new QSpacerItem(20, 20), row++, 1, 1, 4);
    layout->addWidget(cancel_button, row, 3);
    layout->addWidget(save_button, row, 4);

    connect(save_button, SIGNAL(pressed()), this, SLOT(accept()));
    connect(cancel_button, SIGNAL(pressed()), this, SLOT(close()));
    connect(&m_custom_page_size, SIGNAL(toggled(bool)), this, SLOT(custom_page_size_toggled(bool)));
    connect(&m_page_size, SIGNAL(currentIndexChanged(int)), this, SLOT(page_size_changed(int)));
    connect(&m_page_width, SIGNAL(valueChanged(double)), this, SLOT(page_width_changed(double)));
    connect(&m_page_height, SIGNAL(valueChanged(double)), this, SLOT(page_height_changed(double)));
}

void EditMultipageProfileDialog::set_multipage(const Multipage &multipage)
{
    m_name.setText(QString::fromStdString(multipage.name));

    int i = 0;
    bool custom_size = true;
    for (const PaperSize &size : paper_sizes)
    {
        if ((int)(std::round(multipage.page_width * 10)) == (int)(std::round(size.width * 10))
                && (int)(std::round(multipage.page_height * 10)) == (int)(std::round(size.height * 10)))
        {
            m_page_size.setCurrentIndex(i);
            custom_size = false;
            break;
        }
        i++;
    }

    m_page_width.setValue(multipage.page_width);
    m_page_height.setValue(multipage.page_height);

    m_custom_page_size.setChecked(custom_size);
    this->custom_page_size_toggled(custom_size);

    m_rows.setValue(multipage.rows);
    m_columns.setValue(multipage.columns);
    m_rotation.setCurrentIndex(m_rotation.findData(multipage.rotation));
    m_spacing.setValue(multipage.spacing);
    m_h_alignment.setCurrentIndex(m_h_alignment.findData(multipage.h_alignment));
    m_v_alignment.setCurrentIndex(m_v_alignment.findData(multipage.v_alignment));
    m_margin_left.setValue(multipage.margin_left);
    m_margin_right.setValue(multipage.margin_right);
    m_margin_top.setValue(multipage.margin_top);
    m_margin_bottom.setValue(multipage.margin_bottom);
}

Multipage EditMultipageProfileDialog::get_multipage()
{
    return {
        true,
        m_name.text().toStdString(),
        m_page_width.value(), m_page_height.value(),
        m_rows.value(), m_columns.value(),
        m_rotation.currentData().toInt(),
        static_cast<Multipage::Alignment>(m_h_alignment.currentData().toInt()),
        static_cast<Multipage::Alignment>(m_v_alignment.currentData().toInt()),
        m_margin_left.value(), m_margin_right.value(),
        m_margin_top.value(), m_margin_bottom.value(),
        m_spacing.value()
    };
}

void EditMultipageProfileDialog::set_index(int index)
{
    m_index = index;
}

int EditMultipageProfileDialog::get_index()
{
    return m_index;
}

void EditMultipageProfileDialog::custom_page_size_toggled(bool toggled)
{
    if (toggled)
    {
        m_page_size.setDisabled(true);
        m_page_width.setDisabled(false);
        m_page_height.setDisabled(false);
    }
    else
    {
        this->page_size_changed(m_page_size.currentIndex());
        m_page_size.setDisabled(false);
        m_page_width.setDisabled(true);
        m_page_height.setDisabled(true);
    }
}

void EditMultipageProfileDialog::page_size_changed(int index)
{
    m_page_width.setValue(paper_sizes[index].width);
    m_page_height.setValue(paper_sizes[index].height);
}

void EditMultipageProfileDialog::page_width_changed(double value)
{
    m_margin_left.setMaximum(value / 2);
    m_margin_right.setMaximum(value / 2);
    m_spacing.setMaximum(value);
}

void EditMultipageProfileDialog::page_height_changed(double value)
{
    m_margin_top.setMaximum(value / 2);
    m_margin_bottom.setMaximum(value / 2);
    m_spacing.setMaximum(value);
}
