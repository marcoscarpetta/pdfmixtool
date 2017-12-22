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
    src/comboboxdelegate.cpp
    src/comboboxdelegate.h
    src/mainwindow.cpp
    src/mainwindow.h
    src/pdf_edit_lib/definitions.h
    src/pdf_edit_lib/pdfeditor.cpp
    src/pdf_edit_lib/pdfeditor.h
    src/pdf_edit_lib/pdffile.cpp
    src/pdf_edit_lib/pdffile.h
    src/pdf_edit_lib/pdffile_podofo.cpp
    src/pdf_edit_lib/pdffile_podofo.h
)

SET (
    TS_FILES
    languages/pdfmixtool_en.ts
    languages/pdfmixtool_it.ts
)

qt5_create_translation(QM_FILES ${SRCS} ${TS_FILES})

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

add_custom_command (
    OUTPUT ${CMAKE_BINARY_DIR}/changelog.html
    DEPENDS ${CMAKE_SOURCE_DIR}/CHANGELOG.md
    COMMAND markdown ${CMAKE_SOURCE_DIR}/CHANGELOG.md -o ${CMAKE_BINARY_DIR}/changelog.html
)

add_custom_target(
    changelog ALL
    DEPENDS ${CMAKE_BINARY_DIR}/changelog.html
)

install(TARGETS pdfmixtool RUNTIME DESTINATION bin)
install(FILES resources/icon.svg DESTINATION share/icons/hicolor/scalable/apps RENAME pdfmixtool.svg)
install(FILES resources/icon_48.png DESTINATION share/icons/hicolor/48x48/apps RENAME pdfmixtool.png)
install(FILES resources/icon_64.png DESTINATION share/icons/hicolor/64x64/apps RENAME pdfmixtool.png)
install(FILES resources/icon_128.png DESTINATION share/icons/hicolor/128x128/apps RENAME pdfmixtool.png)
install(FILES resources/icon_256.png DESTINATION share/icons/hicolor/256x256/apps RENAME pdfmixtool.png)
install(FILES ${QM_FILES} DESTINATION share/pdfmixtool/translations)
install(FILES ${CMAKE_BINARY_DIR}/changelog.html DESTINATION share/pdfmixtool)
install(FILES pdfmixtool.desktop DESTINATION share/applications)

add_custom_command(
    TARGET pdfmixtool
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR} -P cmake_install.cmake
)
