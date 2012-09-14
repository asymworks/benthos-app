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

#ifndef DIVETAGS_MODEL_HPP_
#define DIVETAGS_MODEL_HPP_

/**
 * @file src/mvf/models/divetags_model.hpp
 * @brief Dive Tags Model
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <set>
#include <string>
#include <utility>
#include <vector>

#include <QAbstractListModel>

#include <benthos/logbook/collection.hpp>
#include <benthos/logbook/dive.hpp>
using namespace benthos::logbook;

/**
 * Dive Tags Model
 *
 * Holds a list of all available dive tags and indicates which ones apply to
 * the current dive.  The tag list is refreshed each time the dive is set, and
 * the model includes a set of default tags which are always displayed along
 * with all custom tags that have been added to the logbook.
 *
 * Default Tags:
 *
 * - Boat
 * - Current
 * - Fresh
 * - Training
 * - Salt
 * - Shore
 * - Surf
 * - Surge
 * - Waves
 */
class DiveTagsModel: public QAbstractListModel
{
	Q_OBJECT

public:

	//! Class Constructor
	DiveTagsModel(QObject * parent = 0);

	//! Class Destructor
	virtual ~DiveTagsModel();

public:

	//! @brief Add a Tag
	void addTag(const std::string & tag);

	//! @return Item Data
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

	//! @return Item Flags
	virtual Qt::ItemFlags flags(const QModelIndex & index) const;

	//! @return Number of Rows in the Model
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;

	//! @brief Set Item Data
	virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::DisplayRole);

	//! @param[in] Current Dive
	void setDive(Dive::Ptr dive);

private:
	std::vector<std::string>		m_tags;
	std::set<std::string, cicmp>	m_divetags;
	Dive::Ptr						m_dive;

};

#endif /* DIVETAGS_MODEL_HPP_ */
