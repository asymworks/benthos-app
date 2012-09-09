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

#include <climits>

#include <QFileDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenuBar>
#include <QSettings>
#include <QSignalMapper>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>

#include "config.hpp"
#include "mainwindow.hpp"

#include "util/qticonloader.hpp"
#include "util/units.hpp"

#include "dialogs/modeladddialog.hpp"
#include "dialogs/modeleditdialog.hpp"

#include "mvf/models/dive_model.hpp"
#include "mvf/views/dive_stackedview.hpp"
#include "mvf/views/dive_editpanel.hpp"

#include "mvf/models/site_model.hpp"
#include "mvf/views/site_stackedview.hpp"
#include "mvf/views/site_editpanel.hpp"

#include "wizards/addcomputerwizard.hpp"

#include <benthos/logbook/dive.hpp>
#include <benthos/logbook/profile.hpp>
using namespace benthos::logbook;

using namespace wizards;

struct dive_compare_dates
{
	bool operator()(Dive::Ptr lhs, Dive::Ptr rhs)
	{
		return (lhs->datetime() < rhs->datetime());
	}
};

MainWindow::MainWindow(QWidget * parent)
	: QMainWindow(parent), m_Logbook(), m_LogbookName("None"), m_LogbookPath()
{
	m_LogbookModel = new LogbookModel(this);

	createActions();
	createMenus();
	createLayout();
	createStatusBar();

	setWindowTitle(tr("Benthos Dive Log - %1").arg("None"));
	setMinimumSize(500, 300);

	readSettings();
}

MainWindow::~MainWindow()
{
}

void MainWindow::actCloseLogbookTriggered()
{
	closeLogbook();
}

void MainWindow::actConfigUnitsTriggered()
{
}

void MainWindow::actDeleteItemsTriggered()
{
	StackedView * sv = dynamic_cast<StackedView *>(m_viewStack->currentWidget());
	if (sv)
		sv->deleteSelection(true);
}

