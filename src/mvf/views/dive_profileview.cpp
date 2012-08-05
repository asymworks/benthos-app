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

#include "dive_profileview.hpp"

DiveProfileView::DiveProfileView(QWidget * parent)
	: QWidget(parent), m_listview(0)
{
	m_listview = new MultiColumnListView;
	m_listview->setSortingEnabled(true);
	m_listview->sortByColumn(-1, Qt::AscendingOrder);

	connect(m_listview, SIGNAL(activated(const QModelIndex &)), this, SLOT(onActivated(const QModelIndex &)));
	connect(m_listview, SIGNAL(clicked(const QModelIndex &)), this, SLOT(onClicked(const QModelIndex &)));
	connect(m_listview, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onDoubleClicked(const QModelIndex &)));
	connect(m_listview, SIGNAL(headerChanged()), this, SLOT(onHeaderChanged()));
	connect(m_listview->header(), SIGNAL(sectionClicked(int)), this, SLOT(onSectionClicked(int)));

	connect(m_listview, SIGNAL(currentIndexChanged(const QModelIndex &, const QModelIndex &)), this, SLOT(onCurrentIndexChanged(const QModelIndex &, const QModelIndex &)));
	connect(m_listview, SIGNAL(currentSelectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(onCurrentSelectionChanged(const QItemSelection &, const QItemSelection &)));

	QVBoxLayout * vbox = new QVBoxLayout;
	vbox->setContentsMargins(0, 0, 0, 0);
	vbox->addSpacing(400);
	vbox->addWidget(m_listview);

	setLayout(vbox);
}

DiveProfileView::~DiveProfileView()
{
}

QModelIndex DiveProfileView::currentIndex() const
{
	return m_listview->currentIndex();
}

void DiveProfileView::loadState(QSettings & s)
{
	m_listview->loadState(s);
}

void DiveProfileView::onActivated(const QModelIndex & arg)
{
	emit activated(arg);
}

void DiveProfileView::onClicked(const QModelIndex & arg)
{
	emit clicked(arg);
}

void DiveProfileView::onDoubleClicked(const QModelIndex & arg)
{
	emit doubleClicked(arg);
}

void DiveProfileView::onHeaderChanged()
{
	emit headerChanged();
}

void DiveProfileView::onSectionClicked(int arg)
{
	emit sectionClicked(arg);
}

void DiveProfileView::onCurrentIndexChanged(const QModelIndex & arg1, const QModelIndex & arg2)
{
	emit currentIndexChanged(arg1, arg2);
}

void DiveProfileView::onCurrentSelectionChanged(const QItemSelection & arg1, const QItemSelection & arg2)
{
	emit currentSelectionChanged(arg1, arg2);
}

void DiveProfileView::saveState(QSettings & s)
{
	m_listview->saveState(s);
}

void DiveProfileView::setModel(QAbstractItemModel * model)
{
	m_listview->setModel(model);
}

void DiveProfileView::setSortIndicator(int logicalIndex, Qt::SortOrder order)
{
	m_listview->header()->setSortIndicator(logicalIndex, order);
}
