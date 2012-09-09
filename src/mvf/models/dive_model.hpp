/*
 * Copyright (C) 2011 Asymworks, LLC.  All Rights Reserved.
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

#ifndef DIVE_MODEL_HPP_
#define DIVE_MODEL_HPP_

/**
 * @file src/mvf/dive_model.hpp
 * @brief Dive Model Classes
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include "mvf/modelif.hpp"
#include "mvf/models.hpp"

#include <benthos/logbook/dive.hpp>
#include <benthos/logbook/profile.hpp>
using namespace benthos::logbook;

/**
 * @brief Dive Model
 *
 * Fields:
 * 0: Date/Time
 * 1: Rating
 * 2: Number
 * 3: Repetition
 * 4: Interval
 * 5: Country
 * 6: Place Name
 * 7: Site Name
 * 8: Max Depth
 * 9: Avg Depth
 * 10: Runtime
 * 11: Computer Name
 * 12: Air Temp
 * 13: Max Temp
 * 14: Min Temp
 * 15: Start Pressure
 * 16: End Pressure
 * 17: Comments
 * 18: Dive Site (FK)
 * 19: Dive Computer (FK)
 * 20: Gas Mix (FK)
 * 21: Gas Mix Name
 * 22: Starting Pressure Group
 * 23: Ending Pressure Group
 * 24: Residual Nitrogen Time
 * 25: Desaturation Time
 * 26: No-Fly Time
 * 27: Algorithm
 * 28: Safety Stop Flag
 * 29: Safety Stop Depth
 * 30: Safety Stop Time
 * 31: Visibility Category
 * 32: Visibility Distance
 * 33: Weight
 * 34: Imported Date/Time
 */
class DiveModel: public LogbookQueryModel<Dive>
{
public:

	//! Class Constructor
	DiveModel(QObject * parent = 0);

	//! Class Destructor
	virtual ~DiveModel();

public:

	/**
	 * @brief Merge Multiple Dives into a single Dive
	 * @param[in] List of Dives to Merge
	 * @param[out] Dive to Populate
	 * @return List of new Profiles
	 *
	 * Merges the selected dives into a single dive by concatenating the profile
	 * data and intelligently merging dive header information.  Each dive must
	 * have the same number of profiles, and each dive's profiles must be from
	 * the same set of devices.  Following the merge, the new dive will have one
	 * profile for each device in the source dive set.
	 *
	 * The returned vector holds strong pointers to the new profile objects,
	 * each of which holds a strong reference to the new dive object.  The new
	 * dive is not added to the session by this method.
	 */
	static std::vector<Profile::Ptr> Merge(const std::vector<Dive::Ptr> & dives, Dive::Ptr & new_dive);

};

#endif /* DIVE_MODEL_HPP_ */
