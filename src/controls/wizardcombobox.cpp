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

#include "wizardcombobox.hpp"

WizardComboBox::WizardComboBox(int role, QWidget * parent)
	: QComboBox(parent), m_itemDataRole(role)
{
	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
}

WizardComboBox::~WizardComboBox()
{
}

QVariant WizardComboBox::currentItemData() const
{
	return itemData(currentIndex(), m_itemDataRole);
}

int WizardComboBox::itemDataRole() const
{
	return m_itemDataRole;
}

void WizardComboBox::onCurrentIndexChanged(int index)
{
	emit currentItemDataChanged(currentItemData());
}

void WizardComboBox::setCurrentItemData(QVariant value)
{
	setItemData(currentIndex(), value, m_itemDataRole);
}

void WizardComboBox::setItemDataRole(int role)
{
	if (m_itemDataRole != role)
	{
		m_itemDataRole = role;
		emit currentItemDataChanged(currentItemData());
	}
}
