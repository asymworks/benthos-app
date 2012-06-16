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

#include <QApplication>
#include <QModelIndex>
#include <QPainter>
#include <QStyleOptionViewItem>

#include "logbook_delegate.hpp"

LogbookDelegate::LogbookDelegate(QObject * parent)
	: QStyledItemDelegate(parent)
{
}

LogbookDelegate::~LogbookDelegate()
{
}

void LogbookDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QStyle * style;
	QStyleOptionViewItemV4 opt(option);
	initStyleOption(& opt, index);

	// Remove the Focus Rectangle
	opt.state &= ~(QStyle::State_HasFocus);

	// Make the Text Uppercase for Root Items
	if (! index.parent().isValid())
	{
		opt.font.setBold(true);
		opt.font.setCapitalization(QFont::AllUppercase);
		opt.palette.setColor(QPalette::Text, Qt::darkGray);
	}

	// Draw the Item
	if (opt.widget == NULL)
		style = QApplication::style();
	else
		style = opt.widget->style();

	style->drawControl(QStyle::CE_ItemViewItem, & opt, painter, opt.widget);
}

QSize LogbookDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QSize s = QStyledItemDelegate::sizeHint(option, index);

	if (! index.parent().isValid())
		s.setHeight(s.height() + 8);

	return s;
}
