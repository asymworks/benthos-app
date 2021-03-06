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

#include <algorithm>
#include <ctime>
#include <string>

#include <QDateTime>
#include <QHBoxLayout>
#include <QSettings>
#include <QThreadPool>
#include <QVBoxLayout>

#include <boost/filesystem.hpp>

#include <benthos/divecomputer/driverclass.hpp>
#include <benthos/divecomputer/registry.hpp>

#include "dialogs/driverparamsdialog.hpp"
#include "dialogs/transferdialog.hpp"
#include "workers/transferworker.hpp"

#include "computer_view.hpp"
#include "config.hpp"

using namespace benthos::dc;

ComputerView::ComputerView(QWidget * parent)
	: QFrame(parent), m_dc()
{
	setObjectName("ComputerView");
	setFrameStyle(QFrame::Panel | QFrame::Sunken);
	setStyleSheet("ComputerView { background-color: white }");

	createLayout();
}

ComputerView::~ComputerView()
{
}

void ComputerView::btnConnectionClicked()
{
	if (! m_dc)
		return;

	// Find the Driver Class
	PluginRegistry::Ptr reg = PluginRegistry::Instance();
	DriverClass::Ptr dclass;
	try
	{
		dclass = reg->loadDriver(m_dc->driver());
	}
	catch (std::exception & e)
	{
		throw std::runtime_error(QString("Failed to load driver '%1': %2")
			.arg(QString::fromStdString(m_dc->driver()))
			.arg(QString::fromStdString(e.what())).toStdString());
		return;
	}

	// Show the Dialog
	DriverParamsDialog * dialog = new DriverParamsDialog(dclass, m_dc->driver_args() ? m_dc->driver_args().get() : "");
	if (dialog->exec() == QDialog::Accepted)
	{
		m_dc->setDriverArgs(dialog->param_string());
		m_dc->session()->add(m_dc);
		m_dc->session()->commit();
	}
}

void ComputerView::btnTransferClicked()
{
	if (! m_dc)
		return;

	bool checkSN = m_chkCheckSN->isChecked();
	bool updateToken = m_chkUpdateToken->isChecked();

	// Create the Transfer Worker
	QThreadPool * tp = QThreadPool::globalInstance();
	TransferWorker * worker = new TransferWorker(m_dc, m_dc->session(), checkSN, updateToken);

	// Create the Progress Dialog
	TransferDialog * dialog = new TransferDialog(this);

	// Connect Signals/Slots
	connect(worker, SIGNAL(finished()), dialog, SLOT(xfrFinished()), Qt::QueuedConnection);
	connect(worker, SIGNAL(parsedDive(Profile::Ptr)), dialog, SLOT(xfrDive(Profile::Ptr)), Qt::QueuedConnection);
	connect(worker, SIGNAL(progress(unsigned long)), dialog, SLOT(xfrProgress(unsigned long)), Qt::QueuedConnection);
	connect(worker, SIGNAL(started(unsigned long)), dialog, SLOT(xfrStarted(unsigned long)), Qt::QueuedConnection);
	connect(worker, SIGNAL(status(const QString &)), dialog, SLOT(xfrStatus(const QString &)), Qt::QueuedConnection);
	connect(worker, SIGNAL(transferError(const QString &)), dialog, SLOT(xfrError(const QString &)), Qt::QueuedConnection);

	connect(dialog, SIGNAL(cancelled()), worker, SLOT(cancel()), Qt::QueuedConnection);

	// Run the Transfer
	tp->start(worker);
	if (dialog->exec() == QDialog::Accepted)
	{
		//FIXME: Be less dumb when merging dives
		std::vector<Profile::Ptr> dives = dialog->dives();
		std::vector<Profile::Ptr>::iterator it;
		for (it = dives.begin(); it != dives.end(); it++)
			m_dc->session()->add(* it);

		m_dc->setLastTransfer(time(NULL));
		m_dc->session()->add(m_dc);
		m_dc->session()->commit();

		setComputer(m_dc);
	}
}

DiveComputer::Ptr ComputerView::computer() const
{
	return m_dc;
}

