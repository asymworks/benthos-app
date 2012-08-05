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

#include <QAbstractProxyModel>
#include <QAction>
#include <QMenu>

#include <vector>

#include "mvf/delegates.hpp"
#include "mvf/modelcolumn.hpp"
#include "multicolumnlistview.hpp"

MultiColumnListView::MultiColumnListView(QWidget * parent)
	: QTreeView(parent)
{
	setAlternatingRowColors(true);
	setItemDelegate(new NoFocusDelegate());
	setItemsExpandable(false);
	setIndentation(0);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	setUniformRowHeights(true);

	m_mapper = new QSignalMapper(this);
	m_ctxMenu = new QMenu(this);

	header()->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(header(), SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));
	connect(header(), SIGNAL(sectionResized(int, int, int)), this, SLOT(saveSections1(int, int, int)));
	connect(header(), SIGNAL(sectionAutoResize(int, QHeaderView::ResizeMode)), this, SLOT(saveSections2(int, QHeaderView::ResizeMode)));
	connect(header(), SIGNAL(sectionMoved(int, int, int)), this, SLOT(saveSections3(int, int, int)));
}

MultiColumnListView::~MultiColumnListView()
{
}

void MultiColumnListView::currentChanged(const QModelIndex & current, const QModelIndex & previous)
{
	QTreeView::currentChanged(current, previous);
	emit currentIndexChanged(current, previous);
}

void MultiColumnListView::drawBranches(QPainter * painter, const QRect & rect, const QModelIndex & index) const
{
}

void MultiColumnListView::initColumns()
{
	// Check that we're using a LiteSQL model (should always be true...)
	CustomTableModel * qmdl = tableModel();
	if (qmdl == NULL)
		return;

	// Delete and re-create the Mapper and Context Menu
	delete m_mapper;
	delete m_ctxMenu;

	m_mapper = new QSignalMapper(this);
	m_ctxMenu = new QMenu(this);

	// Loop through columns to initialize the view
	const std::vector<BaseModelColumn *> & cols = qmdl->columns();
	for (size_t i = 0; i < cols.size(); ++i)
	{
		BaseModelColumn * col = cols[i];

		// Setup Visibility and Delegate
		setColumnHidden(i, col->isHidden() || col->isInternal());
		if (col->hasDelegate())
			setItemDelegateForColumn(i, col->createDelegate());

		if (col->isInternal())
			continue;

		// Add Context Menu Item
		QAction * a = new QAction(qmdl->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString(), m_ctxMenu);
		a->setCheckable(true);
		a->setChecked(not isColumnHidden(i));
		m_ctxMenu->addAction(a);

		m_mapper->setMapping(a, i);
		connect(a, SIGNAL(triggered()), m_mapper, SLOT(map()));
	}

	connect(m_mapper, SIGNAL(mapped(int)), this, SLOT(toggleColumn(int)));
}

void MultiColumnListView::loadState(QSettings & s)
{
	QStringList cw = s.value("col_width").toString().split(",");
	QStringList cv = s.value("col_vidx").toString().split(",");
	int i;

	for (i = 0; i < cv.length(); i++)
	{
		if (cv.at(i).isEmpty())
			continue;

		int vidx = cv.at(i).toInt();
		if (vidx != i)
			header()->moveSection(header()->visualIndex(i), vidx);
	}

	for (i = 0; i < cw.length(); i++)
	{
		if (cw.at(i).isEmpty())
			continue;

		int width = cw.at(i).toInt();
		if (! width)
			header()->hideSection(i);
		else
			header()->resizeSection(i, width);
	}
}

void MultiColumnListView::saveSections1(int logicalIndex, int oldSize, int newSize)
{
	emit headerChanged();
}

void MultiColumnListView::saveSections2(int logicalIndex, QHeaderView::ResizeMode mode)
{
	emit headerChanged();
}

void MultiColumnListView::saveSections3(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
	emit headerChanged();
}

void MultiColumnListView::saveState(QSettings & s)
{
	QStringList col_width;
	QStringList col_vidx;

	int i;
	for (i = 0; i < header()->count(); i++)
	{
		col_width.append(QString::number(header()->sectionSize(i)));
		col_vidx.append(QString::number(header()->visualIndex(i)));
	}

	s.setValue("col_width", col_width.join(","));
	s.setValue("col_vidx", col_vidx.join(","));
}

void MultiColumnListView::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
	QTreeView::selectionChanged(selected, deselected);
	emit currentSelectionChanged(selected, deselected);
}

void MultiColumnListView::setModel(QAbstractItemModel * model)
{
	QTreeView::setModel(model);
	initColumns();
}

void MultiColumnListView::showContextMenu(const QPoint & point)
{
	// Check that we're using a LiteSQL model (should always be true...)
	CustomTableModel * qmdl = tableModel();
	if (qmdl == NULL)
		return;

	// Generate and show the Menu
	QMenu * m = new QMenu();
	const std::vector<BaseModelColumn *> & cols = qmdl->columns();
	for (size_t i = 0; i < cols.size(); ++i)
	{
		if (cols[i]->isInternal())
			continue;

		QAction * a = new QAction(qmdl->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString(), m);
		a->setCheckable(true);
		a->setChecked(not isColumnHidden(i));
		m->addAction(a);

		m_mapper->setMapping(a, i);

		connect(a, SIGNAL(triggered()), m_mapper, SLOT(map()));
	}

	m->exec(mapToGlobal(point));
}

CustomTableModel * MultiColumnListView::tableModel() const
{
	if (model() == NULL)
		return NULL;

	// Skip past Proxy Models
	QAbstractItemModel * mdl = model();
	QAbstractProxyModel * pmdl;
	while ((pmdl = dynamic_cast<QAbstractProxyModel *>(mdl)) != NULL)
		mdl = pmdl->sourceModel();

	// Return LiteSQL model
	return dynamic_cast<CustomTableModel *>(mdl);
}

void MultiColumnListView::toggleColumn(int i)
{
	setColumnHidden(i, not isColumnHidden(i));
	emit headerChanged();
}
