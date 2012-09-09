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

#include <boost/bind.hpp>

#include <benthos/logbook/dive_site.hpp>
#include <benthos/logbook/dive.hpp>
#include <benthos/logbook/session.hpp>

#include "logbook_model.hpp"

using namespace benthos::logbook;

/*
 * Data Source for All Dives
 */
struct AllDivesDataSource: public ILogbookDataSource<Dive>
{
	virtual std::vector<Dive::Ptr> getItems(Session::Ptr session) const
	{
		return session->finder<Dive>()->find();
	}
};

/*
 * Data Source for Dives starting from a given Date Range through Today
 */
struct DateRangeDiveDataSource2: public ILogbookDataSource<Dive>
{
	time_t		start;

	DateRangeDiveDataSource2(time_t start_)
		: start(start_)
	{
	}

	virtual std::vector<Dive::Ptr> getItems(Session::Ptr session) const
	{
		IDiveFinder::Ptr df = boost::dynamic_pointer_cast<IDiveFinder>(session->finder<Dive>());
		return df->findByDates(start, time(NULL));
	}
};

/*
 * Data Source for Dives by Country
 */
struct CountryDiveDataSource: public ILogbookDataSource<Dive>
{
	country		country_;

	CountryDiveDataSource(const country country__)
		: country_(country__)
	{
	}

	virtual std::vector<Dive::Ptr> getItems(Session::Ptr session) const
	{
		IDiveFinder::Ptr df = boost::dynamic_pointer_cast<IDiveFinder>(session->finder<Dive>());
		return df->findByCountry(country_);
	}
};

/*
 * Null Data Source for Dives
 */
struct NullDivesDataSource: public ILogbookDataSource<Dive>
{
	virtual std::vector<Dive::Ptr> getItems(Session::Ptr session) const
	{
		return std::vector<Dive::Ptr>();
	}
};

/*
 * Data Source for All Sites
 */
struct AllSitesDataSource: public ILogbookDataSource<DiveSite>
{
	virtual std::vector<DiveSite::Ptr> getItems(Session::Ptr session) const
	{
		return session->finder<DiveSite>()->find();
	}
};

LogbookModel::LogbookModel(QObject * parent)
	: QAbstractItemModel(parent), m_logbook(), m_items()
{
	m_items.push_back(TopLevelItem::Ptr(new TopLevelItem("Logbook")));
	m_items.push_back(TopLevelItem::Ptr(new TopLevelItem("Date")));
	m_items.push_back(TopLevelItem::Ptr(new TopLevelItem("Countries")));
	m_items.push_back(TopLevelItem::Ptr(new TopLevelItem("Computers")));

	/*
	 * Logbook Items
	 */
	m_items[0]->append(LogbookModelItem::Ptr(new DataSourceItem<Dive>(
		new AllDivesDataSource,
		tr("All Dives"),
		QImage(":/icons/diveflag.png"),
		LogbookModelItem::DiveListItem
	)));

	m_items[0]->append(LogbookModelItem::Ptr(new DataSourceItem<Dive>(
		new NullDivesDataSource,
		tr("Recently Imported"),
		QImage(":/icons/clock.png"),
		LogbookModelItem::DiveListItem
	)));

	m_items[0]->append(LogbookModelItem::Ptr(new DataSourceItem<DiveSite>(
		new AllSitesDataSource,
		tr("Sites"),
		QImage(":/icons/globe.png"),
		LogbookModelItem::SiteListItem
	)));

	/*
	 * Date Items
	 */
	time_t	last_morning;
	time_t	last_year;
	time_t	last_month;
	time_t	last_week;
	time_t	today = time(NULL);
	struct tm * tm;

	tm = localtime(& today);
	tm->tm_hour = 0;
	tm->tm_min = 0;
	tm->tm_sec = 0;
	tm->tm_isdst = -1;
	last_morning = mktime(tm);

	tm = localtime(& today);
	tm->tm_mday -= tm->tm_wday;
	tm->tm_hour = 0;
	tm->tm_min = 0;
	tm->tm_sec = 0;
	tm->tm_isdst = -1;
	last_week = mktime(tm);

	tm = localtime(& today);
	tm->tm_mday = 1;
	tm->tm_hour = 0;
	tm->tm_min = 0;
	tm->tm_sec = 0;
	tm->tm_isdst = -1;
	last_month = mktime(tm);

	tm = localtime(& today);
	tm->tm_mon = 0;
	tm->tm_mday = 1;
	tm->tm_hour = 0;
	tm->tm_min = 0;
	tm->tm_sec = 0;
	tm->tm_isdst = -1;
	last_year = mktime(tm);

	m_items[1]->append(LogbookModelItem::Ptr(new DataSourceItem<Dive>(
		new DateRangeDiveDataSource2(last_morning),
		tr("Today"),
		QImage(":/icons/calendar.png"),
		LogbookModelItem::DiveListItem
	)));

	m_items[1]->append(LogbookModelItem::Ptr(new DataSourceItem<Dive>(
		new DateRangeDiveDataSource2(last_week),
		tr("This Week"),
		QImage(":/icons/calendar.png"),
		LogbookModelItem::DiveListItem
	)));

	m_items[1]->append(LogbookModelItem::Ptr(new DataSourceItem<Dive>(
		new DateRangeDiveDataSource2(last_month),
		tr("This Month"),
		QImage(":/icons/calendar.png"),
		LogbookModelItem::DiveListItem
	)));

	m_items[1]->append(LogbookModelItem::Ptr(new DataSourceItem<Dive>(
		new DateRangeDiveDataSource2(last_year),
		tr("This Year"),
		QImage(":/icons/calendar.png"),
		LogbookModelItem::DiveListItem
	)));
}

