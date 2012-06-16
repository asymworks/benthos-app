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

#include <QDoubleValidator>
#include <QSettings>

#include "quantityedit.hpp"

QuantityEdit::QuantityEdit(quantity_t quantity, const QString & units, QWidget * parent)
	: QLineEdit(parent), m_quantity(quantity), m_units(units)
{
	if (m_units.isEmpty())
	{
		// Lookup the Default Unit
		QSettings s;
		s.beginGroup("Settings");
		QVariant uname = s.value(QString("Unit%1").arg(m_quantity));
		s.endGroup();

		m_units = uname.toString();
	}

	// Lookup the Unit
	(void)findUnit(m_quantity, (const char *)m_units.toAscii());

	// Create the Validator
	setValidator(new QDoubleValidator(this));
}

QuantityEdit::~QuantityEdit()
{
}

double QuantityEdit::native() const
{
	double v = text().toDouble();
	unit_t u = findUnit(m_quantity, (const char *)m_units.toAscii());

	return u.conv->toNative(v);
}

quantity_t QuantityEdit::quantity() const
{
	return m_quantity;
}

void QuantityEdit::setNative(double value)
{
	unit_t u = findUnit(m_quantity, (const char *)m_units.toAscii());
	setText(QString("%1").arg(u.conv->fromNative(value), 0, 'f', 2));
}

void QuantityEdit::setUnits(const QString & value)
{
	double v = native();
	m_units = value;
	setNative(v);
}

const QString & QuantityEdit::units() const
{
	return m_units;
}
