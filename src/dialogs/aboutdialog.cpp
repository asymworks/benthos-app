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

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "aboutdialog.hpp"
#include "config.hpp"

AboutDialog::AboutDialog(QWidget * parent)
{
	QLabel * logo = new QLabel;
	logo->setPixmap(QPixmap(":/graphics/hslogo-96.png"));

	QLabel * ptitle = new QLabel(tr(BENTHOS_APP_TITLE));
	ptitle->setStyleSheet("QLabel { font-size: 26px; font-weight: bold; }");

	QLabel * pdesc = new QLabel(tr("Scuba Diving Logbook"));
	QLabel * pver = new QLabel(tr("Version %1").arg(BENTHOS_VERSION_STRING));

	QLabel * pcopy = new QLabel(tr(BENTHOS_COPYRIGHT));
	pcopy->setStyleSheet("QLabel { font-size: 10px; }");

	QVBoxLayout * vbox1 = new QVBoxLayout;
	vbox1->addWidget(ptitle);
	vbox1->addWidget(pdesc);
	vbox1->addWidget(pver);
	vbox1->addStretch(1);

	QHBoxLayout * hbox1 = new QHBoxLayout;
	hbox1->addWidget(logo);
	hbox1->addSpacing(16);
	hbox1->addLayout(vbox1, 1);

	QFrame * topFrame = new QFrame;
	topFrame->setContentsMargins(16, 16, 16, 16);
	topFrame->setStyleSheet("QFrame { background-color: white; }");
	topFrame->setLayout(hbox1);

	QHBoxLayout * hbox2 = new QHBoxLayout;
	hbox2->addStretch(1);
	hbox2->addWidget(pcopy);
	hbox2->addStretch(1);

	QVBoxLayout * vbox2 = new QVBoxLayout;
	vbox2->setContentsMargins(0, 0, 0, 0);
	vbox2->addWidget(topFrame);
	vbox2->addLayout(hbox2);
	vbox2->addStretch(1);

	setLayout(vbox2);
	setWindowTitle(tr("About %1").arg(BENTHOS_APP_TITLE));
}

AboutDialog::~AboutDialog()
{
}
