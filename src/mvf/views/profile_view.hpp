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

#ifndef PROFILE_VIEW_HPP_
#define PROFILE_VIEW_HPP_

/**
 * @brief src/mvf/views/profile_view.hpp
 * @brief Profile View Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QComboBox>
#include <QStackedWidget>
#include <QFrame>
#include <QWidget>

/*
 * FIX for broken Qt4 moc and BOOST_JOIN error
 */
#ifdef Q_MOC_RUN
#define BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#endif

#include <benthos/logbook/dive.hpp>
#include <benthos/logbook/profile.hpp>
using namespace benthos::logbook;

#include "profile_plot.hpp"
#include "profile_table.hpp"

/**
 * @brief Dive Profile View Widget
 *
 * Master widget to view dive profiles.  The view includes a combo box to
 * select which profile to view, if a dive has more than one, and displays
 * either a depth plot or a table chart showing max depth, pressure groups
 * and RNT/TBT numbers.
 */
class ProfileView: public QFrame
{
	Q_OBJECT

public:

	//! Class Constructor
	ProfileView(QWidget * parent = 0);

	//! Class Destructor
	virtual ~ProfileView();

	//! @param[in] Dive to Display
	void setDive(Dive::Ptr dive);

private:
	ProfilePlotView *	m_pvPlot;
	ProfileTableView *	m_pvTable;
	QWidget *			m_pvBlank;
	QStackedWidget *	m_swView;

};

#endif /* PROFILE_VIEW_HPP_ */
