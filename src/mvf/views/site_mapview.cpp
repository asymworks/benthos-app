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

#include <QSettings>
#include <QStringList>
#include <QUrl>
#include <QVBoxLayout>
#include <QWebFrame>
#include <QWebPage>

#include "controls/maplocationeditor.hpp"
#include "site_mapview.hpp"

#include <benthos/logbook/logging.hpp>
using namespace benthos::logbook;

SiteMapView::SiteMapView(QWidget * parent)
	: QWidget(parent), m_model(NULL), m_mapLoaded(false), m_viewLoaded(false),
	  m_center(0, 0), m_zoom(8), m_typeId("satellite")
{
	m_map = new QWebView(this);
	m_map->setPage(new ChromePage());

	connect(m_map->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(_attachObject()));
	connect(m_map, SIGNAL(loadFinished(bool)), this, SLOT(_viewLoaded(bool)));

	m_map->load(QUrl("qrc:/mapview/sitemap.html"));

	QVBoxLayout * vbox = new QVBoxLayout;
	vbox->setContentsMargins(0, 0, 0, 0);
	vbox->addWidget(m_map);

	setLayout(vbox);
}

SiteMapView::~SiteMapView()
{
}

void SiteMapView::_attachObject()
{
	m_map->page()->mainFrame()->addToJavaScriptWindowObject("MapView", this);
}

void SiteMapView::_mapLoaded()
{
	m_mapLoaded = true;
	initView();
}

void SiteMapView::_mapBoundsChanged(double latNE, double lngNE, double latSW, double lngSW)
{
}

void SiteMapView::_mapCenterChanged(double lat, double lng)
{
	if (! isVisible())
		return;

	m_center = QPointF(lng, lat);
	emit mapViewChanged(m_center, m_zoom, m_typeId);
}

void SiteMapView::_mapHeadingChanged(double hdg)
{
}

void SiteMapView::_mapTypeIdChanged(const QString & typeId)
{
	if (! isVisible())
		return;

	m_typeId = typeId;
	emit mapViewChanged(m_center, m_zoom, m_typeId);
}

void SiteMapView::_mapTiltChanged(double tilt)
{
}

void SiteMapView::_mapZoomChanged(int zoom)
{
	if (! isVisible())
		return;

	m_zoom = zoom;
	emit mapViewChanged(m_center, m_zoom, m_typeId);
}

void SiteMapView::_modelChanged(const QModelIndex &, int, int)
{
	resetSites();
}

void SiteMapView::_modelReset()
{
	resetSites();
}

void SiteMapView::_setMapBounds(const QPointF & ne, const QPointF & sw)
{
	m_map->page()->mainFrame()->evaluateJavaScript(QString("setBounds(%1, %2, %3, %4);").\
		arg(ne.y()).\
		arg(ne.x()).\
		arg(sw.y()).\
		arg(sw.x()));
}

void SiteMapView::_setMapCenter(const QPointF & c)
{
	m_map->page()->mainFrame()->evaluateJavaScript(QString("setCenter(%1, %2);").\
		arg(c.y()).\
		arg(c.x()));
}

void SiteMapView::_setMapHeading(double hdg)
{
	m_map->page()->mainFrame()->evaluateJavaScript(QString("setHeading(%1);").arg(hdg));
}

void SiteMapView::_setMapTypeId(const QString & id)
{
	m_map->page()->mainFrame()->evaluateJavaScript(QString("setTypeId('%1');").arg(id));
}

void SiteMapView::_setMapTilt(double tilt)
{
	m_map->page()->mainFrame()->evaluateJavaScript(QString("setTilt(%1);").arg(tilt));
}

void SiteMapView::_setMapZoom(int zoom)
{
	m_map->page()->mainFrame()->evaluateJavaScript(QString("setZoom(%1);").arg(zoom));
}

void SiteMapView::_tilesLoaded()
{
}

void SiteMapView::_viewLoaded(bool ok)
{
	if (! ok)
		logging::getLogger("gui.mapview")->error("Failed to load SiteMapView page");

	if (ok)
	{
		m_viewLoaded = true;
		initView();
	}
}

