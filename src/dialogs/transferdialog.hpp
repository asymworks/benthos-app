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

#ifndef TRANSFERDIALOG_HPP_
#define TRANSFERDIALOG_HPP_

/**
 * @file src/dialogs/transferdialog.hpp
 * @brief Dive Computer Transfer Dialog Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QRunnable>

/**
 * @brief Dive Computer Transfer Dialog
 *
 * Shows the progress of the dive computer transfer operation.
 */
class TransferDialog: public QDialog
{
	Q_OBJECT

public:

	//! Class Constructor
	TransferDialog(QWidget * parent = 0);

	//! Class Destructor
	virtual ~TransferDialog();

public slots:
	void xfrError(const QString &);
	void xfrFinished();
	void xfrProgress(unsigned long);
	void xfrStarted(unsigned long);
	void xfrStatus(const QString &);

protected slots:
	void btnCancelClicked();

signals:
	void cancelled();

private:
	QLabel *				m_lblStatus;
	QProgressBar *			m_pbTransfer;

};

#endif /* TRANSFERDIALOG_HPP_ */
