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

#include <QComboBox>

#include <mvf/models/drivermodels_model.hpp>
#include "driverparams_delegate.hpp"

DriverParamsDelegate::DriverParamsDelegate(DriverClass::Ptr dclass, QObject * parent)
	: NoFocusDelegate(parent), m_dclass(dclass)
{
}

DriverParamsDelegate::~DriverParamsDelegate()
{
}

QWidget * DriverParamsDelegate::createEditor(QWidget * parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	if (index.data(Qt::UserRole + 1).toInt() == ptModel)
	{
		QComboBox * cbx = new QComboBox(parent);
		cbx->setModel(new DriverModelsModel(m_dclass));
		return cbx;
	}
	else
	{
		return QStyledItemDelegate::createEditor(parent, option, index);
	}
}
