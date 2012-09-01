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

#ifndef DIVE_EDITPANEL_HPP_
#define DIVE_EDITPANEL_HPP_

/**
 * @file src/mvf/dive_editpanel.hpp
 * @brief Dive Edit Panel Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QAbstractItemDelegate>
#include <QCheckBox>
#include <QComboBox>
#include <QDataWidgetMapper>
#include <QDateTimeEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QTextEdit>
#include <QWidget>

#include "controls/ratingedit.hpp"
#include "controls/quantityedit.hpp"

#include "mvf/delegates.hpp"
#include "mvf/modeleditpanel.hpp"

#include <benthos/logbook/session.hpp>
using namespace benthos::logbook;

/**
 * @brief Dive Editor Panel
 */
class DiveEditPanel: public QTabWidget, public IModelEditPanel
{
	Q_OBJECT

public:

	//! Class Constructor
	DiveEditPanel(QWidget * parent = 0);

	//! Class Destructor
	virtual ~DiveEditPanel();

	/**
	 * @brief Bind the Editor to a Data Widget Mapper
	 * @param [in] Data Widget Mapper
	 */
	virtual void bind(Session::Ptr session, QDataWidgetMapper * mapper);

	/**
	 * @brief Create a Delegate for the Editor Panel
	 * @param [in] Delegate Parent
	 * @return Delegate Instance
	 */
	virtual QAbstractItemDelegate * createDelegate(QObject * parent = 0) const;

	/**
	 * @brief Return the Title of the current Item
	 * @return Title
	 */
	virtual QString title() const;

protected:
	void createComputerPage();
	void createDivePage();
	void createExtraPage();
	void createNotesPage();
	void createTablePage();

protected slots:
	void btnNewSiteClicked();

private:
	QDataWidgetMapper *	m_mapper;
	Session::Ptr		m_session;

	QWidget *			m_pgDive;
	QWidget *			m_pgExtra;
	QWidget *			m_pgTable;
	QWidget *			m_pgComputer;
	QWidget * 			m_pgNotes;

	RatingEdit *		m_reRating;
	QLineEdit *			m_txtDiveNumber;
	QDateTimeEdit *		m_txtDateTime;
	QLineEdit *			m_txtInterval;
	QLineEdit *			m_txtRuntime;
	QLineEdit *			m_txtRepetition;

	QComboBox *			m_cbxSite;
	QLineEdit *			m_txtSiteName;
	QLineEdit *			m_txtSitePlace;
	QComboBox *			m_cbxSiteCountry;
	QPushButton *		m_btnNewSite;

	QuantityEdit *		m_txtMaxDepth;
	QuantityEdit *		m_txtAvgDepth;
	QuantityEdit *		m_txtMaxTemp;
	QuantityEdit *		m_txtMinTemp;
	QuantityEdit *		m_txtAirTemp;
	QuantityEdit *		m_txtAirStart;
	QuantityEdit *		m_txtAirEnd;
	QComboBox *			m_cbxMix;

	QCheckBox *			m_chkStop;
	QuantityEdit *		m_txtStopDepth;
	QLineEdit *			m_txtStopTime;
	QComboBox *			m_cbxVizCategory;
	QuantityEdit *		m_txtVizDistance;
	QuantityEdit *		m_txtWeight;

	QComboBox *			m_cbxAlgorithm;
	QLineEdit *			m_txtStartPG;
	QLineEdit *			m_txtEndPG;
	QLineEdit *			m_txtRNT;
	QLineEdit *			m_txtDesat;
	QLineEdit *			m_txtNoFly;

	QComboBox * 		m_cbxComputer;

	QTextEdit *			m_txtNotes;

	QAbstractItemModel *	m_SiteFKModel;
	QAbstractItemModel *	m_ComputerFKModel;
	QAbstractItemModel *	m_MixFKModel;

};

#endif /* DIVE_EDITPANEL_HPP_ */
