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

#ifndef MAINWINDOW_HPP_
#define MAINWINDOW_HPP_

/**
 * @file src/mainwindow.hpp
 * @brief Main Dive Log Window Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QAction>
#include <QCloseEvent>
#include <QMainWindow>
#include <QModelIndex>
#include <QMenu>
#include <QString>

#include <benthos/logbook/logbook.hpp>
using namespace benthos::logbook;

#include "controls/navtree.hpp"
#include "controls/searchedit.hpp"
#include "controls/stackedview.hpp"

#include "mvf/models/logbook_item.hpp"
#include "mvf/models/logbook_model.hpp"

#include "mvf/views/computer_view.hpp"

/**
 * @brief MainWindow Class
 *
 * Main Window for the Benthos application.  All user interaction is performed
 * through this window.
 */
class MainWindow: public QMainWindow
{
	Q_OBJECT

public:

	//! Class Constructor
	MainWindow(QWidget * parent = 0);

	//! Class Destructor
	virtual ~MainWindow();

private:

	//! Override closeEvent
	void closeEvent(QCloseEvent * e);

	//! Close the Logbook File
	void closeLogbook();

	//! Create the Main Window Actions
	void createActions();

	//! Create the Main Window Layout
	void createLayout();

	//! Create a Logbook File
	void createLogbook(const QString & filename);

	//! Create the Main Window Menus
	void createMenus();

	//! Create the Main Window Status Bar
	void createStatusBar();

	//! Open a Logbook File
	void openLogbook(const QString & filename);

	//! Read Settings
	void readSettings();

	//! Update the Window's Controls
	void updateControls();

	//! Update the Window's Logbook View
	void updateLogbook();

	//! Update the current Model View
	void updateView();

	//! Write Settings
	void writeSettings();

private:
	Logbook::Ptr			m_Logbook;
	QString					m_LogbookName;
	QString					m_LogbookPath;

	LogbookModel * 			m_LogbookModel;

private slots:
	void actNewLogbookTriggered();
	void actOpenLogbookTriggered();
	void actCloseLogbookTriggered();

	void actSetMetricTriggered();
	void actSetImperialTriggered();
	void actConfigUnitsTriggered();

	void actViewDetailsTriggered();

	void actNewComputerTriggered();
	void actNewDiveTriggered();
	void actNewDiveSiteTriggered();
	void actDeleteItemsTriggered();
	void actMergeDivesTriggered();
	void actRenumberTriggered();
	void actManageTanksMixesTriggered();

	void navTreeSelectionChanged(const QModelIndex &, const QModelIndex &);
	void txtFilterChanged(const QString &);
	void viewModeChanged(int);

	void viewCurrentChanged(const QModelIndex &, const QModelIndex &);
	void viewSelectionChanged(const QItemSelection &, const QItemSelection &);

private:
	QAction *				m_actNewLogbook;
	QAction *				m_actOpenLogbook;
	QAction *				m_actCloseLogbook;
	QAction *				m_actExit;

	QAction *				m_actNewComputer;
	QAction *				m_actNewDive;
	QAction *				m_actNewDiveSite;
	QAction *				m_actDeleteItems;
	QAction *				m_actMergeDives;
	QAction *				m_actRenumber;
	QAction *				m_actManageTanksMixes;

	QAction *				m_actViewCF;
	QAction *				m_actViewTiled;
	QAction *				m_actViewList;
	QAction *				m_actViewStats;
	QAction *				m_actViewMap;

	QAction *				m_actViewConfig;
	QAction *				m_actViewDetails;

	QAction *				m_actSetMetric;
	QAction *				m_actSetImperial;
	QAction *				m_actConfigUnits;

	QMenu *					m_fileMenu;
	QMenu *					m_logbookMenu;
	QMenu *					m_unitMenu;

	NavTree *				m_navTree;
	QStackedWidget *		m_viewStack;

	StackedView *			m_svDives;
	StackedView *			m_svSites;
	ComputerView *			m_dvComputer;
	QWidget *				m_blankWidget;

	SearchEdit *			m_txtFilter;

};

#endif /* MAINWINDOW_HPP_ */
