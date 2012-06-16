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

#include <QPainter>
#include <QStyleOption>

#include <cmath>

#include "ratingedit.hpp"

RatingEdit::RatingEdit(int maxRating, QWidget * parent)
	: QWidget(parent), m_rating(0), m_maxRating(maxRating),
	  m_active(false)
{
	setAutoFillBackground(true);
	setFocusPolicy(Qt::ClickFocus);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

	m_star = QImage(":/icons/star.png");
	m_blue = QImage(":/icons/star-blue.png");
	m_dot  = QImage(":/icons/star-dot.png");
}

RatingEdit::~RatingEdit()
{
}

int RatingEdit::maxRating() const
{
	return m_maxRating;
}

void RatingEdit::mouseMoveEvent(QMouseEvent * e)
{
	if (m_active)
		updateRating(e);
}

void RatingEdit::mousePressEvent(QMouseEvent * e)
{
	if (e->buttons() == Qt::LeftButton)
	{
		m_active = true;
		updateRating(e);
	}
}

void RatingEdit::mouseReleaseEvent(QMouseEvent * e)
{
	if (m_active)
	{
		m_active = false;
		update();
		emit ratingChanged(m_rating);
	}
}

void RatingEdit::paintEvent(QPaintEvent * e)
{
	QPainter * p = new QPainter(this);

	QStyleOptionFrameV3 opt;
	opt.initFrom(this);
	opt.rect = contentsRect();
	opt.lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, & opt, this);
	opt.midLineWidth = 0;
	opt.state |= QStyle::State_Sunken;

	if (m_active)
		opt.state |= QStyle::State_HasFocus;
	else
		opt.state &= ~(QStyle::State_HasFocus);

	style()->drawPrimitive(QStyle::PE_PanelLineEdit, & opt, p, this);

	int y = (opt.rect.height() - m_star.height()) / 2;
	int w = m_star.width();

	for (int i = 0; i < m_maxRating; ++i)
	{
		int x = i * (w + 1) + w/4 + opt.lineWidth;

		if (m_rating > i)
			p->drawImage(x, y, m_active ? m_blue : m_star);
		else
			p->drawImage(x, y, m_dot);
	}

	p->end();
}

int RatingEdit::rating() const
{
	return m_rating;
}

void RatingEdit::setMaxRating(int value)
{
	if (value < 1)
		value = 1;
	m_maxRating = value;

	if (m_rating < m_maxRating)
		m_rating = m_maxRating;

	update();
}

void RatingEdit::setRating(int value)
{
	if (value < 0)
		value = 0;
	if (value > m_maxRating)
		value = m_maxRating;

	m_rating = value;
	update();
}

QSize RatingEdit::sizeHint() const
{
	return QSize((m_star.width() + 2) * 11/2, m_star.height() + 2);
}

void RatingEdit::updateRating(QMouseEvent * e)
{
	int r = int(round((double)e->x() / m_star.width()));
	setRating(r);
}
