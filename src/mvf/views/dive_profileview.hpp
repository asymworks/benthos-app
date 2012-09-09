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

#ifndef DIVE_PROFILEVIEW_HPP_
#define DIVE_PROFILEVIEW_HPP_

/**
 * @brief src/mvf/views/dive_profileview.hpp
 * @brief Dive Profile View Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QSplitter>
#include <QTreeView>
#include <QWidget>

#include <controls/benthositemview.hpp>
#include <controls/multicolumnlistview.hpp>

#include "profile_view.hpp"

/**
 * @brief
 */
class DiveProfileView: public QWidget, public IBenthosItemView
{
	Q_OBJECT

public:

	//! Class Constructor
	DiveProfileView(QWidget * parent = 0);

	//! Class Destructor
	virtual ~DiveProfileView();

public:

	//! @brief Clear Selection
	virtual void clearSelection();

	//! @return Current Item Index
	virtual QModelIndex currentIndex() const;

	//! Load View State
	virtual void loadState(QSettings &);

	//! Save View State
	virtual void saveState(QSettings &);

	//! @return Item Selection Model
	virtual QItemSelectionModel * selectionModel() const;

	//! Set the Source Model
	virtual void setModel(QAbstractItemModel * model);

	//! Set the Sort Indicator
	virtual void setSortIndicator(int logicalIndex, Qt::SortOrder order);

protected slots:
	void onActivated(const QModelIndex &);
	void onClicked(const QModelIndex &);
	void onDoubleClicked(const QModelIndex &);
	void onHeaderChanged();
	void onSectionClicked(int);
	void onSplitterMoved(int, int);
	void onCurrentIndexChanged(const QModelIndex &, const QModelIndex &);
	void onCurrentSelectionChanged(const QItemSelection &, const QItemSelection &);

signals:
	void activated(const QModelIndex &);
	void clicked(const QModelIndex &);
	void doubleClicked(const QModelIndex &);
	void headerChanged();
	void sectionClicked(int);
	void splitterMoved(int, int);

	void currentIndexChanged(const QModelIndex &, const QModelIndex &);
	void currentSelectionChanged(const QItemSelection &, const QItemSelection &);

private:
	MultiColumnListView *		m_listview;
	ProfileView *				m_profile;
	QSplitter *					m_splitter;

};

#endif /* DIVE_PROFILEVIEW_HPP_ */
