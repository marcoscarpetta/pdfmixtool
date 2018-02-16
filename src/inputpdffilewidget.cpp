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

    m_multipage_combobox->addItem(tr("Disabled"), 0);
    m_multipage_combobox->addItem(tr("2x1, A4 portrait, 0°"), 1);
    m_multipage_combobox->addItem(tr("2x1, A4 landscape, 90°"), 2);
    m_multipage_combobox->addItem(tr("2x2, A4 portrait, 0°"), 3);
    m_multipage_combobox->addItem(tr("2x2, A4 landscape, 90°"), 4);

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

    m_rotation_combobox->setCurrentIndex(m_rotation_combobox->findData(pdf_file->rotation()));
}

void InputPdfFileWidget::set_data_to_pdf_input_file(InputPdfFile *pdf_file)
{
    pdf_file->set_pages_filter_from_string(m_pages_filter_lineedit->text().toStdString());

    pdf_file->set_rotation(m_rotation_combobox->currentData().toInt());
}
