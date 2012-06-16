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

#ifndef SITE_MODEL_HPP_
#define SITE_MODEL_HPP_

/**
 * @file src/mvf/site_model.hpp
 * @brief Dive Site Model Classes
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include "mvf/modelif.hpp"
#include "mvf/models.hpp"

#include <benthos/logbook/dive_site.hpp>
using namespace benthos::logbook;

/**
 * @brief Position Field Adapter Class
 *
 * Specialization of DefaultFieldAdapter to display Latitude/Longitude
 * information as a QPoint while mapping to separate underlying fields.
 */
template <class T>
class PositionFieldAdapter: public IFieldAdapter<T>
{
protected:
	typedef FieldTraits<T, double>			traits_t;
	typedef typename traits_t::getter		getter_t;
	typedef typename traits_t::opt_getter	opt_getter_t;
	typedef typename traits_t::setter		setter_t;
	typedef void (T::*resetter_t)(const boost::none_t &);

public:

	//! Class Constructor
	PositionFieldAdapter(getter_t lat_getter, getter_t lng_getter, setter_t lat_setter = 0, setter_t lng_setter = 0)
		: m_lat_getter(lat_getter), m_lat_opt_getter(0), m_lat_setter(lat_setter), m_lat_resetter(0),
		  m_lng_getter(lng_getter), m_lng_opt_getter(0), m_lng_setter(lng_setter), m_lng_resetter(0)
	{
		if (! m_lat_getter)
			throw std::runtime_error("Latitude Field Getter cannot be NULL");
		if (! m_lng_getter)
			throw std::runtime_error("Longitude Field Getter cannot be NULL");
	}

	//! Class Constructor
	PositionFieldAdapter(opt_getter_t lat_getter, opt_getter_t lng_getter, setter_t lat_setter = 0, setter_t lng_setter = 0,
			resetter_t lat_resetter = 0, resetter_t lng_resetter = 0)
		: m_lat_getter(0), m_lat_opt_getter(lat_getter), m_lat_setter(lat_setter), m_lat_resetter(lat_resetter),
		  m_lng_getter(0), m_lng_opt_getter(lng_getter), m_lng_setter(lng_setter), m_lng_resetter(lng_resetter)
	{
		if (! m_lat_opt_getter)
			throw std::runtime_error("Latitude Field Getter cannot be NULL");
		if (! m_lng_opt_getter)
			throw std::runtime_error("Longitude Field Getter cannot be NULL");
	}

	//! Class Destructor
	virtual ~PositionFieldAdapter()
	{
	}

public:

	//! @param[in] Session Pointer
	virtual void bind(logbook::Session::Ptr session)
	{
	}

	//! @return Decoration Value for the Field
	virtual QVariant decorationData(const boost::shared_ptr<T> & item) const
	{
		return QVariant();
	}

	//! @return Display Value for the Field
	virtual QVariant displayData(const boost::shared_ptr<T> & item) const
	{
		if ((m_lat_opt_getter != 0) && (m_lng_opt_getter != 0))
		{
			boost::optional<double> olat = ((item.get())->*m_lat_opt_getter)();
			boost::optional<double> olng = ((item.get())->*m_lng_opt_getter)();

			if (! olat || ! olng)
				return QVariant();
			else
				return QVariant(QPointF(olng.get(), olat.get()));
		}

		double lat = ((item.get())->*m_lat_getter)();
		double lng = ((item.get())->*m_lng_getter)();

		return QVariant(QPointF(lng, lat));
	}

	//! @return Edit Value for the Field
	virtual QVariant editData(const boost::shared_ptr<T> & item) const
	{
		return displayData(item);
	}

	//! @brief Set the Edit Value for the Field
	virtual bool setEditData(boost::shared_ptr<T> & item, const QVariant & v) const
	{
		if (! m_lat_setter || ! m_lng_setter || (v.isNull() && (! m_lat_resetter || ! m_lng_resetter)))
			return false;

		if (v.isNull())
		{
			((item.get())->*m_lat_resetter)(boost::none);
			((item.get())->*m_lng_resetter)(boost::none);
		}
		else
		{
			QPointF p = v.toPointF();

			((item.get())->*m_lat_setter)(p.y());
			((item.get())->*m_lng_setter)(p.x());
		}

		return true;
	}

protected:
	getter_t			m_lat_getter;
	opt_getter_t		m_lat_opt_getter;
	setter_t			m_lat_setter;
	resetter_t			m_lat_resetter;

	getter_t			m_lng_getter;
	opt_getter_t		m_lng_opt_getter;
	setter_t			m_lng_setter;
	resetter_t			m_lng_resetter;

};

/**
 * @brief Dive Site Model
 *
 * Fields:
 * 0: Site Name
 * 1: Place Name
 * 2: Country
 * 3: Salinity
 * 4: Platform
 * 5: Water Body
 * 6: Bottom Type
 * 7: Altitude
 * 8: Position [Hidden]
 * 9: Num Dives [RO]
 * 10: Rating [RO]
 * 11: Comments [Internal]
 */
class DiveSiteModel: public LogbookQueryModel<DiveSite>
{
public:

	//! Class Constructor
	DiveSiteModel(QObject * parent = 0);

	//! Class Destructor
	virtual ~DiveSiteModel();

};

#endif /* SITE_MODEL_HPP_ */
