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

#ifndef TILEVIEW_HPP_
#define TILEVIEW_HPP_

/**
 * @file src/mvf/tileview.hpp
 * @brief Tiled Item View Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QListView>
#include <QModelIndex>
#include <QWidget>

#include "mvf/modelcolumn.hpp"
#include "compositelistview.hpp"
#include "tileviewsorter.hpp"

/**
 * @brief Tiled Item View Class
 *
 * The Tiled Item View class combines a list view with a custom tile delegate
 * with a sorter button control to control item sorting.
 */
class TileView: public CompositeListView
{
	Q_OBJECT

public:

	//! Class Constructor
	TileView(const std::list<std::string> & columns, IDelegateFactory * delegateFactory, QWidget * parent = 0);

	//! Class Destructor
	virtual ~TileView();

public:

	//! @return Item Model
	QAbstractItemModel * model() const;

	//! @param[in] Item Model
	void setModel(QAbstractItemModel * m);

signals:
	void sortChanged(int, Qt::SortOrder);

public slots:
	void setSortColumn(int);
	void setSortOrder(Qt::SortOrder);

protected slots:
	void onSortChanged(int, Qt::SortOrder);

private:
	QListView *			m_listview;
	TileViewSorter *	m_sorter;

};

#endif /* TILEVIEW_HPP_ */
