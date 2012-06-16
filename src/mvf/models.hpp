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

#ifndef MODELS_HPP_
#define MODELS_HPP_

/**
 * @file src/mvf/models.hpp
 * @brief Base Model Interfaces
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QAbstractTableModel>
#include <QObject>

#include "modelcolumn.hpp"

#include <benthos/logbook/session.hpp>

using namespace benthos::logbook;

/**
 * @brief Custom Table Model
 *
 * Shim model which wraps the dataChanged signal in a method since Q_OBJECT
 * is not supported by templated classes.  This class also carries the column
 * list vector so it can be supported by views that don't need the template.
 */
class CustomTableModel: public QAbstractTableModel
{
	Q_OBJECT

public:

	//! Class Constructor
	CustomTableModel(QObject * parent = 0);

	//! Class Destructor
	virtual ~CustomTableModel();

	//! @param[in] Session Pointer
	void bind(Session::Ptr session);

	//! @return Column List
	const std::vector<BaseModelColumn *> & columns() const;

	//! Emit dataChanged event
	void emitDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight);

	//! @return Column Index
	int findColumn(BaseModelColumn * col) const;

	//! @return Column Index
	int findColumn(const QString & col) const;

protected:
	std::vector<BaseModelColumn *>		m_columns;
	Session::Ptr						m_session;

};

/**
 * @brief LiteSQL Data Source Model
 */
template <class T>
class LogbookQueryModel: public CustomTableModel
{
protected:
	typedef T 	model_type;

public:

	//! Class Constructor
	LogbookQueryModel(QObject * parent = 0)
		: CustomTableModel(parent), m_items()
	{
	}

	//! Class Destructor
	virtual ~LogbookQueryModel()
	{
	}

	//! Reload the Items
	void resetFromList(const std::vector<boost::shared_ptr<T> > & items)
	{
		beginResetModel();
		m_items = items;
		endResetModel();
	}

	//! Clear the Items
	void clearItems()
	{
		beginResetModel();
		m_items.clear();
		endResetModel();
	}

	//! Check if the Item has child items
	virtual bool hasChildren(const QModelIndex & parent = QModelIndex()) const
	{
		if (! parent.isValid())
			// This seems to be a sentinel for the "root" item, which _must_
			// have children
			return true;
		return false;
	}

	//! @return Number of Columns in the Model
	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const
	{
		return m_columns.size();
	}

	//! @return Number of Rows in the Model
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const
	{
		return m_items.size();
	}

	//! @return Model Item
	virtual boost::shared_ptr<T> item(const QModelIndex & index)
	{
		if (! index.isValid())
			return boost::shared_ptr<T>();

		size_t r = index.row();

		if ((r < 0) || (r >= m_items.size()))
			return boost::shared_ptr<T>();
		return m_items[r];
	}

	//! @return Header Data
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
	{
		if ((section < 0) || ((size_t)section >= m_columns.size()))
			return QVariant();

		ModelColumn<T> * col = dynamic_cast<ModelColumn<T> *>(m_columns[section]);
		if (col == NULL)
			return QVariant();

		return col->headerData(orientation, role);
	}

	//! @return Item Data
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const
	{
		if (! index.isValid())
			return QVariant();
		size_t r = index.row();
		size_t c = index.column();

		if ((c < 0) || (c >= m_columns.size()))
			return QVariant();
		if ((r < 0) || (r >= m_items.size()))
			return QVariant();

		ModelColumn<T> * col = dynamic_cast<ModelColumn<T> *>(m_columns[c]);
		if (col == NULL)
			return QVariant();

		return col->data(m_items[r], role);
	}

	//! Set Item Data
	virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::DisplayRole)
	{
		if (! index.isValid())
			return false;
		size_t r = index.row();
		size_t c = index.column();

		if ((c < 0) || (c >= m_columns.size()))
			return false;
		if ((r < 0) || (r >= m_items.size()))
			return false;

		ModelColumn<T> * col = dynamic_cast<ModelColumn<T> *>(m_columns[c]);
		if (col == NULL)
			return false;

		bool ret = col->setData(m_items[r], value, role);
		if (ret)
		{
			m_session->add(m_items[r]);
			emitDataChanged(index, index);
		}
		return ret;
	}

protected:
	std::vector<boost::shared_ptr<T> >		m_items;

};

#define ADD_FIELD_COLUMN(FTYPE, GETTER, SETTER, ...) \
	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>( \
		new DefaultFieldAdapter<LogbookQueryModel::model_type, FTYPE>( \
			& LogbookQueryModel::model_type::GETTER, \
			& LogbookQueryModel::model_type::SETTER \
		), #GETTER, __VA_ARGS__ \
	))

#define ADD_RO_FIELD_COLUMN(FTYPE, GETTER, ...) \
	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>( \
		new DefaultFieldAdapter<LogbookQueryModel::model_type, FTYPE>( \
			& LogbookQueryModel::model_type::GETTER \
		), #GETTER, __VA_ARGS__ \
	))

#define ADD_OPTIONAL_COLUMN(FTYPE, GETTER, SETTER, ...) \
	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>( \
		new DefaultFieldAdapter<LogbookQueryModel::model_type, FTYPE>( \
			& LogbookQueryModel::model_type::GETTER, \
			& LogbookQueryModel::model_type::SETTER, \
			& LogbookQueryModel::model_type::SETTER \
		), #GETTER, __VA_ARGS__ \
	))


#endif /* MODELS_HPP_ */
