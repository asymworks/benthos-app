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

#include <QApplication>
#include <QMetaType>

#include <boost/locale.hpp>

#include <benthos/logbook/logging.hpp>
#include <benthos/logbook/profile.hpp>

#include "config.hpp"
#include "mainwindow.hpp"

using namespace benthos::logbook;

// Declare Custom MetaTypes
Q_DECLARE_METATYPE(Profile::Ptr)

class LevelFilter: public logging::log_filter
{
public:

	LevelFilter(int min = 0, int max = 100)
		: _min(min), _max(max)
	{
	}

	virtual ~LevelFilter()
	{
	}

	bool apply(const logging::log_record & rec) const
	{
		return ((_min <= rec.level()) && (rec.level() <= _max));
	}

protected:
	int _min;
	int _max;
};

struct RedFormatter: public logging::default_formatter
{
	virtual ~RedFormatter() { }

	virtual std::string format(const logging::log_record & record) const
	{
		std::string prefix = "\033[1;31m";
		std::string suffix = "\033[0m";

		return prefix + logging::default_formatter::format(record) + suffix;
	}
};

struct GreyFormatter: public logging::default_formatter
{
	virtual ~GreyFormatter() { }

	virtual std::string format(const logging::log_record & record) const
	{
		std::string prefix = "\033[1;37m";
		std::string suffix = "\033[0m";

		return prefix + logging::default_formatter::format(record) + suffix;
	}
};

void init_logging()
{
	logging::log_filter * f1 = new LevelFilter(logging::level::NOTSET, logging::level::INFO);
	logging::log_handler * h1 = new logging::stream_handler(std::cout);
	h1->addFilter(f1);
	h1->setFormatter(new GreyFormatter);

	logging::log_filter * f2 = new LevelFilter(logging::level::WARNING);
	logging::log_handler * h2 = new logging::stream_handler(std::cerr);
	h2->addFilter(f2);
	h2->setFormatter(new RedFormatter);

	logging::logger * logger = logging::getLogger();
	logger->addHandler(h1);
	logger->addHandler(h2);
	logger->setLevel(logging::level::INFO);
	//logger->setLevel(logging::level::DEBUG);
}

int main(int argc, char **  argv)
{
	init_logging();
	logging::getLogger("main")->debug("Starting benthos");

	// Setup Boost Locales with Default System Locale
	boost::locale::generator gen;
	std::locale::global(gen(""));

	// Setup the Application
	QApplication app(argc, argv);
	app.setApplicationName(BENTHOS_APP_NAME);
	app.setApplicationVersion(BENTHOS_APP_VERSION);
	app.setOrganizationDomain(BENTHOS_ORG_DOMAIN);
	app.setOrganizationName(BENTHOS_ORG_NAME);

	// Register Custom Metatypes
	qRegisterMetaType<Profile::Ptr>();

	// Load Main Window and Execute
	MainWindow * w = new MainWindow;
	w->show();

	return app.exec();
}
