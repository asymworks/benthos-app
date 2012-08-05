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

#ifndef WIZARDCOMBOBOX_HPP_
#define WIZARDCOMBOBOX_HPP_

/**
 * @file src/controls/wizardcombobox.hpp
 * @brief Combo Box for use in Wizards which uses ItemData as default property
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QComboBox>
#include <QVariant>

/**
 * @brief Wizard Combo Box
 *
 * Modified QComboBox which provides the itemData value as the default property
 * rather than currentIndex.  This is a much more logical behavior for use in
 * wizards especially since registering the current index as the field requires
 * stable models and a lookup to get any item data based on the index.
 */
class WizardComboBox: public QComboBox
{
	Q_OBJECT

public:

	Q_PROPERTY(QVariant currentItemData
		READ currentItemData
		WRITE setCurrentItemData
		NOTIFY currentItemDataChanged
		USER true)

public:

	//! Class Constructor
	WizardComboBox(int role = Qt::UserRole, QWidget * parent = 0);

	//! Class Destructor
	virtual ~WizardComboBox();

public:

	//! @return Current Item Data
	QVariant currentItemData() const;

	//! @return Role from which to return Item Data
	int itemDataRole() const;

	//! @param[in] Role from which to return Item Data
	void setItemDataRole(int role);

Q_SIGNALS:
	void currentItemDataChanged(QVariant);

public Q_SLOTS:
	void setCurrentItemData(QVariant value);

private Q_SLOTS:
	void onCurrentIndexChanged(int);

private:
	int			m_itemDataRole;		///< Role from which to return Item Data

};

#endif /* WIZARDCOMBOBOX_HPP_ */
