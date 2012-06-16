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

#ifndef DIVE_STACKEDVIEW_HPP_
#define DIVE_STACKEDVIEW_HPP_

/**
 * @file src/mvf/dive_stackedview.hpp
 * @brief Stacked View for Dives
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QSortFilterProxyModel>
#include <QWidget>

#include "controls/stackedview.hpp"

/**
 * DiveStack Widget
 *
 * Implements a Model-View Stack Widget for Dives.  The supported view
 * modes are list view, statistics view, and cover-flow view (upcoming).
 */
class DiveStack: public StackedView
{
	Q_OBJECT

public:

	//! Class Constructor
	DiveStack(QWidget * parent = 0);

	//! Class Destructor
	virtual ~DiveStack();

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

protected:
	QSortFilterProxyModel * 	m_listProxy;

};

#endif /* DIVE_STACKEDVIEW_HPP_ */
