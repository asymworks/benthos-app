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

#include <QEvent>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

#include "tanksmixdialog.hpp"

#include "controls/multicolumnlistview.hpp"

#include "mvf/delegates.hpp"
#include "mvf/models/mix_model.hpp"
#include "mvf/models/tank_model.hpp"

#include "util/deletekeyfilter.hpp"

TanksMixDialog::TanksMixDialog(Session::Ptr session, QWidget * parent)
	: m_session(session), m_dsTanks(new DefaultDataSource<Tank>), m_dsMixes(new DefaultDataSource<Mix>)
{
	m_mdlMixes = new MixModel;
	m_mdlTanks = new TankModel;
	((LogbookQueryModel<Mix> *)m_mdlMixes)->bind(m_session);
	((LogbookQueryModel<Mix> *)m_mdlMixes)->resetFromSource(m_dsMixes);

	((LogbookQueryModel<Tank> *)m_mdlTanks)->bind(m_session);
	((LogbookQueryModel<Tank> *)m_mdlTanks)->resetFromSource(m_dsTanks);

	m_dwmMixes = new QDataWidgetMapper(this);
	m_dwmMixes->setItemDelegate(new CustomDelegate(this));
	m_dwmMixes->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

	m_dwmTanks = new QDataWidgetMapper(this);
	m_dwmTanks->setItemDelegate(new CustomDelegate(this));
	m_dwmTanks->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);

	createLayout();

	m_dwmMixes->addMapping(m_txtMixName, 1);
	m_dwmMixes->addMapping(m_txtArPerMil, 2);
	m_dwmMixes->addMapping(m_txtH2PerMil, 3);
	m_dwmMixes->addMapping(m_txtHePerMil, 4);
	m_dwmMixes->addMapping(m_txtO2PerMil, 5);
	m_dwmMixes->addMapping(m_txtN2PerMil, 6);

	m_dwmTanks->addMapping(m_txtTankName, 0);
	m_dwmTanks->addMapping(m_cbxTankType, 1);
	m_dwmTanks->addMapping(m_txtPressure, 2);
	m_dwmTanks->addMapping(m_txtVolume, 3);
	m_dwmTanks->addMapping(m_txtCapacity, 4);

	setWindowTitle(tr("Manage Tanks and Mixes"));
}

TanksMixDialog::~TanksMixDialog()
{
}

void TanksMixDialog::btnNewMixClicked()
{
	IMixFinder::Ptr mf = boost::dynamic_pointer_cast<IMixFinder>(m_session->finder<Mix>());

	int i = 0;
	std::string n;

	do
	{
		char buf[255];
		sprintf(buf, "New Mix #%d", ++i);
		n = buf;
	} while (mf->findByName(n));

	Mix::Ptr mix(new Mix);
	mix->setName(n);

	m_session->add(mix);
	m_session->commit();
}

void TanksMixDialog::btnNewTankClicked()
{
	ITankFinder::Ptr tf = boost::dynamic_pointer_cast<ITankFinder>(m_session->finder<Tank>());

	int i = 0;
	std::string n;

	do
	{
		char buf[255];
		sprintf(buf, "New Tank #%d", ++i);
		n = buf;
	} while (tf->findByName(n));

	Tank::Ptr tank(new Tank);
	tank->setName(n);

	m_session->add(tank);
	m_session->commit();
}

void TanksMixDialog::closeEvent(QCloseEvent * e)
{
	m_session->commit();
	QDialog::closeEvent(e);
}

void TanksMixDialog::createLayout()
{
	createMixPage();
	createTankPage();

	QTabWidget * tw = new QTabWidget(this);
	tw->addTab(m_wTanks, tr("Tanks"));
	tw->addTab(m_wMixes, tr("Mixes"));

	QVBoxLayout * vbox = new QVBoxLayout;
	vbox->addWidget(tw);
	setLayout(vbox);
}

