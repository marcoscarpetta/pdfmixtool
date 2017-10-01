cmake_minimum_required (VERSION 2.6)

project (PDFMixTool)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package (Qt5 REQUIRED COMPONENTS Widgets)

add_executable (
    pdfmixtool
    src/aboutdialog.cpp
    src/aboutdialog.h
    src/comboboxdelegate.cpp
    src/comboboxdelegate.h
    src/mainwindow.cpp
    src/mainwindow.h
    src/pdffile.cpp
    src/pdffile.h
    src/main.cpp
)

target_link_libraries (
    pdfmixtool
    podofo
    ${Qt5Widgets_LIBRARIES}
)
