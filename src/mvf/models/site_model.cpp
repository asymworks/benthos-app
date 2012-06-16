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

#include "mvf/delegates.hpp"
#include "site_model.hpp"

DiveSiteModel::DiveSiteModel(QObject * parent)
	: LogbookQueryModel(parent)
{
	ADD_FIELD_COLUMN(std::string, name, setName, "Site Name");
	ADD_OPTIONAL_COLUMN(std::string, place, setPlace, "Place Name");

	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>(
		new CountryFieldAdapter<LogbookQueryModel::model_type>(
			& LogbookQueryModel::model_type::country_,
			& LogbookQueryModel::model_type::setCountry,
			& LogbookQueryModel::model_type::setCountry
		), "country", "Country"
	));

	ADD_OPTIONAL_COLUMN(std::string, salinity, setSalinity, "Salinity", new DelegateFactory<TitleCaseDelegate>);
	ADD_OPTIONAL_COLUMN(std::string, platform, setPlatform, "Platform");
	ADD_OPTIONAL_COLUMN(std::string, water_body, setWaterBody, "Water Body");
	ADD_OPTIONAL_COLUMN(std::string, bottom, setBottom, "Bottom");
	ADD_OPTIONAL_COLUMN(double, altitude, setAltitude, "Altitude", new DelegateFactory<DepthDelegate>);

	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>(
		new PositionFieldAdapter<LogbookQueryModel::model_type>(
			& LogbookQueryModel::model_type::latitude,		& LogbookQueryModel::model_type::longitude,
			& LogbookQueryModel::model_type::setLatitude,	& LogbookQueryModel::model_type::setLongitude,
			& LogbookQueryModel::model_type::setLatitude,	& LogbookQueryModel::model_type::setLongitude
		), "location", "Location", new DelegateFactory<PositionDelegate>, true
	));

	ADD_RO_FIELD_COLUMN(unsigned int, num_dives, "Num Dives");
	ADD_RO_FIELD_COLUMN(double, rating, "Rating", new DelegateFactory<RatingDelegate>);

	ADD_OPTIONAL_COLUMN(std::string, comments, setComments, "Comments", NULL, true, true);

	ADD_RO_FIELD_COLUMN(double, max_depth, "Max Depth", new DelegateFactory<DepthDelegate>, true);
	ADD_RO_FIELD_COLUMN(double, avg_depth, "Avg Depth", new DelegateFactory<DepthDelegate>, true);
	ADD_RO_FIELD_COLUMN(double, avg_temp, "Avg Temp", new DelegateFactory<TemperatureDelegate>, true);
}

DiveSiteModel::~DiveSiteModel()
{
}
