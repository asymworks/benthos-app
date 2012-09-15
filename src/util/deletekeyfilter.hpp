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

#ifndef DELETEKEYFILTER_HPP_
#define DELETEKEYFILTER_HPP_

/**
 * @file src/util/deletekeyfilter.hpp
 * @brief Delete-Key Event Filter Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QEvent>
#include <QObject>

/**
 * @brief Delete-Key Event Filter
 *
 * Captures delete key presses and emits a signal.  Used to delete items from
 * item views without a separate QAction for the deletion.
 */
class DeleteKeyFilter: public QObject
{
	Q_OBJECT

public:
	DeleteKeyFilter(QObject * parent = 0);
	virtual ~DeleteKeyFilter();

signals:
	void deletePressed();

protected:
	bool eventFilter(QObject * obj, QEvent * event);

};

#endif /* DELETEKEYFILTER_HPP_ */