void TanksMixDialog::createMixPage()
{
	m_wMixes = new QWidget;

	m_txtMixName = new QLineEdit(m_wMixes);
	QLabel * lblMixName = new QLabel(tr("Name:"), m_wMixes);
	lblMixName->setBuddy(m_txtMixName);

	m_txtO2PerMil = new QLineEdit(m_wMixes);
	QLabel * lblMixO2 = new QLabel(tr("Oxygen:"), m_wMixes);
	QLabel * lblMixO2Pct = new QLabel(tr("%"), m_wMixes);
	lblMixO2->setBuddy(m_txtO2PerMil);

	m_txtHePerMil = new QLineEdit(m_wMixes);
	QLabel * lblMixHe = new QLabel(tr("Helium:"), m_wMixes);
	QLabel * lblMixHePct = new QLabel(tr("%"), m_wMixes);
	lblMixHe->setBuddy(m_txtHePerMil);

	m_txtN2PerMil = new QLineEdit(m_wMixes);
	m_txtN2PerMil->setReadOnly(true);
	QLabel * lblMixN2 = new QLabel(tr("Nitrogen:"), m_wMixes);
	QLabel * lblMixN2Pct = new QLabel(tr("%"), m_wMixes);
	lblMixN2->setBuddy(m_txtN2PerMil);

	m_txtH2PerMil = new QLineEdit(m_wMixes);
	QLabel * lblMixH2 = new QLabel(tr("Hydrogen:"), m_wMixes);
	QLabel * lblMixH2Pct = new QLabel(tr("%"), m_wMixes);
	lblMixH2->setBuddy(m_txtH2PerMil);

	m_txtArPerMil = new QLineEdit(m_wMixes);
	QLabel * lblMixAr = new QLabel(tr("Argon:"), m_wMixes);
	QLabel * lblMixArPct = new QLabel(tr("%"), m_wMixes);
	lblMixAr->setBuddy(m_txtArPerMil);

	QSortFilterProxyModel * pm = new QSortFilterProxyModel;
	pm->setDynamicSortFilter(true);
	pm->setSortCaseSensitivity(Qt::CaseInsensitive);
	pm->setSourceModel(m_mdlMixes);
	pm->sort(0, Qt::AscendingOrder);

	m_lvMixes = new QListView(m_wMixes);
	m_lvMixes->setAlternatingRowColors(true);
	m_lvMixes->setItemDelegate(new NoFocusDelegate);
	m_lvMixes->setModel(pm);
	m_dwmMixes->setModel(pm);
	connect(m_lvMixes, SIGNAL(clicked(QModelIndex)), m_dwmMixes, SLOT(setCurrentModelIndex(QModelIndex)));

	DeleteKeyFilter * kf = new DeleteKeyFilter(m_wMixes);
	m_lvMixes->installEventFilter(kf);
	connect(kf, SIGNAL(deletePressed()), this, SLOT(deleteMixPressed()));

	m_btnNewMix = new QPushButton(tr("New Mix"), m_wMixes);
	m_btnNewMix->setAutoDefault(false);
	m_btnNewMix->setDefault(false);
	connect(m_btnNewMix, SIGNAL(clicked()), this, SLOT(btnNewMixClicked()));

	QGridLayout * gboxM = new QGridLayout;
	gboxM->setColumnMinimumWidth(0, 70);
	gboxM->addWidget(lblMixName, 0, 0, Qt::AlignRight);
	gboxM->addWidget(m_txtMixName, 0, 1);
	gboxM->setRowMinimumHeight(1, 8);
	gboxM->addWidget(lblMixO2, 2, 0, Qt::AlignRight);
	gboxM->addWidget(m_txtO2PerMil, 2, 1);
	gboxM->addWidget(lblMixO2Pct, 2, 2);
	gboxM->addWidget(lblMixHe, 3, 0, Qt::AlignRight);
	gboxM->addWidget(m_txtHePerMil, 3, 1);
	gboxM->addWidget(lblMixHePct, 3, 2);
	gboxM->addWidget(lblMixN2, 4, 0, Qt::AlignRight);
	gboxM->addWidget(m_txtN2PerMil, 4, 1);
	gboxM->addWidget(lblMixN2Pct, 4, 2);
	gboxM->setRowMinimumHeight(5, 8);
	gboxM->addWidget(lblMixH2, 6, 0, Qt::AlignRight);
	gboxM->addWidget(m_txtH2PerMil, 6, 1);
	gboxM->addWidget(lblMixH2Pct, 6, 2);
	gboxM->addWidget(lblMixAr, 7, 0, Qt::AlignRight);
	gboxM->addWidget(m_txtArPerMil, 7, 1);
	gboxM->addWidget(lblMixArPct, 7, 2);

	QVBoxLayout * vboxM1 = new QVBoxLayout;
	vboxM1->addLayout(gboxM);
	vboxM1->addStretch();

	QHBoxLayout * hboxM1 = new QHBoxLayout;
	hboxM1->addWidget(m_lvMixes);
	hboxM1->addLayout(vboxM1);

	QHBoxLayout * hboxM2 = new QHBoxLayout;
	hboxM2->addWidget(m_btnNewMix);
	hboxM2->addStretch();

	QVBoxLayout * vboxM2 = new QVBoxLayout;
	vboxM2->addLayout(hboxM1, 1);
	vboxM2->addLayout(hboxM2, 0);

	m_wMixes->setLayout(vboxM2);
}

