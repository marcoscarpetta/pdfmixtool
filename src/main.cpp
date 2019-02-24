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

#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include "mainwindow.h"
#include "mouseeventfilter.h"

int main(int argc, char *argv[])
{
    // Set application informations
    QApplication app (argc, argv);

    app.setApplicationName("pdfmixtool");
    app.setApplicationDisplayName("PDF Mix Tool");
    app.setOrganizationDomain("scarpetta.eu");
    app.setOrganizationName("PDFMixTool");
    app.setApplicationVersion("0.3.5");

#if QT_VERSION >= 0x050700
    app.setDesktopFileName("eu.scarpetta.PDFMixTool");
#endif

    // Set up translations
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
            QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QTranslator appTranslator;
    if (appTranslator.load(QString("%1/../share/pdfmixtool/translations/pdfmixtool_%2.qm").arg(
                               qApp->applicationDirPath(),  QLocale::system().name())))
        app.installTranslator(&appTranslator);

    // Event filter
    MouseEventFilter *filter = new MouseEventFilter(&app);
    app.installEventFilter(filter);

    // Create and show the main window
    MainWindow *main_window = new MainWindow(filter);
    main_window->show();

    return app.exec();
}