const QPointF & SiteMapView::center() const
{
	return m_center;
}

void SiteMapView::initView()
{
	if (m_viewLoaded && m_mapLoaded)
	{
		QPointF c = m_center;
		QString i = m_typeId;
		int z = m_zoom;

		m_map->page()->mainFrame()->evaluateJavaScript("resize();");

		_setMapZoom(z);
		_setMapTypeId(i);
		_setMapCenter(c);

		resetSites();
	}
}

QAbstractItemModel * SiteMapView::model() const
{
	return m_model;
}

void SiteMapView::resetSites()
{
	//FIXME: Use a real JSON library

	static QString _site("{"
		"\"site\":\"%1\","
		"\"place\":\"%2\","
		"\"country\":\"%3\","
		"\"code\":\"%4\","
		"\"ndives\":%5,"
		"\"rating\":%6,"
		"\"lat\":%7,"
		"\"lng\":%8}");
	QStringList _sites;

	int i;
	for (i = 0; i < m_model->rowCount(); ++i)
	{
		QString site 	= m_model->data(m_model->index(i, 0)).toString();
		QString place 	= m_model->data(m_model->index(i, 1)).toString();
		QString country = m_model->data(m_model->index(i, 2)).toString();
		QString code	= m_model->data(m_model->index(i, 2), Qt::EditRole).toString();
		int ndives		= m_model->data(m_model->index(i, 9)).toInt();
		double rating	= m_model->data(m_model->index(i, 10)).toDouble();
		QPointF ll		= m_model->data(m_model->index(i, 8)).toPointF();

		_sites.append(_site
				.arg(site)
				.arg(place)
				.arg(country)
				.arg(code)
				.arg(ndives)
				.arg(rating, 0, 'f', 2)
				.arg(ll.y(), 0, 'f', 6)
				.arg(ll.x(), 0, 'f', 6));
	}

	QString json = QString("[%1]").arg(_sites.join(",")).replace("\\", "\\\\").replace("\"", "\\\"");
	m_map->page()->mainFrame()->evaluateJavaScript(QString("reloadSites(\"%1\");").arg(json));
}

void SiteMapView::resizeEvent(QResizeEvent * e)
{
	initView();
	QWidget::resizeEvent(e);
}

void SiteMapView::setCenter(const QPointF & value)
{
	m_center = value;

	if (m_viewLoaded && m_mapLoaded)
		_setMapCenter(m_center);
}

void SiteMapView::setModel(QAbstractItemModel * model)
{
	if (m_model)
	{
		disconnect(m_model, SIGNAL(modelReset()), this, SLOT(_modelReset()));
		disconnect(m_model, SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(_modelChanged(const QModelIndex &, int, int)));
		disconnect(m_model, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(_modelChanged(const QModelIndex &, int, int)));
	}

	m_model = model;

	if (m_model)
	{
		connect(m_model, SIGNAL(modelReset()), this, SLOT(_modelReset()));
		connect(m_model, SIGNAL(rowsInserted(const QModelIndex &, int, int)), this, SLOT(_modelChanged(const QModelIndex &, int, int)));
		connect(m_model, SIGNAL(rowsRemoved(const QModelIndex &, int, int)), this, SLOT(_modelChanged(const QModelIndex &, int, int)));
	}
}

void SiteMapView::setTypeId(const QString & value)
{
	m_typeId = value;

	if (m_viewLoaded && m_mapLoaded)
		_setMapTypeId(m_typeId);
}

void SiteMapView::setZoom(int value)
{
	m_zoom = value;

	if (m_viewLoaded && m_mapLoaded)
		_setMapZoom(m_zoom);
}

void SiteMapView::showEvent(QShowEvent * e)
{
	initView();
	QWidget::showEvent(e);
}

const QString & SiteMapView::typeId() const
{
	return m_typeId;
}

int SiteMapView::zoom() const
{
	return m_zoom;
}
