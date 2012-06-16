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

#ifndef SEARCHEDIT_HPP_
#define SEARCHEDIT_HPP_

/**
 * @file src/controls/searcheditor.hpp
 * @brief Search Editor Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QLineEdit>
#include <QEvent>
#include <QToolButton>

/**
 * @brief Search Edit Class
 *
 * Styled search edit box which includes a search icon, clear button, and
 * pre-defined preview text.
 */
class SearchEdit: public QLineEdit
{
	Q_OBJECT

public:

	//! Class Constructor
	SearchEdit(QWidget * parent = 0);

	//! Class Destructor
	virtual ~SearchEdit();

	//! Return the Size Hint
	virtual QSize sizeHint() const;

protected:

	//! Capture a Resize Event
	virtual void resizeEvent(QResizeEvent * e);

private:
	QToolButton *	m_btnClear;

};

#endif /* SEARCHEDIT_HPP_ */
