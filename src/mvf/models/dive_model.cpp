/*
 * Copyright (C) 2011 Asymworks, LLC.  All Rights Reserved.
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

#include "mvf/delegates.hpp"
#include "dive_model.hpp"

#include <benthos/logbook/dive_computer.hpp>
#include <benthos/logbook/dive_site.hpp>

using namespace benthos::logbook;

DiveModel::DiveModel(QObject * parent)
	: LogbookQueryModel(parent)
{
	ADD_OPTIONAL_COLUMN(time_t, datetime, setDateTime, "Dive Date", new DelegateFactory<DateTimeDelegate>);
	ADD_OPTIONAL_COLUMN(int, rating, setRating, "Rating", new DelegateFactory<RatingDelegate>);
	ADD_OPTIONAL_COLUMN(int, number, setNumber, "Number");
	ADD_FIELD_COLUMN(int, repetition, setRepetition, "Repetition");
	ADD_FIELD_COLUMN(int, interval, setInterval, "Surface Interval", new DelegateFactory<MinutesDelegate>, true);

	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>(
		new ForeignKeyWrapper<LogbookQueryModel::model_type, DiveSite, std::string>(
			& LogbookQueryModel::model_type::site,
			new CountryFieldAdapter<DiveSite>(& DiveSite::country_)
		), "country", "Country", NULL, true
	));

	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>(
		new ForeignKeyWrapper<LogbookQueryModel::model_type, DiveSite, std::string>(
			& LogbookQueryModel::model_type::site,
			new DefaultFieldAdapter<DiveSite, std::string>(& DiveSite::place)
		), "location", "Location"
	));

	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>(
		new ForeignKeyWrapper<LogbookQueryModel::model_type, DiveSite, std::string>(
			& LogbookQueryModel::model_type::site,
			new DefaultFieldAdapter<DiveSite, std::string>(& DiveSite::name)
		), "dive_site", "Dive Site"
	));

	ADD_FIELD_COLUMN(double, max_depth, setMaxDepth, "Max Depth", new DelegateFactory<DepthDelegate>);
	ADD_OPTIONAL_COLUMN(double, avg_depth, setAvgDepth, "Average Depth", new DelegateFactory<DepthDelegate>);
	ADD_FIELD_COLUMN(int, duration, setDuration, "Duration", new DelegateFactory<MinutesDelegate>);

	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>(
		new ForeignKeyWrapper<LogbookQueryModel::model_type, DiveComputer, std::string>(
			& LogbookQueryModel::model_type::computer,
			new DefaultFieldAdapter<DiveComputer, std::string>(
				& DiveComputer::name,
				& DiveComputer::setName,
				& DiveComputer::setName
			)
		), "computer", "Computer"
	));

	ADD_OPTIONAL_COLUMN(double, air_temp, setAirTemp, "Air Temp", new DelegateFactory<TemperatureDelegate>);
	ADD_OPTIONAL_COLUMN(double, max_temp, setMaxTemp, "Max Temp", new DelegateFactory<TemperatureDelegate>);
	ADD_OPTIONAL_COLUMN(double, min_temp, setMinTemp, "Min Temp", new DelegateFactory<TemperatureDelegate>);

	ADD_OPTIONAL_COLUMN(double, start_pressure, setStartPressure, "Start Pressure", new DelegateFactory<PressureDelegate>);
	ADD_OPTIONAL_COLUMN(double, end_pressure, setEndPressure, "End Pressure", new DelegateFactory<PressureDelegate>);

	ADD_OPTIONAL_COLUMN(std::string, comments, setComments, "Comments", NULL, true, true);

	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>(
		new ForeignKeyAdapter<LogbookQueryModel::model_type, DiveSite>(
			& LogbookQueryModel::model_type::site,
			& LogbookQueryModel::model_type::setSite,
			& LogbookQueryModel::model_type::setSite
		), "fk_dive_site", "", NULL, true, true
	));

	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>(
		new ForeignKeyAdapter<LogbookQueryModel::model_type, DiveComputer>(
			& LogbookQueryModel::model_type::computer,
			& LogbookQueryModel::model_type::setComputer,
			& LogbookQueryModel::model_type::setComputer
		), "fk_dive_computer", "", NULL, true, true
	));
}

DiveModel::~DiveModel()
{
}
