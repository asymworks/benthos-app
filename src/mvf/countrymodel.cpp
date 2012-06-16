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

#include <QImage>
#include <QString>
#include <QTextCodec>

#include <benthos/logbook/countries.hpp>

#include "countrymodel.hpp"

CountryModel::CountryModel()
	: m_codes()
{
	// Reorder based on Country Name, not Code
	std::map<std::string, std::string> rmap;
	country_map_t::const_iterator it;
	for (it = ISO3166_COUNTRIES.begin(); it != ISO3166_COUNTRIES.end(); it++)
		rmap[it->second] = it->first;

	std::map<std::string, std::string>::const_iterator it2;
	for (it2 = rmap.begin(); it2 != rmap.end(); it2++)
		m_codes.push_back(it2->second);
}

CountryModel::~CountryModel()
{
}

QVariant CountryModel::data(const QModelIndex & index, int role) const
{
	if (! index.isValid() || (index.row() < 0) || ((size_t)index.row() >= ISO3166_COUNTRIES.size()))
		return QVariant();

	std::string c = m_codes[index.row()];
	country_map_t::const_iterator it = ISO3166_COUNTRIES.find(c);

	if (it == ISO3166_COUNTRIES.end())
		return QVariant();

	if (role == Qt::DisplayRole)
	{
		QTextCodec * c = QTextCodec::codecForName("UTF-8");
		return QVariant(c->toUnicode(it->second.c_str()));
	}
	else if (role == Qt::EditRole)
		return QVariant(QString::fromStdString(it->first).toLower());
	else if (role == Qt::DecorationRole)
		return QVariant(QImage(QString(":/flags/%1.png").arg(QString::fromStdString(it->first).toLower())));
	return QVariant();
}

int CountryModel::rowCount(const QModelIndex & parent) const
{
	return ISO3166_COUNTRIES.size();
}
