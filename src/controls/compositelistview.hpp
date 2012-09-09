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

#ifndef COMPOSITELISTVIEW_HPP_
#define COMPOSITELISTVIEW_HPP_

/**
 * @file src/controls/compositelistview.hpp
 * @brief Composite List View Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QAbstractItemView>
#include <QModelIndex>
#include <QWidget>

#include <controls/benthositemview.hpp>

/**
 * @brief Composite List View Base Class
 *
 * Base class for controls which combine a QAbstractItemView object with other
 * view objects (e.g. the Tile View, Dive Profile, Cover Flow).  This class
 * exposes common QAbstractListView methods such as currentIndex() so that the
 * higher-level view objects can treat this as a standalone item view.
 *
 * Child classes must call setItemView() to set the actual QAbstractItemView
 * object which is wrapped by this class.
 */
class CompositeListView: public QWidget, public IBenthosItemView
{
	Q_OBJECT

public:

	//! Class Constructor
	CompositeListView(QWidget * parent = 0);

	//! Class Destructor
	virtual ~CompositeListView();

public:

	//! @brief Clear Selection
	virtual void clearSelection();

	//! @return Current Item Index
	virtual QModelIndex currentIndex() const;

	//! @return Item Selection Model
	virtual QItemSelectionModel * selectionModel() const;

	//! @return Wrapped Item View
	QAbstractItemView * view() const;

	//! @param[in] Item View to Wrap
	void setView(QAbstractItemView * v);

signals:
	void activated(const QModelIndex &);
	void clicked(const QModelIndex &);
	void doubleClicked(const QModelIndex &);

	void currentChanged(const QModelIndex &, const QModelIndex &);
	void selectionChanged(const QItemSelection &, const QItemSelection &);

public slots:
	void setCurrentIndex(const QModelIndex &);

protected slots:
	void onActivated(const QModelIndex &);
	void onClicked(const QModelIndex &);
	void onDoubleClicked(const QModelIndex &);

	void onCurrentChanged(const QModelIndex &, const QModelIndex &);
	void onSelectionChanged(const QItemSelection &, const QItemSelection &);

private:
	QAbstractItemView *			m_view;

};

#endif /* COMPOSITELISTVIEW_HPP_ */
