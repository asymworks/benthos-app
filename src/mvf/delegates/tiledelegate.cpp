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

#include <stdexcept>

#include <QApplication>
#include <QLinearGradient>
#include <QSettings>

#include "tiledelegate.hpp"

TileDelegate::TileDelegate(QObject * parent)
	: NoFocusDelegate(parent), m_margin(4), m_imgsize(64)
{
	m_textFont = QApplication::font("QListView");
	m_titleFont = QApplication::font("QListView");
	m_titleFont.setBold(true);

	QLinearGradient lg(0, 0, 0, 1);
	lg.setCoordinateMode(QGradient::ObjectBoundingMode);
	lg.setColorAt(0, QColor(187, 187, 187));
	lg.setColorAt(1, QColor(204, 204, 204));

	m_brush = QBrush(lg);
	m_pen = QPen(Qt::black);
}

TileDelegate::~TileDelegate()
{
}

const QBrush & TileDelegate::brush() const
{
	return m_brush;
}

QString TileDelegate::formatUnits(quantity_t quantity, double value, bool showAbbr)
{
	unit_t u;

	//! Lookup the Unit Abbreviation
	QSettings s;
	s.beginGroup("Settings");
	QVariant uname = s.value(QString("Unit%1").arg(quantity));
	s.endGroup();

	try
	{
		u = findUnit(quantity, (const char *)uname.toByteArray());
	}
	catch (std::runtime_error & e)
	{
		return QString("%1").arg(value, 0, 'f', 1);
	}

	//! Convert and Format
	if (showAbbr)
		return QString("%1 %2").arg(u.conv->fromNative(value), 0, 'f', 1).arg(QString::fromStdWString(u.abbr));
	else
		return QString("%1").arg(u.conv->fromNative(value), 0, 'f', 1);
}

int TileDelegate::imgSize() const
{
	return m_imgsize;
}

int TileDelegate::margin() const
{
	return m_margin;
}

void TileDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QStyle * style;
	QStyleOptionViewItemV4 opt(option);
	initStyleOption(& opt, index);

	// Filter the Style Option
	filter(& opt, index);

	// Initialize the Style
	if (opt.widget == NULL)
		style = QApplication::style();
	else
		style = opt.widget->style();

	// Draw the Background
	style->drawPrimitive(QStyle::PE_PanelItemViewItem, & opt, painter, opt.widget);

	// Begin Painting Content
	painter->save();

	painter->setRenderHint(QPainter::Antialiasing);
	painter->setRenderHint(QPainter::TextAntialiasing);

	painter->setBrush(m_brush);
	painter->setPen(m_pen);
	painter->setFont(m_textFont);

	// Draw the Content Rectangle
	QRect rContent = opt.rect.adjusted(m_margin, m_margin, -m_margin, -m_margin);
	painter->drawRoundedRect(rContent, 6, 6);

	// Draw the Tile Content
	paintContent(painter, rContent, index);

	// Finish Painting Content
	painter->restore();
}

const QPen & TileDelegate::pen() const
{
	return m_pen;
}

void TileDelegate::setBrush(const QBrush & brush)
{
	m_brush = brush;
}

void TileDelegate::setImgSize(int value)
{
	m_imgsize = value;
}

void TileDelegate::setMargin(int value)
{
	m_margin = value;
}

void TileDelegate::setPen(const QPen & pen)
{
	m_pen = pen;
}

void TileDelegate::setTextFont(const QFont & font)
{
	m_textFont = font;
}

void TileDelegate::setTitleFont(const QFont & font)
{
	m_titleFont = font;
}

QSize TileDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
	QSize cs = contentSize(option, index);
	return QSize(cs.width() + 2 * m_margin, cs.height() + 2 * m_margin);
}

const QFont & TileDelegate::textFont() const
{
	return m_textFont;
}

const QFont & TileDelegate::titleFont() const
{
	return m_titleFont;
}
