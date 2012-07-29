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

#ifndef LOGBOOK_ITEM_HPP_
#define LOGBOOK_ITEM_HPP_

/**
 * @file src/mvf/logbook_item.hpp
 * @brief Logbook Model Item Classes
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <vector>

#include <boost/shared_ptr.hpp>

#include <QImage>
#include <QString>
#include <QVariant>

#include <benthos/logbook/country.hpp>
#include <benthos/logbook/session.hpp>
using namespace benthos::logbook;

/**
 * @brief Basic Logbook Model Item Class
 *
 * Contains the basic elements of the Model Item including the display
 * string and icon.
 */
class LogbookModelItem
{
public:
	typedef boost::shared_ptr<LogbookModelItem>			Ptr;
	typedef boost::shared_ptr<const LogbookModelItem>	ConstPtr;

	/*
	 * Model Item Type
	 */
	enum
	{
		HeaderItem = -1,
		DiveListItem = 0,
		SiteListItem = 1,
		SiteMapItem = 2,
		DeviceItem = 3
	};

public:

	//! Class Constructor
	LogbookModelItem(const QString & title, const QImage & icon = QImage(), int type = DiveListItem);

	//! Class Destructor
	virtual ~LogbookModelItem();

public:

	//! @return Item Data
	QVariant data(int role) const;

	//! @return Item Icon
	const QImage & icon() const;

	//! @return Item Title
	const QString & title() const;

	//! @return Item Type
	int type() const;

protected:
	QString			m_title;
	QImage			m_icon;
	int				m_type;

};

/**
 * @brief Top-Level Node Item
 *
 * Top-Level Node Items always have a type of HeaderItem and can contain other
 * LogbookModelItem instances.
 */
class TopLevelItem: public LogbookModelItem
{
public:
	typedef boost::shared_ptr<TopLevelItem>			Ptr;
	typedef boost::shared_ptr<const TopLevelItem>	ConstPtr;

public:

	//! Class Constructor
	TopLevelItem(const QString & title, const QImage & icon = QImage());

	//! Class Destructor
	virtual ~TopLevelItem();

public:

	//! Array Accessor
	LogbookModelItem::Ptr operator[] (int index);
	LogbookModelItem::ConstPtr operator[] (int index) const;

	//! @param[in] Child Item to Append
	void append(LogbookModelItem::Ptr item);

	//! @return Begin Iterator
	std::vector<LogbookModelItem::Ptr>::iterator begin();

	//! @return Begin Iterator
	std::vector<LogbookModelItem::Ptr>::const_iterator begin() const;

	//! @brief Clear Child Items
	void clear();

	//! @return Begin Iterator
	std::vector<LogbookModelItem::Ptr>::iterator end();

	//! @return Begin Iterator
	std::vector<LogbookModelItem::Ptr>::const_iterator end() const;

	//! @param[in] Child Item to Remove
	void remove(LogbookModelItem::Ptr item);

	//! @return Number of Child Items
	size_t size() const;

protected:
	std::vector<LogbookModelItem::Ptr>		m_items;

};

/**
 * @brief Data Source Functor
 */
template <class T>
struct ILogbookDataSource
{
	virtual ~ILogbookDataSource() { }

	//! @return List of Items
	virtual std::vector<typename T::Ptr> getItems(Session::Ptr session) const = 0;

};

/**
 * @brief Data Source Node Item
 *
 * Data Source Nodes have a built-in method to fetch a list of items from
 * the database.  At runtime, the getItems() method is called, passing the
 * Logbook Session.
 */
template <class T>
class DataSourceItem: public LogbookModelItem
{
public:

	//! Class Constructor
	DataSourceItem(ILogbookDataSource<T> * source, const QString & title, const QImage & icon = QImage(), int type = DiveListItem)
		: LogbookModelItem(title, icon, type), m_source(source)
	{
	}

	//! Class Destructor
	virtual ~DataSourceItem()
	{
		delete m_source;
	}

	//! @return List of Items
	virtual std::vector<typename T::Ptr> getItems(Session::Ptr session) const
	{
		return m_source->getItems(session);
	}

private:
	ILogbookDataSource<T> *		m_source;

};

/**
 * @brief Item Source Node Item
 *
 * Item Source Nodes have a built-in method to fetch a single item from the
 * database, which is provided in the constructor.
 */
template <class T>
class ItemSourceItem: public LogbookModelItem
{
public:

	//! Class Constructor
	ItemSourceItem(typename T::Ptr item, const QString & title, const QImage & icon = QImage(), int type = DeviceItem)
		: LogbookModelItem(title, icon, type), m_item(item)
	{
	}

	//! Class Destructor
	virtual ~ItemSourceItem()
	{
	}

	//! @return Item
	virtual typename T::Ptr getItem() const
	{
		return m_item;
	}

private:
	typename T::Ptr		m_item;

};

/**
 * @brief Country Node Item
 *
 * Data source node item which is associated with a country.  This allows the
 * Navigation Model to track changes to the list of countries.
 */
template <class T>
class CountryLogbookItem: public DataSourceItem<T>
{
public:

	//! Class Constructor
	CountryLogbookItem(ILogbookDataSource<T> * source, const country & country, int type = LogbookModelItem::DiveListItem)
		: DataSourceItem<T>(source,
				QString::fromStdString(country.name()),
				QImage(QString(":/flags/%1.png").arg(QString::fromStdString(country.code()).toLower())),
		  type), m_country(country)
	{
	}

	//! Class Destructor
	virtual ~CountryLogbookItem()
	{
	}

	//! @return Country
	const country & country_() const
	{
		return m_country;
	}

private:
	country				m_country;

};

#endif /* LOGBOOK_ITEM_HPP_ */
