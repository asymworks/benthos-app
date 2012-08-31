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

#include <QVBoxLayout>

#include "tileview.hpp"

TileView::TileView(const std::list<std::string> & columns, IDelegateFactory * delegateFactory, QWidget * parent)
	: CompositeListView(parent), m_listview(0), m_sorter(0)
{
	if (! delegateFactory)
		throw std::runtime_error("TileView requires a Custom Tile Delegate");

	m_listview = new QListView(this);
	m_listview->setUniformItemSizes(true);
	m_listview->setItemDelegate(delegateFactory->create());
	m_listview->setResizeMode(QListView::Adjust);
	m_listview->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_listview->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_listview->setSpacing(16);
	m_listview->setViewMode(QListView::IconMode);

	setView(m_listview);

	m_sorter = new TileViewSorter(columns, this);
	connect(m_sorter, SIGNAL(sortChanged(int, Qt::SortOrder)), this, SLOT(onSortChanged(int, Qt::SortOrder)));

	QVBoxLayout * vbox = new QVBoxLayout;
	vbox->setContentsMargins(0, 0, 0, 0);
	vbox->setSpacing(0);
	vbox->addWidget(m_listview);
	vbox->addWidget(m_sorter);

	setLayout(vbox);
}

TileView::~TileView()
{
}

QAbstractItemModel * TileView::model() const
{
	return m_listview->model();
}

void TileView::onSortChanged(int col, Qt::SortOrder ord)
{
	emit sortChanged(col, ord);
}

void TileView::setModel(QAbstractItemModel * m)
{
	m_listview->setModel(m);

	if (! m)
		m_sorter->setView(0);
	else
		m_sorter->setView(m_listview);

	/*
	 * Apparantly selectionModel() isn't created until setModel() is called
	 */
	connect(m_listview->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
		this, SLOT(onCurrentChanged(const QModelIndex &, const QModelIndex &)));
	connect(m_listview->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
		this, SLOT(onSelectionChanged(const QItemSelection &, const QItemSelection &)));
}

void TileView::setSortColumn(int col)
{
	m_sorter->setSortColumn(col);
}

void TileView::setSortOrder(Qt::SortOrder ord)
{
	m_sorter->setSortOrder(ord);
}