void TanksMixDialog::createTankPage()
{
	m_wTanks = new QWidget;

	m_txtTankName = new QLineEdit(m_wTanks);
	QLabel * lblTankName = new QLabel(tr("Name:"), m_wTanks);
	lblTankName->setBuddy(m_txtTankName);

	m_cbxTankType = new QComboBox(m_wTanks);
	m_cbxTankType->addItem("aluminum");
	m_cbxTankType->addItem("steel");
	QLabel * lblTankType = new QLabel(tr("Type:"), m_wTanks);
	lblTankType->setBuddy(m_cbxTankType);

	unit_t punit = unitForQuantity(qtPressure);
	unit_t vunit = unitForQuantity(qtVolume);

	m_txtPressure = new QuantityEdit(qtPressure, QString(), m_wTanks);
	QLabel * lblPressure = new QLabel(tr("Working Pressure:"), m_wTanks);
	QLabel * lblPressureUnits = new QLabel(QString::fromStdWString(punit.abbr), m_wTanks);
	lblPressure->setBuddy(m_txtPressure);

	m_txtVolume = new QuantityEdit(qtVolume, QString(), m_wTanks);
	QLabel * lblVolume = new QLabel(tr("Water Volume:"), m_wTanks);
	QLabel * lblVolumeUnits = new QLabel(QString::fromStdWString(vunit.abbr), m_wTanks);
	lblVolume->setBuddy(m_txtVolume);

	m_txtCapacity = new QuantityEdit(qtVolume, QString(), m_wTanks);
	QLabel * lblCapacity = new QLabel(tr("Gas Capacity:"), m_wTanks);
	QLabel * lblCapacityUnits = new QLabel(QString::fromStdWString(vunit.abbr), m_wTanks);
	lblCapacity->setBuddy(m_txtCapacity);

	QSortFilterProxyModel * pm = new QSortFilterProxyModel;
	pm->setDynamicSortFilter(true);
	pm->setSortCaseSensitivity(Qt::CaseInsensitive);
	pm->setSourceModel(m_mdlTanks);
	pm->sort(0, Qt::AscendingOrder);

	m_lvTanks = new QListView(m_wTanks);
	m_lvTanks->setAlternatingRowColors(true);
	m_lvTanks->setItemDelegate(new NoFocusDelegate);
	m_lvTanks->setModel(pm);
	m_dwmTanks->setModel(pm);
	connect(m_lvTanks, SIGNAL(clicked(QModelIndex)), m_dwmTanks, SLOT(setCurrentModelIndex(QModelIndex)));

	DeleteKeyFilter * kf = new DeleteKeyFilter(m_wTanks);
	m_lvTanks->installEventFilter(kf);
	connect(kf, SIGNAL(deletePressed()), this, SLOT(deleteTankPressed()));

	m_btnNewTank = new QPushButton(tr("New Tank"), m_wTanks);
	m_btnNewTank->setAutoDefault(false);
	m_btnNewTank->setDefault(false);
	connect(m_btnNewTank, SIGNAL(clicked()), this, SLOT(btnNewTankClicked()));

	QGridLayout * gboxT = new QGridLayout;
	gboxT->setColumnMinimumWidth(0, 70);
	gboxT->addWidget(lblTankName, 0, 0, Qt::AlignRight);
	gboxT->addWidget(m_txtTankName, 0, 1);
	gboxT->addWidget(lblTankType, 1, 0, Qt::AlignRight);
	gboxT->addWidget(m_cbxTankType, 1, 1);
	gboxT->setRowMinimumHeight(2, 8);
	gboxT->addWidget(lblPressure, 3, 0, Qt::AlignRight);
	gboxT->addWidget(m_txtPressure, 3, 1);
	gboxT->addWidget(lblPressureUnits, 3, 2);
	gboxT->addWidget(lblVolume, 4, 0, Qt::AlignRight);
	gboxT->addWidget(m_txtVolume, 4, 1);
	gboxT->addWidget(lblVolumeUnits, 4, 2);
	gboxT->addWidget(lblCapacity, 5, 0, Qt::AlignRight);
	gboxT->addWidget(m_txtCapacity, 5, 1);
	gboxT->addWidget(lblCapacityUnits, 5, 2);

	QVBoxLayout * vboxT1 = new QVBoxLayout;
	vboxT1->addLayout(gboxT);
	vboxT1->addStretch();

	QHBoxLayout * hboxT1 = new QHBoxLayout;
	hboxT1->addWidget(m_lvTanks);
	hboxT1->addLayout(vboxT1);

	QHBoxLayout * hboxT2 = new QHBoxLayout;
	hboxT2->addWidget(m_btnNewTank);
	hboxT2->addStretch();

	QVBoxLayout * vboxT2 = new QVBoxLayout;
	vboxT2->addLayout(hboxT1, 1);
	vboxT2->addLayout(hboxT2, 0);

	m_wTanks->setLayout(vboxT2);
}

