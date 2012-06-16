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

#ifndef SITE_TILEDELEGATE_HPP_
#define SITE_TILEDELEGATE_HPP_

/**
 * @file src/mvf/site_tiledelegate.hpp
 * @brief Tile Delegate for DiveSite Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QPainter>
#include <QModelIndex>
#include <QRect>
#include <QSize>
#include <QStyleOptionViewItem>

#include "tiledelegate.hpp"

/**
 * @brief Dive Site Tile Delegate
 *
 * Delegate for drawing the Dive Site Tiles in a tiled view.  Each tile shows
 * the Site Name, Picture, Rating, Country Flag, and 5 information lines:
 * Place Name, Number of Dives, Max Depth, Average Depth, and Average Temp.
 */
class SiteTileDelegate: public TileDelegate
{
public:

	//! Class Constructor
	SiteTileDelegate(QObject * parent = 0);

	//! Class Destructor
	virtual ~SiteTileDelegate();

protected:

	//! @return Content Size
	virtual QSize contentSize(const QStyleOptionViewItem & option, const QModelIndex & index) const;

	//! @brief Paint Content
	virtual void paintContent(QPainter * painter, const QRect & rect, const QModelIndex & index) const;
};

#endif /* SITE_TILEDELEGATE_HPP_ */
