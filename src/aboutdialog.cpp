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

#include "aboutdialog.h"

#include <QApplication>
#include <QGridLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QFile>
#include <QScrollArea>

AboutDialog::AboutDialog(QWidget *parent) :
    QMainWindow(parent)
{
    // Dialog header
    this->setWindowIcon(QIcon(QString("%1/../share/icons/hicolor/48x48/apps/pdfmixtool.png").arg(qApp->applicationDirPath())));
    this->setWindowTitle(tr("About PDF Mix Tool"));

    QWidget *central_widget = new QWidget(this);
    QGridLayout *grid_layout = new QGridLayout(central_widget);
    grid_layout->setVerticalSpacing(20);

    central_widget->setContentsMargins(5, 5, 5, 5);
    central_widget->setLayout(grid_layout);
    this->setCentralWidget(central_widget);

    QPushButton *close_button = new QPushButton(QIcon::fromTheme("window-close"), tr("Close"), this);
    close_button->setAutoDefault(true);
    connect(close_button, SIGNAL(pressed()), this, SLOT(close()));

    QLabel *application_icon = new QLabel(this);
    application_icon->setPixmap(QPixmap(QString("%1/../share/icons/hicolor/128x128/apps/pdfmixtool.png").arg(qApp->applicationDirPath())));
    application_icon->setAlignment(Qt::AlignCenter);

    QLabel *application_name = new QLabel(
                "<h3><br>" + qApp->applicationDisplayName() + "</h3>" +
                tr("Version %1").arg(qApp->applicationVersion()),
                this);
    application_name->setTextInteractionFlags(Qt::TextSelectableByMouse);
    application_name->setAlignment(Qt::AlignTop);

    QTabWidget *tab_widget = new QTabWidget(this);
    QString style("background-color: white; padding: 10px");

    // Dialog informations tab
    QLabel *info_label = new QLabel();
    info_label->setWordWrap(true);
    info_label->setText(
                QString("<p>%1</p>").arg(tr("An application to split, merge, rotate and mix PDF files.")) +
                QString("<p><a href=\"http://www.scarpetta.eu/page/pdf-mix-tool\">%1</a></p>").arg(tr("Website")) +
                "<p><small>Copyright © 2017-2018 Marco Scarpetta</small></p>");
    info_label->setTextInteractionFlags(Qt::TextBrowserInteraction);
    info_label->setStyleSheet(style);
    info_label->setOpenExternalLinks(true);
    info_label->setAlignment(Qt::AlignCenter);

    tab_widget->addTab(info_label, tr("About"));

    // Dialog license tab
    QLabel *license = new QLabel();
    license->setText("<p>PDF Mix Tool is free software: you can redistribute it and/or modify<br>"
                     "it under the terms of the GNU General Public License as published by<br>"
                     "the Free Software Foundation, either version 3 of the License, or<br>"
                     "(at your option) any later version.</p>"
                     "<p>PDF Mix Tool is distributed in the hope that it will be useful,<br>"
                     "but WITHOUT ANY WARRANTY; without even the implied warranty of<br>"
                     "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the<br>"
                     "<a href=\"https://www.gnu.org/licenses/gpl-3.0.en.html\">"
                     "GNU General Public License</a> for more details.</p>");
    license->setTextInteractionFlags(Qt::TextBrowserInteraction);
    license->setStyleSheet(style);
    license->setOpenExternalLinks(true);
    license->setAlignment(Qt::AlignCenter);

    tab_widget->addTab(license, tr("License"));

    // Dialog changelog tab
    QScrollArea *scroll_area = new QScrollArea();
    scroll_area->setBackgroundRole(QPalette::Light);

    QFile file(QString("%1/../share/pdfmixtool/changelog.html").arg(qApp->applicationDirPath()));
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QLabel *changelog = new QLabel(this);
    changelog->setText(QString::fromStdString(file.readAll().toStdString()));
    changelog->setTextInteractionFlags(Qt::TextSelectableByMouse);
    changelog->setStyleSheet(style);

    scroll_area->setWidget(changelog);

    tab_widget->addTab(scroll_area, tr("Changelog"));

    grid_layout->addWidget(application_icon, 1, 1);
    grid_layout->addWidget(application_name, 1, 2);

    grid_layout->addWidget(tab_widget, 2, 1, 1, 4);

    grid_layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 3, 3);
    grid_layout->addWidget(close_button, 3, 4);
}
