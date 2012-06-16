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

#ifndef COUNTRYMODEL_HPP_
#define COUNTRYMODEL_HPP_

/**
 * @file src/mvf/countrymodel.hpp
 * @brief ISO3166 Country Model
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QAbstractListModel>
#include <QModelIndex>

#include <string>
#include <vector>

/**
 * @brief ISO-3166 Country Model
 */
class CountryModel: public QAbstractListModel
{
public:

	//! Class Constructor
	CountryModel();

	//! Class Destructor
	virtual ~CountryModel();

	//! @return Item Data
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

	//! @return Number of Rows in the Model
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;

private:
	mutable std::vector<std::string>	m_codes;

};

#endif /* COUNTRYMODEL_HPP_ */
