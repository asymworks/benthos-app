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

#ifndef DRIVERMODELS_MODEL_HPP_
#define DRIVERMODELS_MODEL_HPP_

/**
 * @file src/mvf/models/drivermodels_model.hpp
 * @brief Driver Model List Model
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <utility>
#include <vector>

#include <QAbstractListModel>

#include <benthos/divecomputer/driverclass.hpp>
#include <benthos/divecomputer/registry.hpp>

using namespace benthos::dc;

/**
 * @brief Driver Model List Model
 *
 * Model representing the model types supported by a dive computer driver
 * plugin.  The display text is the model name, and both the edit role and
 * user role contain the model number.
 */
class DriverModelsModel: public QAbstractListModel
{
public:

	//! Class Constructor
	DriverModelsModel(DriverClass::Ptr dclass, QObject * parent = 0);

	//! Class Destructor
	virtual ~DriverModelsModel();

public:

	//! @return Item Data
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

	//! @return Number of Rows in the Model
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;

private:
	std::vector<std::pair<int, std::string> >		m_models;

};

#endif /* DRIVERMODELS_MODEL_HPP_ */
