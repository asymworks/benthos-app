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

#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "workers/transferworker.hpp"
#include "transferdialog.hpp"

TransferDialog::TransferDialog(QWidget * parent)
	: QDialog(parent), m_lblStatus(0), m_pbTransfer(0)
{
	m_lblStatus = new QLabel;
	m_pbTransfer = new QProgressBar;
	m_pbTransfer->setMinimumWidth(400);

	QPushButton * btnCancel = new QPushButton(tr("Cancel"));
	connect(btnCancel, SIGNAL(clicked()), this, SLOT(btnCancelClicked()));

	QHBoxLayout * hbox = new QHBoxLayout;
	hbox->addStretch(1);
	hbox->addWidget(btnCancel, 0);
	hbox->addStretch(1);

	QVBoxLayout * vbox = new QVBoxLayout;
	vbox->addWidget(m_lblStatus);
	vbox->addWidget(m_pbTransfer);
	vbox->addLayout(hbox);

	setLayout(vbox);
	setWindowTitle(tr("Transfer Progress"));
}

TransferDialog::~TransferDialog()
{
}

void TransferDialog::btnCancelClicked()
{
	emit cancelled();
}

void TransferDialog::xfrError(const QString & msg)
{
	QMessageBox::critical(this, tr("Transfer Dives"), msg);

	done(QDialog::Rejected);
}
void TransferDialog::xfrFinished()
{
	done(QDialog::Accepted);
}

void TransferDialog::xfrProgress(unsigned long bytes)
{
	m_pbTransfer->setValue(bytes);
}

void TransferDialog::xfrStarted(unsigned long bytes)
{
	if (! bytes)
		QMessageBox::information(this, tr("Transfer Dives"), tr("No new dives to transfer"));

	m_pbTransfer->setMaximum(bytes);
}

void TransferDialog::xfrStatus(const QString & msg)
{
	m_lblStatus->setText(msg);
}
