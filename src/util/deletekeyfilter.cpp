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

#include <QKeyEvent>
#include "deletekeyfilter.hpp"

DeleteKeyFilter::DeleteKeyFilter(QObject * parent)
	: QObject(parent)
{
}

DeleteKeyFilter::~DeleteKeyFilter()
{
}

bool DeleteKeyFilter::eventFilter(QObject * obj, QEvent * event)
{
	if (event->type() == QEvent::KeyPress)
	{
		QKeyEvent * ke = static_cast<QKeyEvent *>(event);
		if (ke->key() == Qt::Key_Delete)
		{
			emit deletePressed();
			return true;
		}
	}

	return QObject::eventFilter(obj, event);
}
