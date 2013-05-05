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

#include <ctime>
#include <map>
#include <stdexcept>
#include <string>

#include <QMetaType>

#include <benthos/logbook/dive.hpp>
#include <benthos/logbook/mix.hpp>

#include <benthos/divecomputer/config.hpp>
#include <benthos/divecomputer/driverclass.hpp>
#include <benthos/divecomputer/plugin.hpp>
#include <benthos/divecomputer/registry.hpp>

#include <yajl/yajl_gen.h>

#include "transferworker.hpp"

using namespace benthos::dc;
using namespace benthos::logbook;

typedef std::map<uint8_t, int32_t>						vendor_entry_t;
typedef std::map<std::string, vendor_entry_t, cicmp>	vendor_data_t;

typedef struct
{
	Dive::Ptr						dive;
	waypoint						curwp;
	Mix::Ptr						curmix;
	bool							haswp;
	std::list<waypoint>				profile;
	std::map<uint8_t, Mix::Ptr>		mixes;
	vendor_data_t					vendor;
	IMixFinder::Ptr					mixfinder;

} parser_data;

std::string json_encode(const vendor_data_t & data)
{
	yajl_gen g = yajl_gen_alloc(NULL);
	yajl_gen_config(g, yajl_gen_beautify, 0);
	yajl_gen_config(g, yajl_gen_validate_utf8, 1);

	yajl_gen_array_open(g);
	yajl_gen_map_open(g);

	vendor_data_t::const_iterator it;
	for (it = data.begin(); it != data.end(); it++)
	{
		if (it->second.size() > 1)
		{
			vendor_entry_t::const_iterator vit;
			for (vit = it->second.begin(); vit != it->second.end(); vit++)
			{
				char buf[1024];
				int len = sprintf(buf, "%s%d", it->first.c_str(), vit->first);
				yajl_gen_string(g, (const unsigned char *)buf, len);
				yajl_gen_integer(g, vit->second);
			}
		}
		else
		{
			vendor_entry_t::const_iterator vit = it->second.begin();

			if (vit->first != 0)
			{
				char buf[1024];
				int len = sprintf(buf, "%s%d", it->first.c_str(), vit->first);
				yajl_gen_string(g, (const unsigned char *)buf, len);
			}
			else
			{
				yajl_gen_string(g, (const unsigned char *)it->first.c_str(), it->first.size());
			}

			yajl_gen_integer(g, vit->second);
		}
	}

	yajl_gen_map_close(g);
	yajl_gen_array_close(g);

	const unsigned char * buf;
	size_t buflen;

	yajl_gen_get_buf(g, & buf, & buflen);
	std::string result((const char *)buf);
	yajl_gen_clear(g);

	return result;
}

int device_info(void * userdata, uint8_t model, uint32_t serial, uint32_t ticks, char ** token, int * free_token)
{
	TransferWorker * worker = static_cast<TransferWorker *>(userdata);
	if (! worker)
		return -1;

	std::string token_;
	int ret = worker->driver_devinfo(model, serial, ticks, token_);
	if (ret)
		return ret;

	if (token_.size())
	{
		(* token) = strdup(token_.c_str());
		(* free_token) = 1;
	}
	else
	{
		(* token) = NULL;
		(* free_token) = 0;
	}

	return 0;
}

