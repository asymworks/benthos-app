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

#ifndef PROFILE_ALARMITEM_HPP_
#define PROFILE_ALARMITEM_HPP_

/**
 * @brief src/mvf/views/profile_alarmitem.hpp
 * @brief Profile Alarm Plot Item Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QFrame>
#include <QWidget>

#include <util/qcustomplot.h>

/**
 * @brief Alarm Indicator Plot Item
 *
 * QCustomPlot item which represents one or more dive computer alarms during
 * the dive.  The plot item renders a pixmap, and when selected, shows a popup
 * window containing the alarm details.
 */
class AlarmPlotItem: public QCPItemPixmap
{
	Q_OBJECT

public:

	//! Class Constructor
	AlarmPlotItem(QCustomPlot * parentPlot);

	//! Class Destructor
	virtual ~AlarmPlotItem();

	/**
	 * @brief Add Alarm Information to the Indicator
	 * @param[in] Time Offset in Seconds
	 * @param[in] Alarm Name
	 * @param[in] Alarm Description
	 */
	void addAlarm(unsigned int time, const QString & name, const QString & desc);

	//! @return End Time
	unsigned int end_time() const;

	//! @brief Finalize the Alarm Data
	void finalize();

	//! @return Start Time
	unsigned int start_time() const;

protected slots:
	void onSelectionChanged(bool);

private:
	QMap<unsigned int, QStringList>		m_names;
	unsigned int						m_tstart;
	unsigned int						m_tend;

	QFrame *							m_popup;

};

#endif /* PROFILE_ALARMITEM_HPP_ */
