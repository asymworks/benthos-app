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

#include "controls/multicolumnlistview.hpp"
#include "mvf/delegates.hpp"
#include "mvf/models/dive_model.hpp"

#include "dive_editpanel.hpp"
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
	MultiColumnListView * lv = new MultiColumnListView;
	lv->setModel(m_listProxy);
	lv->setSortingEnabled(true);
	lv->sortByColumn(-1, Qt::AscendingOrder);

	m_viewList[ListViewMode] = lv;
	addWidget(lv);

	m_viewMode = TileViewMode;

	readSettings();

	connect(lv, SIGNAL(headerChanged()), this, SLOT(onHeaderChanged()));
	connect(lv->header(), SIGNAL(sectionClicked(int)), this, SLOT(onListSortChanged(int)));
	connect(lv, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(showEditor(const QModelIndex &)));
}

void DiveStack::onHeaderChanged()
{
	writeSettings();
}

void DiveStack::onListSortChanged(int)
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
	((MultiColumnListView *)m_viewList[ListViewMode])->loadState(s);

	m_listProxy->sort(sc.toInt(), (Qt::SortOrder)so.toInt());
	((MultiColumnListView *)m_viewList[ListViewMode])->header()->setSortIndicator(sc.toInt(), (Qt::SortOrder)so.toInt());
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
	((MultiColumnListView *)m_viewList[ListViewMode])->saveState(s);
	s.setValue("sort_column", m_listProxy->sortColumn());
	s.setValue("sort_order", m_listProxy->sortOrder());
	s.endGroup();
}
