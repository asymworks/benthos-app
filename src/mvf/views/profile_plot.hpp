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

#ifndef PROFILE_PLOT_HPP_
#define PROFILE_PLOT_HPP_

/**
 * @brief src/mvf/views/profile_plot.hpp
 * @brief Profile Abstract Plot Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QComboBox>
#include <QLabel>
#include <QWidget>

#include <util/qcustomplot.h>
#include <util/units.hpp>

#include <benthos/logbook/dive.hpp>
#include <benthos/logbook/profile.hpp>
using namespace benthos::logbook;

/**
 * @brief Render a Dive as a Plot View
 *
 * Renders two plots of a dive profile: the first shows depth vs. time, and
 * the second shows one of the other profile data measurands (e.g. temperature)
 * against the same time scale.
 *
 * If a dive contains multiple linked profiles, the control includes a combo
 * box to select the profile to view.  Only one profile may be viewed at a
 * time.
 */
class ProfilePlotView: public QWidget
{
	Q_OBJECT

public:

	//! Class Constructor
	ProfilePlotView(QWidget * parent = 0);

	//! Class Destructor
	virtual ~ProfilePlotView();

	//! @param[in] Dive to Display
	void setDive(Dive::Ptr dive);

protected:

	//! Create Control Layout
	void createLayout();

	//! @return Label for an Alarm
	static QString alarmLabel(const std::string & name);

	//! @return Label for an Aux Key
	static QString profileKeyLabel(const std::string & key);

	//! @return Quantity Type for an Aux Key
	static quantity_t profileKeyQuantity(const std::string & key);

	//! @return Label for a Profile
	static QString profileLabel(Profile::Ptr profile);

	//! @param[in] Aux Key to Display
	void setAuxKey(const QString & key);

	//! @param[in] Profile to Display
	void setProfile(Profile::Ptr profile);

	//! Setup the Alarm Items
	void setupAlarms();

	//! Setup the Aux Plot Axes
	void setupAuxAxis();

	//! Setup the Depth Plot Y Axis
	void setupDepthAxis();

	//! Setup the Depth Plot X Axis
	void setupTimeAxis();

	/**
	 * @brief Generate the Tick Step for a given Range and Desired Tick Count
	 * @param[in] Data Range
	 * @param[in] Number of Ticks
	 * @return Tick Step
	 */
	static double tickStep(const QCPRange & range, int nSteps = 7);

protected slots:
	void cbxAuxKeysIndexChanged(int);
	void cbxProfileIndexChanged(int);
	void pltDepthBeforeReplot();

private:
	void loadAuxPlotData(const std::string &);
	unit_t unitForQuantity(quantity_t) const;

private:
	QLabel *			m_lblProfile;
	QComboBox *			m_cbxProfile;
	QComboBox *			m_cbxAuxKeys;
	QCustomPlot *		m_pltDepth;
	QCustomPlot *		m_pltAux;

	Dive::Ptr			m_curDive;
	Profile::Ptr		m_curProfile;
	QString				m_auxKey;

};

#endif /* PROFILE_PLOT_HPP_ */
