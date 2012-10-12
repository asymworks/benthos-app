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

#include "mvf/delegates.hpp"
#include "dive_model.hpp"

#include <benthos/logbook/dive_computer.hpp>
#include <benthos/logbook/dive_site.hpp>
#include <benthos/logbook/mix.hpp>

using namespace benthos::logbook;

DiveModel::DiveModel(QObject * parent)
	: LogbookQueryModel(parent)
{
	ADD_OPTIONAL_COLUMN(time_t, datetime, setDateTime, "Dive Date", new DelegateFactory<DateTimeDelegate>);
	ADD_OPTIONAL_COLUMN(int, rating, setRating, "Rating", new DelegateFactory<RatingDelegate>);
	ADD_OPTIONAL_COLUMN(int, number, setNumber, "Number");
	ADD_FIELD_COLUMN(int, repetition, setRepetition, "Repetition");
	ADD_FIELD_COLUMN(int, interval, setInterval, "Surface Interval", new DelegateFactory<MinutesDelegate>, true);

	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>(
		new ForeignKeyWrapper<LogbookQueryModel::model_type, DiveSite, std::string>(
			& LogbookQueryModel::model_type::site,
			new CountryFieldAdapter<DiveSite>(& DiveSite::country_)
		), "country", "Country", NULL, true
	));

	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>(
		new ForeignKeyWrapper<LogbookQueryModel::model_type, DiveSite, std::string>(
			& LogbookQueryModel::model_type::site,
			new DefaultFieldAdapter<DiveSite, std::string>(& DiveSite::place)
		), "location", "Location"
	));

	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>(
		new ForeignKeyWrapper<LogbookQueryModel::model_type, DiveSite, std::string>(
			& LogbookQueryModel::model_type::site,
			new DefaultFieldAdapter<DiveSite, std::string>(& DiveSite::name)
		), "dive_site", "Dive Site"
	));

	ADD_FIELD_COLUMN(double, max_depth, setMaxDepth, "Max Depth", new DelegateFactory<DepthDelegate>);
	ADD_OPTIONAL_COLUMN(double, avg_depth, setAvgDepth, "Average Depth", new DelegateFactory<DepthDelegate>);
	ADD_FIELD_COLUMN(int, duration, setDuration, "Duration", new DelegateFactory<MinutesDelegate>);

	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>(
		new ForeignKeyWrapper<LogbookQueryModel::model_type, DiveComputer, std::string>(
			& LogbookQueryModel::model_type::computer,
			new DefaultFieldAdapter<DiveComputer, std::string>(
				& DiveComputer::name,
				& DiveComputer::setName,
				& DiveComputer::setName
			)
		), "computer", "Computer"
	));

	ADD_OPTIONAL_COLUMN(double, air_temp, setAirTemp, "Air Temp", new DelegateFactory<TemperatureDelegate>);
	ADD_OPTIONAL_COLUMN(double, max_temp, setMaxTemp, "Max Temp", new DelegateFactory<TemperatureDelegate>);
	ADD_OPTIONAL_COLUMN(double, min_temp, setMinTemp, "Min Temp", new DelegateFactory<TemperatureDelegate>);

	ADD_OPTIONAL_COLUMN(double, start_pressure, setStartPressure, "Start Pressure", new DelegateFactory<PressureDelegate>);
	ADD_OPTIONAL_COLUMN(double, end_pressure, setEndPressure, "End Pressure", new DelegateFactory<PressureDelegate>);

	ADD_OPTIONAL_COLUMN(std::string, comments, setComments, "Comments", NULL, true, true);

	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>(
		new ForeignKeyAdapter<LogbookQueryModel::model_type, DiveSite>(
			& LogbookQueryModel::model_type::site,
			& LogbookQueryModel::model_type::setSite,
			& LogbookQueryModel::model_type::setSite
		), "fk_dive_site", "", NULL, true, true
	));

	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>(
		new ForeignKeyAdapter<LogbookQueryModel::model_type, DiveComputer>(
			& LogbookQueryModel::model_type::computer,
			& LogbookQueryModel::model_type::setComputer,
			& LogbookQueryModel::model_type::setComputer
		), "fk_dive_computer", "", NULL, true, true
	));

	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>(
		new ForeignKeyAdapter<LogbookQueryModel::model_type, Mix>(
			& LogbookQueryModel::model_type::mix,
			& LogbookQueryModel::model_type::setMix,
			& LogbookQueryModel::model_type::setMix
		), "fk_mix", "", NULL, true, true
	));

	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>(
		new ForeignKeyWrapper<LogbookQueryModel::model_type, Mix, std::string>(
			& LogbookQueryModel::model_type::mix,
			new DefaultFieldAdapter<Mix, std::string>(
				& Mix::name,
				& Mix::setName,
				& Mix::setName
			)
		), "mix", "Mix"
	));

	ADD_OPTIONAL_COLUMN(std::string, start_pressure_group, setStartPressureGroup, "Starting PG", NULL, true, false, Qt::AlignCenter);
	ADD_OPTIONAL_COLUMN(std::string, end_pressure_group, setEndPressureGroup, "Ending PG", NULL, true, false, Qt::AlignCenter);
	ADD_OPTIONAL_COLUMN(int, rnt, setRNT, "RNT", new DelegateFactory<MinutesDelegate>, true);
	ADD_OPTIONAL_COLUMN(int, desat_time, setDesatTime, "Desat Time", new DelegateFactory<MinutesDelegate>, true);
	ADD_OPTIONAL_COLUMN(int, nofly_time, setNoFlyTime, "No-Fly Time", new DelegateFactory<MinutesDelegate>, true);
	ADD_OPTIONAL_COLUMN(std::string, algorithm, setAlgorithm, "Algorithm", NULL, true);

	ADD_FIELD_COLUMN(bool, safety_stop, setSafetyStop, "Safety Stop", NULL, true);
	ADD_OPTIONAL_COLUMN(double, stop_depth, setStopDepth, "Stop Depth", new DelegateFactory<DepthDelegate>, true);
	ADD_OPTIONAL_COLUMN(int, stop_time, setStopTime, "Stop Time", new DelegateFactory<MinutesDelegate>, true);

	ADD_OPTIONAL_COLUMN(std::string, visibility_category, setVisibilityCategory, "Visibility Category", NULL, true);
	ADD_OPTIONAL_COLUMN(double, visibility_distance, setVisibilityDistance, "Visibility Distance", new DelegateFactory<DepthDelegate>, true);
	ADD_OPTIONAL_COLUMN(double, weight, setWeight, "Weight", new DelegateFactory<WeightDelegate>, true);

	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>(
		new ForeignKeyAdapter<LogbookQueryModel::model_type, Tank>(
			& LogbookQueryModel::model_type::tank,
			& LogbookQueryModel::model_type::setTank,
			& LogbookQueryModel::model_type::setTank
		), "fk_tank", "", NULL, true, true
	));

	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>(
		new ForeignKeyWrapper<LogbookQueryModel::model_type, Tank, std::string>(
			& LogbookQueryModel::model_type::tank,
			new DefaultFieldAdapter<Tank, std::string>(
				& Tank::name,
				& Tank::setName,
				& Tank::setName
			)
		), "tank", "Primary Tank"
	));
}

