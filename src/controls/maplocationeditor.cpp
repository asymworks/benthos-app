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

#include <QWebFrame>
#include <QVBoxLayout>

#include "maplocationeditor.hpp"
#include "searchedit.hpp"

#include <benthos/logbook/logging.hpp>
using namespace benthos::logbook;

ChromePage::ChromePage(QObject * parent)
	: QWebPage(parent)
{
}

ChromePage::~ChromePage()
{
}

void ChromePage::javaScriptConsoleMessage(const QString & message, int lineNumber, const QString & sourceId)
{
	logging::logger * l = logging::getLogger("gui.javascript");
	l->info("JavaScript: %s [%s:%d]", message.toAscii().data(), sourceId.toAscii().data(), lineNumber);
}

QString ChromePage::userAgentForUrl(const QUrl & url) const
{
	return QString("Chrome/5.0");
}

MapLocationEditor::MapLocationEditor(QWidget * parent)
	: QWidget(parent), m_latitude(0.0), m_longitude(0.0)
{
	m_txtSearch = new SearchEdit();
	connect(m_txtSearch, SIGNAL(returnPressed()), this, SLOT(_search()));

	m_map = new QWebView();
	m_map->setPage(new ChromePage());
	m_map->load(QUrl("qrc:/mapview/locpick.html"));
	m_map->page()->mainFrame()->addToJavaScriptWindowObject("MapLocationEditor", this);

	QVBoxLayout * vbox = new QVBoxLayout();
	vbox->addWidget(m_txtSearch);
	vbox->addWidget(m_map);

	setLayout(vbox);
}

MapLocationEditor::~MapLocationEditor()
{
}

void MapLocationEditor::_mapLoaded()
{
	// This is an ugly hack, but I don't know a good way around it right
	// now unless the editor is constructed well before the first model item
	// is selected.  Otherwise, we have to trigger off the map initialization
	// event and reload the marker position.
	m_map->page()->mainFrame()->evaluateJavaScript(QString("setPosition(%1, %2);").arg(m_latitude).arg(m_longitude));
}

void MapLocationEditor::_search()
{
	QString s = m_txtSearch->text();
	if (! s.isEmpty())
		m_map->page()->mainFrame()->evaluateJavaScript(QString("geocode(\"%1\");").arg(s));
}

void MapLocationEditor::_setPosition(double latitude, double longitude)
{
	m_latitude = latitude;
	m_longitude = longitude;
	emit positionChanged(QPointF(longitude, latitude));
}

double MapLocationEditor::latitude() const
{
	return m_latitude;
}

double MapLocationEditor::longitude() const
{
	return m_longitude;
}

QPointF MapLocationEditor::position() const
{
	return QPointF(m_longitude, m_latitude);
}

void MapLocationEditor::resizeEvent(QResizeEvent * e)
{
	m_map->page()->mainFrame()->evaluateJavaScript("resize();");
	QWidget::resizeEvent(e);
}

void MapLocationEditor::setPosition(const QPointF & position)
{
	m_latitude = position.y();
	m_longitude = position.x();
	m_map->page()->mainFrame()->evaluateJavaScript(QString("setPosition(%1, %2);").arg(m_latitude).arg(m_longitude));
}

void MapLocationEditor::showEvent(QShowEvent * e)
{
	m_map->page()->mainFrame()->evaluateJavaScript("resize();");
	QWidget::showEvent(e);
}

QSize MapLocationEditor::sizeHint() const
{
	return QSize(500, 300);
}
