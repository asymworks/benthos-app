/*
 * Copyright (C) 2011 Asymworks, LLC.  All Rights Reserved.
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

#include "mvf/delegates.hpp"
#include "mvf/models/dive_model.hpp"

#include "dive_editpanel.hpp"
#include "dive_profileview.hpp"
#include "dive_stackedview.hpp"

ModelFactory<DiveModel> dive_mf;

DiveStack::DiveStack(QWidget * parent)
	: StackedView(& dive_mf, parent), m_listProxy(0)
{
	createProxies();
	createWidgets();
}

DiveStack::~DiveStack()
{
}

IModelEditPanel * DiveStack::createEditor()
{
	return new DiveEditPanel;
}

void DiveStack::createProxies()
{
	m_listProxy = new QSortFilterProxyModel;
	m_listProxy->setDynamicSortFilter(true);
	m_listProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
	m_listProxy->setFilterKeyColumn(-1);
	m_listProxy->setSourceModel(m_model);

	m_proxyList[ListViewMode] = m_listProxy;
}

void DiveStack::createWidgets()
{
	DiveProfileView * pv = new DiveProfileView;
	pv->setModel(m_listProxy);

	m_viewList[ListViewMode] = pv;
	addWidget(pv);

	m_viewMode = TileViewMode;

	readSettings();

	connect(pv, SIGNAL(headerChanged()), this, SLOT(onHeaderChanged()));
	connect(pv, SIGNAL(sectionClicked(int)), this, SLOT(onListSortChanged(int)));
	connect(pv, SIGNAL(splitterMoved(int, int)), this, SLOT(onSplitterMoved(int, int)));
	connect(pv, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(showEditor(const QModelIndex &)));

	connect(pv, SIGNAL(currentIndexChanged(const QModelIndex &, const QModelIndex &)),
			this, SLOT(onViewCurrentChanged(const QModelIndex &, const QModelIndex &)));

	connect(pv, SIGNAL(currentSelectionChanged(const QItemSelection &, const QItemSelection &)),
			this, SLOT(onViewSelectionChanged(const QItemSelection &, const QItemSelection &)));
}

void DiveStack::onHeaderChanged()
{
	writeSettings();
}

void DiveStack::onListSortChanged(int)
{
	writeSettings();
}

void DiveStack::onSplitterMoved(int, int)
{
	writeSettings();
}

void DiveStack::readSettings()
{
	QSettings s;

	QVariant sc;
	QVariant so;

	// Load List View Properties
	s.beginGroup(QString("%1/ListView").arg(metaObject()->className()));
	sc = s.value("sort_column", -1);
	so = s.value("sort_order", Qt::AscendingOrder);
	((DiveProfileView *)m_viewList[ListViewMode])->loadState(s);

	m_listProxy->sort(sc.toInt(), (Qt::SortOrder)so.toInt());
	((DiveProfileView *)m_viewList[ListViewMode])->setSortIndicator(sc.toInt(), (Qt::SortOrder)so.toInt());
	s.endGroup();

	// Load the base settings
	StackedView::readSettings();
}

void DiveStack::writeSettings()
{
	QSettings s;

	// Save the base settings
	StackedView::writeSettings();

	// Save List View Properties
	s.beginGroup(QString("%1/ListView").arg(metaObject()->className()));
	((DiveProfileView *)m_viewList[ListViewMode])->saveState(s);
	s.setValue("sort_column", m_listProxy->sortColumn());
	s.setValue("sort_order", m_listProxy->sortOrder());
	s.endGroup();
}
