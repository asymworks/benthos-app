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
#include <QIntValidator>
#include <QLabel>
#include <QSettings>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

#include "mvf/adapters.hpp"
#include "mvf/countrymodel.hpp"

#include "dive_editpanel.hpp"

#include <benthos/logbook/dive_computer.hpp>
#include <benthos/logbook/dive_site.hpp>
#include <benthos/logbook/mix.hpp>
using namespace benthos::logbook;

class DiveSiteFKModel: public ForeignKeyModel<DiveSite>
{
public:
	DiveSiteFKModel() { }
	virtual ~DiveSiteFKModel() { }

protected:
	virtual QVariant decorationData(DiveSite::Ptr item) const
	{
		if (! item || ! item->country_().is_initialized())
			return QVariant();

		country c(item->country_().get());

		if (c.code().empty())
			return QVariant();
		return QImage(QString(":/flags/%1.png").arg(QString::fromStdString(c.code()).toLower()));
	}

	virtual QVariant displayData(DiveSite::Ptr item) const
	{
		if (! item)
			return QVariant();
		return QString::fromStdString(item->long_name());
	}

};

class DiveComputerFKModel: public ForeignKeyModel<DiveComputer>
{
public:
	DiveComputerFKModel() { }
	virtual ~DiveComputerFKModel() { }

protected:
	virtual QVariant decorationData(DiveComputer::Ptr item) const
	{
		return QVariant();
	}

	virtual QVariant displayData(DiveComputer::Ptr item) const
	{
		if (! item || ! item->name().is_initialized())
			return QVariant();
		return QString::fromStdString(item->name().get());
	}

};

class MixFKModel: public ForeignKeyModel<Mix>
{
public:
	MixFKModel() { }
	virtual ~MixFKModel() { }

protected:
	virtual QVariant decorationData(Mix::Ptr item) const
	{
		return QVariant();
	}

	virtual QVariant displayData(Mix::Ptr item) const
	{
		if (! item)
			return QVariant();
		return QString::fromStdString(item->toString());
	}
};

DiveEditPanel::DiveEditPanel(QWidget * parent)
	: QTabWidget(parent), m_mapper(NULL), m_session()
{
	m_SiteFKModel = new DiveSiteFKModel;
	m_ComputerFKModel = new DiveComputerFKModel;
	m_MixFKModel = new MixFKModel;

	createDivePage();
	createExtraPage();
	createTablePage();
	createComputerPage();
	createNotesPage();

	addTab(m_pgDive, tr("Dive"));
	addTab(m_pgExtra, tr("More"));
	addTab(m_pgTable, tr("Table"));
	addTab(m_pgComputer, tr("Computer"));
	addTab(m_pgNotes, tr("Notes"));
}

DiveEditPanel::~DiveEditPanel()
{
	delete m_SiteFKModel;
	delete m_ComputerFKModel;
	delete m_MixFKModel;
}

