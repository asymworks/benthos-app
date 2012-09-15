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

#ifndef TANKSMIXDIALOG_HPP_
#define TANKSMIXDIALOG_HPP_

/**
 * @file src/dialogs/tanksmixdialog.hpp
 * @brief Manage Tanks and Mixes Dialog Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <string>

#include <QAbstractItemModel>
#include <QComboBox>
#include <QDataWidgetMapper>
#include <QDialog>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>

#include <controls/quantityedit.hpp>
#include <mvf/models.hpp>
#include <util/units.hpp>

#include <benthos/logbook/logbook.hpp>
#include <benthos/logbook/mix.hpp>
#include <benthos/logbook/session.hpp>
#include <benthos/logbook/tank.hpp>
using namespace benthos::logbook;

/**
 * @brief Manage Tanks and Mixes Dialog
 *
 * Allows the user to graphically edit tanks and gas mixes in the logbook.
 */
class TanksMixDialog: public QDialog
{
	Q_OBJECT

public:

	/**
	 * @brief Class Constructor
	 * @param[in] Logbook
	 * @param[in] Parent Widget
	 */
	TanksMixDialog(Logbook::Ptr logbook, QWidget * parent = 0);

	//! Class Destructor
	virtual ~TanksMixDialog();

public slots:
	void deleteMixPressed();
	void deleteTankPressed();

protected:
	void closeEvent(QCloseEvent * e);
	void createMixPage();
	void createTankPage();
	void createLayout();
	unit_t unitForQuantity(quantity_t) const;

protected slots:
	void btnNewMixClicked();
	void btnNewTankClicked();
	void mapperIndexChanged(int);

private:
	Session::Ptr				m_session;
	ILogbookDataSource<Tank> *	m_dsTanks;
	ILogbookDataSource<Mix> *	m_dsMixes;

	QAbstractItemModel *		m_mdlTanks;
	QAbstractItemModel *		m_mdlMixes;
	QDataWidgetMapper *			m_dwmTanks;
	QDataWidgetMapper *			m_dwmMixes;

	QWidget *					m_wTanks;
	QListView *					m_lvTanks;
	QLineEdit *					m_txtTankName;
	QComboBox *					m_cbxTankType;
	QuantityEdit *				m_txtPressure;
	QuantityEdit *				m_txtVolume;
	QuantityEdit *				m_txtCapacity;
	QPushButton *				m_btnNewTank;

	QWidget *					m_wMixes;
	QListView *					m_lvMixes;
	QLineEdit *					m_txtMixName;
	QLineEdit *					m_txtArPerMil;
	QLineEdit *					m_txtH2PerMil;
	QLineEdit *					m_txtHePerMil;
	QLineEdit *					m_txtO2PerMil;
	QLineEdit *					m_txtN2PerMil;
	QPushButton *				m_btnNewMix;

};

#endif /* TANKSMIXDIALOG_HPP_ */
