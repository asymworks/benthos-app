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

#ifndef MODELIF_HPP_
#define MODELIF_HPP_

/**
 * @file src/mvf/modelif.hpp
 * @brief Model Class Interfaces
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <map>
#include <stdexcept>
#include <string>

#include <QDateTime>
#include <QVariant>

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

#include <benthos/logbook/country.hpp>
#include <benthos/logbook/persistent.hpp>
#include <benthos/logbook/session.hpp>

using namespace benthos;

/**
 * @brief Field Adapter Interface
 *
 * Adapts a single model field into a Qt Model-View Framework compatible set
 * of getters and setters corresponding to individual data roles, namely the
 * DisplayRole, EditRole and DecorationRole roles.
 */
template <class T>
struct IFieldAdapter
{
	virtual ~IFieldAdapter() { }

	//! @param[in] Session Pointer
	virtual void bind(logbook::Session::Ptr session) = 0;

	//! @return Decoration Value for the Field
	virtual QVariant decorationData(const boost::shared_ptr<T> &) const = 0;

	//! @return Display Value for the Field
	virtual QVariant displayData(const boost::shared_ptr<T> &) const = 0;

	//! @return Edit Value for the Field
	virtual QVariant editData(const boost::shared_ptr<T> &) const = 0;

	//! @brief Set the Edit Value for the Field
	virtual bool setEditData(boost::shared_ptr<T> &, const QVariant &) const = 0;

};

/**
 * @brief Traits Class for a Model Field
 *
 * Stores getter/setter function signatures for model fields, parameterized
 * on the basic return type.
 */
template <class T, typename V>
struct FieldTraits
{
	typedef V (T::*getter)(void) const;
	typedef const boost::optional<V> & (T::*opt_getter)(void) const;
	typedef void (T::*setter)(V);

	static QVariant toVariant(const V & v)
	{
		return QVariant::fromValue(v);
	}

	static V fromVariant(const QVariant & v)
	{
		return v.value<V>();
	}
};

/**
 * @brief Model Field Type Traits for std::string
 */
template <class T>
struct FieldTraits<T, std::string>
{
	typedef const std::string & (T::*getter)(void) const;
	typedef const boost::optional<std::string> & (T::*opt_getter)(void) const;
	typedef void (T::*setter)(const std::string &);

	static QVariant toVariant(const std::string & v)
	{
		return QVariant(QString::fromStdString(v));
	}

	static std::string fromVariant(const QVariant & v)
	{
		return std::string((const char *)v.toByteArray());
	}
};

/**
 * @brief Model Field Type Traits for logbook::country
 */
template <class T>
struct FieldTraits<T, logbook::country>
{
	typedef const logbook::country & (T::*getter)(void) const;
	typedef const boost::optional<logbook::country> & (T::*opt_getter)(void) const;
	typedef void (T::*setter)(const logbook::country &);

	static QVariant toVariant(const logbook::country & v)
	{
		return QVariant();
	}

	static logbook::country fromVariant(const QVariant & v)
	{
		return logbook::country();
	}
};

/**
 * @brief Model Field Type Traits for time_t
 */
template <class T>
struct FieldTraits<T, time_t>
{
	typedef time_t (T::*getter)(void) const;
	typedef const boost::optional<time_t> & (T::*opt_getter)(void) const;
	typedef void (T::*setter)(time_t);

	static QVariant toVariant(const time_t & v)
	{
		return QVariant(QDateTime::fromTime_t(v));
	}

	static time_t fromVariant(const QVariant & v)
	{
		return v.toDateTime().toTime_t();
	}
};

/**
 * @brief Default Field Adapter Class
 *
 * Links the IFieldAdapter interface to getter/setter methods on the target
 * model class.  Supports both standard and optional fields depending on the
 * function signatures passed to the constructor.
 */
template <class T, typename V, class Traits = FieldTraits<T, V> >
class DefaultFieldAdapter: public IFieldAdapter<T>
{
protected:
	typedef Traits							traits_t;
	typedef typename traits_t::getter		getter_t;
	typedef typename traits_t::opt_getter	opt_getter_t;
	typedef typename traits_t::setter		setter_t;
	typedef void (T::*resetter_t)(const boost::none_t &);

public:

	//! Class Constructor
	DefaultFieldAdapter(getter_t getter, setter_t setter = 0)
		: m_getter(getter), m_opt_getter(0), m_setter(setter), m_resetter(0)
	{
		if (! m_getter)
			throw std::runtime_error("Field Getter cannot be NULL");
	}