void DiveEditPanel::bind(Session::Ptr session, QDataWidgetMapper * mapper)
{
	m_session = session;
	m_mapper = mapper;

	mapper->clearMapping();

	mapper->addMapping(m_txtDateTime, 0);
	mapper->addMapping(m_reRating, 1);
	mapper->addMapping(m_txtDiveNumber, 2);
	mapper->addMapping(m_txtRepetition, 3);
	mapper->addMapping(m_txtInterval, 4);

	mapper->addMapping(m_txtMaxDepth, 8);
	mapper->addMapping(m_txtAvgDepth, 9);
	mapper->addMapping(m_txtRuntime, 10);
	mapper->addMapping(m_txtAirTemp, 12);
	mapper->addMapping(m_txtMaxTemp, 13);
	mapper->addMapping(m_txtMinTemp, 14);

	mapper->addMapping(m_txtAirStart, 15);
	mapper->addMapping(m_txtAirEnd, 16);

	mapper->addMapping(m_txtNotes, 17, "plainText");

	mapper->addMapping(m_cbxSite, 18);
	mapper->addMapping(m_cbxComputer, 19);
	mapper->addMapping(m_cbxMix, 20);

	mapper->addMapping(m_txtStartPG, 22);
	mapper->addMapping(m_txtEndPG, 23);
	mapper->addMapping(m_txtRNT, 24);

	//mapper->addMapping(m_txtDesat, 25);
	//mapper->addMapping(m_txtNoFly, 26);

	mapper->addMapping(m_chkStop, 28);
	mapper->addMapping(m_txtStopDepth, 29);
	mapper->addMapping(m_txtStopTime, 30);

	mapper->addMapping(m_cbxVizCategory, 31);
	mapper->addMapping(m_txtVizDistance, 32);
	mapper->addMapping(m_txtWeight, 33);

	((ForeignKeyModel<DiveSite> *)m_SiteFKModel)->bind(session);
	((ForeignKeyModel<DiveComputer> *)m_ComputerFKModel)->bind(session);
	((ForeignKeyModel<Mix> *)m_MixFKModel)->bind(session);

	((QSortFilterProxyModel *)m_cbxSite->model())->sort(0, Qt::AscendingOrder);
	((QSortFilterProxyModel *)m_cbxMix->model())->sort(0, Qt::AscendingOrder);
	((QSortFilterProxyModel *)m_cbxComputer->model())->sort(0, Qt::AscendingOrder);
}

void DiveEditPanel::btnNewSiteClicked()
{
	if (m_txtSiteName->text().isEmpty() || ! m_session)
		return;

	DiveSite::Ptr ds(new DiveSite);
	ds->setName(m_txtSiteName->text().toStdString());

	if (! m_txtSitePlace->text().isEmpty())
		ds->setPlace(m_txtSitePlace->text().toStdString());

	try
	{
		country c(m_cbxSiteCountry->itemData(m_cbxSiteCountry->currentIndex(), Qt::EditRole).toString().toStdString());
		ds->setCountry(c);
	}
	catch (std::exception & e)
	{
		ds->setCountry(boost::none);
	}

	m_session->add(ds);
	m_session->commit();

	((ForeignKeyModel<DiveSite> *)m_SiteFKModel)->bind(m_session);
	((QSortFilterProxyModel *)m_cbxSite->model())->sort(0, Qt::AscendingOrder);

	m_cbxSite->setCurrentIndex(m_cbxSite->findData(QVariant::fromValue<int64_t>(ds->id()), Qt::EditRole, Qt::MatchFlags()));

	m_txtSiteName->clear();
	m_txtSitePlace->clear();

	QModelIndex mi = m_mapper->model()->index(m_mapper->currentIndex(), m_mapper->mappedSection(m_cbxSite));
	m_mapper->model()->setData(mi, QVariant::fromValue<int64_t>(ds->id()), Qt::EditRole);
}

QAbstractItemDelegate * DiveEditPanel::createDelegate(QObject * parent) const
{
	return NULL;
}

