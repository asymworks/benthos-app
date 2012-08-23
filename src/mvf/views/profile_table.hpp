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

#ifndef PROFILE_TABLE_HPP_
#define PROFILE_TABLE_HPP_

/**
 * @brief src/mvf/views/profile_view.hpp
 * @brief Profile Table View Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QSize>
#include <QWidget>

#include <benthos/logbook/dive.hpp>
using namespace benthos::logbook;

/**
 * @brief Render a Dive as a "Table" View
 *
 * Renders a "table" view of a dive - a square profile with text indicating
 * the maximum depth, bottom time, surface interval, safety stops, pressure
 * groups and nitrogen time table.
 */
class ProfileTableView: public QWidget
{
public:

	//! Class Constructor
	ProfileTableView(QWidget * parent = 0);

	//! Class Destructor
	virtual ~ProfileTableView();

	//! @return Minimum Size Hint
	virtual QSize minimumSizeHint() const;

	//! @param[in] Dive to Display
	void setDive(Dive::Ptr dive);

	//! @return Default Size Hint
	virtual QSize sizeHint() const;

protected:

	//! @brief Render the Widget
	virtual void paintEvent(QPaintEvent * e);

private:
	Dive::Ptr			m_Dive;

};

#endif /* PROFILE_TABLE_HPP_ */
