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

#ifndef MAPLOCATIONEDITOR_HPP_
#define MAPLOCATIONEDITOR_HPP_

/**
 * @file src/controls/maplocationeditor.hpp
 * @brief Map Location Editor Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QEvent>
#include <QLineEdit>
#include <QPointF>
#include <QUrl>
#include <QWebPage>
#include <QWebView>
#include <QWidget>

/**
 * @brief Chrome User Agent Web Page
 *
 * QWebPage subclass which uses the 'Chrome/1.0' user agent to fool Google
 * Maps into sending a non-touch UI to webkit.
 */
class ChromePage: public QWebPage
{
public:

	//! Class Constructor
	ChromePage(QObject * parent = 0);

	//! Class Destructor
	virtual ~ChromePage();

protected:

	//! Print JavaScript Messages to the Logger
	virtual void javaScriptConsoleMessage(const QString & message, int lineNumber, const QString & sourceID);

	//! Pretend to be Chrome/5.0 or below
	virtual QString userAgentForUrl(const QUrl & url) const;
};

/**
 * @brief MapLocationEditor
 *
 * Provides an editor control which uses a Google Maps object to graphically
 * choose a latitude/longitude location coordinate.  The widget includes an
 * embedded search box which can be used to obtain coordinates from a geocode
 * string using the Google API.
 */
class MapLocationEditor: public QWidget
{
	Q_OBJECT

public:
	Q_PROPERTY(QPointF position
		READ position
		WRITE setPosition
		NOTIFY positionChanged
		USER true)

	//! Class Constructor
	MapLocationEditor(QWidget * parent = 0);

	//! Class Destructor
	virtual ~MapLocationEditor();

	//! @return Current Latitude
	double latitude() const;

	//! @return Current Longitude
	double longitude() const;

	//! @return Current Position
	QPointF position() const;

	//! Return the Size Hint
	virtual QSize sizeHint() const;

protected:

	//! Capture a Resize Event
	virtual void resizeEvent(QResizeEvent * e);

	//! Capture a Show Event
	virtual void showEvent(QShowEvent * e);

public slots:

	//! Called when the Map is loaded
	void _mapLoaded();

	//! Searches with the Geocode API
	void _search();

	//! Reset the Center Position of the Map (called by JavaScript)
	void _setPosition(double latitude, double longitude);

	//! Set the Center Position of the Map
	void setPosition(const QPointF & position);

signals:

	//! Map Position Changed
	void positionChanged(const QPointF & position);

private:
	double		m_latitude;
	double		m_longitude;

private:
	QLineEdit *	m_txtSearch;
	QWebView *	m_map;

};

#endif /* MAPLOCATIONEDITOR_HPP_ */
