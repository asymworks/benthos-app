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

#include <stdexcept>

#include <QFont>

#include <benthos/divecomputer/arglist.hpp>
#include <benthos/logbook/logging.hpp>

#include "driverparams_model.hpp"

DriverParamsModel::DriverParamsModel(DriverClass::Ptr cls, const std::string & args)
	: QAbstractTableModel(), m_class(cls), m_params(), m_values()
{
	// Load the list of Parameters
	m_params.assign(cls->parameters().begin(), cls->parameters().end());

	// Load the Argument Values
	ArgumentList al(args);
	std::vector<param_info_t>::const_iterator it;
	for (it = m_params.begin(); it != m_params.end(); it++)
	{
		if (! al.has_value(it->param_name))
		{
			m_values.push_back(QVariant());
			continue;
		}

		switch (it->param_type)
		{
		case ptString:
		{
			boost::optional<std::string> v = al.read_string(it->param_name);
			m_values.push_back(v.is_initialized() ? QVariant(QString::fromStdString(v.get())) : QVariant());
			break;
		}

		case ptInt:
		{
			boost::optional<int32_t> v = al.read_integer(it->param_name);
			m_values.push_back(v.is_initialized() ? QVariant(v.get()) : QVariant());
			break;
		}

		case ptUInt:
		{
			boost::optional<uint32_t> v = al.read_unsigned(it->param_name);
			m_values.push_back(v.is_initialized() ? QVariant(v.get()) : QVariant());
			break;
		}

		case ptFloat:
		{
			boost::optional<double> v = al.read_float(it->param_name);
			m_values.push_back(v.is_initialized() ? QVariant(v.get()) : QVariant());
			break;
		}

		case ptModel:
		{
			boost::optional<int32_t> v = al.read_integer(it->param_name);
			m_values.push_back(v.is_initialized() ? QVariant(v.get()) : QVariant());
			break;
		}

		default:
			throw std::runtime_error("Unsupported Parameter Type for " + it->param_name);
		}
	}
}

DriverParamsModel::~DriverParamsModel()
{
}

int DriverParamsModel::columnCount(const QModelIndex & parent) const
{
	return 2;
}

QVariant DriverParamsModel::data(const QModelIndex & index, int role) const
{
	if (! index.isValid())
		return QVariant();
	size_t r = index.row();
	size_t c = index.column();

	if ((c < 0) || (c >= 2))
		return QVariant();
	if ((r < 0) || (r >= m_params.size()))
		return QVariant();

	if (c == 0)
	{
		// Parameter Name
		if (role == Qt::DisplayRole)
		{
			return QVariant(QString::fromStdString(m_params[r].param_name));
		}
		else if (role == Qt::UserRole + 0)
		{
			return QVariant(QString::fromStdString(m_params[r].param_desc));
		}
		else if (role == Qt::UserRole + 1)
		{
			return QVariant(m_params[r].param_type);
		}
	}
	else
	{
		// Parameter Value
		if ((role == Qt::DisplayRole) || (role == Qt::EditRole))
		{
			if (m_values[r].isNull())
				return defaultValue(r);

			if (m_params[r].param_type == ptModel)
			{
				std::map<int, model_info_t>::const_iterator it = m_class->models().find(m_values[r].toInt());
				if (it == m_class->models().end())
					return QVariant();

				if (role == Qt::DisplayRole)
				{
					std::string dname = it->second.model_name;
					if (! it->second.manuf_name.empty())
						dname = it->second.manuf_name + " " + dname;

					return QVariant(QString::fromStdString(dname));
				}
				else
				{
					return QVariant(it->first);
				}
			}

			return m_values[r];
		}
		else if (role == Qt::FontRole)
		{
			if (! m_values[r].isNull())
			{
				QFont f;
				f.setBold(true);
				return QVariant(f);
			}
		}
		else if (role == Qt::UserRole + 0)
		{
			return QVariant(QString::fromStdString(m_params[r].param_desc));
		}
		else if (role == Qt::UserRole + 1)
		{
			return QVariant(m_params[r].param_type);
		}
	}

	return QVariant();
}

QVariant DriverParamsModel::defaultValue(int row) const
{
	if (m_params[row].param_default.empty())
		return QVariant();

	std::string dummy = m_params[row].param_name + "=" + m_params[row].param_default;
	ArgumentList al(dummy);

	switch (m_params[row].param_type)
	{
	case ptString:
		return QVariant(QString::fromStdString(al.read_string(m_params[row].param_name).get()));

	case ptInt:
		return QVariant(al.read_integer(m_params[row].param_name).get());

	case ptUInt:
		return QVariant(al.read_unsigned(m_params[row].param_name).get());

	case ptFloat:
		return QVariant(al.read_float(m_params[row].param_name).get());

	case ptModel:
		return QVariant(al.read_integer(m_params[row].param_name).get());
	}

	return QVariant();
}

Qt::ItemFlags DriverParamsModel::flags(const QModelIndex & index) const
{
	if (! index.isValid())
		return Qt::ItemFlags();

	Qt::ItemFlags f = QAbstractTableModel::flags(index);

	if (index.column() == 0)
	{
		f = f & (~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
	}
	else
	{
		f = f | Qt::ItemIsEditable;
	}

	return f;
}

QVariant DriverParamsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if ((section < 0) || (section >= 2) || (role != Qt::DisplayRole) || (orientation != Qt::Horizontal))
		return QVariant();

	if (section == 0)
		return QVariant(QString(tr("Property")));
	else
		return QVariant(tr("Value"));
}

int DriverParamsModel::rowCount(const QModelIndex & parent) const
{
	return m_params.size();
}

bool DriverParamsModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if (! index.isValid())
		return false;
	size_t r = index.row();
	size_t c = index.column();

	if (c != 1)
		return false;
	if ((r < 0) || (r >= m_params.size()))
		return false;

	if (value.isNull() || value.toString().isEmpty())
		m_values[r].clear();
	else
		m_values[r] = value;

	emit dataChanged(index, index);
	return true;
}

std::string DriverParamsModel::toString(const std::string & skipParam) const
{
	std::string ret;
	for (size_t i = 0; i < m_params.size(); i++)
	{
		if (m_params[i].param_name == skipParam)
			continue;

		if (! m_values[i].isNull())
		{
			if (! ret.empty())
				ret += ":";

			ret += m_params[i].param_name;
			ret += "=";
			ret += m_values[i].toString().toStdString();
		}
	}

	return ret;
}
