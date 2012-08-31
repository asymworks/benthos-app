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

#ifndef BENTHOSITEMVIEW_HPP_
#define BENTHOSITEMVIEW_HPP_

/**
 * @brief src/controls/benthositemview.hpp
 * @brief Interface for Item Views
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QModelIndex>

/**
 * @brief Interface for Item Views
 *
 * This interface should be implemented by custom views which use the default
 * functionality in StackedView for editors.
 */
struct IBenthosItemView
{
	virtual ~IBenthosItemView() { }

	//! @return Current Model Index
	virtual QModelIndex currentIndex() const = 0;

	//! @return Item Selection Model
	virtual QItemSelectionModel * selectionModel() const = 0;

};

#endif /* BENTHOSITEMVIEW_HPP_ */
