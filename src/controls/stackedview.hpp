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

#ifndef STACKEDVIEW_HPP_
#define STACKEDVIEW_HPP_

/**
 * @file src/controls/stackedview.hpp
 * @brief Stacked Model-View Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <map>

#include <QModelIndex>
#include <QSortFilterProxyModel>
#include <QStackedWidget>
#include <QWidget>

#include <benthos/logbook/logbook.hpp>
using namespace benthos::logbook;

#include "mvf/modeleditpanel.hpp"

struct IModelFactory
{
	virtual ~IModelFactory() { }
	virtual QAbstractItemModel * create() const = 0;
};

template <class M>
struct ModelFactory: public IModelFactory
{
	virtual ~ModelFactory() { }
	virtual QAbstractItemModel * create() const { return new M; }
};

/**
 * Model View Stack Widget
 *
 * The Model View Stack widget encapsulates multiple view modes for a single
 * model class, e.g. both a list and tile view of Dive Sites.
 */
class StackedView: public QStackedWidget
{
	Q_OBJECT

public:

	/**
	 * List of View Modes
	 */
	enum ViewMode
	{
		InvalidViewMode = -1,
		ListViewMode = 0,
		TileViewMode = 1,
		StatsViewMode = 2,
		CFViewMode = 3,
		MapViewMode = 4,
		UserViewMode = 16,
	};

public:

	//! Class Constructor
	StackedView(IModelFactory * mfactory, QWidget * parent = 0);

	//! Class Destructor
	virtual ~StackedView();

public:

	//! @param[in] Logbook Pointer
	void bind(Logbook::Ptr logbook);

	//! @return Current Model Index
	QModelIndex currentModelIndex() const;

	//! @return Current Model Index for a View Mode
	QModelIndex currentModelIndexForMode(ViewMode vm) const;

	//! @return Filter String
	const QString & filter_string() const;

	//! @return If the ViewMode is supported by this View
	bool hasViewMode(ViewMode vm) const;

	//! @return Model Instance
	QAbstractItemModel * model() const;

	//! @param[in] New Filter String
	void setFilterString(const QString & filter);

	//! @return Current View Mode
	ViewMode view_mode() const;

public slots:

	//! @brief Refresh the View
	void refreshView();

	//! @param[in] New View Mode
	void setViewMode(ViewMode);

	//! @brief Show the Editor
	void showEditor(const QModelIndex &);

signals:
	void viewModeChanged(ViewMode vm);

protected:

	//! @brief Create a new Editor Panel Instance for this Stacked View
	virtual IModelEditPanel * createEditor() = 0;

	//! @brief Read Settings for the Stacked View
	virtual void readSettings();

	//! @brief Write Settings for the Stacked View
	virtual void writeSettings();

protected:
	ViewMode										m_viewMode;
	std::map<ViewMode, QWidget *>					m_viewList;
	std::map<ViewMode, QSortFilterProxyModel *>		m_proxyList;

	QString											m_filter;
	QAbstractItemModel *							m_model;

	Logbook::Ptr									m_logbook;

};

#endif /* STACKEDVIEW_HPP_ */
