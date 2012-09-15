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
#include "mix_model.hpp"

/**
 * @brief Model Field Type Traits for std::string immediate
 */
template <class T>
struct ToStringFieldTraits: public FieldTraits<T, std::string>
{
	typedef std::string (T::*getter)(void) const;
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

MixModel::MixModel(QObject * parent)
	: LogbookQueryModel(parent)
{
	m_columns.push_back(new ModelColumn<LogbookQueryModel::model_type>(
		new DefaultFieldAdapter<LogbookQueryModel::model_type, std::string, ToStringFieldTraits<LogbookQueryModel::model_type> >(
			& LogbookQueryModel::model_type::toString
		), "mix", "Mix"
	));

	ADD_OPTIONAL_COLUMN(std::string, name, setName, "Mix Name");
	ADD_FIELD_COLUMN(unsigned int, ar_permil, setArPerMil, "Argon %", new DelegateFactory<PerMilDelegate>);
	ADD_FIELD_COLUMN(unsigned int, h2_permil, setH2PerMil, "Hydrogen %", new DelegateFactory<PerMilDelegate>);
	ADD_FIELD_COLUMN(unsigned int, he_permil, setHePerMil, "Helium %", new DelegateFactory<PerMilDelegate>);
	ADD_FIELD_COLUMN(unsigned int, o2_permil, setO2PerMil, "Oxygen %", new DelegateFactory<PerMilDelegate>);
	ADD_RO_FIELD_COLUMN(unsigned int, n2_permil, "Nitrogen %", new DelegateFactory<PerMilDelegate>);
}

MixModel::~MixModel()
{
}
