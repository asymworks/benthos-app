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

#ifndef DRIVERPARAMS_DELEGATE_HPP_
#define DRIVERPARAMS_DELEGATE_HPP_

/**
 * @file src/mvf/driverparams_delegate.hpp
 * @brief Delegate for Driver Parameters
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QStyledItemDelegate>
#include <QWidget>

#include <mvf/delegates.hpp>

#include <benthos/divecomputer/driverclass.hpp>
using namespace benthos::dc;

/**
 * @brief Driver Parameters Delegate
 *
 * Custom delegate to provide the correct editor for model parameters.
 */
class DriverParamsDelegate: public NoFocusDelegate
{
public:

	//! Class Constructor
	DriverParamsDelegate(DriverClass::Ptr dclass, QObject * parent = 0);

	//! Class Destructor
	virtual ~DriverParamsDelegate();

public:

	//! Create Editor
	QWidget * createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const;

private:
	DriverClass::Ptr	m_dclass;

};

#endif /* DRIVERPARAMS_DELEGATE_HPP_ */
