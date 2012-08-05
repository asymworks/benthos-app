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

#ifndef ADDCOMPUTERWIZARD_HPP_
#define ADDCOMPUTERWIZARD_HPP_

/**
 * @file src/wizards/addcomputerwizard.hpp
 * @brief Add Dive Computer Wizard
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QWidget>
#include <QWizard>

#include <benthos/logbook/dive_computer.hpp>
using namespace benthos::logbook;

namespace wizards {

//! Add Dive Computer Wizard
class AddComputerWizard: public QWizard
{
	Q_OBJECT

public:

	//! Class Constructor
	AddComputerWizard(QWidget * parent = 0);

	//! Class Destructor
	virtual ~AddComputerWizard();

public:

	//! Run the Dive Computer Wizard
	static DiveComputer::Ptr RunWizard(QWidget * parent = 0);

};

} /* wizards */

#endif /* ADDCOMPUTERWIZARD_HPP_ */
