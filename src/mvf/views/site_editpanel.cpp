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

#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>

#include "mvf/adapters.hpp"
#include "mvf/countrymodel.hpp"
#include "site_editpanel.hpp"

#include <benthos/logbook/dive_site.hpp>
using namespace benthos::logbook;

SiteEditPanel::SiteEditPanel(QWidget * parent)
	: QTabWidget(parent), m_mapper(NULL)
{
	createSitePage();
	createMorePage();
	createNotesPage();

	m_pgMap = new MapLocationEditor(this);
	connect(m_pgMap, SIGNAL(positionChanged(const QPointF &)), this, SLOT(pgMapPositionChanged(const QPointF &)));

	addTab(m_pgSite, tr("Site"));
	addTab(m_pgMore, tr("More"));
	addTab(m_pgMap, tr("Map"));
	addTab(m_pgNotes, tr("Notes"));
}

SiteEditPanel::~SiteEditPanel()
{
}

void SiteEditPanel::bind(Session::Ptr session, QDataWidgetMapper * mapper)
{
	m_mapper = mapper;

	mapper->clearMapping();

	mapper->addMapping(m_txtSite, 0);
	mapper->addMapping(m_txtPlace, 1);
	mapper->addMapping(m_cbxCountry, 2);
	mapper->addMapping(m_cbxSalinity, 3);

	mapper->addMapping(m_txtPlatform, 4);
	mapper->addMapping(m_txtWater, 5);
	mapper->addMapping(m_txtBottom, 6);
	mapper->addMapping(m_txtAltitude, 7);

	mapper->addMapping(m_pgMap, 8);
	mapper->addMapping(m_txtNotes, 11, "plainText");

	IDiveSiteFinder::Ptr f(boost::dynamic_pointer_cast<IDiveSiteFinder>(session->finder<DiveSite>()));

	VectorModel<std::string> * cmb = dynamic_cast<VectorModel<std::string> *>(m_cmpBottom->model());
	if (cmb)
		cmb->reset(f->bottomValues());

	VectorModel<std::string> * cmp = dynamic_cast<VectorModel<std::string> *>(m_cmpPlatform->model());
	if (cmp)
		cmp->reset(f->platformValues());

	VectorModel<std::string> * cmw = dynamic_cast<VectorModel<std::string> *>(m_cmpWater->model());
	if (cmw)
		cmw->reset(f->waterBodyValues());
}

QAbstractItemDelegate * SiteEditPanel::createDelegate(QObject * parent) const
{
	return NULL;
}

void SiteEditPanel::createSitePage()
{
	m_pgSite = new QWidget(this);

	m_txtSite = new QLineEdit(m_pgSite);
	QLabel * lblSite = new QLabel(tr("Dive &Site"), m_pgSite);
	lblSite->setBuddy(m_txtSite);

	m_txtPlace = new QLineEdit(m_pgSite);
	QLabel * lblPlace = new QLabel(tr("&Place"), m_pgSite);
	lblPlace->setBuddy(m_txtPlace);

	m_cbxCountry = new QComboBox(m_pgSite);
	m_cbxCountry->setModel(new CountryModel());
	QLabel * lblCountry = new QLabel(tr("&Country"), m_pgSite);
	lblCountry->setBuddy(m_cbxCountry);

	m_cbxSalinity = new QComboBox(m_pgSite);
	m_cbxSalinity->addItem("fresh");
	m_cbxSalinity->addItem("salt");
	QLabel * lblSalinity = new QLabel(tr("Sali&nity"), m_pgSite);
	lblSalinity->setBuddy(m_cbxSalinity);

	QGridLayout * gbox = new QGridLayout();
	gbox->setContentsMargins(0, 0, 0, 0);
	gbox->addWidget(lblSite, 0, 0);
	gbox->addWidget(m_txtSite, 1, 0);
	gbox->setRowMinimumHeight(2, 8);
	gbox->addWidget(lblPlace, 3, 0);
	gbox->addWidget(m_txtPlace, 4, 0);
	gbox->setRowMinimumHeight(5, 8);
	gbox->addWidget(lblCountry, 6, 0);
	gbox->addWidget(m_cbxCountry, 7, 0);
	gbox->setRowMinimumHeight(8, 8);
	gbox->addWidget(lblSalinity, 9, 0);
	gbox->addWidget(m_cbxSalinity, 10, 0);
	gbox->setRowMinimumHeight(11, 8);

	QVBoxLayout * vbox = new QVBoxLayout();
	vbox->addLayout(gbox);
	vbox->addStretch();

	m_pgSite->setLayout(vbox);
}

