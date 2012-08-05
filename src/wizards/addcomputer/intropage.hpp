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

#ifndef WIZARDS_ADDCOMPUTER_INTROPAGE_HPP_
#define WIZARDS_ADDCOMPUTER_INTROPAGE_HPP_

/**
 * @file src/wizards/addcomputer/intropage.hpp
 * @brief Add Dive Computer Wizard Intro Page
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <string>

#include <QLabel>
#include <QLineEdit>
#include <QWizardPage>

#include <controls/wizardcombobox.hpp>

#include <benthos/divecomputer/driverclass.hpp>
#include <benthos/divecomputer/registry.hpp>
using namespace benthos::dc;

namespace wizards { namespace addcomputer {

/**
 * @brief Add Computer Wizard Introduction Page
 *
 * Shows the introduction page of the add computer wizard, which holds the
 * components for configuring the driver.
 */
class IntroPage: public QWizardPage
{
	Q_OBJECT

public:

	//! Class Constructor
	IntroPage(QWidget * parent = 0);

	//! Class Destructor
	virtual ~IntroPage();

public:

	//! @return Next Page Id
	virtual int nextId() const;

	//! @brief Test the Dive Computer connection
	virtual bool validatePage();

protected slots:
	void btnParamsClicked();
	void updateModels(int);

private:

	//! Create Layout
	void createLayout();

	//! Get the current Driver Class
	DriverClass::Ptr driverClass() const;

	//! Get the Parameter String
	std::string paramString() const;

private:
	PluginRegistry::Ptr		m_registry;
	WizardComboBox *		m_cbxType;
	WizardComboBox *		m_cbxModel;
	WizardComboBox *		m_cbxDevice;

	QLineEdit *				m_txtParams;
	QLineEdit *				m_txtSerial;
	QLineEdit *				m_txtManuf;
	QLineEdit *				m_txtModel;

	QLabel *				m_lblModel;
	QLabel *				m_lblDevice;

	std::string				m_params;

};

} } /* wizards::addcomputer */

#endif /* WIZARDS_ADDCOMPUTER_INTROPAGE_HPP_ */
