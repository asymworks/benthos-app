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

#ifndef SITE_MAPVIEW_HPP_
#define SITE_MAPVIEW_HPP_

/**
 * @file src/mvf/sitemapview.hpp
 * @brief Site Map View Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QPointF>
#include <QResizeEvent>
#include <QShowEvent>
#include <QWebView>
#include <QWidget>

#include <benthos/logbook/dive_site.hpp>
using namespace benthos::logbook;

/**
 * @brief Dive Site Map View
 *
 * Custom read-only item view which displays sites in a Google Maps view.
 * This relies on the Google Maps API "feature" that Chrome user agents with
 * version less than 5 do not use the Touch UI even though the ontouchstart
 * and friend events are enabled.
 */
class SiteMapView: public QWidget
{
	Q_OBJECT

public:

	//! Class Constructor
	SiteMapView(QWidget * parent = NULL);

	//! Class Destructor
	virtual ~SiteMapView();

	//! @return Map Center Position
	const QPointF & center() const;

	//! @return Model
	QAbstractItemModel * model() const;

	//! @param[in] Site Model
	void setModel(QAbstractItemModel * model);

	//! @param[in] Map Center Position
	void setCenter(const QPointF & value);

	//! @param[in] Map Type Id
	void setTypeId(const QString & value);

	//! @param[in] Map Zoom Level
	void setZoom(int value);

	//! @return Map Type Id
	const QString & typeId() const;

	//! @return Map Zoom Level
	int zoom() const;

protected:
	void resizeEvent(QResizeEvent * e);
	void showEvent(QShowEvent * e);

signals:
	void mapViewChanged(QPointF, int, const QString &);

public slots:
	void _attachObject();
	void _mapLoaded();
	void _mapBoundsChanged(double, double, double, double);
	void _mapCenterChanged(double, double);
	void _mapHeadingChanged(double);
	void _mapTypeIdChanged(const QString &);
	void _mapTiltChanged(double);
	void _mapZoomChanged(int);
	void _modelChanged(const QModelIndex &, int, int);
	void _modelReset();
	void _tilesLoaded();
	void _viewLoaded(bool);

private:
	void _setMapBounds(const QPointF &, const QPointF &);
	void _setMapCenter(const QPointF &);
	void _setMapHeading(double);
	void _setMapTypeId(const QString &);
	void _setMapTilt(double);
	void _setMapZoom(int);

private:
	void initView();
	void resetSites();

protected:
	QAbstractItemModel *			m_model;
	QWebView *						m_map;

	bool							m_mapLoaded;
	bool							m_viewLoaded;

	QPointF							m_center;
	int								m_zoom;
	QString							m_typeId;

};

#endif /* SITE_MAPVIEW_HPP_ */