void DiveEditPanel::createDivePage()
{
	m_pgDive = new QWidget(this);

	m_reRating = new RatingEdit(5, m_pgDive);
	QLabel * lblRating = new QLabel(tr("Rating"), m_pgDive);
	lblRating->setBuddy(m_reRating);

	m_txtDiveNumber = new QLineEdit(m_pgDive);
	m_txtDiveNumber->setValidator(new QIntValidator(1, INT_MAX, m_pgDive));
	QLabel * lblNumber = new QLabel(tr("Dive Number"), m_pgDive);
	lblNumber->setBuddy(m_txtDiveNumber);

	QSettings s;
	s.beginGroup("Settings");
	QString dtfmt = s.value(QString("DTFormat"), QString("MM/dd/yy hh:mm AP")).toString();
	s.endGroup();

	m_txtDateTime = new QDateTimeEdit(m_pgDive);
	m_txtDateTime->setDisplayFormat(dtfmt);
	QLabel * lblDateTime = new QLabel(tr("Dive Date/Time"), m_pgDive);
	lblDateTime->setBuddy(m_txtDateTime);

	//TODO: Make an HH:MM editor
	m_txtInterval = new QLineEdit(m_pgDive);
	m_txtInterval->setValidator(new QIntValidator(0, INT_MAX, this));
	QLabel * lblInterval = new QLabel(tr("Surface Interval"), m_pgDive);
	lblInterval->setBuddy(m_txtInterval);

	//TODO: Make an HH:MM editor
	m_txtRuntime = new QLineEdit(m_pgDive);
	m_txtRuntime->setValidator(new QIntValidator(0, INT_MAX, this));
	QLabel * lblRuntime = new QLabel(tr("Bottom Time"), m_pgDive);
	lblRuntime->setBuddy(m_txtRuntime);

	m_txtRepetition = new QLineEdit(m_pgDive);
	m_txtRepetition->setValidator(new QIntValidator(0, INT_MAX, this));
	QLabel * lblRepetition = new QLabel(tr("Repetitive Dive"), m_pgDive);
	lblRepetition->setBuddy(m_txtRepetition);

	QHBoxLayout * hbox1 = new QHBoxLayout();
	hbox1->setContentsMargins(0, 0, 0, 0);
	hbox1->addWidget(m_reRating);
	hbox1->addStretch();

	QGridLayout * gbox1 = new QGridLayout();
	gbox1->setContentsMargins(0, 0, 0, 0);
	gbox1->addWidget(lblRating, 0, 0);
	gbox1->addLayout(hbox1, 1, 0);
	gbox1->addWidget(lblNumber, 0, 1);
	gbox1->addWidget(m_txtDiveNumber, 1, 1);
	gbox1->setRowMinimumHeight(2, 8);
	gbox1->addWidget(lblDateTime, 3, 0);
	gbox1->addWidget(m_txtDateTime, 4, 0);
	gbox1->addWidget(lblInterval, 3, 1);
	gbox1->addWidget(m_txtInterval, 4, 1);
	gbox1->addWidget(lblRuntime, 3, 2);
	gbox1->addWidget(m_txtRuntime, 4, 2);
	gbox1->addWidget(lblRepetition, 3, 3);
	gbox1->addWidget(m_txtRepetition, 4, 3);
	gbox1->setRowMinimumHeight(5, 16);

	QSortFilterProxyModel * pmdl = new QSortFilterProxyModel(m_pgDive);
	pmdl->setDynamicSortFilter(true);
	pmdl->setSortCaseSensitivity(Qt::CaseInsensitive);
	pmdl->setSourceModel(m_SiteFKModel);

	m_cbxSite = new QComboBox(m_pgDive);
	m_cbxSite->setModel(pmdl);
	QLabel * lblSite = new QLabel(tr("Dive Site"), m_pgDive);
	lblSite->setBuddy(m_cbxSite);

	m_txtSiteName = new QLineEdit(m_pgDive);
	m_txtSiteName->setPlaceholderText(tr("Site Name"));

	m_txtSitePlace = new QLineEdit(m_pgDive);
	m_txtSitePlace->setPlaceholderText(tr("Place Name"));

	m_cbxSiteCountry = new QComboBox(m_pgDive);
	m_cbxSiteCountry->setModel(new CountryModel());

	m_btnNewSite = new QPushButton(tr("Create New"));
	m_btnNewSite->setAutoDefault(false);
	m_btnNewSite->setDefault(false);
	connect(m_btnNewSite, SIGNAL(clicked()), this, SLOT(btnNewSiteClicked()));

	QGridLayout * gbox2 = new QGridLayout();
	gbox2->setContentsMargins(0, 0, 0, 0);
	gbox2->addWidget(lblSite, 0, 0, 1, 3);
	gbox2->addWidget(m_cbxSite, 1, 0, 1, 4);
	gbox2->addWidget(m_txtSiteName, 2, 0);
	gbox2->addWidget(m_txtSitePlace, 2, 1);
	gbox2->addWidget(m_cbxSiteCountry, 2, 2);
	gbox2->addWidget(m_btnNewSite, 2, 3);
	gbox2->setRowMinimumHeight(3, 16);

	m_txtMaxDepth = new QuantityEdit(qtDepth, QString(), m_pgDive);
	QLabel * lblMaxDepth = new QLabel(tr("Max Depth"));
	lblMaxDepth->setBuddy(m_txtMaxDepth);

	m_txtAvgDepth = new QuantityEdit(qtDepth, QString(), m_pgDive);
	QLabel * lblAvgDepth = new QLabel(tr("Avg Depth"));
	lblAvgDepth->setBuddy(m_txtAvgDepth);

	m_txtMaxTemp = new QuantityEdit(qtTemperature, QString(), m_pgDive);
	QLabel * lblMaxTemp = new QLabel(tr("Max Temp"));
	lblMaxTemp->setBuddy(m_txtMaxTemp);

	m_txtMinTemp = new QuantityEdit(qtTemperature, QString(), m_pgDive);
	QLabel * lblMinTemp = new QLabel(tr("Min Temp"));
	lblMinTemp->setBuddy(m_txtMinTemp);

	m_txtAirTemp = new QuantityEdit(qtTemperature, QString(), m_pgDive);
	QLabel * lblAirTemp = new QLabel(tr("Air Temp"));
	lblAirTemp->setBuddy(m_txtAirTemp);

	m_txtAirStart = new QuantityEdit(qtPressure, QString(), m_pgDive);
	QLabel * lblAirStart = new QLabel(tr("Start Pressure"));
	lblAirStart->setBuddy(m_txtAirStart);

	m_txtAirEnd = new QuantityEdit(qtPressure, QString(), m_pgDive);
	QLabel * lblAirEnd = new QLabel(tr("End Pressure"));
	lblAirEnd->setBuddy(m_txtAirEnd);

	QSortFilterProxyModel * pmdl_mix = new QSortFilterProxyModel(m_pgDive);
	pmdl_mix->setDynamicSortFilter(true);
	pmdl_mix->setSortCaseSensitivity(Qt::CaseInsensitive);
	pmdl_mix->setSourceModel(m_MixFKModel);

	m_cbxMix = new QComboBox(m_pgDive);
	m_cbxMix->setModel(pmdl_mix);
	QLabel * lblMix = new QLabel(tr("Gas Mix"), m_pgDive);
	lblMix->setBuddy(m_cbxMix);

	QGridLayout * gbox3 = new QGridLayout();
	gbox3->setContentsMargins(0, 0, 0, 0);
	gbox3->addWidget(lblMaxDepth, 0, 0);
	gbox3->addWidget(m_txtMaxDepth, 1, 0);
	gbox3->addWidget(lblAvgDepth, 0, 1);
	gbox3->addWidget(m_txtAvgDepth, 1, 1);
	gbox3->setColumnMinimumWidth(2, 8);
	gbox3->addWidget(lblMaxTemp, 0, 3);
	gbox3->addWidget(m_txtMaxTemp, 1, 3);
	gbox3->addWidget(lblMinTemp, 0, 4);
	gbox3->addWidget(m_txtMinTemp, 1, 4);
	gbox3->addWidget(lblAirTemp, 0, 5);
	gbox3->addWidget(m_txtAirTemp, 1, 5);
	gbox2->setRowMinimumHeight(3, 8);
	gbox3->addWidget(lblAirStart, 3, 0);
	gbox3->addWidget(m_txtAirStart, 4, 0);
	gbox3->addWidget(lblAirEnd, 3, 1);
	gbox3->addWidget(m_txtAirEnd, 4, 1);
	gbox3->addWidget(lblMix, 3, 4, 1, 2);
	gbox3->addWidget(m_cbxMix, 4, 4, 1, 2);

	QVBoxLayout * vbox = new QVBoxLayout();
	vbox->addLayout(gbox1);
	vbox->addLayout(gbox2);
	vbox->addLayout(gbox3);
	vbox->addStretch();

	m_pgDive->setLayout(vbox);
}

