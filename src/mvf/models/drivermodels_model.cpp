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

#include "drivermodels_model.hpp"

DriverModelsModel::DriverModelsModel(DriverClass::Ptr dclass, QObject * parent)
	: QAbstractListModel(parent), m_models()
{
	if (dclass)
	{
		std::map<int, model_info_t>::const_iterator it;
		for (it = dclass->models().begin(); it != dclass->models().end(); it++)
		{
			std::string dname = it->second.model_name;
			if (! it->second.manuf_name.empty())
				dname = it->second.manuf_name + " " + dname;

			m_models.push_back(std::pair<int, std::string>(it->first, dname));
		}
	}
}

DriverModelsModel::~DriverModelsModel()
{
}

QVariant DriverModelsModel::data(const QModelIndex & index, int role) const
{
	if (! index.isValid() || (index.row() < 0) || ((size_t)index.row() >= m_models.size()))
		return QVariant();

	std::pair<int, std::string> mdl = m_models[index.row()];

	if (role == Qt::DisplayRole)
		return QVariant(QString::fromStdString(mdl.second));
	if ((role == Qt::EditRole) || (role == Qt::UserRole))
		return QVariant(mdl.first);
	return QVariant();
}

int DriverModelsModel::rowCount(const QModelIndex & parent) const
{
	return m_models.size();
}
