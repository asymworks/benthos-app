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

#include <QPixmap>
#include <QStyle>

#include "searchedit.hpp"

#define max(a, b) ((a >= b) ? a : b)

SearchEdit::SearchEdit(QWidget * parent)
	: QLineEdit(parent)
{
	QPixmap clr_pm(":/icons/clear.png");

	m_btnClear = new QToolButton(this);
	m_btnClear->setCursor(Qt::ArrowCursor);
	m_btnClear->setIcon(clr_pm);
	m_btnClear->setIconSize(clr_pm.size());
	m_btnClear->setStyleSheet("QToolButton { border: none; padding: 0px; }");
	connect(m_btnClear, SIGNAL(clicked()), this, SLOT(clear()));

	int fw = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);

	//TODO: Make a Search glyph show up properly

	setStyleSheet(QString("QLineEdit { padding-right: %1px; }").arg(m_btnClear->sizeHint().width() + fw + 1));
	QSize msz = minimumSizeHint();
	setMinimumSize(max(msz.width(), m_btnClear->sizeHint().height() + fw * 2 + 2),
				   max(msz.height(), m_btnClear->sizeHint().height() + fw * 2 + 2));
	setPlaceholderText("Search");
}

SearchEdit::~SearchEdit()
{
}

void SearchEdit::resizeEvent(QResizeEvent * e)
{
	QSize sz = m_btnClear->sizeHint();
	int fw = style()->pixelMetric(QStyle::PM_DefaultFrameWidth);

	m_btnClear->move(rect().right() - fw - sz.width(),
		(rect().bottom() + 1 - sz.height()) / 2 + 1);
}

QSize SearchEdit::sizeHint() const
{
	QSize s = QLineEdit::sizeHint();
	s.setWidth(250);
	return s;
}
