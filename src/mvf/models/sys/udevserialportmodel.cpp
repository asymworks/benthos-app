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

#include <cstdio>
#include <cstring>
#include <stdexcept>

#include "udevserialportmodel.hpp"

UDevSerialPortModel::UDevSerialPortModel(QObject * parent)
	: QAbstractListModel(parent)
{
	// Create the udev Handle
	m_udev = udev_new();
	if (! m_udev)
		throw std::runtime_error("Failed to create udev handle");

	// Enumerate Current Serial Ports
	m_devices = enumerate();

	// Setup Monitor
	m_monitor = udev_monitor_new_from_netlink(m_udev, "udev");
	udev_monitor_filter_add_match_subsystem_devtype(m_monitor, "tty", 0);
	udev_monitor_enable_receiving(m_monitor);

	m_sn = new QSocketNotifier(udev_monitor_get_fd(m_monitor), QSocketNotifier::Read);
	connect(m_sn, SIGNAL(activated(int)), this, SLOT(_sn_dataready(int)));
}

UDevSerialPortModel::~UDevSerialPortModel()
{
	udev_unref(m_udev);
	delete m_sn;

	if (m_monitor)
		udev_monitor_unref(m_monitor);
}

void UDevSerialPortModel::_sn_dataready(int)
{
	m_sn->setEnabled(false);
	struct udev_device * dev = udev_monitor_receive_device(m_monitor);
	m_sn->setEnabled(true);

	if (! dev)
		return;

	// Retrieve Data about the Device
	const char * devpath = udev_device_get_property_value(dev, "DEVPATH");
	const char * devnode = udev_device_get_devnode(dev);
	const char * action = udev_device_get_action(dev);

	// Ignore Virtual Devices
	if (strncmp(devpath, "/devices/virtual", strlen("/devices/virtual")) == 0)
		return;

	// Retrieve Links to the Device
	std::vector<std::string> linklist;
	udev_list_entry * links;
	udev_list_entry * links_le;

	links = udev_device_get_devlinks_list_entry(dev);
	udev_list_entry_foreach(links_le, links)
	{
		const char * linkpath = udev_list_entry_get_name(links_le);

		// Skip Auto-Generated Links
		if (strstr(linkpath, "by-id") || (strstr(linkpath, "by-path")))
			continue;

		linklist.push_back(linkpath);
	}

	// Process the Add/Remove Operation
	if (strcmp(action, "add") == 0)
	{
		// Determine Port Number and Type
		char pname[255];
		char punktype[255];
		int pnum = 0;

		const char * lpos = strrchr(devnode, '/');
		if (sscanf(lpos, "/ttyS%d", & pnum) == 1)
			sprintf(pname, "Built-In Serial Port %s", lpos+1);
		else if (sscanf(lpos, "/ttyUSB%d", & pnum) == 1)
			sprintf(pname, "USB Serial Port %s", lpos+1);
		else if (sscanf(lpos, "/ttyACM%d", & pnum) == 1)
			sprintf(pname, "ACM Modem %s", lpos+1);
		else if (sscanf(lpos, "/%s%d", punktype, & pnum) == 2)
			sprintf(pname, "Unknown Port %s", lpos+1);
		else
			return;

		// Add the Port and Links
		beginResetModel();
		m_devices.push_back(std::pair<std::string, std::string>(pname, devnode));

		std::vector<std::string>::const_iterator it;
		for (it = linklist.begin(); it != linklist.end(); it++)
		{
			const char * linkpos = strrchr(it->c_str(), '/');
			char linkname[255];
			sprintf(linkname, "Alias %s (links to %s)", linkpos+1, lpos+1);
			m_devices.push_back(std::pair<std::string, std::string>(linkname, * it));
		}
		endResetModel();
	}
	else if (strcmp(action, "remove") == 0)
	{
		beginResetModel();

		// Find the Main Device
		std::vector<std::pair<std::string, std::string> >::iterator it;
		for (it = m_devices.begin(); it != m_devices.end(); )
		{
			if (it->second == devnode)
			{
				it = m_devices.erase(it);
			}
			else
			{
				++it;
			}
		}

		// Find Links
		std::vector<std::string>::const_iterator it2;
		for (it2 = linklist.begin(); it2 != linklist.end(); it2++)
		{
			for (it = m_devices.begin(); it != m_devices.end(); )
			{
				if (it->second == * it2)
				{
					it = m_devices.erase(it);
				}
				else
				{
					++it;
				}
			}
		}

		endResetModel();
	}
}