	//! Class Constructor
	DefaultFieldAdapter(opt_getter_t getter, setter_t setter = 0, resetter_t resetter = 0)
		: m_getter(0), m_opt_getter(getter), m_setter(setter), m_resetter(resetter)
	{
		if (! m_opt_getter)
			throw std::runtime_error("Field Getter cannot be NULL");
	}

	//! Class Destructor
	virtual ~DefaultFieldAdapter()
	{
	}

public:

	//! @param[in] Session Pointer
	virtual void bind(logbook::Session::Ptr session)
	{
	}

	//! @return Decoration Value for the Field
	virtual QVariant decorationData(const boost::shared_ptr<T> &) const
	{
		return QVariant();
	}

	//! @return Display Value for the Field
	virtual QVariant displayData(const boost::shared_ptr<T> & item) const
	{
		if (m_opt_getter != 0)
		{
			if (! ((item.get())->*m_opt_getter)())
				return QVariant();
			else
				return traits_t::toVariant(((item.get())->*m_opt_getter)().get());
		}

		return traits_t::toVariant(((item.get())->*m_getter)());
	}

	//! @return Edit Value for the Field
	virtual QVariant editData(const boost::shared_ptr<T> & item) const
	{
		return displayData(item);
	}

	//! @brief Set the Edit Value for the Field
	virtual bool setEditData(boost::shared_ptr<T> & item, const QVariant & v) const
	{
		if (! m_setter || (v.isNull() && ! m_resetter))
			return false;

		if (v.isNull())
			((item.get())->*m_resetter)(boost::none);
		else
			((item.get())->*m_setter)(traits_t::fromVariant(v));

		return true;
	}

protected:
	getter_t			m_getter;
	opt_getter_t		m_opt_getter;
	setter_t			m_setter;
	resetter_t			m_resetter;

};

/**
 * @brief Country Field Adapter Class
 *
 * Specialization of DefaultFieldAdapter to display Country information
 * including a flag decoration.
 */
template <class T>
class CountryFieldAdapter: public IFieldAdapter<T>
{
protected:
	typedef FieldTraits<T, logbook::country>						traits_t;
	typedef typename FieldTraits<T, logbook::country>::getter		getter_t;
	typedef typename FieldTraits<T, logbook::country>::opt_getter	opt_getter_t;
	typedef typename FieldTraits<T, logbook::country>::setter		setter_t;
	typedef void (T::*resetter_t)(const boost::none_t &);

public:

	//! Class Constructor
	CountryFieldAdapter(getter_t getter, setter_t setter = 0)
		: m_getter(getter), m_opt_getter(0), m_setter(setter), m_resetter(0)
	{
		if (! m_getter)
			throw std::runtime_error("Field Getter cannot be NULL");
	}

	//! Class Constructor
	CountryFieldAdapter(opt_getter_t getter, setter_t setter = 0, resetter_t resetter = 0)
		: m_getter(0), m_opt_getter(getter), m_setter(setter), m_resetter(resetter)
	{
		if (! m_opt_getter)
			throw std::runtime_error("Field Getter cannot be NULL");
	}

	//! Class Destructor
	virtual ~CountryFieldAdapter()
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
		logbook::country c(getValue(item));

		if (c.code().empty())
			return QVariant();
		return QImage(QString(":/flags/%1.png").arg(QString::fromStdString(c.code()).toLower()));
	}

	//! @return Display Value for the Field
	virtual QVariant displayData(const boost::shared_ptr<T> & item) const
	{
		logbook::country c(getValue(item));

		if (c.code().empty())
			return QVariant();
		return QVariant(QString::fromStdString(c.name()));
	}

	//! @return Edit Value for the Field
	virtual QVariant editData(const boost::shared_ptr<T> & item) const
	{
		logbook::country c(getValue(item));

		if (c.code().empty())
			return QVariant();
		return QVariant(QString::fromStdString(c.code()).toLower());
	}

	//! @brief Set the Edit Value for the Field
	virtual bool setEditData(boost::shared_ptr<T> & item, const QVariant & v) const
	{
		if (! m_setter || (v.isNull() && ! m_resetter))
			return false;

		if (v.isNull() || (v.toString().isEmpty()))
			((item.get())->*m_resetter)(boost::none);
		else
		{
			((item.get())->*m_setter)(logbook::country(v.toString().toStdString()));
		}

		return true;
	}

protected:

	//! @return Country Value or NULL
	logbook::country getValue(const boost::shared_ptr<T> & item) const
	{
		if (m_opt_getter != 0)
		{
			if (! ((item.get())->*m_opt_getter)())
				return logbook::country();
			else
				return ((item.get())->*m_opt_getter)().get();
		}

		return ((item.get())->*m_getter)();
	}

