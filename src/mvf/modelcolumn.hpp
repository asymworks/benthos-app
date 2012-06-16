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

#ifndef MODELCOLUMN_HPP_
#define MODELCOLUMN_HPP_

/**
 * @file src/mvf/modelcolumn.hpp
 * @brief Model Column Interfaces
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QAbstractItemDelegate>
#include <QPointF>
#include <QString>

#include "modelif.hpp"

#include <benthos/logbook/session.hpp>

using namespace benthos::logbook;

/**
 * @brief Item Delegate Factory Interface
 *
 * Implements a factory class for Item Delegates.
 */
struct IDelegateFactory
{
	virtual ~IDelegateFactory() { }

	//! @return New Delegate Class
	virtual QAbstractItemDelegate * create() = 0;
};

/**
 * @brief Typed Item Delegate Factory Class
 */
template <class T>
struct DelegateFactory: public IDelegateFactory
{
	virtual ~DelegateFactory() { }

	//! @return New Delegate Class
	virtual QAbstractItemDelegate * create() { return new T(); }
};

/**
 * @brief Model Column Base Class
 *
 * Stores column appearance and metadata for a Qt Model-View Framework model.
 * This base class is used by ModelColumn to provide column-level appearance
 * and data customization in Qt MVF models.
 */
class BaseModelColumn
{
public:

	//! Class Constructor
	BaseModelColumn(const QString & name, const QString & label, IDelegateFactory * delegateFactory = NULL,
		bool hidden = false, bool internal = false, Qt::Alignment align = Qt::AlignLeft | Qt::AlignVCenter);

	//! Class Destructor
	virtual ~BaseModelColumn();

public:

	//! @return Text Alignment
	Qt::Alignment alignment() const;

	//! @param[in] Session Pointer
	virtual void bind(Session::Ptr session);

	//! @return New Item Delegate
	QAbstractItemDelegate * createDelegate() const;

	//! @return Column has a Custom Delegate
	bool hasDelegate() const;

	//! @return Column Hidden
	bool isHidden() const;

	//! @return Column Internal
	bool isInternal() const;

	//! @return Header Label
	const QString & label() const;

	//! @return Column Name
	const QString & name() const;

	//! @brief Set Text Alignment
	void setAlignment(Qt::Alignment value);

	//! @brief Set if the Column is Hidden
	void setHidden(bool value);

	//! @brief Set if the Column is Internal
	void setInternal(bool value);

	//! @brief Set Header Label
	void setLabel(const QString & value);

protected:
	QString				m_name;
	QString				m_label;
	Qt::Alignment		m_align;
	bool				m_hidden;
	bool				m_internal;
	IDelegateFactory *	m_delegatef;
	Session::Ptr		m_session;

};

/**
 * @brief Model Column Class
 *
 * Templated version of BaseModelColumn which includes getters and setters
 * for item fields.  The LogbookDomainModel class will translate the requested
 * index into an object reference and pass it to ModelColumn for all data get/
 * set operations.
 */
template <class T>
class ModelColumn: public BaseModelColumn
{
public:

	//! Class Constructor
	ModelColumn(IFieldAdapter<T> * adapter, const QString & name, const QString & label,
			IDelegateFactory * delegateFactory = NULL, bool hidden = false,
			bool internal = false, Qt::Alignment align = Qt::AlignLeft | Qt::AlignVCenter)
		: BaseModelColumn(name, label, delegateFactory, hidden, internal, align),
		  m_adapter(adapter)
	{ }

	//! Class Destructor
	virtual ~ModelColumn()
	{
		if (m_adapter != NULL)
		{
			delete m_adapter;
			m_adapter = NULL;
		}
	}

	//! @param[in] Session Pointer
	virtual void bind(Session::Ptr session)
	{
		BaseModelColumn::bind(session);
		if (m_adapter != NULL)
			m_adapter->bind(session);
	}

	/**
	 * @brief Return Item Data for the Column
	 *
	 * Returns item data for the column.  If the role is Qt::TextAlignmentRole,
	 * the align value is returned; otherwise for Qt::DisplayRole,
	 * Qt::DecorationRole, Qt::CheckStateRole and Qt::EditRole, one of
	 * getDisplayRole(), getDecorationRole(), getCheckStateRole(), and
	 * getEditRole() are called, respectively.
	 *
	 * For all other roles, an empty variant is returned.
	 */
	virtual QVariant data(const boost::shared_ptr<T> & item, int role = Qt::DisplayRole)
	{
		if (role == Qt::DisplayRole)
			return m_adapter->displayData(item);
		else if (role == Qt::DecorationRole)
			return m_adapter->decorationData(item);
		else if (role == Qt::EditRole)
			return m_adapter->editData(item);
		else if (role == Qt::TextAlignmentRole)
			return QVariant(m_align);
		return QVariant();
	}

	/**
	 * @brief Return Header Data for the Column
	 *
	 * Returns data for the Horizontal Header.  If the role is Qt::DisplayRole,
	 * the label is returned; if the role is Qt::TextAlignmentRole, the align
	 * value is returned.  For all other roles, an empty variant is returned.
	 */
	virtual QVariant headerData(Qt::Orientation orientation, int role = Qt::DisplayRole) const
	{
		if (orientation != Qt::Horizontal)
			return QVariant();
		if (role == Qt::DisplayRole)
			return QVariant(m_label);
		else if (role == Qt::TextAlignmentRole)
			return QVariant(m_align);
		return QVariant();
	}

	/**
	 * @brief Set Data for the Column
	 *
	 * Sets the edit data for the given item.  If the role is not Qt::EditData,
	 * no operation is performed and false is returned.
	 */
	virtual bool setData(boost::shared_ptr<T> & item, const QVariant & data, int role = Qt::EditRole)
	{
		if (role == Qt::EditRole)
			return m_adapter->setEditData(item, data);
		return false;
	}

protected:
	IFieldAdapter<T> *	m_adapter;

};

#endif /* MODELCOLUMN_HPP_ */
