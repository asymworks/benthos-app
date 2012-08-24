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

#include <QGridLayout>
#include <QLabel>
#include <QPixmap>

#include "profile_alarmitem.hpp"

AlarmPlotItem::AlarmPlotItem(QCustomPlot * parentPlot)
	: QCPItemPixmap(parentPlot), m_names(), m_tstart(0), m_tend(0), m_popup(0)
{
	setPixmap(QPixmap(":/icons/plot-warning.png"));
	setSelectedPen(Qt::NoPen);

	connect(this, SIGNAL(selectionChanged(bool)), this, SLOT(onSelectionChanged(bool)));
}

AlarmPlotItem::~AlarmPlotItem()
{
	if (m_popup)
		m_popup->hide();
}

void AlarmPlotItem::addAlarm(unsigned int time, const QString & name, const QString & desc)
{
	if (m_names.isEmpty())
	{
		m_tstart = time;
		m_tend = time;
	}
	else
	{
		if (time < m_tstart)
			m_tstart = time;
		if (time > m_tend)
			m_tend = time;
	}

	m_names[time].append(name);
}

unsigned int AlarmPlotItem::end_time() const
{
	return m_tend;
}

void AlarmPlotItem::finalize()
{
	QGridLayout * gbox = new QGridLayout;

	int row = 0;
	QMap<unsigned int, QStringList>::const_iterator it;
	for (it = m_names.begin(); it != m_names.end(); it++)
	{
		QLabel * ltime = new QLabel(tr("At %1:%2").arg(it.key() / 60).arg(it.key() % 60, 2, 10, QChar('0')));
		QLabel * lname = new QLabel(it->join("\n"));
		lname->setStyleSheet("QLabel { font-weight: bold; }");

		gbox->addWidget(ltime, row, 0);
		gbox->addWidget(lname, row, 1);

		++row;
	}

	m_popup = new QFrame(parentPlot(), Qt::ToolTip);
	m_popup->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	m_popup->setLayout(gbox);
	m_popup->setMaximumWidth(100);
	m_popup->show();
	m_popup->hide();
}

void AlarmPlotItem::onSelectionChanged(bool selected)
{
	if (selected)
	{
		int l = (int)top->pixelPoint().x();
		int t = (int)top->pixelPoint().y() - m_popup->height() - 8;
		int r = l + m_popup->width();

		if (r > parentPlot()->contentsRect().right() - parentPlot()->marginRight())
			l -= (r - parentPlot()->contentsRect().right() + parentPlot()->marginRight());

		m_popup->move(parentPlot()->mapToGlobal(QPoint(l, t)));
		m_popup->show();
	}
	else
	{
		m_popup->hide();
	}
}

unsigned int AlarmPlotItem::start_time() const
{
	return m_tstart;
}
