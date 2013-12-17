/*
 * wizardpage_mach_type.c: This file is part of installer.
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

#include <QDebug>
#include <QLabel>
#include <QRadioButton>
#include <QString>
#include <QVariant>
#include <QVBoxLayout>
#include <cassert>
#include "wizardpage_mach_type.h"

WizardPage_Mach_Type::WizardPage_Mach_Type(QWidget *parent)
: QWizardPage(parent)
{
	// set the GUI elements.
	//m_ls2e_box_button = new QRadioButton(this);
	//m_ls2e_box_label = new QLabel(this);
	m_ls2f_box_button = new QRadioButton(this);
	m_ls2f_box_label = new QLabel(this);
	m_ls2f_note_button = new QRadioButton(this);
	m_ls2f_note_label = new QLabel(this);
	m_ls2f_pc_button = new QRadioButton(this);
	m_ls2f_pc_label = new QLabel(this);

	m_layout = new QVBoxLayout(this);

	//m_ls2e_box_label->setWordWrap(true);
	m_ls2f_box_label->setWordWrap(true);
	m_ls2f_note_label->setWordWrap(true);
	m_ls2f_pc_label->setWordWrap(true);

	//m_ls2e_box_label->setAlignment(Qt::AlignLeft);
	m_ls2f_box_label->setAlignment(Qt::AlignLeft);
	m_ls2f_note_label->setAlignment(Qt::AlignLeft);
	m_ls2f_pc_label->setAlignment(Qt::AlignLeft);

	const int indent =50;
	//m_ls2e_box_label->setIndent(indent);
	m_ls2f_box_label->setIndent(indent);
	m_ls2f_note_label->setIndent(indent);
	m_ls2f_pc_label->setIndent(indent);

	//m_ls2e_box_button->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );
	m_ls2f_box_button->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );
	m_ls2f_note_button->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );
	m_ls2f_pc_button->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Fixed );

	//m_ls2e_box_label->setTextFormat(Qt::RichText);
	m_ls2f_box_label->setTextFormat(Qt::RichText);
	m_ls2f_note_label->setTextFormat(Qt::RichText);
	m_ls2f_pc_label->setTextFormat(Qt::RichText);

	//m_layout->addWidget(m_ls2e_box_button);
	//m_layout->addWidget(m_ls2e_box_label);
	m_layout->addWidget(m_ls2f_box_button);
	m_layout->addWidget(m_ls2f_box_label);
	m_layout->addWidget(m_ls2f_note_button);
	m_layout->addWidget(m_ls2f_note_label);
	m_layout->addWidget(m_ls2f_pc_button);
	m_layout->addWidget(m_ls2f_pc_label);

	registerField( "m_machine_type", this, "m_machine_type", SIGNAL(type_changed()));

}

void WizardPage_Mach_Type::initializePage()
{
	m_ls2f_box_button->setChecked(true);
	m_machine_type = "ls2f_box";
	setField( "m_machine_type", m_machine_type);

	setTitle(tr("Machine Type") );
	setSubTitle(tr("Please select your hardware type:") );

	//m_ls2e_box_button->setText( tr("Loongson 2E Box") );
	m_ls2f_box_button->setText( tr("Loongson 2F Box") );
	m_ls2f_note_button->setText( tr("Loongson Notebook") );
	m_ls2f_pc_button->setText( tr("Loongson PC") );

	//m_ls2e_box_label->setText(tr("Machine with Loongson 2E cpu, like Fuloong 2E Box."));

	m_ls2f_box_label->setText(tr("Machine with Loongson 2F cpu, like Fuloong 2F Box."));

	m_ls2f_note_label->setText(tr("Notebook with Loongson cpu, like 8.9 inch YeeLoong Notebook"));
	m_ls2f_pc_label->setText(tr("PC with Loongson cpu"));

}

int WizardPage_Mach_Type::nextId() const
{
	return Page_Partition_Advanced;
}

void WizardPage_Mach_Type::set_mach_type(const QString& type)
{
	if ( m_machine_type != type) 
	{
		m_machine_type = type;
		emit type_changed();
	}
}

const QString& WizardPage_Mach_Type::get_mach_type() const
{
	return m_machine_type;
}

bool WizardPage_Mach_Type::validatePage()
{
	qDebug() << "Mode ValidatePage.";

	//if ( m_ls2e_box_button->isChecked() )
	//	set_mach_type("ls2e_box");
	if ( m_ls2f_box_button->isChecked() )
		set_mach_type("ls2f_box");
	else if ( m_ls2f_pc_button->isChecked() )
		set_mach_type("ls2f_pc");
	else 
		set_mach_type("ls2f_notebook");

	g_engine->cmdSetMachine(m_machine_type.toLatin1());
	return true;
}
