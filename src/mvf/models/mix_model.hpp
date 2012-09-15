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

#ifndef MIX_MODEL_HPP_
#define MIX_MODEL_HPP_

/**
 * @file src/mvf/mix_model.hpp
 * @brief Mix Model Classes
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include "mvf/modelif.hpp"
#include "mvf/models.hpp"

#include <benthos/logbook/mix.hpp>
using namespace benthos::logbook;

/**
 * @brief Breathing Mix Model
 *
 * Fields:
 * 0: Mix String [RO]
 * 1: Mix Name
 * 2: Argon PerMil
 * 3: Hydrogen PerMil
 * 4: Helium PerMil
 * 5: Oxygen PerMil
 * 6: Nitrogen PerMil [RO]
 */
class MixModel: public LogbookQueryModel<Mix>
{
public:

	//! Class Constructor
	MixModel(QObject * parent = 0);

	//! Class Destructor
	virtual ~MixModel();

};

#endif /* MIX_MODEL_HPP_ */
