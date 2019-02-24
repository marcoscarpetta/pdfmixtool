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

#include "multipageprofilesmanager.h"

#include <QBoxLayout>
#include <QMessageBox>

MultipageProfilesManager::MultipageProfilesManager(QMap<int, Multipage> *custom_multipages,
        QSettings *app_settings,
        QWidget *parent) :
    QMainWindow(parent),
    m_edit_dialog(new EditMultipageProfileDialog(this)),
    m_custom_multipages(custom_multipages),
    m_settings(app_settings),
    m_new_profile_button(new QPushButton(QIcon::fromTheme("list-add"), tr("New profile…"), this)),
    m_delete_profile_button(new QPushButton(QIcon::fromTheme("list-remove"), tr("Delete profile"), this)),
    m_profiles_view(new QListView(this)),
    m_profiles_model(new QStandardItemModel(this))
{
    this->restoreGeometry(m_settings->value("multipage_profiles_manager_geometry").toByteArray());
    this->setWindowTitle(tr("Manage multipage profiles"));

    m_profiles_view->setWordWrap(false);
    m_profiles_view->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_profiles_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_profiles_view->setEditTriggers(QAbstractItemView::DoubleClicked);
    m_profiles_view->setModel(m_profiles_model);
    m_profiles_view->setFocusPolicy(Qt::ClickFocus);
    m_profiles_view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    QMap<int, Multipage>::const_iterator it;
    for (it = m_custom_multipages->constBegin(); it != m_custom_multipages->constEnd(); ++it)
    {
        QStandardItem *item = new QStandardItem(QString::fromStdString(it.value().name));
        item->setData(it.key(), Qt::UserRole);
        m_profiles_model->appendRow(item);
    }

    m_delete_profile_button->setDefault(true);
    m_new_profile_button->setAutoDefault(true);

    QVBoxLayout *v_layout = new QVBoxLayout();
    QWidget *central_widget = new QWidget(this);
    central_widget->setLayout(v_layout);
    this->setCentralWidget(central_widget);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(m_new_profile_button);
    layout->addWidget(m_delete_profile_button);

    v_layout->addLayout(layout);

    v_layout->addWidget(m_profiles_view);

    connect(m_new_profile_button, SIGNAL(pressed()), this, SLOT(new_profile_button_pressed()));

    connect(m_delete_profile_button, SIGNAL(pressed()), this, SLOT(delete_profile_button_pressed()));

    connect(m_profiles_view, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(profile_double_clicked(QModelIndex)));

    connect(m_edit_dialog, SIGNAL(accepted()), this, SLOT(edit_dialog_accepted()));
}

bool MultipageProfilesManager::profile_name_exists(const QString &name)
{
    QMap<int, Multipage>::const_iterator it;
    for (it = m_custom_multipages->constBegin(); it != m_custom_multipages->constEnd(); ++it)
        if (QString::fromStdString(it.value().name) == name)
            return true;

    return false;
}

void MultipageProfilesManager::new_profile_button_pressed()
{
    int i = 0;
    QString name = tr("Custom profile") + QString(" %1").arg(i);
    while (this->profile_name_exists(name))
        name = tr("Custom profile") + QString(" %1").arg(++i);

    i = 100;
    QMap<int, Multipage>::const_iterator it;
    for (it = m_custom_multipages->constBegin(); it != m_custom_multipages->constEnd(); ++it)
        if (it.key() == i)
            ++i;

    Multipage multipage = {
        true,
        name.toStdString(),
        21.0, 29.7,
        1, 1, 0,
        Multipage::Center, Multipage::Center,
        1, 1, 1, 1, 1
    };

    m_edit_dialog->set_multipage(multipage);
    m_edit_dialog->set_index(i);
    m_edit_dialog->show();
}

void MultipageProfilesManager::delete_profile_button_pressed()
{
    if (m_profiles_view->selectionModel()->selectedIndexes().size() > 0)
    {
        m_custom_multipages->remove(m_profiles_view->selectionModel()->
                                    selectedIndexes().at(0).data(Qt::UserRole).toInt());
        m_profiles_model->removeRow(m_profiles_view->selectionModel()->selectedIndexes().at(0).row());
    }
}

void MultipageProfilesManager::profile_double_clicked(const QModelIndex &index)
{
    m_edit_dialog->set_multipage(m_custom_multipages->value(index.data(Qt::UserRole).toInt()));
    m_edit_dialog->set_index(index.data(Qt::UserRole).toInt());
    m_edit_dialog->show();
}

void MultipageProfilesManager::edit_dialog_accepted()
{
    Multipage multipage = m_edit_dialog->get_multipage();

    // Check profile name
    if (multipage.name.size() == 0)
    {
        QMessageBox::critical(this, tr("Error"), tr("Profile name can not be empty."));
        m_edit_dialog->show();
        return;
    }

    if (multipage.name == tr("Disabled").toStdString())
    {
        QMessageBox::critical(this, tr("Error"), tr("Profile name already exists."));
        m_edit_dialog->show();
        return;
    }

    for (const Multipage &m : multipage_defaults)
        if (multipage.name == m.name)
        {
            QMessageBox::critical(this, tr("Error"), tr("Profile name already exists."));
            m_edit_dialog->show();
            return;
        }

    QMap<int, Multipage>::const_iterator it;
    for (it = m_custom_multipages->constBegin(); it != m_custom_multipages->constEnd(); ++it)
        if (multipage.name == it.value().name && m_edit_dialog->get_index() != it.key())
        {
            QMessageBox::critical(this, tr("Error"), tr("Profile name already exists."));
            m_edit_dialog->show();
            return;
        }

    m_custom_multipages->insert(m_edit_dialog->get_index(), multipage);

    // Update model
    for (int i = 0; i < m_profiles_model->rowCount(); i++)
    {
        QStandardItem *item = m_profiles_model->item(i);
        if (item->data(Qt::UserRole).toInt() == m_edit_dialog->get_index())
        {
            item->setText(QString::fromStdString(multipage.name));
            return;
        }
    }

    QStandardItem *item = new QStandardItem(QString::fromStdString(multipage.name));
    item->setData(m_edit_dialog->get_index(), Qt::UserRole);
    m_profiles_model->appendRow(item);
}

void MultipageProfilesManager::show()
{
    this->restoreGeometry(m_settings->value("multipage_profiles_manager_geometry").toByteArray());
    QMainWindow::show();
}

void MultipageProfilesManager::closeEvent(QCloseEvent *event)
{
    m_settings->setValue("multipage_profiles_manager_geometry", this->saveGeometry());
    QMainWindow::closeEvent(event);
}
