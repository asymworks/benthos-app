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

#ifndef MODELEDITDIALOG_HPP_
#define MODELEDITDIALOG_HPP_

/**
 * @file src/dialogs/modeleditdialog.hpp
 * @brief Edit Model Dialog Classes
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QAbstractItemModel>
#include <QDataWidgetMapper>
#include <QDialog>
#include <QEvent>
#include <QModelIndex>
#include <QPushButton>

/*
 * FIX for broken Qt4 moc and BOOST_JOIN error
 */
#ifdef Q_MOC_RUN
#define BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
#endif

#include <benthos/logbook/logbook.hpp>
using namespace benthos::logbook;

#include "mvf/modeleditpanel.hpp"

/**
 * @brief Class for a Edit Model Dialog
 *
 * Wrapper class which contains a Edit Panel and a QAbstractItemModel which
 * allows for each item in the model to be modified using the Edit Panel.
 */
class ModelEditDialog: public QDialog
{
	Q_OBJECT

public:

	//! Class Constructor
	ModelEditDialog(QAbstractItemModel * model, IModelEditPanel * panel,
		Logbook::Ptr logbook, const QModelIndex & index = QModelIndex(),
		QWidget * parent = 0);

	//! Class Destructor
	virtual ~ModelEditDialog();

protected:
	virtual void closeEvent(QCloseEvent * event);

protected slots:
	void mapperIndexChanged(int index);

protected:
	QDataWidgetMapper *		m_mapper;
	QAbstractItemModel * 	m_model;
	IModelEditPanel *		m_panel;
	Logbook::Ptr			m_logbook;

	QPushButton *			m_btnPrev;
	QPushButton *			m_btnNext;

};

#endif /* MODELEDITDIALOG_HPP_ */
