/*
 * Copyright (C) 2012 Asymworks, LLC.  All Rights Reserved.
 * www.asymworks.com / info@asymworks.com
 *
 * This file is part of the Benthos Dive Log Package (benthos-log.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "configpage.hpp"
#include "pages.hpp"

using namespace wizards::addcomputer;

ConfigPage::ConfigPage(QWidget * parent)
	: QWizardPage(parent)
{
	createLayout();

	setTitle(tr("Dive Computer Settings"));
	setSubTitle(tr("Benthos successfully connected to your dive computer!  Review the settings below and click Finish to exit the wizard."));

	registerField("name", m_txtName);
}

ConfigPage::~ConfigPage()
{
}

void ConfigPage::createLayout()
{
	QLabel * lblSerNo = new QLabel(tr("Serial Number: "));
	QLabel * lblManuf = new QLabel(tr("Manufacturer: "));
	QLabel * lblModel = new QLabel(tr("Model Name: "));
	lblSerNo->setStyleSheet("QLabel { font-weight: bold }");
	lblManuf->setStyleSheet("QLabel { font-weight: bold }");
	lblModel->setStyleSheet("QLabel { font-weight: bold }");

	m_lblSerial = new QLabel;
	m_lblManuf = new QLabel;
	m_lblModel = new QLabel;

	QGridLayout * gbox1 = new QGridLayout;
	gbox1->addWidget(lblSerNo, 0, 0);
	gbox1->addWidget(lblManuf, 1, 0);
	gbox1->addWidget(lblModel, 2, 0);
	gbox1->addWidget(m_lblSerial, 0, 1);
	gbox1->addWidget(m_lblManuf, 1, 1);
	gbox1->addWidget(m_lblModel, 2, 1);

	QLabel * lblAutoDet = new QLabel(tr("Automatically-Detected Settings:"));
	QFrame * frame1 = new QFrame;
	frame1->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	frame1->setLayout(gbox1);

	m_txtName = new QLineEdit;
	QLabel * lblName = new QLabel(tr("Device Name:"));
	lblName->setBuddy(m_txtName);

	QGridLayout * gbox2 = new QGridLayout;
	gbox2->addWidget(lblName, 0, 0);
	gbox2->addWidget(m_txtName, 0, 1);

	QVBoxLayout * vbox = new QVBoxLayout;
	vbox->addWidget(lblAutoDet);
	vbox->addWidget(frame1);
	vbox->addSpacing(16);
	vbox->addLayout(gbox2);
	vbox->addStretch();

	setLayout(vbox);
}

void ConfigPage::initializePage()
{
	m_lblSerial->setText(field("serial").toString());
	m_lblManuf->setText(field("manuf").toString());
	m_lblModel->setText(field("model").toString());
}

int ConfigPage::nextId() const
{
	return ExitWizard;
}