void TanksMixDialog::deleteMixPressed()
{
	QModelIndexList items = m_lvMixes->selectionModel()->selectedIndexes();
	if (items.size() != 1)
		return;

	QModelIndex idx = removeProxyModels<MixModel>(items.at(0));
	if (! idx.isValid())
		return;

	Mix::Ptr mix = ((MixModel *)m_mdlMixes)->item(idx);
	if (! mix)
		return;

	if (QMessageBox::question(this, tr("Delete Mix?"), tr("Delete Mix \"%1\"?").arg(QString::fromStdString(mix->toString())),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)
		return;

	m_session->delete_(mix);
	m_session->commit();

	m_lvMixes->clearSelection();
	m_dwmMixes->setCurrentIndex(-1);

	m_txtMixName->clear();
	m_txtArPerMil->clear();
	m_txtH2PerMil->clear();
	m_txtHePerMil->clear();
	m_txtO2PerMil->clear();
	m_txtN2PerMil->clear();
}

void TanksMixDialog::deleteTankPressed()
{
	QModelIndexList items = m_lvTanks->selectionModel()->selectedIndexes();
	if (items.size() != 1)
		return;

	QModelIndex idx = removeProxyModels<TankModel>(items.at(0));
	if (! idx.isValid())
		return;

	Tank::Ptr tank = ((TankModel *)m_mdlTanks)->item(idx);
	if (! tank)
		return;

	QString tname = tank->name() ? tr("Tank \"%1\"").arg(QString::fromStdString(tank->name().get())) : tr("Tank");
	if (QMessageBox::question(this, tr("Delete Tank?"), tr("Delete %1?").arg(tname),
			QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No)
		return;

	m_session->delete_(tank);
	m_session->commit();

	m_lvTanks->clearSelection();
	m_dwmTanks->setCurrentIndex(-1);

	m_txtTankName->clear();
	m_txtPressure->clear();
	m_txtVolume->clear();
	m_txtCapacity->clear();
}

void TanksMixDialog::mapperIndexChanged(int)
{
	m_session->commit();
}

unit_t TanksMixDialog::unitForQuantity(quantity_t q) const
{
	QSettings s;
	s.beginGroup("Settings");
	QVariant uname = s.value(QString("Unit%1").arg(q));
	s.endGroup();

	try
	{
		if (! uname.isValid())
			return findUnit(q, 0);
		else
			return findUnit(q, (const char *)uname.toByteArray());
	}
	catch (std::runtime_error & e)
	{
	}

	return unit_t();
}
