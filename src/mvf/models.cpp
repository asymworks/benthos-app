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

#include "models.hpp"

CustomTableModel::CustomTableModel(QObject * parent)
	: QAbstractTableModel(parent), m_session()
{
}

CustomTableModel::~CustomTableModel()
{
}

void CustomTableModel::bind(Session::Ptr session)
{
	m_session = session;
	std::vector<BaseModelColumn *>::const_iterator it;
	for (it = m_columns.begin(); it != m_columns.end(); it++)
		(* it)->bind(session);
}

const std::vector<BaseModelColumn *> & CustomTableModel::columns() const
{
	return m_columns;
}

void CustomTableModel::emitDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight)
{
	emit dataChanged(topLeft, bottomRight);
}

int CustomTableModel::findColumn(BaseModelColumn * col) const
{
	std::vector<BaseModelColumn *>::const_iterator it;
	for (it = m_columns.begin(); it != m_columns.end(); it++)
	{
		if (col == *it)
		{
			return (it - m_columns.begin());
		}
	}

	return -1;
}

int CustomTableModel::findColumn(const QString & col) const
{
	std::vector<BaseModelColumn *>::const_iterator it;
	for (it = m_columns.begin(); it != m_columns.end(); it++)
	{
		if (col == (*it)->name())
		{
			return (it - m_columns.begin());
		}
	}

	return -1;
}
