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

#ifndef LOGBOOK_DELEGATE_HPP_
#define LOGBOOK_DELEGATE_HPP_

/**
 * @file src/mvf/logbook_delegate.hpp
 * @brief Logbook Navigation Delegate Classes
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QStyledItemDelegate>

/**
 * @brief Navigation Tree Delegate
 *
 * Custom delegate which draws Navigation Tree-View Items in a style similar to
 * iTunes explorer tree view.
 */
class LogbookDelegate: public QStyledItemDelegate
{
public:

	//! Class Constructor
	LogbookDelegate(QObject* parent = 0);

	//! Class Destructor
	virtual ~LogbookDelegate();

	//! Reimplemented from QStyledItemDelegate
	virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

	//! Reimplemented from QStyledItemDelegate
	virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;

};

#endif /* LOGBOOK_DELEGATE_HPP_ */
