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
#include <QPainter>
#include <QPaintEvent>
#include <QSettings>

#include "profile_table.hpp"
#include "util/units.hpp"

ProfileTableView::ProfileTableView(QWidget * parent)
	: QWidget(parent), m_Dive()
{
}

ProfileTableView::~ProfileTableView()
{
}

QSize ProfileTableView::minimumSizeHint() const
{
	return QSize(400, 280);
}

void ProfileTableView::paintEvent(QPaintEvent * e)
{
	unit_t u;

	//! Lookup the Unit Abbreviation
	QSettings s;
	s.beginGroup("Settings");
	QVariant uname = s.value(QString("Unit%1").arg(qtDepth));
	s.endGroup();

	try
	{
		if (! uname.isValid())
			u = findUnit(qtDepth, 0);
		else
			u = findUnit(qtDepth, (const char *)uname.toByteArray());
	}
	catch (std::runtime_error & e)
	{
		u = findUnit(qtDepth, 0);
	}

	// Value Strings
	QString interval;
	QString pgStart;
	QString pgEnd;

	QString depth;
	QString btime;
	QString sstop;

	QString rnt;
	QString abt;
	QString tbt;

	if (m_Dive)
	{
		interval = QString("%1:%2").arg(m_Dive->interval() / 60).arg(m_Dive->interval() % 60, 2, 10, QChar('0'));
		depth = QString("%1 %2").arg(u.conv->fromNative(m_Dive->max_depth()), 0, 'f', 1).arg(QString::fromStdWString(u.abbr));
		btime = QString("%1:%2").arg(m_Dive->duration() / 60).arg(m_Dive->duration() % 60, 2, 10, QChar('0'));

		if (m_Dive->start_pressure_group())
			pgStart = QString::fromStdString(m_Dive->start_pressure_group().get());
		if (m_Dive->end_pressure_group())
			pgEnd = QString::fromStdString(m_Dive->end_pressure_group().get());

		if (m_Dive->safety_stop() && m_Dive->stop_depth())
		{
			double sdepth = m_Dive->stop_depth().get();
			double ddepth = u.conv->fromNative(sdepth);
			if ((sdepth == 5.0) && ! strcasecmp(u.name, "feet"))
				// Special Case... 5m <=> 15ft for safety stop.
				ddepth = 15;

			if (m_Dive->stop_time())
			{
				sstop = QString("%1' @ %2 %3")
					.arg(m_Dive->stop_time().get())
					.arg(ddepth, 0, 'f', 0)
					.arg(QString::fromStdWString(u.abbr));
			}
			else
			{
				sstop = QString("%1 %2")
					.arg(ddepth, 0, 'f', 0)
					.arg(QString::fromStdWString(u.abbr));
			}
		}
		else if (m_Dive->safety_stop() && m_Dive->stop_time())
		{
			sstop = QString(tr("%1 min")).arg(m_Dive->stop_time().get());
		}

		if (m_Dive->rnt())
		{
			rnt = QString("%1").arg(m_Dive->rnt().get());
			abt = QString("%1").arg(m_Dive->duration());
			tbt = QString("%1").arg(m_Dive->rnt().get() + m_Dive->duration());
		}
	}

	// Get the Large and Small Fonts
	QFont fL(font());	fL.setPixelSize(fontInfo().pixelSize() * 5 / 3);
	QFont fM(font());	fM.setPixelSize(fontInfo().pixelSize() * 5 / 4);
	QFont fS(font());	fS.setPixelSize(fontInfo().pixelSize() * 3 / 4);
	QFont fB(font());	fB.setWeight(QFont::Bold);

	QFontMetrics fmL(fL);
	QFontMetrics fmM(fS);
	QFontMetrics fmS(fS);
	QFontMetrics fmB(fB);

	// Calculate the ideal (10:7) rectangle
	QRect contentRect;
	int cw = width();
	int ch = height();

	if (ch > cw * 7 / 10)
	{
		// Letterbox
		contentRect.setWidth(cw);
		contentRect.setHeight(cw * 7 / 10);
		contentRect.translate(0, (ch - contentRect.height()) / 2);
	}
	else
	{
		// Pillarbox
		contentRect.setWidth(ch * 10 / 7);
		contentRect.setHeight(ch);
		contentRect.translate((cw - contentRect.width()) / 2, 0);
	}

	// Calculate the other Rect's
	QRect intervalRect;
	QRect pgStartRect;
	QRect pgEndRect;
	QRect depthRect;
	QRect btimeRect;
	QRect stopRect;

	QRect ntValRect;
	QRect ntRect;

	intervalRect.setHeight(0.20 * contentRect.height());
	intervalRect.setWidth(0.20 * contentRect.width());
	intervalRect.translate(0, 0);

	pgStartRect.setHeight(0.20 * contentRect.height());
	pgStartRect.setWidth(0.10 * contentRect.width());
	pgStartRect.translate(0.20 * contentRect.width(), 0);

	pgEndRect.setHeight(0.20 * contentRect.height());
	pgEndRect.setWidth(0.10 * contentRect.width());
	pgEndRect.translate(0.90 * contentRect.width(), 0);

	depthRect.setHeight(0.10 * contentRect.height());
	depthRect.setWidth(fmS.width(tr("MAX DEPTH")) * 3 / 2);
	depthRect.translate((0.30 * contentRect.width() - depthRect.width()) / 2, 0.40 * contentRect.height());

	btimeRect.setHeight(0.10 * contentRect.height());
	btimeRect.setWidth(fmS.width(tr("BOTTOM TIME")) * 3 / 2);
	btimeRect.translate(0.35 * contentRect.width() + (0.25 * contentRect.width() - btimeRect.width()) / 2, 0.82 * contentRect.height());

	stopRect.setHeight(0.10 * contentRect.height());
	stopRect.setWidth(fmS.width(tr("SAFETY STOP")) * 5 / 3);
	stopRect.translate(0.75 * contentRect.width() + (0.20 * contentRect.width() - stopRect.width()) / 2, 0.35 * contentRect.height());

	ntValRect.setHeight(fmB.height() * 5 / 4);
	ntValRect.setWidth(fmB.width("0000") * 5 / 4);
	ntValRect.translate(0.75 * contentRect.width() + fmS.width("TNT: "), 0.60 * contentRect.height());

	ntRect.setHeight(ntValRect.height());
	ntRect.setWidth(fmS.width("TNT: ") + fmS.width("min") + ntValRect.width());
	ntRect.translate(0.75 * contentRect.width(), 0.60 * contentRect.height());

	// Setup the Painter
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::NoPen);

	// Fill Background with White
	painter.save();
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::white);
	painter.drawRect(contentRect);
	painter.restore();

	// Setup the Profile Path
	QPainterPath ppProfile;
	ppProfile.moveTo(0, 0);
	ppProfile.lineTo(0.30 * contentRect.width(), 0);
	ppProfile.lineTo(0.35 * contentRect.width(), 0.60 * contentRect.height());
	ppProfile.lineTo(0.60 * contentRect.width(), 0.60 * contentRect.height());
	ppProfile.lineTo(0.75 * contentRect.width(), 0.15 * contentRect.height());
	ppProfile.lineTo(0.85 * contentRect.width(), 0.15 * contentRect.height());
	ppProfile.lineTo(0.90 * contentRect.width(), 0);
	ppProfile.lineTo(1.00 * contentRect.width(), 0);

	// Depth Profile Brush
	QLinearGradient lg(0, 0, 0, 1);
	lg.setCoordinateMode(QGradient::StretchToDeviceMode);
	lg.setColorAt(0, QColor(255, 255, 255, 255));
	lg.setColorAt(0.6, QColor(0, 64, 112, 255));

	// Draw the Depth Profile
	painter.save();
	painter.translate(contentRect.topLeft());
	painter.translate(0, 0.20 * contentRect.height());
	painter.setPen(QPen(QColor(112, 112, 112, 255), 2));
	painter.setBrush(lg);
	painter.drawPath(ppProfile);
	painter.restore();

	// Draw the Surface Interval / PGStart Separator
	painter.save();
	painter.translate(contentRect.topLeft());
	painter.translate(intervalRect.width(), 0.20 * contentRect.height());
	painter.setPen(QPen(Qt::black, 2));
	painter.drawLine(
		-fmL.height() / 8,	-fmL.height() / 4,
		 fmL.height() / 8,  -fmL.height()
	);
	painter.restore();

	// Draw the Surface Interval and Pressure Groups
	painter.save();
	painter.translate(contentRect.topLeft());
	painter.setPen(Qt::black);
	painter.setPen(Qt::NoBrush);
	painter.setFont(fL);
	painter.drawText(intervalRect, Qt::AlignHCenter | Qt::AlignBottom, interval);
	painter.drawText(pgStartRect, Qt::AlignHCenter | Qt::AlignBottom, pgStart);
	painter.drawText(pgEndRect, Qt::AlignHCenter | Qt::AlignBottom, pgEnd);
	painter.translate(0, intervalRect.height() + fmS.height() / 2);
	painter.setFont(fS);
	painter.drawText(intervalRect, Qt::AlignHCenter | Qt::AlignTop, tr("INTERVAL"));
	painter.drawText(pgStartRect, Qt::AlignHCenter | Qt::AlignTop, tr("PG"));
	painter.drawText(pgEndRect, Qt::AlignHCenter | Qt::AlignTop, tr("PG"));
	painter.restore();

	// Draw the Depth Information
	painter.save();
	painter.translate(contentRect.topLeft());
	painter.setPen(Qt::black);
	painter.setPen(Qt::NoBrush);
	painter.setFont(fM);
	painter.drawText(depthRect, Qt::AlignHCenter | Qt::AlignBottom, depth);
	painter.save();
	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.setPen(QColor(112, 112, 112, 255));
	painter.drawLine(
		depthRect.bottomLeft(),
		depthRect.bottomRight()
	);
	painter.restore();
	painter.translate(0, depthRect.height() + fmS.height() / 4);
	painter.setFont(fS);
	painter.drawText(depthRect, Qt::AlignHCenter | Qt::AlignTop, tr("MAX DEPTH"));
	painter.restore();

	// Draw the Bottom Time Information
	painter.save();
	painter.translate(contentRect.topLeft());
	painter.setPen(Qt::black);
	painter.setPen(Qt::NoBrush);
	painter.setFont(fM);
	painter.drawText(btimeRect, Qt::AlignHCenter | Qt::AlignBottom, btime);
	painter.save();
	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.setPen(QColor(112, 112, 112, 255));
	painter.drawLine(
		btimeRect.bottomLeft(),
		btimeRect.bottomRight()
	);
	painter.restore();
	painter.translate(0, btimeRect.height() + fmS.height() / 4);
	painter.setFont(fS);
	painter.drawText(btimeRect, Qt::AlignHCenter | Qt::AlignTop, tr("BOTTOM TIME"));
	painter.restore();

	// Draw the Safety Stop Information
	painter.save();
	painter.translate(contentRect.topLeft());
	painter.setPen(Qt::black);
	painter.setPen(Qt::NoBrush);
	painter.setFont(fM);
	painter.drawText(stopRect, Qt::AlignHCenter | Qt::AlignBottom, sstop);
	painter.save();
	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.setPen(QColor(112, 112, 112, 255));
	painter.drawLine(
		stopRect.bottomLeft(),
		stopRect.bottomRight()
	);
	painter.restore();
	painter.translate(0, stopRect.height() + fmS.height() / 4);
	painter.setFont(fS);
	painter.drawText(stopRect, Qt::AlignHCenter | Qt::AlignTop, tr("SAFETY STOP"));
	painter.restore();

	// Draw the Nitrogen Time Table
	painter.save();
	painter.translate(contentRect.topLeft());
	painter.setPen(Qt::black);
	painter.setPen(Qt::NoBrush);
	// RNT
	painter.save();
	painter.setFont(fB);
	painter.drawText(ntValRect, Qt::AlignCenter | Qt::AlignBottom, rnt);
	painter.setFont(fS);
	painter.drawText(ntRect, Qt::AlignLeft | Qt::AlignBottom, tr("RNT:"));
	painter.drawText(ntRect, Qt::AlignRight | Qt::AlignBottom, tr("min"));
	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.setPen(QColor(112, 112, 112, 255));
	painter.drawLine(
		ntValRect.bottomLeft(),
		ntValRect.bottomRight()
	);
	painter.restore();
	painter.translate(0, ntRect.height());
	// ABT
	painter.save();
	painter.setFont(fB);
	painter.drawText(ntValRect, Qt::AlignCenter | Qt::AlignBottom, abt);
	painter.setFont(fS);
	painter.drawText(ntRect, Qt::AlignLeft | Qt::AlignBottom, tr("ABT:"));
	painter.drawText(ntRect, Qt::AlignRight | Qt::AlignBottom, tr("min"));
	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.setPen(QColor(112, 112, 112, 255));
	painter.drawLine(
		ntValRect.bottomLeft(),
		ntValRect.bottomRight()
	);
	painter.restore();
	painter.translate(0, ntRect.height());
	// Line
	painter.save();
	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.setPen(QColor(192, 192, 192, 255));
	painter.drawLine(
		ntRect.left(), ntRect.center().y(),
		ntRect.right(), ntRect.center().y()
	);
	painter.restore();
	painter.translate(0, ntRect.height() / 2);
	// TBT
	painter.save();
	painter.setFont(fB);
	painter.drawText(ntValRect, Qt::AlignCenter | Qt::AlignBottom, tbt);
	painter.setFont(fS);
	painter.drawText(ntRect, Qt::AlignLeft | Qt::AlignBottom, tr("TBT:"));
	painter.drawText(ntRect, Qt::AlignRight | Qt::AlignBottom, tr("min"));
	painter.setRenderHint(QPainter::Antialiasing, false);
	painter.setPen(QColor(112, 112, 112, 255));
	painter.drawLine(
		ntValRect.bottomLeft(),
		ntValRect.bottomRight()
	);
	painter.restore();
	painter.restore();
}

void ProfileTableView::setDive(Dive::Ptr dive)
{
	m_Dive = dive;
	update();
}

QSize ProfileTableView::sizeHint() const
{
	return QSize(600, 420);
}
