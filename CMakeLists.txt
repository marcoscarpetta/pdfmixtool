cmake_minimum_required (VERSION 3.1)

project (PDFMixTool)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package (Qt5 REQUIRED COMPONENTS Widgets LinguistTools)

SET (
    SRCS
    src/main.cpp
    src/aboutdialog.cpp
    src/aboutdialog.h
    src/editmultipageprofiledialog.cpp
    src/editmultipageprofiledialog.h
    src/inputpdffiledelegate.cpp
    src/inputpdffiledelegate.h
    src/inputpdffilewidget.cpp
    src/inputpdffilewidget.h
    src/mainwindow.cpp
    src/mainwindow.h
    src/mouseeventfilter.cpp
    src/mouseeventfilter.h
    src/multipageprofilesmanager.cpp
    src/multipageprofilesmanager.h
    src/pdf_edit_lib/definitions.h
    src/pdf_edit_lib/pdfeditor.cpp
    src/pdf_edit_lib/pdfeditor.h
    src/pdf_edit_lib/pdffile.cpp
    src/pdf_edit_lib/pdffile.h
    src/pdf_edit_lib/podofo_pdffile.cpp
    src/pdf_edit_lib/podofo_pdffile.h
    src/pdf_edit_lib/podofo_pdftranslator.cpp
    src/pdf_edit_lib/podofo_pdftranslator.h
)

file(GLOB TS_FILES
     LIST_DIRECTORIES false
     languages/pdfmixtool_*.ts
)

qt5_create_translation(QM_FILES src/ ${TS_FILES} OPTIONS -no-obsolete)

add_executable (
    pdfmixtool
    ${SRCS}
    ${QM_FILES}
)

target_link_libraries (
    pdfmixtool
    podofo
    ${Qt5Widgets_LIBRARIES}
)

install(TARGETS pdfmixtool RUNTIME DESTINATION bin)
install(FILES resources/icon.svg DESTINATION share/icons/hicolor/scalable/apps RENAME pdfmixtool.svg)
install(FILES resources/icon_48.png DESTINATION share/icons/hicolor/48x48/apps RENAME pdfmixtool.png)
install(FILES resources/icon_64.png DESTINATION share/icons/hicolor/64x64/apps RENAME pdfmixtool.png)
install(FILES resources/icon_128.png DESTINATION share/icons/hicolor/128x128/apps RENAME pdfmixtool.png)
install(FILES resources/icon_256.png DESTINATION share/icons/hicolor/256x256/apps RENAME pdfmixtool.png)
install(FILES ${QM_FILES} DESTINATION share/pdfmixtool/translations)
install(FILES CHANGELOG.md DESTINATION share/pdfmixtool)
install(FILES AUTHORS.md DESTINATION share/pdfmixtool)
install(FILES TRANSLATORS.md DESTINATION share/pdfmixtool)
install(FILES pdfmixtool.desktop DESTINATION share/applications)

add_custom_command(
    TARGET pdfmixtool
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR} -P cmake_install.cmake
)
