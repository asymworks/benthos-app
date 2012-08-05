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

#ifndef UDEVSERIALPORTMODEL_HPP_
#define UDEVSERIALPORTMODEL_HPP_

/**
 * @file src/models/udevserialportmodel.hpp
 * @brief Serial Port Model (udev backend)
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <libudev.h>

#include <string>
#include <utility>
#include <vector>

#include <QAbstractListModel>
#include <QModelIndex>
#include <QSocketNotifier>

/**
 * @brief Serial Port Model (udev)
 *
 * Presents a list of serial ports on the system using a udev backend.  The
 * model also registers to receive updates on serial ports that are added and
 * removed from the system.
 */
class UDevSerialPortModel: public QAbstractListModel
{
	Q_OBJECT

public:

	enum
	{
		TYPE_UNKNOWN = -1,
		TYPE_PLATFORM = 0,
		TYPE_USB,
		TYPE_ACM,
		TYPE_LINK,
	};

public:

	//! Class Constructor
	UDevSerialPortModel(QObject * parent = 0);

	//! Class Destructor
	virtual ~UDevSerialPortModel();

public:

	//! @return Item Data
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

	//! @return Number of Rows in the Model
	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;

protected:

	//! @brief Enumerate Serial Ports
	std::vector<std::pair<std::string, std::string> > enumerate();

protected slots:
	void _sn_dataready(int);

private:
	struct udev *										m_udev;
	struct udev_monitor *								m_monitor;
	QSocketNotifier *									m_sn;

	std::vector<std::pair<std::string, std::string> >	m_devices;

};

#endif /* UDEVSERIALPORTMODEL_HPP_ */
