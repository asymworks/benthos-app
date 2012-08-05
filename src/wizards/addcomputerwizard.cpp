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

#include "addcomputerwizard.hpp"
#include "addcomputer/configpage.hpp"
#include "addcomputer/intropage.hpp"
#include "addcomputer/pages.hpp"

using namespace wizards;
using namespace wizards::addcomputer;

AddComputerWizard::AddComputerWizard(QWidget * parent)
	: QWizard(parent)
{
	setDefaultProperty("WizardComboBox", "currentItemData", "currentItemDataChanged");

	setWindowTitle(tr("Add Dive Computer Wizard"));

	setPage(pgIntro, new IntroPage(this));
	setPage(pgConfig, new ConfigPage(this));
}

AddComputerWizard::~AddComputerWizard()
{
}

DiveComputer::Ptr AddComputerWizard::RunWizard(QWidget * parent)
{
	AddComputerWizard * wiz = new AddComputerWizard(parent);
	if (wiz->exec() == QDialog::Rejected)
		return DiveComputer::Ptr();

	DiveComputer::Ptr dc(new DiveComputer);
	dc->setDriver(wiz->field("driver").toString().toStdString());
	dc->setSerial(wiz->field("serial").toString().toStdString());

	if (! wiz->field("params").isNull() && ! wiz->field("params").toString().isEmpty())
		dc->setDriverArgs(wiz->field("params").toString().toStdString());

	if (! wiz->field("device").isNull() && ! wiz->field("device").toString().isEmpty())
		dc->setDevice(wiz->field("device").toString().toStdString());

	if (! wiz->field("manuf").isNull() && ! wiz->field("manuf").toString().isEmpty())
		dc->setManufacturer(wiz->field("manuf").toString().toStdString());

	if (! wiz->field("model").isNull() && ! wiz->field("model").toString().isEmpty())
		dc->setModel(wiz->field("model").toString().toStdString());

	if (! wiz->field("name").isNull() && ! wiz->field("name").toString().isEmpty())
		dc->setName(wiz->field("name").toString().toStdString());

	return dc;
}