void MainWindow::actMergeDivesTriggered()
{
	StackedView * sv = dynamic_cast<StackedView *>(m_viewStack->currentWidget());
	if (sv != m_svDives)
		return;

	// Retrieve List of Dives
	std::vector<Dive::Ptr> dives;
	std::vector<Dive::Ptr>::iterator it;
	QModelIndexList items = m_svDives->selectionModel()->selectedRows(0);
	if (items.count() < 2)
		return;

	for (int i = items.count() - 1; i > -1; --i)
	{
		QModelIndex idx(items.at(i));
		QAbstractItemModel * m = (QAbstractItemModel *)idx.model();
		QSortFilterProxyModel * p = dynamic_cast<QSortFilterProxyModel *>(m);
		while (p != NULL)
		{
			idx = p->mapToSource(idx);
			m = p->sourceModel();
			p = dynamic_cast<QSortFilterProxyModel *>(m);
		}

		LogbookQueryModel<Dive> * lqm = dynamic_cast<LogbookQueryModel<Dive> *>(m);
		if (! lqm)
			continue;

		dives.push_back(lqm->item(idx));
	}

	// Confirm the Merge
	if (QMessageBox::question(this, tr("Confirm Merge"), tr("Merge %1 dives?  This action cannot be undone.").arg(dives.size()), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
		return;

	Dive::Ptr newDive;
	std::vector<Profile::Ptr> plist;
	std::vector<Profile::Ptr>::iterator pit;

	try
	{
		std::vector<Profile::Ptr> profiles = DiveModel::Merge(dives, newDive);
		plist.assign(profiles.begin(), profiles.end());
	}
	catch (std::runtime_error & e)
	{
		QMessageBox mb(QMessageBox::Critical, tr("Merge Failed"), QString());
		mb.setText(tr("Failed to merge dives.  Your logbook was not modified."));
		mb.setDetailedText(QString::fromStdString(e.what()));
		mb.setStandardButtons(QMessageBox::Ok);
		mb.setDefaultButton(QMessageBox::Ok);
		mb.exec();

		return;
	}

	for (pit = plist.begin(); pit != plist.end(); pit++)
		m_Logbook->session()->add(* pit);
	for (it = dives.begin(); it != dives.end(); it++)
		m_Logbook->session()->delete_(* it);
	m_Logbook->session()->add(newDive);
	m_Logbook->session()->commit();
}

void MainWindow::actNewComputerTriggered()
{
	DiveComputer::Ptr dc(AddComputerWizard::RunWizard());
	if (! dc)
		return;

	IDiveComputerFinder::Ptr dcf = boost::dynamic_pointer_cast<IDiveComputerFinder>(m_Logbook->session()->finder<DiveComputer>());
	if (dcf->findBySerial(dc->driver(), dc->serial()))
	{
		QMessageBox::warning(this, tr("Existing Computer"), tr("The dive computer already exists in the Logbook.  Benthos does not support adding the same computer more than once."));
		return;
	}

	m_Logbook->session()->add(dc);
	m_Logbook->session()->commit();

	//FIXME: NAVTREE, I ADD COMPUTER. Y U NO UPDATE?
}

void MainWindow::actNewDiveTriggered()
{
	DiveModel mdl;
	Dive::Ptr dv(new Dive);

	// Setup the Temporary Model
	std::vector<Dive::Ptr> v;
	v.push_back(dv);
	mdl.resetFromList(v);

	// Show the Editor
	IModelEditPanel * pnl = new DiveEditPanel();
	ModelAddDialog d(& mdl, pnl, m_Logbook);
	d.exec();

	if (d.result() == QDialog::Accepted)
	{
		//FIXME: This belongs within ModelAddDialog so if something fails
		// the dialog doesn't close and allows the user to fix it.
		try
		{
			d.submit();

			m_Logbook->session()->add(dv);
			m_Logbook->session()->commit();

			updateView();
		}
		catch (std::exception & e)
		{
			fprintf(stderr, "%s\n", e.what());
		}
	}
}

void MainWindow::actNewDiveSiteTriggered()
{
	DiveSiteModel mdl;
	DiveSite::Ptr ds(new DiveSite);

	// Setup the Temporary Model
	std::vector<DiveSite::Ptr> v;
	v.push_back(ds);
	mdl.resetFromList(v);

	// Show the Editor
	IModelEditPanel * pnl = new SiteEditPanel();
	ModelAddDialog d(& mdl, pnl, m_Logbook);
	d.exec();

	if (d.result() == QDialog::Accepted)
	{
		try
		{
			d.submit();

			m_Logbook->session()->add(ds);
			m_Logbook->session()->commit();

			updateView();
		}
		catch (std::exception & e)
		{
			fprintf(stderr, "%s\n", e.what());
		}
	}
}

void MainWindow::actNewLogbookTriggered()
{
	QString dir;
	if (m_Logbook != NULL)
		dir = QFileInfo(m_LogbookPath).absolutePath();
	else
		dir = QDir::homePath();

	QString fn = QFileDialog::getSaveFileName(this, tr("Save new Logbook file as..."), dir, tr("Logbook Files (*.lbk);;All Files (*.*)"));

	if (fn.isEmpty())
		return;

	QFileInfo fi(fn);

	// Delete the File if it exists
	if (fi.exists())
	{
		// Note: Qt handles the "Do you want to overwrite?" question internally
		if (! QDir::root().remove(fn))
		{
			QMessageBox::critical(this, tr("Cannot remove Logbook"),
				tr("Unable to remove Logbook file \"%1\". Please delete this file manually.").arg(fi.fileName()),
				QMessageBox::Ok, QMessageBox::Ok);
			return;
		}
	}

	// Create the Logbook
	createLogbook(fn);
}

void MainWindow::actOpenLogbookTriggered()
{
	QString dir;
	if (m_Logbook != NULL)
		dir = QFileInfo(m_LogbookPath).absolutePath();
	else
		dir = QDir::homePath();

	QString fn = QFileDialog::getOpenFileName(this, tr("Select a Logbook file..."), dir, tr("Logbook Files (*.lbk);;All Files (*.*)"));

	if (fn.isEmpty())
		return;

	QFileInfo fi(fn);

	if (! fi.exists())
	{
		if (QMessageBox::question(this, tr("Create new Logbook?"),
				tr("Logbook \"%1\" does not exist.  Would you like to create it?").arg(fi.fileName()),
				QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) != QMessageBox::Yes)
			return;

		createLogbook(fn);
	}
	else
	{
		openLogbook(fn);
	}
}

void MainWindow::actRenumberTriggered()
{
	bool ok = false;
	StackedView * sv = dynamic_cast<StackedView *>(m_viewStack->currentWidget());
	if (sv != m_svDives)
		return;

	// Retrieve List of Dives
	std::vector<Dive::Ptr> dives;
	QModelIndexList items = m_svDives->selectionModel()->selectedRows(0);
	if (items.count() < 2)
	{
		// Renumber all dives
		if (QMessageBox::question(this, tr("Confirm Renumber"), tr("Renumber all dives?"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
			return;

		for (int i = 0; i < m_svDives->model()->rowCount(); ++i)
		{
			QModelIndex idx(m_svDives->model()->index(i, 0, QModelIndex()));
			QAbstractItemModel * m = (QAbstractItemModel *)idx.model();
			QSortFilterProxyModel * p = dynamic_cast<QSortFilterProxyModel *>(m);
			while (p != NULL)
			{
				idx = p->mapToSource(idx);
				m = p->sourceModel();
				p = dynamic_cast<QSortFilterProxyModel *>(m);
			}

			LogbookQueryModel<Dive> * lqm = dynamic_cast<LogbookQueryModel<Dive> *>(m);
			if (! lqm)
				continue;

			dives.push_back(lqm->item(idx));
		}
	}
	else
	{
		// Renumber Selected dives
		for (int i = items.count() - 1; i > -1; --i)
		{
			QModelIndex idx(items.at(i));
			QAbstractItemModel * m = (QAbstractItemModel *)idx.model();
			QSortFilterProxyModel * p = dynamic_cast<QSortFilterProxyModel *>(m);
			while (p != NULL)
			{
				idx = p->mapToSource(idx);
				m = p->sourceModel();
				p = dynamic_cast<QSortFilterProxyModel *>(m);
			}

			LogbookQueryModel<Dive> * lqm = dynamic_cast<LogbookQueryModel<Dive> *>(m);
			if (! lqm)
				continue;

			dives.push_back(lqm->item(idx));
		}
	}

	// Get the Starting Number
	int snum = QInputDialog::getInt(this, tr("Enter the starting number"), tr("Dives will be renumbered starting at:"), 1, 0, INT_MAX, 1, & ok);
	if (! ok)
		return;

	// Renumber Dives
	std::sort(dives.begin(), dives.end(), dive_compare_dates());
	std::vector<Dive::Ptr>::iterator it;
	int cur = snum;
	for (it = dives.begin(); it != dives.end(); it++)
	{
		(* it)->setNumber(cur++);
		m_Logbook->session()->add(* it);
	}

	m_Logbook->session()->commit();
}

void MainWindow::actSetImperialTriggered()
{
	//TODO: Make less hacky?  Store as member variables and use writeSettings()?

	QSettings s;
	s.beginGroup("Settings");
	s.setValue(QString("Unit%1").arg(qtDepth), "Feet");
	s.setValue(QString("Unit%1").arg(qtTemperature), "Farenheit");
	s.setValue(QString("Unit%1").arg(qtPressure), "PSI");
	s.setValue(QString("Unit%1").arg(qtWeight), "Pounds");
	s.endGroup();

	update();
}

void MainWindow::actSetMetricTriggered()
{
	//TODO: Make less hacky?  Store as member variables and use writeSettings()?

	QSettings s;
	s.beginGroup("Settings");
	s.setValue(QString("Unit%1").arg(qtDepth), "Meters");
	s.setValue(QString("Unit%1").arg(qtTemperature), "Celsius");
	s.setValue(QString("Unit%1").arg(qtPressure), "Bar");
	s.setValue(QString("Unit%1").arg(qtWeight), "Kilograms");
	s.endGroup();

	update();
}

void MainWindow::actViewDetailsTriggered()
{
	StackedView * sv = dynamic_cast<StackedView *>(m_viewStack->currentWidget());
	if (sv)
		sv->showEditor(sv->currentModelIndex());
}

void MainWindow::closeEvent(QCloseEvent * e)
{
	writeSettings();
	e->accept();
}

void MainWindow::closeLogbook()
{
	if (! m_Logbook)
		return;

	m_Logbook.reset();
	m_LogbookName = "None";
	m_LogbookPath = "";

	writeSettings();
	setWindowTitle(tr("Benthos Dive Log - %1").arg(m_LogbookName));
}

void MainWindow::createActions()
{
	/*
	 * File Menu Actions
	 */
	m_actNewLogbook = new QAction(QtIconLoader::icon("document-new", QPixmap()), tr("&New Logbook..."), this);
	m_actNewLogbook->setShortcut(QKeySequence::New);
	m_actNewLogbook->setStatusTip(tr("Create a new Logbook file"));
	connect(m_actNewLogbook, SIGNAL(triggered()), this, SLOT(actNewLogbookTriggered()));

	m_actOpenLogbook = new QAction(QtIconLoader::icon("document-open", QPixmap()), tr("&Open Logbook..."), this);
	m_actOpenLogbook->setShortcut(QKeySequence::Open);
	m_actOpenLogbook->setStatusTip(tr("Open an existing Logbook file"));
	connect(m_actOpenLogbook, SIGNAL(triggered()), this, SLOT(actOpenLogbookTriggered()));

	m_actCloseLogbook = new QAction(tr("&Close Logbook..."), this);
	m_actCloseLogbook->setStatusTip(tr("Close the current Logbook file"));
	connect(m_actCloseLogbook, SIGNAL(triggered()), this, SLOT(actCloseLogbookTriggered()));

	m_actExit = new QAction(QtIconLoader::icon("application-exit", QPixmap()), tr("E&xit"), this);
	m_actExit->setShortcut(QKeySequence::Quit);
	m_actExit->setStatusTip(tr("Exit the Benthos Application"));
	connect(m_actExit, SIGNAL(triggered()), this, SLOT(close()));

	m_actNewComputer = new QAction(tr("New Dive &Computer..."), this);
	m_actNewComputer->setStatusTip(tr("Add a new Dive Computer"));
	connect(m_actNewComputer, SIGNAL(triggered()), this, SLOT(actNewComputerTriggered()));

	m_actNewDive = new QAction(tr("New &Dive..."), this);
	m_actNewDive->setStatusTip(tr("Manually add a new Dive log entry"));
	connect(m_actNewDive, SIGNAL(triggered()), this, SLOT(actNewDiveTriggered()));

	m_actNewDiveSite = new QAction(tr("New Dive &Site..."), this);
	m_actNewDiveSite->setStatusTip(tr("Manually add a new Dive Site"));
	connect(m_actNewDiveSite, SIGNAL(triggered()), this, SLOT(actNewDiveSiteTriggered()));

	m_actDeleteItems = new QAction(tr("Delete &Item"), this);
	m_actDeleteItems->setShortcut(QKeySequence::Delete);
	m_actDeleteItems->setStatusTip(tr("Delete the currently-selected item"));
	connect(m_actDeleteItems, SIGNAL(triggered()), this, SLOT(actDeleteItemsTriggered()));

	m_actMergeDives = new QAction(tr("&Merge Dives..."), this);
	m_actMergeDives->setStatusTip(tr("Merge two dive profiles into one"));
	connect(m_actMergeDives, SIGNAL(triggered()), this, SLOT(actMergeDivesTriggered()));

	m_actRenumber = new QAction(tr("&Renumber Dives..."), this);
	m_actRenumber->setStatusTip(tr("Renumber the selected dives"));
	connect(m_actRenumber, SIGNAL(triggered()), this, SLOT(actRenumberTriggered()));

	/*
	 * Toolbar Actions
	 */
	m_actViewCF = new QAction(QPixmap(":/icons/cf-view.png"), tr("View as Cover Flow"), this);
	m_actViewCF->setCheckable(true);

	m_actViewTiled = new QAction(QPixmap(":/icons/tile-view.png"), tr("View as Tiles"), this);
	m_actViewTiled->setCheckable(true);

	m_actViewList = new QAction(QPixmap(":/icons/list-view.png"), tr("View as List"), this);
	m_actViewList->setCheckable(true);

	m_actViewStats = new QAction(QPixmap(":/icons/stat-view.png"), tr("View Statistics"), this);
	m_actViewStats->setCheckable(true);

	m_actViewMap = new QAction(QPixmap(":/icons/map-view.png"), tr("View Map"), this);
	m_actViewMap->setCheckable(true);

	m_actViewConfig = new QAction(QPixmap(":/icons/config.png"), tr("Configure View"), this);
	m_actViewDetails = new QAction(QPixmap(":/icons/detail.png"), tr("Details"), this);

	QActionGroup * ag = new QActionGroup(this);
	ag->addAction(m_actViewCF);
	ag->addAction(m_actViewTiled);
	ag->addAction(m_actViewList);
	ag->addAction(m_actViewStats);
	ag->addAction(m_actViewMap);

	QSignalMapper * sm = new QSignalMapper(this);
	sm->setMapping(m_actViewCF, StackedView::CFViewMode);
	sm->setMapping(m_actViewTiled, StackedView::TileViewMode);
	sm->setMapping(m_actViewList, StackedView::ListViewMode);
	sm->setMapping(m_actViewStats, StackedView::StatsViewMode);
	sm->setMapping(m_actViewMap, StackedView::MapViewMode);

	connect(m_actViewCF, SIGNAL(triggered()), sm, SLOT(map()));
	connect(m_actViewTiled, SIGNAL(triggered()), sm, SLOT(map()));
	connect(m_actViewList, SIGNAL(triggered()), sm, SLOT(map()));
	connect(m_actViewStats, SIGNAL(triggered()), sm, SLOT(map()));
	connect(m_actViewMap, SIGNAL(triggered()), sm, SLOT(map()));

	connect(sm, SIGNAL(mapped(int)), this, SLOT(viewModeChanged(int)));
	connect(m_actViewDetails, SIGNAL(triggered()), this, SLOT(actViewDetailsTriggered()));

	/*
	 * Units Menu Actions
	 */
	m_actSetMetric = new QAction(tr("&Metric"), this);
	m_actSetMetric->setStatusTip(tr("Change display units to all Metric"));
	connect(m_actSetMetric, SIGNAL(triggered()), this, SLOT(actSetMetricTriggered()));

	m_actSetImperial = new QAction(tr("&Imperial"), this);
	m_actSetImperial->setStatusTip(tr("Change display units to all Imperial"));
	connect(m_actSetImperial, SIGNAL(triggered()), this, SLOT(actSetImperialTriggered()));

	m_actConfigUnits = new QAction(tr("&Customize Units..."), this);
	m_actConfigUnits->setStatusTip(tr("Customize display units"));
	connect(m_actConfigUnits, SIGNAL(triggered()), this, SLOT(actConfigUnitsTriggered()));
}

void MainWindow::createLayout()
{
	m_navTree = new NavTree(this);
	m_navTree->setModel(m_LogbookModel);
	m_navTree->expandAll();
	m_navTree->selectionModel()->clear();

	connect(
		m_navTree->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)),
		this, SLOT(navTreeSelectionChanged(const QModelIndex &, const QModelIndex &))
	);

	QToolButton * tbtn_viewConfig = new QToolButton;
	tbtn_viewConfig->setDefaultAction(m_actViewConfig);
	tbtn_viewConfig->setFocusPolicy(Qt::NoFocus);
	tbtn_viewConfig->setPopupMode(QToolButton::InstantPopup);

	QToolBar * tbViewMode = new QToolBar;
	tbViewMode->addAction(m_actViewCF);
	tbViewMode->addAction(m_actViewTiled);
	tbViewMode->addAction(m_actViewList);
	tbViewMode->addAction(m_actViewStats);
	tbViewMode->addAction(m_actViewMap);
	tbViewMode->addSeparator();
	tbViewMode->addWidget(tbtn_viewConfig);
	tbViewMode->addSeparator();
	tbViewMode->addAction(m_actViewDetails);

	m_txtFilter = new SearchEdit;
	connect(m_txtFilter, SIGNAL(textChanged(const QString &)), this, SLOT(txtFilterChanged(const QString &)));

	QHBoxLayout * hbox = new QHBoxLayout;
	hbox->addWidget(tbViewMode);
	hbox->addStretch(1);
	hbox->addWidget(m_txtFilter);
	hbox->addSpacing(8);

	m_svDives = new DiveStack(this);
	m_svSites = new DiveSiteStack(this);
	m_dvComputer = new ComputerView(this);
	m_blankWidget = new QWidget(this);

	connect(m_svDives, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
		this, SLOT(viewCurrentChanged(const QModelIndex &, const QModelIndex &)));
	connect(m_svDives, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
		this, SLOT(viewSelectionChanged(const QItemSelection &, const QItemSelection &)));

	connect(m_svSites, SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
		this, SLOT(viewCurrentChanged(const QModelIndex &, const QModelIndex &)));
	connect(m_svSites, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
		this, SLOT(viewSelectionChanged(const QItemSelection &, const QItemSelection &)));

	m_viewStack = new QStackedWidget(this);
	m_viewStack->addWidget(m_svDives);
	m_viewStack->addWidget(m_svSites);
	m_viewStack->addWidget(m_dvComputer);
	m_viewStack->addWidget(m_blankWidget);

	QGridLayout * gbox = new QGridLayout;
	gbox->setContentsMargins(0, 0, 0, 0);
	gbox->setSpacing(0);
	gbox->addLayout(hbox, 0, 1);
	gbox->addWidget(m_navTree, 1, 0);
	gbox->addWidget(m_viewStack, 1, 1);
	gbox->setColumnStretch(1, 1);
	gbox->setRowStretch(1, 1);

	QWidget * w = new QWidget(this);
	w->setLayout(gbox);
	setCentralWidget(w);
}

void MainWindow::createLogbook(const QString & filename)
{
	// Create a new Logbook
	m_Logbook = Logbook::Create(filename.toStdString(), BENTHOS_DB_CREATOR, BENTHOS_DB_VERSION);

	QFileInfo fi(filename);
	if (! fi.exists())
	{
		QMessageBox::warning(this, tr("Missing Logbook File"),
			tr("Unable to open Logbook file \"%1\": file not found").arg(fi.fileName()),
			QMessageBox::Ok, QMessageBox::Ok);
		return;
	}

	// Initialize the Window
	m_LogbookName = fi.fileName();
	m_LogbookPath = filename;
	updateLogbook();
}

void MainWindow::createMenus()
{
	m_fileMenu = menuBar()->addMenu(tr("&File"));
	m_fileMenu->addAction(m_actNewLogbook);
	m_fileMenu->addAction(m_actOpenLogbook);
	m_fileMenu->addAction(m_actCloseLogbook);
	m_fileMenu->addSeparator();
	m_fileMenu->addAction(m_actExit);

	m_logbookMenu = menuBar()->addMenu(tr("&Logbook"));
	m_logbookMenu->addAction(m_actNewDive);
	m_logbookMenu->addAction(m_actNewDiveSite);
	m_logbookMenu->addAction(m_actNewComputer);
	m_logbookMenu->addSeparator();
	m_logbookMenu->addAction(m_actDeleteItems);
	m_logbookMenu->addSeparator();
	m_logbookMenu->addAction(m_actMergeDives);
	m_logbookMenu->addAction(m_actRenumber);

	m_unitMenu = new QMenu(tr("Display &Units"));
	m_unitMenu->addAction(m_actSetMetric);
	m_unitMenu->addAction(m_actSetImperial);
	m_unitMenu->addSeparator();
	m_unitMenu->addAction(m_actConfigUnits);

	QMenu * viewMenu = new QMenu;
	viewMenu->addMenu(m_unitMenu);
	//viewMenu->addSeparator();
	//viewMenu->addAction(m_actConfigView);

	m_actViewConfig->setMenu(viewMenu);
}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

void MainWindow::navTreeSelectionChanged(const QModelIndex & selected, const QModelIndex & deselected)
{
	LogbookModelItem::Ptr desel_item = m_LogbookModel->item(deselected);
	LogbookModelItem::Ptr sel_item = m_LogbookModel->item(selected);

	if (! sel_item)
		return;

	m_txtFilter->clear();

	/*
	 * Load the new View
	 */
	switch (sel_item->type())
	{
	case LogbookModelItem::DiveListItem:
	{
		boost::shared_ptr<DataSourceItem<Dive> > dsi = boost::dynamic_pointer_cast<DataSourceItem<Dive> >(sel_item);
		if (! dsi)
		{
			m_viewStack->setCurrentWidget(m_blankWidget);
			break;
		}

		LogbookQueryModel<Dive> * mdl = dynamic_cast<LogbookQueryModel<Dive> *>(m_svDives->model());
		mdl->bind(m_Logbook->session());
		mdl->resetFromSource(dsi->source());
		m_svDives->clearSelection();
		m_viewStack->setCurrentWidget(m_svDives);
		statusBar()->showMessage(tr("Showing %1").arg(m_svDives->summary()));

		break;
	}

	case LogbookModelItem::SiteListItem:
	{
		boost::shared_ptr<DataSourceItem<DiveSite> > dsi = boost::dynamic_pointer_cast<DataSourceItem<DiveSite> >(sel_item);
		if (! dsi)
			throw std::runtime_error("Navigation Item with type SiteListItem must inherit from DataSourceItem<DiveSite>");

		LogbookQueryModel<DiveSite> * mdl = dynamic_cast<LogbookQueryModel<DiveSite> *>(m_svSites->model());
		mdl->bind(m_Logbook->session());
		mdl->resetFromSource(dsi->source());
		m_svSites->clearSelection();
		m_viewStack->setCurrentWidget(m_svSites);
		statusBar()->showMessage(tr("Showing %1").arg(m_svSites->summary()));

		break;
	}

	case LogbookModelItem::DeviceItem:
	{
		boost::shared_ptr<ItemSourceItem<DiveComputer> > isi = boost::dynamic_pointer_cast<ItemSourceItem<DiveComputer> >(sel_item);
		if (! isi)
			throw std::runtime_error("Navigation Item with type DeviceItem must inherit from ItemSourceItem<DiveComputer>");

		DiveComputer::Ptr dc = isi->getItem();

		if (! dc)
			throw std::runtime_error("Item is NULL in ItemSourceItem");

		m_viewStack->setCurrentWidget(m_dvComputer);
		m_dvComputer->setComputer(dc);
		statusBar()->clearMessage();
		break;
	}

	default:
		m_viewStack->setCurrentWidget(m_blankWidget);
		statusBar()->clearMessage();
		break;
	}

	updateControls();
	writeSettings();
}

void MainWindow::openLogbook(const QString & filename)
{
	QFileInfo fi(filename);
	if (! fi.exists())
	{
		QMessageBox::warning(this, tr("Missing Logbook File"),
			tr("Unable to open Logbook file \"%\": file not found").arg(fi.fileName()),
			QMessageBox::Ok, QMessageBox::Ok);
		return;
	}

	// Create a new Logbook
	m_Logbook = Logbook::Open(filename.toStdString());

	// Initialize the Window
	m_LogbookName = fi.fileName();
	m_LogbookPath = filename;
	updateLogbook();
}

void MainWindow::readSettings()
{
	QSettings settings;
	settings.beginGroup("MainWindow");
	QVariant pos = settings.value("pos");
	QVariant size = settings.value("size");
	QVariant max = settings.value("max");
	QVariant file = settings.value("file");
	QVariant view = settings.value("view");
	settings.endGroup();

	if (size.isValid())
		resize(size.toSize());
	if (pos.isValid())
		move(pos.toPoint());

	if (max.isValid() && max.toBool())
		showMaximized();

	if (file.isValid() && ! file.toString().isEmpty())
		openLogbook(file.toString());

	QModelIndex idx = m_LogbookModel->defaultIndex();
	if (view.isValid())
	{
		QStringList sl(view.toString().split(","));
		if (sl.length() == 3)
		{
			bool rok, cok, iok;
			int r, c;
			quint32 i;

			r = sl.at(0).toInt(& rok);
			c = sl.at(1).toInt(& cok);
			i = sl.at(2).toInt(& iok);

			if (rok && cok && iok)
			{
				idx = m_LogbookModel->makeIndex(r, c, i);
			}
		}
	}

	m_navTree->selectionModel()->setCurrentIndex(idx, QItemSelectionModel::SelectCurrent);
}

void MainWindow::txtFilterChanged(const QString & value)
{
	StackedView * sv = dynamic_cast<StackedView *>(m_viewStack->currentWidget());
	if (sv)
		sv->setFilterString(value);
}

void MainWindow::updateControls()
{
	StackedView * sv = dynamic_cast<StackedView *>(m_viewStack->currentWidget());

	//TODO: Fix lots of unsafe pointer code (sv could be 0, sv->selectionModel() could be 0, etc)

	/*
	 * Dive List Commands
	 */
	if (sv && (sv == m_svDives))
	{
		m_actMergeDives->setEnabled(sv->selectionModel()->selectedRows(0).count() > 1);
		m_actRenumber->setEnabled(true);
	}
	else
	{
		m_actMergeDives->setEnabled(false);
		m_actRenumber->setEnabled(false);
	}

	/*
	 * Delete Item Command
	 */
	if (sv)
		m_actDeleteItems->setEnabled(sv->selectionModel()->selectedRows(0).count() > 0);
	else
		m_actDeleteItems->setEnabled(false);

	/*
	 * View Mode Commands
	 */
	if (sv)
	{
		m_actViewCF->setVisible(sv->hasViewMode(StackedView::CFViewMode));
		m_actViewTiled->setVisible(! sv->hasViewMode(StackedView::CFViewMode));

		m_actViewMap->setVisible(sv->hasViewMode(StackedView::MapViewMode));
		m_actViewStats->setVisible(! sv->hasViewMode(StackedView::MapViewMode));

		m_actViewCF->setEnabled(sv->hasViewMode(StackedView::CFViewMode));
		m_actViewCF->setChecked(sv->view_mode() == StackedView::CFViewMode);

		m_actViewTiled->setEnabled(sv->hasViewMode(StackedView::TileViewMode));
		m_actViewTiled->setChecked(sv->view_mode() == StackedView::TileViewMode);

		m_actViewList->setEnabled(sv->hasViewMode(StackedView::ListViewMode));
		m_actViewList->setChecked(sv->view_mode() == StackedView::ListViewMode);

		m_actViewStats->setEnabled(sv->hasViewMode(StackedView::StatsViewMode));
		m_actViewStats->setChecked(sv->view_mode() == StackedView::StatsViewMode);

		m_actViewMap->setEnabled(sv->hasViewMode(StackedView::MapViewMode));
		m_actViewMap->setChecked(sv->view_mode() == StackedView::MapViewMode);

		m_actViewDetails->setEnabled(true);
	}
	else
	{
		m_actViewCF->setVisible(false);
		m_actViewMap->setVisible(false);

		m_actViewTiled->setVisible(true);
		m_actViewStats->setVisible(true);

		m_actViewCF->setEnabled(false);
		m_actViewCF->setChecked(false);

		m_actViewTiled->setEnabled(false);
		m_actViewTiled->setChecked(false);

		m_actViewList->setEnabled(false);
		m_actViewList->setChecked(false);

		m_actViewStats->setEnabled(false);
		m_actViewStats->setChecked(false);

		m_actViewMap->setEnabled(false);
		m_actViewMap->setChecked(false);

		m_actViewDetails->setEnabled(false);

		m_actDeleteItems->setEnabled(false);
		m_actMergeDives->setEnabled(false);
		m_actRenumber->setEnabled(false);
	}
}

void MainWindow::updateLogbook()
{
	m_LogbookModel->setLogbook(m_Logbook);
	m_svDives->bind(m_Logbook);
	m_svSites->bind(m_Logbook);

	m_navTree->expandAll();

	if (m_Logbook)
		m_navTree->selectionModel()->setCurrentIndex(
			m_LogbookModel->index(0, 0),
			QItemSelectionModel::SelectCurrent);
	else
		m_navTree->selectionModel()->clear();

	updateControls();
	writeSettings();
	setWindowTitle(tr("Benthos Dive Log - %1").arg(m_LogbookName));
}

void MainWindow::updateView()
{
	//FIXME: Does Nothing
	m_navTree->selectionModel()->setCurrentIndex(m_navTree->currentIndex(),
		QItemSelectionModel::SelectCurrent);
}

void MainWindow::viewCurrentChanged(const QModelIndex & current, const QModelIndex &)
{
	updateControls();
}

void MainWindow::viewSelectionChanged(const QItemSelection &, const QItemSelection &)
{
	updateControls();

	StackedView * sv = dynamic_cast<StackedView *>(m_viewStack->currentWidget());
	QItemSelectionModel * sm = sv ? sv->selectionModel() : 0;

	if (! sv || ! sm)
		statusBar()->clearMessage();

	if (sm->selectedRows(0).count() > 1)
		statusBar()->showMessage(tr("Selected %1").arg(sv->summary()));
	else
		statusBar()->showMessage(tr("Showing %1").arg(sv->summary()));
}

void MainWindow::writeSettings()
{
	QSettings settings;
	settings.beginGroup("MainWindow");
	settings.setValue("pos", QVariant(pos()));
	settings.setValue("size", QVariant(size()));
	settings.setValue("max", QVariant(isMaximized()));
	settings.setValue("file", QVariant(m_LogbookPath));

	QModelIndex i = m_navTree->currentIndex();
	settings.setValue("view", QVariant(QString("%1,%2,%3").arg(i.row()).arg(i.column()).arg(i.internalId())));

	settings.endGroup();
}

void MainWindow::viewModeChanged(int vm)
{
	StackedView * sv = dynamic_cast<StackedView *>(m_viewStack->currentWidget());
	if (sv)
		sv->setViewMode((StackedView::ViewMode)vm);
}
