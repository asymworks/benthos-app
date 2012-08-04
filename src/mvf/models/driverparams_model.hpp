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

#ifndef DRIVERPARAMS_MODEL_HPP_
#define DRIVERPARAMS_MODEL_HPP_

/**
 * @file src/mvf/driverparams_model.hpp
 * @brief Dive Computer Driver Parameters Model Classes
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <string>
#include <vector>

#include <QAbstractTableModel>
#include <QVariant>

#include <benthos/divecomputer/driverclass.hpp>
#include <benthos/divecomputer/plugin.hpp>
using namespace benthos::dc;

/**
 * @brief Driver Parameters Model
 *
 * Holds a set of name-value pairs corresponding to parameters which are
 * passed to a dive computer driver.
 */
class DriverParamsModel: public QAbstractTableModel
{
public:

	/**
	 * @brief Class Constructor
	 * @param[in] Driver Model
	 * @param[in] Parameter String
	 *
	 * Creates a model for the given driver class and parameter string, loading
	 * the values from the parameter string with the ArgumentList class.
	 */
	DriverParamsModel(DriverClass::Ptr cls, const std::string & args);

	//! Class Destructor
	~DriverParamsModel();

	//! @return Number of Columns in the Model
	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;

	//! @return Item Data
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

	//! @return Item Flags
	virtual Qt::ItemFlags flags(const QModelIndex & index) const;

	//! @return Header Data
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	//! @return Number of Rows in the Model
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;

	//! @brief Set Item Data
	virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::DisplayRole);

	//! @return String Representation of the Parameters
	virtual std::string toString() const;

protected:

	//! @return Default Value for an Argument
	QVariant defaultValue(int row) const;

	//! @return True if the Argument has its Default Value
	bool is_default(int row) const;

private:
	DriverClass::Ptr			m_class;
	std::vector<param_info_t>	m_params;
	std::vector<QVariant>		m_values;

};

#endif /* DRIVERPARAMS_MODEL_HPP_ */