QFrame * ComputerView::createInfoLayout()
{
	// Basic Information Frame
	QLabel * lblName = new QLabel(tr("Device Name: "));
	QLabel * lblSerNo = new QLabel(tr("Serial Number: "));
	QLabel * lblManuf = new QLabel(tr("Manufacturer: "));
	QLabel * lblModel = new QLabel(tr("Model Name: "));
	QLabel * lblDives = new QLabel(tr("Dives: "));
	QLabel * lblLastXfr = new QLabel(tr("Last Transfer: "));

	lblName->setStyleSheet("QLabel { font-weight: bold }");
	lblSerNo->setStyleSheet("QLabel { font-weight: bold }");
	lblManuf->setStyleSheet("QLabel { font-weight: bold }");
	lblModel->setStyleSheet("QLabel { font-weight: bold }");
	lblDives->setStyleSheet("QLabel { font-weight: bold }");
	lblLastXfr->setStyleSheet("QLabel { font-weight: bold }");

	m_lblName = new QLabel;
	m_lblSerial = new QLabel;
	m_lblManf = new QLabel;
	m_lblModel = new QLabel;
	m_lblDives = new QLabel;
	m_lblLastXfr = new QLabel;

	// Dive Computer Image
	m_lblImage = new QLabel;
	m_lblImage->setAlignment(Qt::AlignCenter);
	m_lblImage->setMinimumHeight(160);
	m_lblImage->setMinimumWidth(100);
	m_lblImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	m_lblImage->setPixmap(QPixmap(":/graphics/dc-unknown.png"));

	// Label/Value Grid Box
	QFrame * hline = new QFrame;
	hline->setFrameStyle(QFrame::HLine | QFrame::Sunken);

	QGridLayout * gbox = new QGridLayout;
	gbox->addWidget(lblName, 0, 0, Qt::AlignRight);
	gbox->addWidget(lblSerNo, 1, 0, Qt::AlignRight);
	gbox->addWidget(lblManuf, 2, 0, Qt::AlignRight);
	gbox->addWidget(lblModel, 3, 0, Qt::AlignRight);
	gbox->addWidget(m_lblName, 0, 1);
	gbox->addWidget(m_lblSerial, 1, 1);
	gbox->addWidget(m_lblManf, 2, 1);
	gbox->addWidget(m_lblModel, 3, 1);

	gbox->addWidget(hline, 4, 0, 1, 2);

	gbox->addWidget(lblDives, 5, 0, Qt::AlignRight);
	gbox->addWidget(lblLastXfr, 6, 0, Qt::AlignRight);
	gbox->addWidget(m_lblDives, 5, 1);
	gbox->addWidget(m_lblLastXfr, 6, 1);

	gbox->setColumnMinimumWidth(0, 150);
	gbox->setColumnMinimumWidth(1, 150);

	// Transfer and Settings Buttons
	m_btnTransfer = new QPushButton(tr("Transfer Dives"));
	m_btnSettings = new QPushButton(tr("Computer Settings"));
	m_btnSettings->setEnabled(false);

	connect(m_btnTransfer, SIGNAL(clicked()), this, SLOT(btnTransferClicked()));

	// Transfer Label
	QLabel * lblTransfer = new QLabel(tr("Use the 'Transfer Dives' button to transfer dives from your dive computer into Benthos."));
	lblTransfer->setMinimumWidth(350);
	lblTransfer->setMaximumWidth(350);
	lblTransfer->setWordWrap(true);

	// Button Layout
	QVBoxLayout * vbox1 = new QVBoxLayout;
	vbox1->addWidget(m_btnTransfer);
	vbox1->addWidget(m_btnSettings);

	// Bottom Frame Layout
	QHBoxLayout * hbox1 = new QHBoxLayout;
	hbox1->addWidget(lblTransfer);
	hbox1->addSpacing(16);
	hbox1->addLayout(vbox1);

	// Top Frame Layout
	QFrame * vline = new QFrame;
	vline->setFrameStyle(QFrame::VLine | QFrame::Sunken);

	QHBoxLayout * hbox2 = new QHBoxLayout;
	hbox2->addWidget(m_lblImage);
	hbox2->addWidget(vline);
	hbox2->addLayout(gbox);

	// Main Frame Layout
	QVBoxLayout * vbox2 = new QVBoxLayout;
	vbox2->addLayout(hbox2);
	vbox2->addSpacing(8);
	vbox2->addLayout(hbox1);

	// Create the Frame
	QFrame * fBasicInfo = new QFrame;
	fBasicInfo->setAutoFillBackground(true);
	fBasicInfo->setBackgroundRole(QPalette::Background);
	fBasicInfo->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	fBasicInfo->setLayout(vbox2);

	return fBasicInfo;
}

