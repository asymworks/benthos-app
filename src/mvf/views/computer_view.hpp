/*
 * Copyright (C) 2011 Asymworks, LLC.  All Rights Reserved.
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

#ifndef COMPUTER_VIEW_HPP_
#define COMPUTER_VIEW_HPP_

/**
 * @file src/mvf/computer_view.hpp
 * @brief View Class for Dive Computers
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <string>

#include <QCheckBox>
#include <QFrame>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QString>

#include <benthos/logbook/dive_computer.hpp>
#include <benthos/logbook/profile.hpp>

using namespace benthos::logbook;

/**
 * Dive Computer View Class
 *
 * Information frame for Dive Computers which shows basic computer information
 * and allows the user to transfer dives off the dive computer.
 */
class ComputerView: public QFrame
{
	Q_OBJECT

public:

	//! Class Constructor
	ComputerView(QWidget * parent = 0);

	//! Class Destructor
	virtual ~ComputerView();

public:

	//! @return Dive Computer
	DiveComputer::Ptr computer() const;

	//! @param[in] Dive Computer
	void setComputer(DiveComputer::Ptr);

protected slots:
	void btnConnectionClicked();
	void btnTransferClicked();

	void updateSettings();

protected:

	//! @return Image Path for Dive Computer
	static std::string imagePath(DiveComputer::Ptr);

	//! Create View for Basic Information
	QFrame * createInfoLayout();

	//! Create View for Options
	QFrame * createOptionsLayout();

	//! Create View Layout
	void createLayout();

private:
	DiveComputer::Ptr			m_dc;

	QLabel *					m_lblImage;
	QLabel *					m_lblName;
	QLabel *					m_lblSerial;
	QLabel *					m_lblDives;
	QLabel *					m_lblDriver;
	QLabel *					m_lblDevice;
	QLabel *					m_lblLastXfr;

	QLabel *					m_lblManf;
	QLabel *					m_lblModel;
	QLabel *					m_lblSWVersion;
	QLabel *					m_lblHWVersion;

	QPushButton *				m_btnTransfer;
	QPushButton *				m_btnSettings;

	QCheckBox *					m_chkCheckSN;
	QCheckBox *					m_chkUpdateToken;

	QPushButton *				m_btnConnection;

};

#endif /* COMPUTER_VIEW_HPP_ */