void DiveEditPanel::createExtraPage()
{
	m_pgExtra = new QWidget(this);

	m_chkStop = new QCheckBox(tr("Safety Stop"), m_pgExtra);

	m_txtStopDepth = new QuantityEdit(qtDepth, QString(), m_pgExtra);
	QLabel * lblStopDepth = new QLabel(tr("Stop Depth"));
	lblStopDepth->setBuddy(m_txtStopDepth);

	//TODO: Make Minutes/Seconds Edit
	m_txtStopTime = new QLineEdit(QString(), m_pgExtra);
	QLabel * lblStopTime = new QLabel(tr("Stop Time"));
	lblStopTime->setBuddy(m_txtStopTime);

	m_cbxVizCategory = new QComboBox(m_pgExtra);
	m_cbxVizCategory->addItem("excellent");
	m_cbxVizCategory->addItem("good");
	m_cbxVizCategory->addItem("fair");
	m_cbxVizCategory->addItem("poor");
	QLabel * lblVizCategory = new QLabel(tr("Visibility Category"));
	lblVizCategory->setBuddy(m_cbxVizCategory);

	m_txtVizDistance = new QuantityEdit(qtDepth, QString(), m_pgExtra);
	QLabel * lblVizDistance = new QLabel(tr("Visibility Distance"));
	lblVizDistance->setBuddy(m_txtVizDistance);

	m_txtWeight = new QuantityEdit(qtWeight, QString(), m_pgExtra);
	QLabel * lblWeight = new QLabel(tr("Weight"));
	lblWeight->setBuddy(m_txtWeight);

	QGridLayout * gbox1 = new QGridLayout();
	gbox1->setContentsMargins(0, 0, 0, 0);
	gbox1->addWidget(m_chkStop, 0, 1);
	gbox1->addWidget(lblStopDepth, 1, 0);
	gbox1->addWidget(m_txtStopDepth, 1, 1);
	gbox1->addWidget(lblStopTime, 2, 0);
	gbox1->addWidget(m_txtStopTime, 2, 1);
	gbox1->setRowMinimumHeight(3, 8);
	gbox1->addWidget(lblVizCategory, 4, 0);
	gbox1->addWidget(m_cbxVizCategory, 4, 1);
	gbox1->addWidget(lblVizDistance, 5, 0);
	gbox1->addWidget(m_txtVizDistance, 5, 1);
	gbox1->setRowMinimumHeight(6, 8);
	gbox1->addWidget(lblWeight, 7, 0);
	gbox1->addWidget(m_txtWeight, 7, 1);

	QVBoxLayout * vbox = new QVBoxLayout();
	vbox->addLayout(gbox1);
	vbox->addStretch();

	QHBoxLayout * hbox = new QHBoxLayout();
	hbox->addLayout(vbox);
	hbox->addStretch();

	m_pgExtra->setLayout(hbox);
}

