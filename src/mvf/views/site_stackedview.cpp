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
#include "controls/tileview.hpp"

#include "mvf/delegates.hpp"

#include "mvf/delegates/site_tiledelegate.hpp"
#include "mvf/models/site_model.hpp"

#include "site_editpanel.hpp"
#include "site_mapview.hpp"
#include "site_stackedview.hpp"

ModelFactory<DiveSiteModel> site_mf;

DiveSiteStack::DiveSiteStack(QWidget * parent)
	: StackedView(& site_mf, parent), m_listProxy(0), m_tileProxy(0)
{
	createProxies();
	createWidgets();
}

DiveSiteStack::~DiveSiteStack()
{
}

IModelEditPanel * DiveSiteStack::createEditor()
{
	return new SiteEditPanel;
}

void DiveSiteStack::createProxies()
{
	m_listProxy = new QSortFilterProxyModel;
	m_listProxy->setDynamicSortFilter(true);
	m_listProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
	m_listProxy->setFilterKeyColumn(-1);
	m_listProxy->setSourceModel(m_model);

	m_tileProxy = new QSortFilterProxyModel;
	m_tileProxy->setDynamicSortFilter(true);
	m_tileProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
	m_tileProxy->setFilterKeyColumn(-1);
	m_tileProxy->setSourceModel(m_model);

	m_mapProxy = new QSortFilterProxyModel;
	m_mapProxy->setDynamicSortFilter(true);
	m_mapProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
	m_mapProxy->setFilterKeyColumn(-1);
	m_mapProxy->setSourceModel(m_model);

	m_proxyList[ListViewMode] = m_listProxy;
	m_proxyList[TileViewMode] = m_tileProxy;
	m_proxyList[MapViewMode]  = m_mapProxy;
}

void DiveSiteStack::createWidgets()
{
	std::list<std::string> cols;
	cols.push_back("name");
	cols.push_back("country");
	cols.push_back("num_dives");
	cols.push_back("rating");

	TileView * tv = new TileView(cols, new DelegateFactory<SiteTileDelegate>);
	tv->setModel(m_tileProxy);

	MultiColumnListView * lv = new MultiColumnListView;
	lv->setModel(m_listProxy);
	lv->setSortingEnabled(true);
	lv->sortByColumn(-1, Qt::AscendingOrder);

	SiteMapView * mv = new SiteMapView;
	mv->setModel(m_mapProxy);

	m_viewList[TileViewMode] = tv;
	m_viewList[ListViewMode] = lv;
	m_viewList[MapViewMode] = mv;

	addWidget(tv);
	addWidget(lv);
	addWidget(mv);

	m_viewMode = TileViewMode;

	readSettings();

	connect(tv, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(showEditor(const QModelIndex &)));
	connect(tv, SIGNAL(sortChanged(int, Qt::SortOrder)), this, SLOT(onTileSortChanged(int, Qt::SortOrder)));
	connect(tv, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
		this, SLOT(onViewCurrentChanged(const QModelIndex &, const QModelIndex &)));
	connect(tv, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
		this, SLOT(onViewSelectionChanged(const QItemSelection &, const QItemSelection &)));

	connect(lv, SIGNAL(headerChanged()), this, SLOT(onHeaderChanged()));
	connect(lv->header(), SIGNAL(sectionClicked(int)), this, SLOT(onListSortChanged(int)));
	connect(lv, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(showEditor(const QModelIndex &)));;
	connect(lv->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
		this, SLOT(onViewCurrentChanged(const QModelIndex &, const QModelIndex &)));
	connect(lv->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
		this, SLOT(onViewSelectionChanged(const QItemSelection &, const QItemSelection &)));

	connect(mv, SIGNAL(mapViewChanged(QPointF, int, const QString &)), this, SLOT(onMapViewChanged(QPointF, int, const QString &)));
}

void DiveSiteStack::onHeaderChanged()
{
	writeSettings();
}

void DiveSiteStack::onMapViewChanged(QPointF, int, const QString &)
{
	writeSettings();
}

void DiveSiteStack::onListSortChanged(int)
{
	writeSettings();
}

void DiveSiteStack::onTileSortChanged(int, Qt::SortOrder)
{
	writeSettings();
}

void DiveSiteStack::readSettings()
{
	QSettings s;

	QVariant sc;
	QVariant so;
	QVariant mp;
	QVariant mz;
	QVariant mt;

	// Load List View Properties
	s.beginGroup(QString("%1/ListView").arg(metaObject()->className()));
	sc = s.value("sort_column", -1);
	so = s.value("sort_order", Qt::AscendingOrder);
	((MultiColumnListView *)m_viewList[ListViewMode])->loadState(s);

	m_listProxy->sort(sc.toInt(), (Qt::SortOrder)so.toInt());
	((MultiColumnListView *)m_viewList[ListViewMode])->header()->setSortIndicator(sc.toInt(), (Qt::SortOrder)so.toInt());
	s.endGroup();

	// Load Tile View Properties
	s.beginGroup(QString("%1/TileView").arg(metaObject()->className()));
	sc = s.value("sort_column", -1);
	so = s.value("sort_order", Qt::AscendingOrder);

	m_tileProxy->sort(sc.toInt(), (Qt::SortOrder)so.toInt());
	((TileView *)m_viewList[TileViewMode])->setSortColumn(sc.toInt());
	((TileView *)m_viewList[TileViewMode])->setSortOrder((Qt::SortOrder)so.toInt());
	s.endGroup();

	// Load Map View Properties
	s.beginGroup(QString("%1/MapView").arg(metaObject()->className()));
	mp = s.value("center", QVariant(QPointF(0, 0)));
	mz = s.value("zoom", QVariant((int)7));
	mt = s.value("type", QVariant((int)0));

	((SiteMapView *)m_viewList[MapViewMode])->setCenter(mp.toPointF());
	((SiteMapView *)m_viewList[MapViewMode])->setZoom(mz.toInt());
	((SiteMapView *)m_viewList[MapViewMode])->setTypeId(mt.toString());
	s.endGroup();

	// Load the base settings
	StackedView::readSettings();
}

void DiveSiteStack::writeSettings()
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

	// Save Tile View Properties
	s.beginGroup(QString("%1/TileView").arg(metaObject()->className()));
	s.setValue("sort_column", m_tileProxy->sortColumn());
	s.setValue("sort_order", m_tileProxy->sortOrder());
	s.endGroup();

	// Save Map View Properties
	s.beginGroup(QString("%1/MapView").arg(metaObject()->className()));
	s.setValue("center", ((SiteMapView *)m_viewList[MapViewMode])->center());
	s.setValue("zoom", ((SiteMapView *)m_viewList[MapViewMode])->zoom());
	s.setValue("type", ((SiteMapView *)m_viewList[MapViewMode])->typeId());
	s.endGroup();
}