DiveModel::~DiveModel()
{
}

struct dive_compare_dates
{
	bool operator()(Dive::Ptr lhs, Dive::Ptr rhs)
	{
		return (lhs->datetime() < rhs->datetime());
	}
};

std::vector<Profile::Ptr> DiveModel::Merge(const std::vector<Dive::Ptr> & dives_, Dive::Ptr & new_dive)
{
	std::vector<Dive::Ptr> dives(dives_.begin(), dives_.end());
	std::sort(dives.begin(), dives.end(), dive_compare_dates());

	/*
	 * Ensure all dives belong to the same, valid Session
	 */
	std::vector<Dive::Ptr>::iterator it;
	for (it = dives.begin(); it != dives.end(); it++)
	{
		if (! (* it) || ! (* it)->session())
			throw std::runtime_error("Dives do not belong to a Session");
		if ((* it)->session() != (* dives.begin())->session())
			throw std::runtime_error("Dives do not belong to the same Session");
	}

	Dive::Ptr dFirst = * dives.begin();
	Dive::Ptr dLast = * (dives.end()-1);

	IProfileFinder::Ptr pf = boost::dynamic_pointer_cast<IProfileFinder>(dFirst->session()->finder<Profile>());
	if (! pf)
		throw std::runtime_error("Failed to obtain IProfileFinder");

	/*
	 * Get the list of Profiles to Merge
	 */
	std::map<DiveComputer::Ptr, std::list<Profile::Ptr> > merge_profiles;
	for (it = dives.begin(); it != dives.end(); it++)
	{
		std::vector<Profile::Ptr> plist = pf->findByDive((* it)->id());
		std::vector<Profile::Ptr>::const_iterator pit;
		for (pit = plist.begin(); pit != plist.end(); pit++)
		{
			if (merge_profiles.find((* pit)->computer()) == merge_profiles.end())
				merge_profiles.insert(std::pair<DiveComputer::Ptr, std::list<Profile::Ptr> >((* pit)->computer(), std::list<Profile::Ptr>()));

			merge_profiles.at((* pit)->computer()).push_back(* pit);
		}
	}

	/*
	 * Check for consistency in the list of Profiles to Merge
	 */
	std::map<DiveComputer::Ptr, std::list<Profile::Ptr> >::iterator mit;
	for (mit = merge_profiles.begin(); mit != merge_profiles.end(); mit++)
	{
		if (mit->second.size() != dives.size())
			throw std::runtime_error("Dives do not have the same number and source of Profiles");
	}

	/*
	 * Create the new Dive
	 */
	new_dive = Dive::Ptr(new Dive);

	/*
	 * Merge Profiles
	 */
	std::vector<Profile::Ptr> profiles;
	for (mit = merge_profiles.begin(); mit != merge_profiles.end(); mit++)
	{
		Profile::Ptr fProfile = * mit->second.begin();
		Profile::Ptr profile(new Profile);
		profile->setComputer(mit->first);
		profile->setDive(new_dive);

		if (fProfile->name())
			profile->setName(fProfile->name().get());

		if (fProfile->vendor())
			profile->setVendor(fProfile->vendor().get());

		int offset = 0;
		std::list<waypoint> waypoints;
		std::list<Profile::Ptr>::iterator pit;
		for (pit = mit->second.begin(); pit != mit->second.end(); pit++)
		{
			int last = 0;
			int diff = 0;

			std::list<waypoint>::const_iterator wit;
			for (wit = (* pit)->profile().begin(); wit != (* pit)->profile().end(); wit++)
			{
				waypoint w;
				w.time = wit->time + offset;
				w.mix = wit->mix;
				w.alarms = wit->alarms;
				w.data = wit->data;
				waypoints.push_back(w);

				diff = wit->time - last;
				last = wit->time;
			}

			offset += last + diff;
		}

		profile->setProfile(waypoints);
		profiles.push_back(profile);
	}

	/*
	 * Guess at how much "extra" time is included in the profile.  This is
	 * based only on the profile that matches the dive computer set in the
	 * first dive.
	 */
	int extra = 0;
	DiveComputer::Ptr dc = dFirst->computer();
	if (dc && (merge_profiles.find(dc) != merge_profiles.end()))
	{
		Profile::Ptr pFirst = * merge_profiles.at(dc).begin();
		std::list<waypoint>::const_iterator wit = pFirst->profile().end();
		--wit;

		int wduration = wit->time / 60 + 1;
		extra = wduration - dFirst->duration();
	}

	/*
	 * Calculate Merged Dive Statistics
	 */
	int duration = extra * (dives.size() - 1);
	double max_depth = 0;
	double avg_depth = 0;
	double air_temp = 0;
	double max_temp = 0;
	double min_temp = 200;

	for (it = dives.begin(); it != dives.end(); it++)
	{
		duration += (* it)->duration();
		if ((* it)->max_depth() > max_depth)
			max_depth = (* it)->max_depth();

		if (! (* it)->avg_depth())
			avg_depth = -1;
		if (avg_depth != -1)
			avg_depth += (* it)->avg_depth().get() * (* it)->duration();

		if ((* it)->air_temp() && ((* it)->air_temp().get() > air_temp))
			air_temp = (* it)->air_temp().get();

		if ((* it)->max_temp() && ((* it)->max_temp().get() > max_temp))
			max_temp = (* it)->max_temp().get();

		if ((* it)->min_temp() && ((* it)->min_temp().get() < min_temp))
			min_temp = (* it)->min_temp().get();
	}

	if (avg_depth != -1)
		avg_depth /= duration;

	/*
	 * Merge Dive Header Data
	 */
	if (dFirst->datetime())
		new_dive->setDateTime(dFirst->datetime().get());
	if (dFirst->utc_offset())
		new_dive->setUTCOffset(dFirst->utc_offset().get());
	if (dFirst->number())
		new_dive->setNumber(dFirst->number().get());

	new_dive->setComputer(dFirst->computer());
	new_dive->setSite(dFirst->site());

	new_dive->setInterval(dFirst->interval());
	new_dive->setRepetition(dFirst->repetition());
	new_dive->setDuration(duration);
	new_dive->setMaxDepth(max_depth);

	if (avg_depth != -1)
		new_dive->setAvgDepth(avg_depth);

	if (air_temp != 0)
		new_dive->setAirTemp(air_temp);
	if (max_temp != 0)
		new_dive->setMaxTemp(max_temp);
	if (min_temp != 200)
		new_dive->setMinTemp(min_temp);

	if (dFirst->start_pressure())
		new_dive->setStartPressure(dFirst->start_pressure().get());
	if (dLast->end_pressure())
		new_dive->setEndPressure(dLast->end_pressure().get());

	new_dive->setMix(dFirst->mix());

	if (dFirst->salinity())
		new_dive->setSalinity(dFirst->salinity().get());
	if (dFirst->comments())
		new_dive->setComments(dFirst->comments().get());
	if (dFirst->rating())
		new_dive->setRating(dFirst->rating().get());

	new_dive->setSafetyStop(dLast->safety_stop());
	if (dLast->stop_depth())
		new_dive->setStopDepth(dLast->stop_depth().get());
	if (dLast->stop_time())
		new_dive->setStopTime(dLast->stop_time().get());

	if (dFirst->weight())
		new_dive->setWeight(dFirst->weight().get());
	if (dFirst->visibility_category())
		new_dive->setVisibilityCategory(dFirst->visibility_category().get());
	if (dFirst->visibility_distance())
		new_dive->setVisibilityDistance(dFirst->visibility_distance().get());

	if (dFirst->start_pressure_group())
		new_dive->setStartPressureGroup(dFirst->start_pressure_group().get());
	if (dLast->end_pressure_group())
		new_dive->setEndPressureGroup(dLast->end_pressure_group().get());
	if (dLast->rnt())
		new_dive->setRNT(dLast->rnt().get());
	if (dLast->desat_time())
		new_dive->setDesatTime(dLast->desat_time().get());
	if (dLast->nofly_time())
		new_dive->setNoFlyTime(dLast->nofly_time().get());
	if (dFirst->algorithm())
		new_dive->setAlgorithm(dFirst->algorithm().get());

	new_dive->tags()->assign(dFirst->tags());

	/*
	 * Return Profiles
	 */
	return profiles;
}
