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

#ifndef PROPERTYGRID_HPP_
#define PROPERTYGRID_HPP_

/**
 * @file src/controls/propertygrid.hpp
 * @brief Simple Property Grid Control
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QObject>
#include <QTreeView>

/**
 * @brief Simple Property Grid Class
 *
 * Implements a property grid using a Tree View.  Does not support multi-level
 * properties or nesting.
 */
class PropertyGridView: public QTreeView
{
	Q_OBJECT

public:

	//! Class Constructor
	PropertyGridView(QWidget * parent = 0);

	//! Class Destructor
	virtual ~PropertyGridView();


signals:
	void currentIndexChanged(const QModelIndex &);

protected:

	//! Override drawBranches to hide all tree branches
	virtual void drawBranches(QPainter * painter, const QRect & rect, const QModelIndex & index) const;

protected slots:

	//! @brief Called when the Current Index Changes
	virtual void currentChanged(const QModelIndex & current, const QModelIndex & previous);

};

#endif /* PROPERTYGRID_HPP_ */