protected:
	getter_t			m_getter;
	opt_getter_t		m_opt_getter;
	setter_t			m_setter;
	resetter_t			m_resetter;

};

/**
 * @brief Enumerated Field Adapter Class
 *
 * Specialization of DefaultFieldAdapter to adapt enumerated columns.  The
 * underlying type may be any supported type which maps to a unique 'name'
 * string which is presented as the Display value.
 *
 * The enumeration dictionary must be present in the Field Traits.
 */
template <class T, typename V, class Traits = FieldTraits<T, V> >
class EnumFieldAdapter: public IFieldAdapter<T>
{
protected:
	typedef Traits							traits_t;
	typedef typename traits_t::getter		getter_t;
	typedef typename traits_t::opt_getter	opt_getter_t;
	typedef typename traits_t::setter		setter_t;
	typedef void (T::*resetter_t)(const boost::none_t &);

public:

	//! Class Constructor
	EnumFieldAdapter(getter_t getter, setter_t setter = 0)
		: m_getter(getter), m_opt_getter(0), m_setter(setter), m_resetter(0)
	{
		if (! m_getter)
			throw std::runtime_error("Field Getter cannot be NULL");
	}

	//! Class Constructor
	EnumFieldAdapter(opt_getter_t getter, setter_t setter = 0, resetter_t resetter = 0)
		: m_getter(0), m_opt_getter(getter), m_setter(setter), m_resetter(resetter)
	{
		if (! m_opt_getter)
			throw std::runtime_error("Field Getter cannot be NULL");
	}

	//! Class Destructor
	virtual ~EnumFieldAdapter()
	{
	}

public:

	//! @param[in] Session Pointer
	virtual void bind(logbook::Session::Ptr session)
	{
	}

	//! @return Decoration Value for the Field
	virtual QVariant decorationData(const boost::shared_ptr<T> &) const
	{
		return QVariant();
	}

	//! @return Display Value for the Field
	virtual QVariant displayData(const boost::shared_ptr<T> & item) const
	{
		if (m_opt_getter != 0)
		{
			if (! ((item.get())->*m_opt_getter)())
				return QVariant();
			else
				return traits_t::toVariant(((item.get())->*m_opt_getter)().get());
		}

		return traits_t::toVariant(((item.get())->*m_getter)());
	}

	//! @return Edit Value for the Field
	virtual QVariant editData(const boost::shared_ptr<T> & item) const
	{
		if (m_opt_getter != 0)
		{
			if (! ((item.get())->*m_opt_getter)())
				return QVariant();
			else
				return traits_t::toVariant(((item.get())->*m_opt_getter)().get());
		}

		return traits_t::toVariant(((item.get())->*m_getter)());
	}

	//! @brief Set the Edit Value for the Field
	virtual bool setEditData(boost::shared_ptr<T> & item, const QVariant & v) const
	{
		if (! m_setter || (v.isNull() && ! m_resetter))
			return false;

		if (v.isNull())
			((item.get())->*m_resetter)(boost::none);
		else
			((item.get())->*m_setter)(traits_t::fromVariant(v));

		return true;
	}

protected:
	getter_t			m_getter;
	opt_getter_t		m_opt_getter;
	setter_t			m_setter;
	resetter_t			m_resetter;

};

/**
 * @brief Foreign Key Field Wrapper Adapter
 *
 * Wraps a field of a foreign-key object.  The class takes an accessor to the
 * foreign key instance, and another instance of IFieldAdapter templated on the
 * foreign key class.  All operations are passed through to the underlying
 * foreign instance.
 */
template <class T, class FK, typename V, typename Traits = FieldTraits<FK, V> >
class ForeignKeyWrapper: public IFieldAdapter<T>
{
protected:
	typedef boost::shared_ptr<FK> (T::*getter_t)() const;

public:

	//! Class Constructor
	ForeignKeyWrapper(getter_t getter, IFieldAdapter<FK> * adapter)
		: m_getter(getter), m_adapter(adapter)
	{
		if (! m_getter)
			throw std::runtime_error("Field Getter cannot be NULL");
		if (! m_adapter)
			throw std::runtime_error("Foreign Key Field Adapter cannot be NULL");
	}

	//! Class Destructor
	virtual ~ForeignKeyWrapper()
	{
		delete m_adapter;
	}

public:

	//! @param[in] Session Pointer
	virtual void bind(logbook::Session::Ptr session)
	{
	}

