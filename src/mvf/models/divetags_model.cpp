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

#include <benthos/logbook/session.hpp>

#include "divetags_model.hpp"

static std::set<std::string, cicmp> s_DefaultTags;
class s_DefaultTags_InitializerClass
{
public:
	s_DefaultTags_InitializerClass()
	{
		s_DefaultTags.insert("Boat");
		s_DefaultTags.insert("Current");
		s_DefaultTags.insert("Fresh");
		s_DefaultTags.insert("Training");
		s_DefaultTags.insert("Salt");
		s_DefaultTags.insert("Shore");
		s_DefaultTags.insert("Surf");
		s_DefaultTags.insert("Surge");
		s_DefaultTags.insert("Waves");
	}
};
s_DefaultTags_InitializerClass s_DefaultTags_Initializer;

DiveTagsModel::DiveTagsModel(QObject * parent)
	: QAbstractListModel(parent), m_tags(), m_divetags(), m_dive()
{
}

DiveTagsModel::~DiveTagsModel()
{
}

void DiveTagsModel::addTag(const std::string & tag)
{
	if (! m_dive)
		return;

	std::vector<std::string>::iterator it;
	for (it = m_tags.begin(); it != m_tags.end(); it++)
	{
		int ret = strcasecmp(tag.c_str(), it->c_str());
		if (! ret)
			return;
		if (ret < 0)
			break;
	}

	int row = it - m_tags.begin();
	beginInsertRows(QModelIndex(), row, row);

	m_tags.insert(it, tag);
	m_divetags.insert(tag);
	m_dive->tags()->add(tag);

	if (m_dive->session())
		m_dive->session()->add(m_dive);

	endInsertRows();
}

QVariant DiveTagsModel::data(const QModelIndex & index, int role) const
{
	if (! index.isValid() || (index.row() < 0) || ((size_t)index.row() >= m_tags.size()))
		return QVariant();

	std::string tag = m_tags[index.row()];

	if (role == Qt::DisplayRole)
		return QVariant(QString::fromStdString(tag));
	else if (role == Qt::CheckStateRole)
		return QVariant(m_divetags.find(tag) != m_divetags.end() ? Qt::Checked : Qt::Unchecked);
	return QVariant();
}

Qt::ItemFlags DiveTagsModel::flags(const QModelIndex & index) const
{
	return (Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
}

int DiveTagsModel::rowCount(const QModelIndex & parent) const
{
	return m_tags.size();
}

bool DiveTagsModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if (! index.isValid() || (index.row() < 0) || ((size_t)index.row() >= m_tags.size()))
		return false;

	if ((role != Qt::CheckStateRole) || ! m_dive)
		return false;

	std::string tag = m_tags[index.row()];
	if (value.toBool())
	{
		m_dive->tags()->add(tag);
		m_divetags.insert(tag);
	}
	else
	{
		m_dive->tags()->remove(tag);
		m_divetags.erase(tag);
	}

	if (m_dive->session())
		m_dive->session()->add(m_dive);

	emit dataChanged(index, index);
	return true;
}

void DiveTagsModel::setDive(Dive::Ptr dive)
{
	beginResetModel();

	m_divetags.clear();
	m_tags.clear();
	m_dive.reset();

	if (dive)
	{
		m_dive = dive;

		std::set<std::string, cicmp> atags(s_DefaultTags);
		std::list<std::string> ltags;

		// Merge Logbook Tags with Default Tags
		if (m_dive->session())
		{
			IDiveFinder::Ptr df = boost::dynamic_pointer_cast<IDiveFinder>(m_dive->session()->finder<Dive>());
			if (df)
			{
				std::vector<std::string> at(df->allTags());
				ltags.assign(at.begin(), at.end());
			}
		}

		// Initialize the All Tags collection
		atags.insert(ltags.begin(), ltags.end());
		m_tags.assign(atags.begin(), atags.end());

		// Initialize the Dive Tags collection
		std::list<std::string> dtags(dive->tags()->all());
		m_divetags.insert(dtags.begin(), dtags.end());
	}

	printf("m_divetags.size() = %d\n", m_divetags.size());

	endResetModel();
}
