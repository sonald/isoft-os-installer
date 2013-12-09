/*
 * wizardpage_mach_type.h: This file is part of installer.
 *
 * Copyright (C) 2008 yetist <wuxiaotian@redflag-linux.com>
 *
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * */

#ifndef __WIZARDPAGE_MACH_TYPE_H__ 
#define __WIZARDPAGE_MACH_TYPE_H__  1

#include <QWizardPage>
#include "installer_global.h"

class QLabel;
class QRadioButton;
class QString;
class QVBoxLayout;

class WizardPage_Mach_Type: public QWizardPage 
{
    Q_OBJECT
    Q_PROPERTY(QString m_machine_type READ get_mach_type WRITE set_mach_type NOTIFY type_changed())
 public:
    WizardPage_Mach_Type(QWidget *parent =0);
    void initializePage();
    int nextId() const;
    bool validatePage();

    void set_mach_type(const QString& type);
    const QString& get_mach_type() const;

 signals:
    void type_changed();

 private:
    QString		m_machine_type;

    QRadioButton 	*m_ls2e_box_button;
    QLabel		*m_ls2e_box_label;
    QRadioButton 	*m_ls2f_box_button;
    QLabel		*m_ls2f_box_label;
    QRadioButton 	*m_ls2f_note_button;
    QLabel		*m_ls2f_note_label;
    QRadioButton 	*m_ls2f_pc_button;
    QLabel		*m_ls2f_pc_label;
    QVBoxLayout		*m_layout;
};

#endif /* __WIZARDPAGE_MACH_TYPE_H__ */
