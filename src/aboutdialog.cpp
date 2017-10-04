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

#include "aboutdialog.h"

#include <QApplication>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>

AboutDialog::AboutDialog(QWidget *parent) :
    QMainWindow(parent)
{
    this->setWindowIcon(QIcon(QString("%1/../share/icons/hicolor/48x48/apps/pdfmixtool.png").arg(qApp->applicationDirPath())));
    this->setWindowTitle(tr("About PDF Mix Tool"));

    QWidget *central_widget = new QWidget(this);
    QGridLayout *grid_layout = new QGridLayout(central_widget);
    grid_layout->setVerticalSpacing(20);

    central_widget->setContentsMargins(20, 20, 20, 20);
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

    QLabel *application_description = new QLabel(this);
    application_description->setWordWrap(true);
    application_description->setText(tr("An application to split, merge, rotate and mix PDF files."));
    application_description->setTextInteractionFlags(Qt::TextSelectableByMouse);
    application_description->setAlignment(Qt::AlignCenter);

    QLabel *license = new QLabel(this);
    license->setText("PDF Mix Tool is free software: you can redistribute it and/or modify\n"
                     "it under the terms of the GNU General Public License as published by\n"
                     "the Free Software Foundation, either version 3 of the License, or\n"
                     "(at your option) any later version.\n\n"
                     "PDF Mix Tool is distributed in the hope that it will be useful,\n"
                     "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                     "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n"
                     "GNU General Public License for more details.");
    license->setTextInteractionFlags(Qt::TextSelectableByMouse);
    license->setAlignment(Qt::AlignCenter);

    QLabel *website = new QLabel(this);
    website->setText(QString("<a href=\"http://www.scarpetta.eu/page/pdf-mix-tool\">%1</a>").arg(tr("Website")));
    website->setTextFormat(Qt::RichText);
    website->setTextInteractionFlags(Qt::TextBrowserInteraction);
    website->setOpenExternalLinks(true);
    website->setAlignment(Qt::AlignCenter);

    QLabel *copyright = new QLabel("<small>Copyright © 2017 Marco Scarpetta</small>", this);
    copyright->setTextInteractionFlags(Qt::TextSelectableByMouse);
    copyright->setAlignment(Qt::AlignCenter);

    grid_layout->addWidget(application_icon, 1, 1);
    grid_layout->addWidget(application_name, 1, 2, 1, 2);
    grid_layout->addWidget(application_description, 2, 1, 1, 3);
    grid_layout->addWidget(license, 3, 1, 1, 3);
    grid_layout->addWidget(website, 4, 1, 1, 3);
    grid_layout->addWidget(copyright, 5, 1, 1, 3);

    grid_layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 6, 1, 1, 2);
    grid_layout->addWidget(close_button, 6, 3);
}
