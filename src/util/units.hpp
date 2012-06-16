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

#ifndef UNITS_HPP_
#define UNITS_HPP_

/**
 * @file src/util/units.hpp
 * @brief Dive Log Unit Handling Support
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <vector>

/**
 * @brief Quantity Type Enumeration
 *
 * Enumerates the known quantity types.
 */
enum quantity_t
{
	qtDepth,
	qtTemperature,
	qtPressure,
};

/**
 * @brief Unit Conversion Class
 *
 * Class which abstracts out converting from one unit to another.  This
 * base class must be overridden by concrete classes to perform unit conversion,
 * e.g. linear, affine, user-defined.  All converters operate using the notion
 * of "native" units, that is, the units in which quantities are stored in some
 * data storage (e.g. a database).
 */
struct IUnitConverter
{
	virtual ~IUnitConverter() { }

	/**
	 * @brief Convert from this Unit to Native Units
	 * @param [in] Quantity
	 * @return Quantity [Native Units]
	 */
	virtual double toNative(double value) const = 0;

	/**
	 * @brief Convert from Native Units to this Unit
	 * @param [in] Quantity [Native Units]
	 * @return Quantity
	 */
	virtual double fromNative(double nativeValue) const = 0;

};

/**
 * @brief Linear Unit Conversion Class
 *
 * Concrete unit conversion class which uses a pure linear transform, e.g.
 * native units are some multiple of the user units.  The conversion direction
 * is as follows:
 *
 * @code
 * userUnits = factor * nativeUnits;
 * @endcode
 */
class LinearUnitConverter: public IUnitConverter
{
public:

	//! Class Constructor
	LinearUnitConverter(double factor);

	//! Class Destructor
	virtual ~LinearUnitConverter();

	/**
	 * @brief Convert from this Unit to Native Units
	 * @param [in] Quantity
	 * @return Quantity [Native Units]
	 */
	virtual double toNative(double value) const;

	/**
	 * @brief Convert from Native Units to this Unit
	 * @param [in] Quantity [Native Units]
	 * @return Quantity
	 */
	virtual double fromNative(double nativeValue) const;

protected:
	double			m_factor;

};

/**
 * @brief Affine Unit Conversion Class
 *
 * Concrete unit conversion class which uses a pure linear transform, e.g.
 * native units are some multiple of the user units.  The conversion direction
 * is as follows:
 *
 * @code
 * userUnits = factor * nativeUnits + offset;
 * @endcode
 */
class AffineUnitConverter: public IUnitConverter
{
public:

	//! Class Constructor
	AffineUnitConverter(double factor, double offset);

	//! Class Destructor
	virtual ~AffineUnitConverter();

	/**
	 * @brief Convert from this Unit to Native Units
	 * @param [in] Quantity
	 * @return Quantity [Native Units]
	 */
	virtual double toNative(double value) const;

	/**
	 * @brief Convert from Native Units to this Unit
	 * @param [in] Quantity [Native Units]
	 * @return Quantity
	 */
	virtual double fromNative(double nativeValue) const;

protected:
	double			m_factor;
	double			m_offset;

};

/**
 * @brief Unit Type Record
 */
typedef struct
{
	int					idx;
	const char *		name;
	std::wstring		abbr;
	IUnitConverter *	conv;
} unit_t;

/**
 * @brief Convert a Unit from Native Units
 * @param [in] Quantity
 * @param [in] Unit Name
 * @param [in] Value to Convert
 * @return User Units
 */
double convertFromNative(quantity_t quantity, const char * name, double value);

/**
 * @brief Convert a Unit to Native Units
 * @param [in] Quantity
 * @param [in] Unit Name
 * @param [in] Value to Convert
 * @return Native Units
 */
double convertToNative(quantity_t quantity, const char * name, double value);

/**
 * @brief Find a Unit by Name
 * @param [in] Quantity
 * @param [in] Name
 * @return Unit Record
 * @throws UnitNotRegistered if the unit is not found
 * @note String comparision is case-insensitive.
 */
unit_t findUnit(quantity_t quantity, const char * name);

/**
 * @brief Register a new Unit for a Quantity
 * @param [in] Quantity
 * @param [in] Unit Name
 * @param [in] Unit Abbreviation
 * @param [in] Conversion Class
 */
void registerUnit(quantity_t quantity, const char * name, const std::wstring & abbr, IUnitConverter * converter);

/**
 * @brief Get all registered Units for a Quantity
 * @return vector<unit_t>
 */
const std::vector<unit_t> & registeredUnits(quantity_t quantity);

#endif /* UNITS_HPP_ */