void parse_header(void * userdata, uint8_t token, int32_t value, uint8_t index, const char * name)
{
	time_t st;
	parser_data * _data = (parser_data *)userdata;

	switch (token)
	{
	case DIVE_HEADER_START_TIME:
	{
		st = (time_t)value;
		struct tm * tmp;
		tmp = gmtime(& st);
		_data->dive->setDateTime(mktime(tmp));
		break;
	}

	case DIVE_HEADER_UTC_OFFSET:
	{
		_data->dive->setUTCOffset(value);
		break;
	}

	case DIVE_HEADER_DURATION:
	{
		_data->dive->setDuration(value);
		break;
	}

	case DIVE_HEADER_INTERVAL:
	{
		_data->dive->setInterval(value);
		break;
	}

	case DIVE_HEADER_REPETITION:
	{
		_data->dive->setRepetition(value);
		break;
	}

	case DIVE_HEADER_DESAT_BEFORE:
	{
		// Not Used
		break;
	}

	case DIVE_HEADER_DESAT_AFTER:
	{
		_data->dive->setDesatTime(value);
		break;
	}

	case DIVE_HEADER_NOFLY_BEFORE:
	{
		// Not Used
		break;
	}

	case DIVE_HEADER_NOFLY_AFTER:
	{
		_data->dive->setNoFlyTime(value);
		break;
	}

	case DIVE_HEADER_MAX_DEPTH:
	{
		_data->dive->setMaxDepth(value / 100.0f);
		break;
	}

	case DIVE_HEADER_AVG_DEPTH:
	{
		_data->dive->setAvgDepth(value / 100.0f);
		break;
	}

	case DIVE_HEADER_AIR_TEMP:
	{
		_data->dive->setAirTemp(value / 100.0f);
		break;
	}

	case DIVE_HEADER_MAX_TEMP:
	{
		_data->dive->setMaxTemp(value / 100.0f);
		break;
	}

	case DIVE_HEADER_MIN_TEMP:
	{
		_data->dive->setMinTemp(value / 100.0f);
		break;
	}

	case DIVE_HEADER_PX_START:
	{
		// Saves header data only for main tank
		if (! index)
			_data->dive->setStartPressure(value / 1000.0f);
		break;
	}

	case DIVE_HEADER_PX_END:
	{
		// Saves header data only for main tank
		if (! index)
			_data->dive->setEndPressure(value / 1000.0f);
		break;
	}

	case DIVE_HEADER_PMO2:
	{
		std::map<uint8_t, Mix::Ptr>::iterator it = _data->mixes.find(index);
		if (it == _data->mixes.end())
		{
			// New Mix
			Mix::Ptr mix = Mix::Ptr(new Mix);
			mix->setO2PerMil(value);

			_data->mixes.insert(std::pair<uint8_t, Mix::Ptr>(index, mix));
		}
		else
		{
			// Update Mix
			it->second->setO2PerMil(value);
		}

		break;
	}

	case DIVE_HEADER_PMHe:
	{
		std::map<uint8_t, Mix::Ptr>::iterator it = _data->mixes.find(index);
		if (it == _data->mixes.end())
		{
			// New Mix
			Mix::Ptr mix = Mix::Ptr(new Mix);
			mix->setHePerMil(value);

			_data->mixes.insert(std::pair<uint8_t, Mix::Ptr>(index, mix));
		}
		else
		{
			// Update Mix
			it->second->setHePerMil(value);
		}

		break;
	}

	case DIVE_HEADER_VENDOR:
	{
		std::string key(name);
		vendor_data_t::iterator it = _data->vendor.find(key);

		if (it == _data->vendor.end())
		{
			std::pair<vendor_data_t::iterator, bool> ret = _data->vendor.insert(std::pair<std::string, vendor_entry_t>(key, vendor_entry_t()));
			if (ret.second)
				it = ret.first;
			else
				break;
		}

		it->second.insert(std::pair<uint8_t, int32_t>(index, value));
		break;
	}
	}
}

void process_header(parser_data * _data)
{
	// Check for mixes that are already in the database
	std::map<uint8_t, Mix::Ptr>::iterator it;
	for (it = _data->mixes.begin(); it != _data->mixes.end(); it++)
	{
		Mix::Ptr mix = _data->mixfinder->findByMix(it->second->o2_permil(), it->second->he_permil());
		if (mix)
			it->second = mix;
	}
}