void DiveEditPanel::createTablePage()
{
	m_pgTable = new QWidget(this);

	m_cbxAlgorithm = new QComboBox(m_pgTable);
	m_cbxAlgorithm->setEnabled(false);
	QLabel * lblAlgorithm = new QLabel(tr("Algorithm"));
	lblAlgorithm->setBuddy(m_cbxAlgorithm);

	m_txtStartPG = new QLineEdit(m_pgTable);
	m_txtStartPG->setMaxLength(2);
	QLabel * lblStartPG = new QLabel(tr("Starting Pressure Group"));
	lblStartPG->setBuddy(m_txtStartPG);

	m_txtEndPG = new QLineEdit(m_pgTable);
	m_txtEndPG->setMaxLength(2);
	QLabel * lblEndPG = new QLabel(tr("Ending Pressure Group"));
	lblStartPG->setBuddy(m_txtEndPG);

	m_txtRNT = new QLineEdit(m_pgTable);
	m_txtRNT->setValidator(new QIntValidator(0, 600));
	QLabel * lblRNT = new QLabel(tr("Residual Nitrogen Time (RNT)"));
	lblRNT->setBuddy(m_txtRNT);

	m_txtDesat = new QLineEdit(m_pgTable);
	m_txtDesat->setEnabled(false);
	QLabel * lblDesat = new QLabel(tr("Desaturation Time"));
	lblDesat->setBuddy(m_txtDesat);

	m_txtNoFly = new QLineEdit(m_pgTable);
	m_txtNoFly->setEnabled(false);
	QLabel * lblNoFly = new QLabel(tr("No-Fly Time"));
	lblNoFly->setBuddy(m_txtNoFly);

	QGridLayout * gbox1 = new QGridLayout();
	gbox1->setContentsMargins(0, 0, 0, 0);
	gbox1->addWidget(lblAlgorithm, 0, 0);
	gbox1->addWidget(m_cbxAlgorithm, 0, 1);
	gbox1->setRowMinimumHeight(1, 8);
	gbox1->addWidget(lblStartPG, 2, 0);
	gbox1->addWidget(m_txtStartPG, 2, 1);
	gbox1->addWidget(lblEndPG, 3, 0);
	gbox1->addWidget(m_txtEndPG, 3, 1);
	gbox1->addWidget(lblRNT, 4, 0);
	gbox1->addWidget(m_txtRNT, 4, 1);
	gbox1->setRowMinimumHeight(5, 8);
	gbox1->addWidget(lblDesat, 6, 0);
	gbox1->addWidget(m_txtDesat, 6, 1);
	gbox1->addWidget(lblNoFly, 7, 0);
	gbox1->addWidget(m_txtNoFly, 7, 1);

	QVBoxLayout * vbox = new QVBoxLayout();
	vbox->addLayout(gbox1);
	vbox->addStretch();

	QHBoxLayout * hbox = new QHBoxLayout();
	hbox->addLayout(vbox);
	hbox->addStretch();

	m_pgTable->setLayout(hbox);
}

