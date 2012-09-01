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

#include <cstring>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "units.hpp"

LinearUnitConverter::LinearUnitConverter(double factor)
	: m_factor(factor)
{
}

LinearUnitConverter::~LinearUnitConverter()
{
}

double LinearUnitConverter::toNative(double value) const
{
	return value / m_factor;
}

double LinearUnitConverter::fromNative(double value) const
{
	return value * m_factor;
}

AffineUnitConverter::AffineUnitConverter(double factor, double offset)
	: m_factor(factor), m_offset(offset)
{
}

AffineUnitConverter::~AffineUnitConverter()
{
}

double AffineUnitConverter::toNative(double value) const
{
	return (value - m_offset) / m_factor;
}

double AffineUnitConverter::fromNative(double value) const
{
	return value * m_factor + m_offset;
}

typedef std::vector<unit_t>				unit_list_t;
typedef std::map<int, unit_list_t>		unit_map_t;

void _registerUnits()
{
	/*
	 * NB: Depth and altitude are measured in *pressure*, not *length*, so use
	 * feet of sea water (fsw) and meters of sea water (msw).  The conversion
	 * for pressure units is 3.2568 fsw/msw, not 3.048 ft/m for length!
	 *
	 * To keep things simple for the user, the abbreviation is still set to 'm'
	 * and 'ft'.
	 */

	// Register Depth Units
	registerUnit(qtDepth, "Meters", L"m", new LinearUnitConverter(1));
	registerUnit(qtDepth, "Feet", L"ft", new LinearUnitConverter(3.2568));

	// Register Temperature Units
	registerUnit(qtTemperature, "Celsius", L"\u00b0C", new AffineUnitConverter(1, 0));
	registerUnit(qtTemperature, "Farenheit", L"\u00b0F", new AffineUnitConverter(1.8, 32));

	// Register Pressure Units
	registerUnit(qtPressure, "Bar", L"bar", new LinearUnitConverter(1));
	registerUnit(qtPressure, "PSI", L"psi", new LinearUnitConverter(14.5038));

	// Register Time Units
	registerUnit(qtTime, "Minutes", L"min", new LinearUnitConverter(1));
	registerUnit(qtTime, "Seconds", L"sec", new LinearUnitConverter(60));
	registerUnit(qtTime, "Hours", L"hr", new LinearUnitConverter(1/(double)(60)));

	// Register Heartrate Units
	registerUnit(qtHeartrate, "BPM", L"bpm", new LinearUnitConverter(1));

	// Register Heading Units
	registerUnit(qtHeading, "Degrees", L"deg", new LinearUnitConverter(1));

	// Register Weight Units
	registerUnit(qtWeight, "Kilograms", L"kg", new LinearUnitConverter(1));
	registerUnit(qtWeight, "Pounds", L"lb", new LinearUnitConverter(2.20462));
}

class _unit_initdata
{
public:
	unit_map_t		_unit_map;

	_unit_initdata()
	{
		_registerUnits();
	}

	~_unit_initdata()
	{
		unit_map_t::iterator itm;
		unit_list_t::iterator itl;

		for (itm = _unit_map.begin(); itm != _unit_map.end(); itm++)
			for (itl = itm->second.begin(); itl != itm->second.end(); itl++)
				delete itl->conv;
	}
};

_unit_initdata _libdata;

void registerUnit(quantity_t quantity, const char * name, const std::wstring & abbr, IUnitConverter * converter)
{
	unit_t u;

	u.idx = _libdata._unit_map[quantity].size();
	u.name = name;
	u.abbr = abbr;
	u.conv = converter;

	_libdata._unit_map[quantity].push_back(u);
}

const std::vector<unit_t> & registeredUnits(quantity_t quantity)
{
	return _libdata._unit_map[quantity];
}

unit_t findUnit(quantity_t q, const char * name)
{
	if (! strlen(name))
		return *_libdata._unit_map[q].begin();

	unit_list_t::const_iterator it;
	for (it = _libdata._unit_map[q].begin(); it != _libdata._unit_map[q].end(); it++)
	{
		if (! strcasecmp(it->name, name))
			return *it;
	}

	throw std::runtime_error(std::string("Unit '") + name + "' not registered");
}

double convertFromNative(quantity_t quantity, const char * name, double value)
{
	return findUnit(quantity, name).conv->fromNative(value);
}

double convertToNative(quantity_t quantity, const char * name, double value)
{
	return findUnit(quantity, name).conv->toNative(value);
}