QFrame * ComputerView::createOptionsLayout()
{
	// Options Fields
	QLabel * lblDriver = new QLabel(tr("Driver Name: "));
	QLabel * lblDevice = new QLabel(tr("Device Path: "));
	lblDriver->setStyleSheet("QLabel { font-weight: bold }");
	lblDevice->setStyleSheet("QLabel { font-weight: bold }");

	m_lblDriver = new QLabel;
	m_lblDevice = new QLabel;

	m_chkCheckSN = new QCheckBox(tr("Verify Serial Number before Transfer"));
	m_chkUpdateToken = new QCheckBox(tr("Only Download New Dives"));

	connect(m_chkCheckSN, SIGNAL(clicked()), this, SLOT(updateSettings()));
	connect(m_chkUpdateToken, SIGNAL(clicked()), this, SLOT(updateSettings()));

	// Label/Value Grid Box
	QGridLayout * gbox = new QGridLayout;
	gbox->addWidget(lblDriver, 0, 0, Qt::AlignRight);
	gbox->addWidget(lblDevice, 1, 0, Qt::AlignRight);
	gbox->addWidget(m_lblDriver, 0, 1);
	gbox->addWidget(m_lblDevice, 1, 1);

	gbox->setColumnMinimumWidth(0, 150);
	gbox->setColumnMinimumWidth(1, 150);

	// Label/Value Layout
	QVBoxLayout * vbox1 = new QVBoxLayout;
	vbox1->addLayout(gbox);

	// Connection Settings Button
	m_btnConnection = new QPushButton(tr("Configure Connection"));

	connect(m_btnConnection, SIGNAL(clicked()), this, SLOT(btnConnectionClicked()));

	// Button Layout
	QVBoxLayout * vbox2 = new QVBoxLayout;
	vbox2->addWidget(m_btnConnection);
	vbox2->setAlignment(Qt::AlignTop);

	// Top Frame Layout
	QHBoxLayout * hbox = new QHBoxLayout;
	hbox->addLayout(vbox1);
	hbox->addStretch(1);
	hbox->addLayout(vbox2);

	// Configuration Label
	QLabel * lblConfigText = new QLabel(tr("Use the 'Configure Connection' button to change the way Benthos connects to this dive computer"));
	lblConfigText->setMaximumWidth(550);
	lblConfigText->setWordWrap(true);

	// Main Frame Layout
	QVBoxLayout * vbox3 = new QVBoxLayout;
	vbox3->addLayout(hbox);
	vbox3->addSpacing(8);
	vbox3->addWidget(lblConfigText);
	vbox3->addSpacing(8);
	vbox3->addWidget(m_chkCheckSN);
	vbox3->addWidget(m_chkUpdateToken);

	// Create the Frame
	QFrame * fOptions = new QFrame;
	fOptions->setAutoFillBackground(true);
	fOptions->setBackgroundRole(QPalette::Background);
	fOptions->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	fOptions->setLayout(vbox3);

	return fOptions;
}

void ComputerView::createLayout()
{
	QLabel * lblDiveComputer = new QLabel(tr("Dive Computer"));
	lblDiveComputer->setStyleSheet(QString("QLabel { color: dimgray; font-size: %1pt; font-weight: bold; font-variant: small-caps }").arg(font().pointSize() * 4 / 3));

	QLabel * lblOptions = new QLabel(tr("Options"));
	lblOptions->setStyleSheet(QString("QLabel { color: dimgray; font-size: %1pt; font-weight: bold; font-variant: small-caps }").arg(font().pointSize() * 4 / 3));

	QFrame * fBasicInfo = createInfoLayout();
	QFrame * fOptions = createOptionsLayout();

	/*
	QLabel * lblHWVer = new QLabel(tr("Hardware Version: "));
	QLabel * lblSWVer = new QLabel(tr("Software Version: "));
	lblHWVer->setStyleSheet("QLabel { font-weight: bold }");
	lblSWVer->setStyleSheet("QLabel { font-weight: bold }");
	m_lblHWVersion = new QLabel;
	m_lblSWVersion = new QLabel;
	*/

	QVBoxLayout * vbox = new QVBoxLayout;
	vbox->addSpacing(16);
	vbox->addWidget(lblDiveComputer);
	vbox->addWidget(fBasicInfo);
	vbox->addSpacing(16);
	vbox->addWidget(lblOptions);
	vbox->addWidget(fOptions);
	vbox->addStretch();

	QHBoxLayout * hbox = new QHBoxLayout;
	hbox->addStretch(1);
	hbox->addLayout(vbox);
	hbox->addStretch(1);

	setLayout(hbox);
}