QVariant UDevSerialPortModel::data(const QModelIndex & index, int role) const
{
	if (! index.isValid() || (index.row() < 0) || ((size_t)index.row() >= m_devices.size()))
		return QVariant();

	int r = index.row();
	if (role == Qt::DisplayRole)
	{
		return QVariant(QString::fromStdString(m_devices[r].first));
	}
	else if ((role == Qt::UserRole) || (role == Qt::EditRole))
	{
		return QVariant(QString::fromStdString(m_devices[r].second));
	}

	return QVariant();
}

std::vector<std::pair<std::string, std::string> > UDevSerialPortModel::enumerate()
{
	std::vector<std::pair<std::string, std::string> > result;

	struct udev_enumerate * enumerate;
	struct udev_list_entry * devices;
	struct udev_list_entry * dev_list_entry;
	struct udev_device * dev;

	// Scan all devices in the 'tty' subsystem
	enumerate = udev_enumerate_new(m_udev);
	udev_enumerate_add_match_subsystem(enumerate, "tty");
	udev_enumerate_scan_devices(enumerate);
	devices = udev_enumerate_get_list_entry(enumerate);

	// Keep devices that are not virtual
	udev_list_entry_foreach(dev_list_entry, devices)
	{
		const char * syspath = udev_list_entry_get_name(dev_list_entry);

		dev = udev_device_new_from_syspath(m_udev, syspath);
		const char * devpath = udev_device_get_property_value(dev, "DEVPATH");
		const char * devnode = udev_device_get_devnode(dev);

		// Skip Virtual Devices
		if (strncmp(devpath, "/devices/virtual", strlen("/devices/virtual")) == 0)
			continue;

		// Determine Port Number and Type
		char pname[255];
		char punktype[255];
		int pnum = 0;

		const char * lpos = strrchr(devnode, '/');
		if (sscanf(lpos, "/ttyS%d", & pnum) == 1)
			sprintf(pname, "Built-In Serial Port %s", lpos+1);
		else if (sscanf(lpos, "/ttyUSB%d", & pnum) == 1)
			sprintf(pname, "USB Serial Port %s", lpos+1);
		else if (sscanf(lpos, "/ttyACM%d", & pnum) == 1)
			sprintf(pname, "ACM Modem %s", lpos+1);
		else if (sscanf(lpos, "/%s%d", punktype, & pnum) == 2)
			sprintf(pname, "Unknown Port %s", lpos+1);
		else
			// Unknown Port String... skip it.  Links are fetched later.
			continue;

		// Add the Device
		result.push_back(std::pair<std::string, std::string>(pname, devnode));

		// Add Links to the Device
		udev_list_entry * links;
		udev_list_entry * links_le;

		links = udev_device_get_devlinks_list_entry(dev);
		udev_list_entry_foreach(links_le, links)
		{
			const char * linkpath = udev_list_entry_get_name(links_le);
			const char * linkpos = strrchr(linkpath, '/');
			char linkname[255];

			// Skip Auto-Generated Links
			if (strstr(linkpath, "by-id") || (strstr(linkpath, "by-path")))
				continue;

			sprintf(linkname, "Alias %s (links to %s)", linkpos+1, lpos+1);
			result.push_back(std::pair<std::string, std::string>(linkname, linkpath));
		}
	}

	return result;
}

int UDevSerialPortModel::rowCount(const QModelIndex & parent) const
{
	return m_devices.size();
}