void DiveEditPanel::createComputerPage()
{
	m_pgComputer = new QWidget(this);

	QSortFilterProxyModel * pmdl = new QSortFilterProxyModel(m_pgComputer);
	pmdl->setDynamicSortFilter(true);
	pmdl->setSortCaseSensitivity(Qt::CaseInsensitive);
	pmdl->setSourceModel(m_ComputerFKModel);

	m_cbxComputer = new QComboBox(m_pgComputer);
	m_cbxComputer->setMinimumWidth(200);
	m_cbxComputer->setModel(pmdl);
	QLabel * lblComputer = new QLabel(tr("Computer"), m_pgComputer);
	lblComputer->setBuddy(m_cbxComputer);

	QVBoxLayout * vbox = new QVBoxLayout();
	vbox->addWidget(lblComputer);
	vbox->addWidget(m_cbxComputer);
	vbox->addStretch();

	QHBoxLayout * hbox = new QHBoxLayout();
	hbox->addLayout(vbox);
	hbox->addStretch();

	m_pgComputer->setLayout(hbox);
}

void DiveEditPanel::createNotesPage()
{
	m_pgNotes = new QWidget(this);
	m_txtNotes = new QTextEdit(m_pgNotes);

	QVBoxLayout * vbox = new QVBoxLayout();
	vbox->addWidget(m_txtNotes);

	m_pgNotes->setLayout(vbox);
}

QString DiveEditPanel::title() const
{
	if (m_txtDiveNumber->text().isEmpty())
		return tr("Dive");
	return QString(tr("Dive %1")).arg(m_txtDiveNumber->text());
}