LogbookModel::~LogbookModel()
{

}

int LogbookModel::columnCount(const QModelIndex & parent) const
{
	// Always a single column
	return 1;
}

void LogbookModel::computer_deleted(AbstractMapper::Ptr, Persistent::Ptr obj)
{
}

void LogbookModel::computer_inserted(AbstractMapper::Ptr, Persistent::Ptr obj)
{
}

void LogbookModel::computer_updated(AbstractMapper::Ptr, Persistent::Ptr obj)
{
}

QVariant LogbookModel::data(const QModelIndex & index, int role) const
{
	if (! index.isValid())
		return QVariant();

	if (! index.parent().isValid())
	{
		if ((index.row() >= (int)m_items.size()) || (index.column() > 0))
			return QVariant();
		return m_items[index.row()]->data(role);
	}

	return (* m_items[index.parent().row()])[index.row()]->data(role);
}

QModelIndex LogbookModel::defaultIndex() const
{
	return createIndex(0, 0, 0);
}

Qt::ItemFlags LogbookModel::flags(const QModelIndex & index) const
{
	if (! index.isValid())
		return 0;

	if (! index.parent().isValid())
		// Top-Level Item
		return Qt::ItemIsEnabled;

	// Leaf Node
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool LogbookModel::hasChildren(const QModelIndex & parent) const
{
	return rowCount(parent) > 0;
}

QVariant LogbookModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}

QModelIndex LogbookModel::index(int row, int column, const QModelIndex & parent) const
{
	if (parent.isValid())
		return createIndex(row, column, parent.row());

	// A parent id of -1 signifies no parent (top-level node)
	return createIndex(row, column, (quint32)-1);
}

LogbookModelItem::Ptr LogbookModel::item(const QModelIndex & index) const
{
	if (! index.isValid() || ! index.parent().isValid())
		return LogbookModelItem::Ptr();

	return (* m_items[index.parent().row()])[index.row()];
}

QModelIndex LogbookModel::makeIndex(int row, int column, quint32 internal_id) const
{
	return createIndex(row, column, internal_id);
}

Logbook::Ptr LogbookModel::logbook() const
{
	return m_logbook;
}

QModelIndex LogbookModel::parent(const QModelIndex & index) const
{
	if (! index.isValid())
		return QModelIndex();

	if (index.internalId() == (quint32)-1)
		return QModelIndex();

	return createIndex((int)index.internalId(), 0, (quint32)-1);
}

int LogbookModel::rowCount(const QModelIndex & parent) const
{
	if (parent.isValid())
	{
		if (parent.parent().isValid())
		{
			// No more than 2 levels
			return 0;
		}

		// Return number of Leaf Nodes
		return (int)m_items[parent.row()]->size();
	}

	// Return number of Top-Level Nodes
	return (int)m_items.size();
}

void LogbookModel::setLogbook(Logbook::Ptr logbook)
{
	beginResetModel();

	m_logbook = logbook;
	m_items[2]->clear();
	m_items[3]->clear();

	IDiveSiteFinder::Ptr dsf = boost::dynamic_pointer_cast<IDiveSiteFinder>(m_logbook->session()->finder<DiveSite>());
	std::vector<country> countries = dsf->countries();
	std::vector<country>::const_iterator it1;
	for (it1 = countries.begin(); it1 != countries.end(); it1++)
	{
		m_items[2]->append(LogbookModelItem::Ptr(new CountryLogbookItem<Dive>(
			new CountryDiveDataSource(* it1),
			* it1,
			LogbookModelItem::DiveListItem
		)));
	}

	m_logbook->session()->mapper<DiveSite>()->events().before_delete.connect(boost::bind(& LogbookModel::site_deleted, this, _1, _2));
	m_logbook->session()->mapper<DiveSite>()->events().after_insert.connect(boost::bind(& LogbookModel::site_inserted, this, _1, _2));
	m_logbook->session()->mapper<DiveSite>()->events().after_update.connect(boost::bind(& LogbookModel::site_updated, this, _1, _2));

	IDiveComputerFinder::Ptr dcf = boost::dynamic_pointer_cast<IDiveComputerFinder>(m_logbook->session()->finder<DiveComputer>());
	std::vector<DiveComputer::Ptr> computers = dcf->find();
	std::vector<DiveComputer::Ptr>::const_iterator it2;
	for (it2 = computers.begin(); it2 != computers.end(); it2++)
	{
		QString mfg;
		QString name;
		if (! (* it2)->name())
			continue;

		if ((* it2)->manufacturer())
			mfg = QString::fromStdString((* it2)->manufacturer().get());

		m_items[3]->append(LogbookModelItem::Ptr(new ItemSourceItem<DiveComputer>(
			* it2,
			QString::fromStdString((* it2)->name().get()),
			mfg.isEmpty() ? QImage() : QImage(QString(":/icons/%1.ico").arg(mfg.toLower())),
			LogbookModelItem::DeviceItem
		)));
	}

	endResetModel();
}

void LogbookModel::site_deleted(AbstractMapper::Ptr m, Persistent::Ptr obj)
{
	DiveSite::Ptr ds = boost::shared_polymorphic_downcast<DiveSite>(obj);
	if (! ds || ! (ds->country_().is_initialized()))
		return;
}

void LogbookModel::site_inserted(AbstractMapper::Ptr, Persistent::Ptr obj)
{
	DiveSite::Ptr ds = boost::shared_polymorphic_downcast<DiveSite>(obj);
	if (! ds)
		return;
}

void LogbookModel::site_updated(AbstractMapper::Ptr, Persistent::Ptr obj)
{
	DiveSite::Ptr ds = boost::shared_polymorphic_downcast<DiveSite>(obj);
	if (! ds)
		return;
}