void parse_profile(void * userdata, uint8_t token, int32_t value, uint8_t index, const char * name)
{
	parser_data * _data = (parser_data *)userdata;

	switch (token)
	{
	case DIVE_WAYPOINT_TIME:
	{
		if (! _data->haswp)
		{
			// Process Mix Data
			process_header(_data);

			// Initialize Mix to Mix 0
			std::map<uint8_t, Mix::Ptr>::iterator it = _data->mixes.find(0);
			if (it != _data->mixes.end())
				_data->curmix = it->second;
		}

		if (_data->haswp)
			_data->profile.push_back(_data->curwp);

		_data->curwp.alarms.clear();
		_data->curwp.data.clear();
		_data->curwp.mix = _data->curmix;
		_data->curwp.time = value;
		_data->haswp = true;

		break;
	}

	case DIVE_WAYPOINT_DEPTH:
	{
		_data->curwp.data.insert(std::pair<std::string, double>("depth", value / 100.0f));
		break;
	}

	case DIVE_WAYPOINT_TEMP:
	{
		_data->curwp.data.insert(std::pair<std::string, double>("temp", value / 100.0f));
		break;
	}

	case DIVE_WAYPOINT_PX:
	{
		_data->curwp.data.insert(std::pair<std::string, double>("pressure", value / 1000.0f));
		break;
	}

	case DIVE_WAYPOINT_MIX:
	{
		std::map<uint8_t, Mix::Ptr>::iterator it = _data->mixes.find(index);
		if (it != _data->mixes.end())
			_data->curwp.mix = it->second;

		break;
	}

	case DIVE_WAYPOINT_ALARM:
	{
		if (name)
			_data->curwp.alarms.insert(name);
		break;
	}
	}
}

TransferWorker::TransferWorker(DiveComputer::Ptr dc, Session::Ptr session, bool checkSerNo, bool updateToken, QObject * parent)
	: QObject(parent), m_dc(dc), m_session(session), m_checkSN(checkSerNo), m_updateToken(updateToken),
	  m_cancel(false), m_started(false)
{
}

TransferWorker::~TransferWorker()
{
}

void TransferWorker::cancel()
{
	m_cancel = true;
}

bool TransferWorker::cancelled() const
{
	return m_cancel;
}

int TransferWorker::driver_devinfo(uint8_t model, uint32_t serial, uint32_t ticks, std::string & token)
{
	QString dcname;
	if (m_dc->name().is_initialized())
		dcname = QString::fromStdString(m_dc->name().get());
	else
		dcname = QString("'%1' Device").arg(QString::fromStdString(m_dc->driver()));

	char buf1[255];
	char buf2[255];

	sprintf(buf1, "%u", serial);
	sprintf(buf2, "%u", (uint32_t)(-1));

	// Validate Serial Number
	if ((m_checkSN || (m_dc->serial() != buf2)) && (m_dc->serial() != buf1))
	{
		emit transferError(QString("Serial number did not match with '%1': Got serial number %2")
			.arg(dcname)
			.arg(QString::fromAscii(buf1, strlen(buf1))));
		return -1;
	}

	// Set the Transfer Token
	if (m_dc->token().is_initialized() && ! m_dc->token().get().empty())
		token = m_dc->token().get();

	// Continue Transfer
	return 0;
}

void TransferWorker::driver_progress(uint32_t transferred, uint32_t total)
{
	if (! m_started)
	{
		if (! total)
		{
			emit started(0);
			emit status(QString("No data to transfer"));
			emit finished();
		}
		else
		{
			emit status(QString("Transferring %1 bytes").arg(total));
			emit started(total);
		}

		m_started = true;
	}

	emit progress(transferred);
}

void transfer_callback_fn(void * data, uint32_t transferred, uint32_t total, int * cancel)
{
	TransferWorker * obj = static_cast<TransferWorker *>(data);
	if (obj && obj->cancelled())
	{
		* cancel = 1;
		return;
	}

	if (obj)
		obj->driver_progress(transferred, total);
}

