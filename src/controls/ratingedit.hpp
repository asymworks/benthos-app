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

#ifndef RATINGEDIT_HPP_
#define RATINGEDIT_HPP_

/**
 * @file src/controls/ratingedit.hpp
 * @brief Star Rating Editor Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QImage>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QSize>
#include <QWidget>

/**
 * Star Rating Editor
 *
 * Editor class to edit a 5-star rating.  Clicking the control enables dragging
 * the mouse to select the rating.  This control is compatible with Qt MVF.
 */
class RatingEdit: public QWidget
{
	Q_OBJECT

public:
	Q_PROPERTY(int maxRating READ maxRating WRITE setMaxRating)
	Q_PROPERTY(int rating READ rating WRITE setRating NOTIFY ratingChanged USER true)

public:

	//! Class Constructor
	RatingEdit(int maxRating = 5, QWidget * parent = NULL);

	//! Class Destructor
	virtual ~RatingEdit();

	//! @return Maximum Rating
	int maxRating() const;

	//! @return Rating
	int rating() const;

	//! @brief Set Maximum Rating
	void setMaxRating(int value);

	//! @brief Set Rating
	void setRating(int value);

	//! @return Size Hint
	virtual QSize sizeHint() const;

protected:
	virtual void mousePressEvent(QMouseEvent * e);
	virtual void mouseMoveEvent(QMouseEvent * e);
	virtual void mouseReleaseEvent(QMouseEvent * e);
	virtual void paintEvent(QPaintEvent * e);
	virtual void updateRating(QMouseEvent * e);

signals:
	void ratingChanged(int);

private:
	QImage 				m_star;
	QImage 				m_blue;
	QImage 				m_dot;

	int					m_rating;
	int					m_maxRating;
	bool				m_active;

};

#endif /* RATINGEDIT_HPP_ */