	//! @return Decoration Value for the Field
	virtual QVariant decorationData(const boost::shared_ptr<T> & item) const
	{
		boost::shared_ptr<FK> obj(((item.get())->*m_getter)());

		if (obj)
			return m_adapter->decorationData(obj);
		return QVariant();
	}

	//! @return Display Value for the Field
	virtual QVariant displayData(const boost::shared_ptr<T> & item) const
	{
		boost::shared_ptr<FK> obj(((item.get())->*m_getter)());

		if (obj)
			return m_adapter->displayData(obj);
		return QVariant();
	}

	//! @return Edit Value for the Field
	virtual QVariant editData(const boost::shared_ptr<T> & item) const
	{
		boost::shared_ptr<FK> obj(((item.get())->*m_getter)());

		if (obj)
			return m_adapter->editData(obj);
		return QVariant();
	}

	//! @brief Set the Edit Value for the Field
	virtual bool setEditData(boost::shared_ptr<T> & item, const QVariant & v) const
	{
		boost::shared_ptr<FK> obj(((item.get())->*m_getter)());

		if (obj)
			return m_adapter->setEditData(obj, v);
		return false;
	}

protected:
	getter_t				m_getter;
	IFieldAdapter<FK> *		m_adapter;

};

/**
 * @brief Foreign Key Adapter
 */
template <class T, class FK>
class ForeignKeyAdapter: public IFieldAdapter<T>
{
protected:
	typedef boost::shared_ptr<FK> (T::*getter_t)() const;
	typedef void (T::*setter_t)(boost::shared_ptr<FK>);
	typedef void (T::*resetter_t)(const boost::none_t &);

public:

	//! Class Constructor
	ForeignKeyAdapter(getter_t getter, setter_t setter = 0, resetter_t resetter = 0,
			IFieldAdapter<FK> * displayAdapter = 0)
		: m_getter(getter), m_setter(setter), m_resetter(resetter),
		  m_adapter(displayAdapter), m_session()
	{
		if (! m_getter)
			throw std::runtime_error("Field Getter cannot be NULL");
	}

	//! Class Destructor
	virtual ~ForeignKeyAdapter()
	{
		delete m_adapter;
	}

public:

	//! @param[in] Session Pointer
	virtual void bind(logbook::Session::Ptr session)
	{
		m_session.swap(session);
	}

	//! @return Decoration Value for the Field
	virtual QVariant decorationData(const boost::shared_ptr<T> & item) const
	{
		boost::shared_ptr<FK> obj(((item.get())->*m_getter)());

		if (obj && m_adapter)
			return m_adapter->decorationData(obj);
		return QVariant();
	}

	//! @return Display Value for the Field
	virtual QVariant displayData(const boost::shared_ptr<T> & item) const
	{
		boost::shared_ptr<FK> obj(((item.get())->*m_getter)());

		if (obj && m_adapter)
			return m_adapter->displayData(obj);
		return QVariant();
	}

	//! @return Edit Value for the Field
	virtual QVariant editData(const boost::shared_ptr<T> & item) const
	{
		boost::shared_ptr<FK> obj(((item.get())->*m_getter)());

		if (obj)
			return QVariant::fromValue<int64_t>(obj->id());
		return QVariant();
	}

	//! @brief Set the Edit Value for the Field
	virtual bool setEditData(boost::shared_ptr<T> & item, const QVariant & v) const
	{
		bool ok;
		int64_t id = v.toLongLong(& ok);

		// If we don't have a session, or if the value is not a number, exit
		if ((! v.isNull() && ! ok) || ! m_session)
			return false;

		// Check if the id is NULL, or -1 and reset if so
		if (v.isNull() || (id == -1))
		{
			if (m_resetter)
			{
				((item.get())->*m_resetter)(boost::none);
				return true;
			}
			else
			{
				return false;
			}
		}

		// Find the Object
		boost::shared_ptr<FK> obj = m_session->finder<FK>()->find(id);
		if (! obj)
		{
			char msg[255];
			sprintf(msg, "Error: %s id %ld does not exist", typeid(FK).name(), id);
			throw std::runtime_error(msg);
		}

		// Set the Object if we have a setter
		if (m_setter)
		{
			((item.get())->*m_setter)(obj);
			return true;
		}

		return false;
	}

protected:
	getter_t				m_getter;
	setter_t				m_setter;
	resetter_t				m_resetter;
	IFieldAdapter<FK> *		m_adapter;
	logbook::Session::Ptr	m_session;

};

#endif /* MODELIF_HPP_ */
