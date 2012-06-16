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

#include "logbook_item.hpp"

LogbookModelItem::LogbookModelItem(const QString & title, const QImage & icon, int type)
	: m_title(title), m_icon(icon), m_type(type)
{
}

LogbookModelItem::~LogbookModelItem()
{
}

QVariant LogbookModelItem::data(int role) const
{
	switch (role)
	{
	case Qt::DisplayRole:
		return m_title;
		break;

	case Qt::DecorationRole:
		return m_icon;
		break;

	default:
		return QVariant();
	}
}

const QImage & LogbookModelItem::icon() const
{
	return m_icon;
}

const QString & LogbookModelItem::title() const
{
	return m_title;
}

int LogbookModelItem::type() const
{
	return m_type;
}

TopLevelItem::TopLevelItem(const QString & title, const QImage & icon)
	: LogbookModelItem(title, icon, HeaderItem), m_items()
{
}

TopLevelItem::~TopLevelItem()
{
}

LogbookModelItem::Ptr TopLevelItem::operator[] (int index)
{
	return m_items[index];
}

LogbookModelItem::ConstPtr TopLevelItem::operator[] (int index) const
{
	return m_items[index];
}

void TopLevelItem::append(LogbookModelItem::Ptr item)
{
	m_items.push_back(item);
}

std::vector<LogbookModelItem::Ptr>::iterator TopLevelItem::begin()
{
	return m_items.begin();
}

std::vector<LogbookModelItem::Ptr>::const_iterator TopLevelItem::begin() const
{
	return m_items.begin();
}

void TopLevelItem::clear()
{
	m_items.clear();
}

std::vector<LogbookModelItem::Ptr>::iterator TopLevelItem::end()
{
	return m_items.end();
}

std::vector<LogbookModelItem::Ptr>::const_iterator TopLevelItem::end() const
{
	return m_items.end();
}

void TopLevelItem::remove(LogbookModelItem::Ptr item)
{
	std::vector<LogbookModelItem::Ptr>::iterator it;
	for (it = m_items.begin(); it != m_items.end(); it++)
	{
		if (*it == item)
		{
			m_items.erase(it);
			break;
		}
	}
}

size_t TopLevelItem::size() const
{
	return m_items.size();
}