std::string ComputerView::imagePath(DiveComputer::Ptr dc)
{
	boost::filesystem::path bPath(BENTHOS_DATADIR);
	bPath /= "graphics";
	bPath /= "dc";

	std::string mfg(dc->manufacturer() ? dc->manufacturer().get() : "");
	std::string mdl(dc->model() ? dc->model().get() : "");

	// If Manufacturer or Model is missing, use default
	if (mfg.empty() || mdl.empty())
		return ":/graphics/dc-unknown.png";

	// Construct File Name
	std::replace(mfg.begin(), mfg.end(), ' ', '_');
	std::replace(mdl.begin(), mdl.end(), ' ', '_');
	bPath /= (mfg + "_" + mdl + ".png");

	if (boost::filesystem::exists(bPath))
		return bPath.native();
	return ":/graphics/dc-unknown.png";
}

void ComputerView::setComputer(DiveComputer::Ptr value)
{
	QSettings s;
	s.beginGroup("Settings");
	QVariant fmt = s.value(QString("DTFormat"), QString("MM/dd/yy hh:mm AP"));
	s.endGroup();

	m_dc = value;

	if (! m_dc)
		return;

	s.beginGroup(QString("DiveComputer-%1").arg(m_dc->id()));
	QVariant checksn = s.value("checksn", Qt::Checked);
	QVariant update = s.value("update", Qt::Checked);
	s.endGroup();

	m_lblImage->setPixmap(QPixmap(imagePath(m_dc).c_str()));
	m_chkCheckSN->setCheckState((Qt::CheckState)checksn.toInt());
	m_chkUpdateToken->setCheckState((Qt::CheckState)update.toInt());

	if (! m_dc->name())
		m_lblName->clear();
	else
		m_lblName->setText(QString::fromStdString(m_dc->name().get()));

	m_lblSerial->setText(QString::fromStdString(m_dc->serial()));
	m_lblDives->setText(QString("%1 dives").arg(m_dc->dives()->count()));

	if (! m_dc->last_transfer())
		m_lblLastXfr->setText(tr("Never"));
	else
		m_lblLastXfr->setText(QDateTime::fromTime_t(m_dc->last_transfer().get()).toString(fmt.toString()));

	m_lblDriver->setText(QString::fromStdString(m_dc->driver()));

	if (! m_dc->device())
		m_lblDevice->setText(tr("<auto>"));
	else
		m_lblDevice->setText(QString::fromStdString(m_dc->device().get()));

	if (! m_dc->manufacturer())
		m_lblManf->clear();
	else
		m_lblManf->setText(QString::fromStdString(m_dc->manufacturer().get()));

	if (! m_dc->model())
		m_lblModel->clear();
	else
		m_lblModel->setText(QString::fromStdString(m_dc->model().get()));

	/*
	if (! m_dc->hw_version())
		m_lblHWVersion->clear();
	else
		m_lblHWVersion->setText(QString::fromStdString(m_dc->hw_version().get()));

	if (! m_dc->sw_version())
		m_lblSWVersion->clear();
	else
		m_lblSWVersion->setText(QString::fromStdString(m_dc->sw_version().get()));
	*/
}

void ComputerView::updateSettings()
{
	if (! m_dc)
		return;

	QSettings s;
	s.beginGroup(QString("DiveComputer-%1").arg(m_dc->id()));
	s.setValue("checksn", m_chkCheckSN->checkState());
	s.setValue("update", m_chkUpdateToken->checkState());
	s.endGroup();
}

