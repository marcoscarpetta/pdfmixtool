#ifndef INPUTPDFFILEWIDGET_H
#define INPUTPDFFILEWIDGET_H

#include <QComboBox>
#include <QLineEdit>
#include <QLabel>

#include "pdf_edit_lib/pdffile.h"

class InputPdfFileWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InputPdfFileWidget(QWidget *parent = nullptr);

    void set_data_from_pdf_input_file(InputPdfFile *pdf_file);

    void set_data_to_pdf_input_file(InputPdfFile *pdf_file);

private:
    QLineEdit *m_pages_filter_lineedit;
    QComboBox *m_multipage_combobox;
    QComboBox *m_rotation_combobox;
};

#endif // INPUTPDFFILEWIDGET_H
