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

#include <QFrame>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <benthos/logbook/profile.hpp>
#include <benthos/logbook/session.hpp>
using namespace benthos;

#include "profile_plot.hpp"
#include "profile_table.hpp"
#include "profile_view.hpp"

ProfileView::ProfileView(QWidget * parent)
	: QFrame(parent), m_pvPlot(0), m_pvTable(0), m_pvBlank(0), m_swView(0)
{
	m_pvPlot = new ProfilePlotView;
	m_pvTable = new ProfileTableView;

	QLabel * logo = new QLabel;
	logo->setPixmap(QPixmap(":/graphics/hslogo-96.png"));
	QVBoxLayout * vboxl = new QVBoxLayout;
	vboxl->setContentsMargins(0, 0, 0, 0);
	vboxl->addStretch();
	vboxl->addWidget(logo);
	QHBoxLayout * hboxl = new QHBoxLayout;
	hboxl->setContentsMargins(16, 16, 16, 16);
	hboxl->addLayout(vboxl);
	hboxl->addStretch();

	m_pvBlank = new QWidget;
	m_pvBlank->setLayout(hboxl);

	m_swView = new QStackedWidget;
	m_swView->addWidget(m_pvBlank);
	m_swView->addWidget(m_pvTable);
	m_swView->addWidget(m_pvPlot);

	m_pvPlot->setDive(Dive::Ptr());
	m_pvTable->setDive(Dive::Ptr());

	setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	setStyleSheet("QFrame { background-color: white; }");

	QVBoxLayout * vbox = new QVBoxLayout;
	vbox->setContentsMargins(0, 0, 0, 0);
	vbox->addWidget(m_swView);
	setLayout(vbox);
}

ProfileView::~ProfileView()
{
}

void ProfileView::setDive(Dive::Ptr dive)
{
	if (! dive)
	{
		m_swView->setCurrentWidget(m_pvBlank);
		m_pvTable->setDive(dive);
		m_pvPlot->setDive(dive);

		return;
	}

	//TODO: Use Collection
	IProfileFinder::Ptr pf = boost::dynamic_pointer_cast<IProfileFinder>(dive->session()->finder<Profile>());
	std::vector<Profile::Ptr> p = pf->findByDive(dive->id());
	if (! p.empty())
	{
		m_swView->setCurrentWidget(m_pvPlot);
		m_pvPlot->setDive(dive);
	}
	else
	{
		m_swView->setCurrentWidget(m_pvTable);
		m_pvTable->setDive(dive);
	}
}
