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

#ifndef MODELEDITPANEL_HPP_
#define MODELEDITPANEL_HPP_

/**
 * @file src/models/modeeditpanel.hpp
 * @brief Model Editor Panel Interface
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <benthos/logbook/session.hpp>

#include <QAbstractItemDelegate>
#include <QDataWidgetMapper>

using namespace benthos::logbook;

struct IModelEditPanel
{
	virtual ~IModelEditPanel() { }

	/**
	 * @brief Bind the Editor to a Data Widget Mapper
	 * @param [in] Database Session Instance
	 * @param [in] Data Widget Mapper
	 */
	virtual void bind(Session::Ptr session, QDataWidgetMapper * mapper) = 0;

	/**
	 * @brief Create a Delegate for the Editor Panel
	 * @param [in] Delegate Parent
	 * @return Delegate Instance
	 */
	virtual QAbstractItemDelegate * createDelegate(QObject * parent = 0) const = 0;

	/**
	 * @brief Return the Title of the current Item
	 * @return Title
	 */
	virtual QString title() const = 0;

};

#endif /* MODELEDITPANEL_HPP_ */