void TransferWorker::parse_dives(Driver::Ptr driver, const dive_data_t & dives)
{
	IMixFinder::Ptr mf = boost::dynamic_pointer_cast<IMixFinder>(m_session->finder<Mix>());
	Mix::Ptr air = mf->findByName("Air");

	parser_data data;
	data.mixfinder = mf;

	// Parse the Dives
	dive_data_t::const_iterator it;
	for (it = dives.begin(); it != dives.end(); it++)
	{
		// Setup Parser Data
		data.dive.reset(new Dive);
		data.mixes.clear();
		data.profile.clear();
		data.vendor.clear();

		data.haswp = false;
		data.curmix = air;

		// Parse the Header and Profile
		driver->parse(it->first, & parse_header, & parse_profile, & data);

		// Append the final Waypoint
		if (data.haswp)
			data.profile.push_back(data.curwp);

		// Update Dive Data
		data.dive->setComputer(m_dc);

		// Set the Dive Mix as the first Mix in the profile
		if (data.haswp)
			data.dive->setMix(data.profile.begin()->mix);

		// Create Profile
		Profile::Ptr profile = Profile::Ptr(new Profile);
		profile->setComputer(m_dc);
		profile->setDive(data.dive);
		profile->setImported(time(NULL));
		profile->setProfile(data.profile);
		profile->setRawProfile(it->first);
		profile->setVendor(json_encode(data.vendor));

		// Emit Parsed Dive
		emit parsedDive(profile);
	}
}

void TransferWorker::run()
{
	dive_data_t dive_data;
	QString dcname;
	if (m_dc->name().is_initialized())
		dcname = QString::fromStdString(m_dc->name().get());
	else
		dcname = QString("'%1' Device").arg(QString::fromStdString(m_dc->driver()));

	emit status(QString("Starting Transfer from %1").arg(dcname));

	// Load the Driver
	PluginRegistry::Ptr reg = PluginRegistry::Instance();
	DriverClass::Ptr dclass;
	Driver::Ptr dev;
	try
	{
		dclass = reg->loadDriver(m_dc->driver());
	}
	catch (std::exception & e)
	{
		emit transferError(QString("Failed to load driver '%1': %2")
			.arg(QString::fromStdString(m_dc->driver()))
			.arg(QString::fromStdString(e.what())));
		return;
	}

	emit status(QString("Loaded driver '%1'").arg(QString::fromStdString(m_dc->driver())));
	emit status(QString("Connecting to '%1'").arg(dcname));

	// Connect to the Device
	try
	{
		std::string devpath;
		std::string devargs;

		if (m_dc->device().is_initialized())
			devpath = m_dc->device().get();
		if (m_dc->driver_args().is_initialized())
			devargs = m_dc->driver_args().get();

		dev = dclass->open(devpath, devargs);
	}
	catch (std::exception & e)
	{
		emit transferError(QString("Failed to connect to '%1': %2")
			.arg(dcname)
			.arg(QString::fromStdString(e.what())));
		return;
	}

	emit status(QString("Connected to '%1'").arg(dcname));

	// Transfer Data
	try
	{
		dive_data = dev->transfer(& device_info, & transfer_callback_fn, this);
	}
	catch (std::exception & e)
	{
		emit transferError(QString::fromStdString(e.what()));
		return;
	}

	// No guarantee that transfer_callback_fn is called
	if (! m_started)
	{
		emit started(0);
		emit status(QString("No data to transfer"));
		emit finished();
	}

	if (dive_data.size())
	{
		// Store new Token
		if (m_updateToken)
			m_dc->setToken(dive_data.back().second);

		// Parse Data
		emit status(QString("Parsing %1 dives").arg(dive_data.size()));
		parse_dives(dev, dive_data);
	}

	emit status(QString("Transfer Successful"));
	emit finished();
}
