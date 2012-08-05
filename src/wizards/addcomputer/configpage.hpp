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

#ifndef WIZARDS_ADDCOMPUTER_CONFIGPAGE_HPP_
#define WIZARDS_ADDCOMPUTER_CONFIGPAGE_HPP_

/**
 * @file src/wizards/addcomputer/configpage.hpp
 * @brief Add Dive Computer Wizard Configuration Page
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QLabel>
#include <QLineEdit>
#include <QWizardPage>

namespace wizards { namespace addcomputer {

/**
 * @brief Add Computer Wizard Configuration Page
 *
 * Shows the configuration page of the Add Computer wizard, which confirms the
 * dive computer settings and allows the user to enter other information about
 * the device.
 */
class ConfigPage: public QWizardPage
{
	Q_OBJECT

public:

	//! Class Constructor
	ConfigPage(QWidget * parent = 0);

	//! Class Destructor
	virtual ~ConfigPage();

public:

	//! Initialize Page Contents
	virtual void initializePage();

	//! @return Next Page Id
	virtual int nextId() const;

private:

	//! Create Layout
	void createLayout();

private:
	QLabel *		m_lblSerial;
	QLabel *		m_lblManuf;
	QLabel *		m_lblModel;

	QLineEdit *		m_txtName;

};

} } /* wizards::addcomputer */

#endif /* WIZARDS_ADDCOMPUTER_CONFIGPAGE_HPP_ */
