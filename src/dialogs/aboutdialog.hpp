/*
 * Copyright (C) 2013 Asymworks, LLC.  All Rights Reserved.
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

#ifndef ABOUTDIALOG_HPP_
#define ABOUTDIALOG_HPP_

/**
 * @file src/dialogs/aboutdialog.hpp
 * @brief About Benthos Dialog Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QDialog>

/**
 * @brief About Dialog Class
 *
 * Displays the "About Benthos" information, including logo, version number,
 * and links to the web site.
 */
class AboutDialog: public QDialog
{
	Q_OBJECT

public:

	//! Class Constructor
	AboutDialog(QWidget * parent = 0);

	//! Class Destructor
	~AboutDialog();

};

#endif /* ABOUTDIALOG_HPP_ */
