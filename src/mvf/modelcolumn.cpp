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

#include "modelcolumn.hpp"

BaseModelColumn::BaseModelColumn(const QString & name, const QString & label,
		IDelegateFactory * delegateFactory,	bool hidden, bool internal, Qt::Alignment align)
	: m_name(name), m_label(label), m_align(align), m_hidden(hidden), m_internal(internal),
	  m_delegatef(delegateFactory)
{
}

BaseModelColumn::~BaseModelColumn()
{
	if (m_delegatef != NULL)
		delete m_delegatef;
}

Qt::Alignment BaseModelColumn::alignment() const
{
	return m_align;
}

void BaseModelColumn::bind(Session::Ptr session)
{
	m_session = session;
}

QAbstractItemDelegate * BaseModelColumn::createDelegate() const
{
	if (m_delegatef != NULL)
		return m_delegatef->create();
	return NULL;
}

bool BaseModelColumn::hasDelegate() const
{
	return (m_delegatef != NULL);
}

bool BaseModelColumn::isHidden() const
{
	return m_hidden;
}

bool BaseModelColumn::isInternal() const
{
	return m_internal;
}

const QString & BaseModelColumn::label() const
{
	return m_label;
}

const QString & BaseModelColumn::name() const
{
	return m_name;
}

void BaseModelColumn::setAlignment(Qt::Alignment value)
{
	m_align = value;
}

void BaseModelColumn::setHidden(bool value)
{
	m_hidden = value;
}

void BaseModelColumn::setInternal(bool value)
{
	m_internal = value;
}

void BaseModelColumn::setLabel(const QString & value)
{
	m_label = value;
}
