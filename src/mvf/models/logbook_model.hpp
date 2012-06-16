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

#ifndef LOGBOOK_MODEL_HPP_
#define LOGBOOK_MODEL_HPP_

/**
 * @file src/mvf/logbook_model.hpp
 * @brief Logbook Model Classes
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

#include "logbook_item.hpp"

#include <benthos/logbook/logbook.hpp>
using namespace benthos::logbook;

/**
 * @brief Logbook Model Class
 *
 * Implements a Qt Model for Logbook Navigation.  Each leaf node in the model
 * corresponds to a unique view of Logbook data, either dives, sites, a map of
 * dive sites, or a dive computer.  Navigation within Benthos is accomplished
 * through this model.
 *
 * Leaf nodes (implmented by LogbookModelItem) are subclassed to provide a
 * data source interface for the UI controls.  See LogbookModelItem for more
 * details on the implementation.
 */
class LogbookModel: public QAbstractItemModel
{
public:

	//! Class Constructor
	LogbookModel(QObject * parent = 0);

	//! Class Destructor
	virtual ~LogbookModel();

public:

	//! @return Column Count for the given Parent
	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;

	//! @return Item Data
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

	//! @return Default Model Index
	virtual QModelIndex defaultIndex() const;

	//! @return Item Flags
	virtual Qt::ItemFlags flags(const QModelIndex & index) const;

	//! @return If the Item has Children
	virtual bool hasChildren(const QModelIndex & parent = QModelIndex()) const;

	//! @return Header Data
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	//! @return Model Index for the given Row and Column
	virtual QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;

	//! @brief Public Alias for createIndex
	virtual QModelIndex makeIndex(int row, int column, quint32 internal_id) const;

	//! @return Parent Index for the given Item
	virtual QModelIndex parent(const QModelIndex & index) const;

	//! @return Row Count for the given Parent
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;

public:

	//! @return Model Item at the given Index
	LogbookModelItem::Ptr item(const QModelIndex & index) const;

	//! @return Logbook
	Logbook::Ptr logbook() const;

	//! @param[in] Logbook to Model
	void setLogbook(Logbook::Ptr logbook);

public:

	//! Called by the Mapper when a Dive Computer is deleted
	void computer_deleted(AbstractMapper::Ptr, Persistent::Ptr obj);

	//! Called by the Mapper when a Dive Computer is inserted
	void computer_inserted(AbstractMapper::Ptr, Persistent::Ptr obj);

	//! Called by the Mapper when a Dive Computer is updated
	void computer_updated(AbstractMapper::Ptr, Persistent::Ptr obj);

	//! Called by the Mapper when a Dive Site is deleted
	void site_deleted(AbstractMapper::Ptr, Persistent::Ptr obj);

	//! Called by the Mapper when a Dive Site is inserted
	void site_inserted(AbstractMapper::Ptr, Persistent::Ptr obj);

	//! Called by the Mapper when a Dive Site is updated
	void site_updated(AbstractMapper::Ptr, Persistent::Ptr obj);

private:
	Logbook::Ptr					m_logbook;
	std::vector<TopLevelItem::Ptr>	m_items;

};

#endif /* LOGBOOK_MODEL_HPP_ */
