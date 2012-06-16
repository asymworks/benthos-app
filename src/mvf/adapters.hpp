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

#ifndef ADAPTERS_HPP_
#define ADAPTERS_HPP_

/**
 * @file src/mvf/adapters.hpp
 * @brief Qt MVF Adapter Interfaces
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QAbstractListModel>
#include <QObject>

#include <vector>

#include <benthos/logbook/session.hpp>
using namespace benthos::logbook;

/**
 * @brief QAbstractListModel Wrapper for std::vector
 *
 * Template type T must be convertable to/from QVariant.
 */
template <typename T>
class VectorModel: public QAbstractListModel
{
public:

	//! Class Constructor
	VectorModel(const std::vector<T> & data)
		: m_data(data)
	{
	}

	//! Class Destructor
	virtual ~VectorModel() { }

public:

	//! @return Row Count
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const
	{
		return m_data.size();
	}

	//! @return Item Data
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const
	{
		if (! index.isValid())
			return QVariant();
		size_t r = index.row();
		size_t c = index.column();

		if (c != 0)
			return QVariant();
		if ((r < 0) || (r >= m_data.size()))
			return QVariant();

		if ((role == Qt::DisplayRole) || (role == Qt::EditRole))
			return QVariant(m_data[r]);
		return QVariant();
	}

	//! @brief Reset Item Data
	virtual void reset(const std::vector<T> & data)
	{
		beginResetModel();
		m_data = data;
		endResetModel();
	}

protected:
	std::vector<T>		m_data;

};

/**
 * @brief QAbstractListModel Wrapper for std::vector<std::string>
 * TODO: Consolidate all QVariant <-> std::string logic in one place
 */
template <>
class VectorModel<std::string>: public QAbstractListModel
{
public:

	//! Class Constructor
	VectorModel(const std::vector<std::string> & data)
		: m_data(data)
	{
	}

	//! Class Destructor
	virtual ~VectorModel() { }

public:

	//! @return Row Count
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const
	{
		return m_data.size();
	}

	//! @return Item Data
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const
	{
		if (! index.isValid())
			return QVariant();
		size_t r = index.row();
		size_t c = index.column();

		if (c != 0)
			return QVariant();
		if ((r < 0) || (r >= m_data.size()))
			return QVariant();

		if ((role == Qt::DisplayRole) || (role == Qt::EditRole))
			return QVariant(QString::fromStdString(m_data[r]));
		return QVariant();
	}

	//! @brief Reset Item Data
	virtual void reset(const std::vector<std::string> & data)
	{
		beginResetModel();
		m_data = data;
		endResetModel();
	}

protected:
	std::vector<std::string>		m_data;

};

/**
 * @brief Foreign Key List Model
 */
template <class T>
class ForeignKeyModel: public QAbstractListModel
{
public:

	//! Class Constructor
	ForeignKeyModel()
		: m_session()
	{
	}

	//! Class Destructor
	virtual ~ForeignKeyModel() { }

public:

	//! @param[in] Logbook Session Pointer
	void bind(Session::Ptr session)
	{
		beginResetModel();
		m_session.swap(session);
		m_data = m_session->finder<T>()->find();
		endResetModel();
	}

	//! @return Row Count
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const
	{
		return m_data.size();
	}

	//! @return Item Data
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const
	{
		if (! index.isValid())
			return QVariant();
		size_t r = index.row();
		size_t c = index.column();

		if (c != 0)
			return QVariant();
		if ((r < 0) || (r >= m_data.size()))
			return QVariant();

		if (role == Qt::EditRole)
		{
			return QVariant::fromValue<int64_t>(m_data[r]->id());
		}
		else if (role == Qt::DisplayRole)
		{
			return displayData(m_data[r]);
		}
		else if (role == Qt::DecorationRole)
		{
			return decorationData(m_data[r]);
		}
		return QVariant();
	}

protected:

	//! @return Decoration Data
	virtual QVariant decorationData(boost::shared_ptr<T> item) const = 0;

	//! @return Display Data
	virtual QVariant displayData(boost::shared_ptr<T> item) const = 0;

protected:
	Session::Ptr						m_session;
	std::vector<boost::shared_ptr<T> >	m_data;

};

#endif /* ADAPTERS_HPP_ */