void SiteEditPanel::createMorePage()
{
	m_pgMore = new QWidget(this);

	m_cmpPlatform = new QCompleter(new VectorModel<std::string>(std::vector<std::string>()));
	m_cmpPlatform->setCaseSensitivity(Qt::CaseInsensitive);

	m_cmpWater = new QCompleter(new VectorModel<std::string>(std::vector<std::string>()));
	m_cmpWater->setCaseSensitivity(Qt::CaseInsensitive);

	m_cmpBottom = new QCompleter(new VectorModel<std::string>(std::vector<std::string>()));
	m_cmpBottom->setCaseSensitivity(Qt::CaseInsensitive);

	m_txtPlatform = new QLineEdit(m_pgMore);
	m_txtPlatform->setCompleter(m_cmpPlatform);
	QLabel * lblPlatform = new QLabel(tr("&Platform"), m_pgMore);
	lblPlatform->setBuddy(m_txtPlatform);

	m_txtWater = new QLineEdit(m_pgMore);
	m_txtWater->setCompleter(m_cmpWater);
	QLabel * lblWater = new QLabel(tr("&Water"), m_pgMore);
	lblWater->setBuddy(m_txtWater);

	m_txtBottom = new QLineEdit(m_pgMore);
	m_txtBottom ->setCompleter(m_cmpBottom);
	QLabel * lblBottom = new QLabel(tr("&Bottom Composition"), m_pgMore);
	lblBottom->setBuddy(m_txtBottom);

	m_txtAltitude = new QuantityEdit(qtDepth, QString(), m_pgMore);
	QLabel * lblAltitude = new QLabel(tr("&Altitude"), m_pgMore);
	lblAltitude->setBuddy(m_txtAltitude);

	QGridLayout * gbox = new QGridLayout();
	gbox->setContentsMargins(0, 0, 0, 0);
	gbox->addWidget(lblPlatform, 0, 0);
	gbox->addWidget(m_txtPlatform, 1, 0);
	gbox->setRowMinimumHeight(2, 8);
	gbox->addWidget(lblWater, 3, 0);
	gbox->addWidget(m_txtWater, 4, 0);
	gbox->setRowMinimumHeight(5, 8);
	gbox->addWidget(lblBottom, 6, 0);
	gbox->addWidget(m_txtBottom, 7, 0);
	gbox->setRowMinimumHeight(8, 8);

	QGridLayout * gbox2 = new QGridLayout();
	gbox2->setRowMinimumHeight(0, 8);
	gbox2->addWidget(lblAltitude, 1, 0);
	gbox2->addWidget(m_txtAltitude, 2, 0);

	QVBoxLayout * vbox = new QVBoxLayout();
	vbox->addLayout(gbox);
	vbox->addLayout(gbox2);
	vbox->addStretch();

	m_pgMore->setLayout(vbox);
}

void SiteEditPanel::createNotesPage()
{
	m_pgNotes = new QWidget(this);
	m_txtNotes = new QTextEdit(m_pgNotes);

	QVBoxLayout * vbox = new QVBoxLayout();
	vbox->addWidget(m_txtNotes);

	m_pgNotes->setLayout(vbox);
}

void SiteEditPanel::pgMapPositionChanged(const QPointF &)
{
	if (m_mapper != NULL)
		m_mapper->submit();
}

QString SiteEditPanel::title() const
{
	if (m_txtSite->text().isEmpty())
		return tr("Untitled Site");
	return m_txtSite->text();
}
