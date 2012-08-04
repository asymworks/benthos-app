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

#ifndef DRIVERPARAMSDIALOG_HPP_
#define DRIVERPARAMSDIALOG_HPP_

/**
 * @file src/dialogs/driverparamsdialog.hpp
 * @brief Dive Computer Parameters Dialog Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <string>

#include <QDialog>
#include <QLabel>
#include <QModelIndex>
#include <QTreeView>

#include <benthos/divecomputer/driverclass.hpp>
using namespace benthos::dc;

#include <mvf/models/driverparams_model.hpp>

/**
 * @brief Dive Computer Parameters Dialog
 *
 * Allows the user to graphically edit the dive comptuer driver parameter list
 * with a property grid interface.
 */
class DriverParamsDialog: public QDialog
{
	Q_OBJECT

public:

	/**
	 * @brief Class Constructor
	 * @param[in] Driver Model
	 * @param[in] Parameter String
	 * @param[in] Parent Widget
	 */
	DriverParamsDialog(DriverClass::Ptr cls, const std::string & args, QWidget * parent = 0);

	//! Class Destructor
	virtual ~DriverParamsDialog();

	//! @return Parameter String
	std::string param_string() const;

protected slots:
	void currentIndexChanged(const QModelIndex &);

private:
	DriverParamsModel *			m_model;
	QTreeView *					m_view;
	QLabel *					m_lblTitle;
	QLabel *					m_lblTip;

};

#endif /* DRIVERPARAMSDIALOG_HPP_ */
