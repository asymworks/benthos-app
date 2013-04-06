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

#ifndef SITE_EDITPANEL_HPP_
#define SITE_EDITPANEL_HPP_

/**
 * @file src/mvf/site_editpanel.hpp
 * @brief Dive Site Edit Panel Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QAbstractItemDelegate>
#include <QComboBox>
#include <QCompleter>
#include <QDataWidgetMapper>
#include <QLineEdit>
#include <QString>
#include <QTabWidget>
#include <QTextEdit>
#include <QWidget>

#include "controls/maplocationeditor.hpp"
#include "controls/quantityedit.hpp"

#include "mvf/delegates.hpp"
#include "mvf/modeleditpanel.hpp"

/*
 * FIX for broken Qt4 moc and BOOST_JOIN error
 */
#ifdef Q_MOC_RUN
#define BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#endif

#include <benthos/logbook/session.hpp>
using namespace benthos::logbook;

/**
 * @brief Dive Site Editor Panel
 */
class SiteEditPanel: public QTabWidget, public IModelEditPanel
{
	Q_OBJECT

public:

	//! Class Constructor
	SiteEditPanel(QWidget * parent = 0);

	//! Class Destructor
	virtual ~SiteEditPanel();

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
	void createSitePage();
	void createMorePage();
	void createNotesPage();

protected slots:
	void pgMapPositionChanged(const QPointF & position);

private:
	QDataWidgetMapper *	m_mapper;

	QWidget *			m_pgSite;
	QWidget *			m_pgMore;
	QWidget *			m_pgNotes;
	MapLocationEditor *	m_pgMap;

	QLineEdit *			m_txtSite;
	QLineEdit *			m_txtPlace;
	QComboBox *			m_cbxCountry;
	QComboBox *			m_cbxSalinity;

	QLineEdit *			m_txtPlatform;
	QLineEdit *			m_txtWater;
	QLineEdit *			m_txtBottom;
	QuantityEdit *		m_txtAltitude;

	QTextEdit *			m_txtNotes;

	QCompleter *		m_cmpPlatform;
	QCompleter * 		m_cmpWater;
	QCompleter *		m_cmpBottom;

};

#endif /* SITE_EDITPANEL_HPP_ */
