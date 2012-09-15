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

#ifndef TANK_MODEL_HPP_
#define TANK_MODEL_HPP_

/**
 * @file src/mvf/tank_model.hpp
 * @brief Tank Model Classes
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include "mvf/modelif.hpp"
#include "mvf/models.hpp"

#include <benthos/logbook/tank.hpp>
using namespace benthos::logbook;

/**
 * @brief Scuba Tank Model
 *
 * Fields:
 * 1: Tank Name
 * 2: Tank Type
 * 3: Working Pressure
 * 4: Water Volume
 * 5: Gas Capacity
 */
class TankModel: public LogbookQueryModel<Tank>
{
public:

	//! Class Constructor
	TankModel(QObject * parent = 0);

	//! Class Destructor
	virtual ~TankModel();

};


#endif /* TANK_MODEL_HPP_ */
