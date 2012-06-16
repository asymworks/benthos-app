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

#ifndef NAVTREE_HPP_
#define NAVTREE_HPP_

/**
 * @file src/controls/navtree.hpp
 * @brief Navigation Tree View Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QString>
#include <QTreeView>
#include <QWidget>

/**
 * @brief Navigation Tree View
 *
 * The Navigation Tree View is the primary navigation control for Benthos.  It
 * includes the navigation items to view dives, sites, site map and computers
 * as well as shortcuts to dives by country and by date.
 */
class NavTree: public QTreeView
{
	Q_OBJECT

protected:
	static const QString		stylesheet;

public:

	//! Class Constructor
	NavTree(QWidget * parent = 0);

	//! Class Destructor
	virtual ~NavTree();

};

#endif /* NAVTREE_HPP_ */
