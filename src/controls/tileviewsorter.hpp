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

#ifndef TILEVIEWSORTER_HPP_
#define TILEVIEWSORTER_HPP_

/**
 * @file src/controls/tileviewsorter.hpp
 * @brief Tile View Sorter Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <list>
#include <string>
#include <vector>
#include <utility>

#include <QAbstractItemView>
#include <QButtonGroup>
#include <QFrame>
#include <QPushButton>

#include "mvf/modelcolumn.hpp"

/**
 * @brief Tile View Sorter Widget Class
 *
 * Custom-drawn control which implements a sorting widget for custom item
 * views which do not have a header view item.  The control draws buttons for
 * each sortable column and for ascending/descending sort order.  An item view
 * is linked to the View Sorter and automatically updated when the sort order
 * changes.
 *
 * The semantics require that the custom item view have a QSortFilterProxyModel
 * set as the view's model, and that the QSortFilterProxyModel's source model
 * be zero or more QAbstractProxyModels which have an ultimate source model
 * which subclasses LogbookQueryModel.
 *
 * This is true for all use cases in benthos but is important to note for
 * third-party/future use.
 */
class TileViewSorter: public QFrame
{
	Q_OBJECT

protected:
	static const QString		stylesheet;

public:

	/**
	 * @brief Class Constructor
	 * @param[in] List of Model Column Names
	 * @param[in] Parent Object
	 */
	TileViewSorter(const std::list<std::string> & columns, QWidget * parent = 0);

	//! Class Destructor
	virtual ~TileViewSorter();

public:

	//! @return Linked Item View
	QAbstractItemView * view() const;

	//! @param[in] Item View to Link
	void setView(QAbstractItemView * v);

	//! @return Sort Column
	int sortColumn() const;

	//! @return Sort Order
	Qt::SortOrder sortOrder() const;

signals:
	void sortChanged(int, Qt::SortOrder);

public slots:
	void setSortColumn(int);
	void setSortOrder(Qt::SortOrder);

protected slots:
	void setSortOrderByIndex(int);

private:
	QAbstractItemView *										m_view;				///< Item View
	std::list<std::string>									m_sortColumns;		///< List of Columns on which to Sort
	int														m_sortColumn;		///< Current Column on which to Sort
	Qt::SortOrder											m_sortOrder;		///< Current Sort Ordering

	std::list<std::pair<std::string, QPushButton *>	>		m_colButtons;		///< List of Column Sorter Buttons
	std::list<std::pair<Qt::SortOrder, QPushButton *> >		m_dirButtons;		///< List of Column Ordering Buttons

	QButtonGroup *											m_bgSortCol;		///< Column Sorter Button Group
	QButtonGroup *											m_bgSortDir;		///< Column Ordering Button Group

};

#endif /* TILEVIEWSORTER_HPP_ */
