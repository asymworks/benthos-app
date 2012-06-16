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

#ifndef QUANTITYEDIT_HPP_
#define QUANTITYEDIT_HPP_

/**
 * @file src/quantityedit.hpp
 * @brief Quantity Editor Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QLineEdit>
#include <QString>

#include "util/units.hpp"

/**
 * @brief Quantity Edit Class
 *
 * Unit-Aware quantity editor class.  Contains a field which holds the
 * quantity being edited.  Unit conversion is handled by the delegate class.
 * The constructor takes the quantity ('depth', 'temperature', etc) as well
 * as the default units ('feet', 'meters', etc).  If the default is None the
 * stored default units will be used.
 */
class QuantityEdit: public QLineEdit
{
	Q_OBJECT

public:

	Q_PROPERTY(double native READ native WRITE setNative USER true)

public:

	//! Class Constructor
	QuantityEdit(quantity_t quantity, const QString & units = QString(), QWidget * parent = 0);

	//! Class Destructor
	virtual ~QuantityEdit();

	//! @return Native Unit Value
	double native() const;

	//! @return Quantity Type
	quantity_t quantity() const;

	//! @brief Set Native Units Value
	void setNative(double value);

	//! @brief Set Unit Name
	void setUnits(const QString & value);

	//! @return Unit Name
	const QString & units() const;

private:
	quantity_t		m_quantity;
	QString			m_units;

};

#endif /* QUANTITYEDIT_HPP_ */
