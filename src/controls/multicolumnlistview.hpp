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

#ifndef MULTICOLUMNLISTVIEW_HPP_
#define MULTICOLUMNLISTVIEW_HPP_

/**
 * @file src/controls/multicolumnlistview.hpp
 * @brief Multi-Column List View Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QHeaderView>
#include <QMenu>
#include <QObject>
#include <QSettings>
#include <QSignalMapper>
#include <QTreeView>

#include "mvf/models.hpp"

/**
 * @brief Multi-Column List View
 *
 * Since Qt does not ship with a useful, multi-column list-view, this class
 * forces a Tree View to fulfill the same purpose.  This class should in
 * general be used for read-only list views (use QTableView for editable
 * views).
 *
 * This class also supports showing/hiding columns by right-clicking on the
 * header area.  For proper operation, only use this class with models which
 * descend from LiteSQLQueryModel and use ModelColumn column objects.
 */
class MultiColumnListView: public QTreeView
{
	Q_OBJECT

public:

	//! Class Constructor
	MultiColumnListView(QWidget * parent = 0);

	//! Class Destructor
	virtual ~MultiColumnListView();

	//! Initialize Model Columns
	virtual void initColumns();

	//! Load View State
	virtual void loadState(QSettings &);

	//! Save View State
	virtual void saveState(QSettings &);

	//! Set the Source Model
	virtual void setModel(QAbstractItemModel * model);

protected:

	//! Override drawBranches to hide all tree branches
	virtual void drawBranches(QPainter * painter, const QRect & rect, const QModelIndex & index) const;

	//! Return the CustomTableModel Source Model (or NULL)
	CustomTableModel * tableModel() const;

protected:
	QMenu *				m_ctxMenu;
	QSignalMapper *		m_mapper;

protected slots:
	void saveSections1(int logicalIndex, int oldSize, int newSize);
	void saveSections2(int logicalIndex, QHeaderView::ResizeMode mode);
	void saveSections3(int logicalIndex, int oldVisualIndex, int newVisualIndex);
	void showContextMenu(const QPoint & point);
	void toggleColumn(int i);

	virtual void currentChanged(const QModelIndex &, const QModelIndex &);
	virtual void selectionChanged(const QItemSelection &, const QItemSelection &);

signals:
	void headerChanged();
	void currentIndexChanged(const QModelIndex &, const QModelIndex &);
	void currentSelectionChanged(const QItemSelection &, const QItemSelection &);

};

#endif /* MULTICOLUMNLISTVIEW_HPP_ */
