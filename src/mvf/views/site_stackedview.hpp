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

#ifndef SITE_STACKEDVIEW_HPP_
#define SITE_STACKEDVIEW_HPP_

/**
 * @file src/mvf/site_stackedview.hpp
 * @brief Stacked View for Dive Sites
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QIdentityProxyModel>
#include <QSortFilterProxyModel>
#include <QWidget>

#include "controls/stackedview.hpp"

/**
 * DiveSiteStack Widget
 *
 * Implements a Model-View Stack Widget for Dive Sites.  The supported view
 * modes are list view and tile view.
 */
class DiveSiteStack: public StackedView
{
	Q_OBJECT

public:

	//! Class Constructor
	DiveSiteStack(QWidget * parent = 0);

	//! Class Destructor
	virtual ~DiveSiteStack();

protected:

	//! @brief Create Proxy Models
	void createProxies();

	//! @brief Create View Widgets
	void createWidgets();

	//! @brief Create a new Editor Panel Instance for this Stacked View
	virtual IModelEditPanel * createEditor();

	//! @brief Read Settings for the Stacked View
	virtual void readSettings();

	//! @brief Write Settings for the Stacked View
	virtual void writeSettings();

protected slots:
	void onHeaderChanged();
	void onListSortChanged(int);
	void onTileSortChanged(int, Qt::SortOrder);
	void onMapViewChanged(QPointF, int, const QString &);

protected:
	QSortFilterProxyModel * 	m_listProxy;
	QSortFilterProxyModel * 	m_tileProxy;
	QSortFilterProxyModel *		m_mapProxy;

};

#endif /* SITE_STACKEDVIEW_HPP_ */
