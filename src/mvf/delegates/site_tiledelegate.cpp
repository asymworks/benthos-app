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

#include <QFontMetrics>
#include <QImage>
#include <QPainter>

#include <sstream>

#include "site_tiledelegate.hpp"

#include <benthos/logbook/dive_site.hpp>
using namespace benthos::logbook;

SiteTileDelegate::SiteTileDelegate(QObject * parent)
	: TileDelegate(parent)
{
}

SiteTileDelegate::~SiteTileDelegate()
{
}

QSize SiteTileDelegate::contentSize(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QFontMetrics fmT(m_titleFont);
	QFontMetrics fmt(m_textFont);

	double ht = fmT.height() * 1.75 + fmt.height() * 7;
	double wt = ht * 1.9;

	return QSize((int)wt, (int)ht);
}

void SiteTileDelegate::paintContent(QPainter * painter, const QRect & rect, const QModelIndex & index) const
{
	QFontMetrics fmT(m_titleFont);
	DiveSite::Ptr obj = TileDelegate::rowObject<DiveSite>(index);
	if (! obj)
		return;

	// Setup Drawing Rects
	QRect rTitle(rect.x(), rect.y(), rect.width(), (int)(fmT.height() * 1.75));
	QRect rText(rect.x() + m_imgsize + 20, rect.y() + (int)(fmT.height() * 1.75) + 10, rect.width() - m_imgsize - 30, rect.height() - (int)(fmT.height() * 1.75 + 10));
	QRect rPicture(rect.x() + 10, rect.y() + (int)(fmT.height() * 1.75 + 10), m_imgsize, m_imgsize);

	// Draw the Title and Underline
	painter->save();
	painter->setPen(Qt::black);
	painter->setFont(m_titleFont);
	painter->drawText(rTitle, Qt::AlignCenter, QString::fromStdString(obj->name()));
	painter->restore();

	painter->drawLine(rect.x() + 10, rect.y() + (int)(fmT.height() * 1.75), rect.x() + rect.width() - 10, rect.y() + (int)(fmT.height() * 1.75));

	// Build the Content Text
	std::stringstream content;

	if (obj->place().is_initialized())
		content << obj->place().get();
	content << std::endl;

	if (obj->country_().is_initialized())
		content << obj->country_().get().name();
	content << std::endl;
	content << obj->num_dives() << " Dives Here" << std::endl;

	if (obj->max_depth().is_initialized())
		content << "Max Depth: " << formatUnits(qtDepth, obj->max_depth().get()).toStdString();
	content << std::endl;

	if (obj->avg_depth().is_initialized())
		content << "Avg Depth: " << formatUnits(qtDepth, obj->avg_depth().get()).toStdString();
	content << std::endl;

	if (obj->max_depth().is_initialized())
		content << "Avg Temp: " << formatUnits(qtTemperature, obj->avg_temp().get()).toStdString();
	content << std::endl;

	// Draw the Content Text
	painter->drawText(rText, Qt::AlignLeft, QString::fromStdString(content.str()));

	// Draw the Picture
	painter->save();
	painter->setBrush(Qt::white);
	painter->drawRoundedRect(rPicture, 6, 6);
	painter->restore();

	// Draw the Rating
	if (obj->rating().is_initialized())
	{
		QImage star(":/icons/star-sm.png");
		painter->save();
		painter->translate(rPicture.x(), rPicture.y() + rPicture.height() + 4);
		int i;
		for (i = 0; i <= 5; ++i)
		{
			if (obj->rating().get() >= (i + 1))
				painter->drawImage(i*(star.width()+1), 0, star);
		}
		painter->restore();
	}

	// Draw the Country Flag
	if (obj->country_().is_initialized())
	{
		QString code(QString::fromStdString(obj->country_().get().code()));
		QImage flag(QString(":/flags/%1.png").arg(code.toLower()));

		if (! flag.isNull())
		{
			int w = flag.width();
			int h = flag.height();
			int x = rect.x() + rect.width() - 10 - w;
			int y = rect.y() + rect.height() - 10 - h;

			painter->drawImage(x, y, flag);
		}
	}
}
