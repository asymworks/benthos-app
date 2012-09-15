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
#include "tank_model.hpp"

TankModel::TankModel(QObject * parent)
	: LogbookQueryModel(parent)
{
	ADD_OPTIONAL_COLUMN(std::string, name, setName, "Tank Name");
	ADD_OPTIONAL_COLUMN(std::string, type, setType, "Tank Type", new DelegateFactory<TitleCaseDelegate>);
	ADD_FIELD_COLUMN(double, pressure, setPressure, "Working Pressure", new DelegateFactory<PressureDelegate>);
	ADD_FIELD_COLUMN(double, volume, setVolume, "Volume", new DelegateFactory<VolumeDelegate>);
	ADD_FIELD_COLUMN(double, capacity, setCapacity, "Capacity", new DelegateFactory<VolumeDelegate>);
}

TankModel::~TankModel()
{
}
