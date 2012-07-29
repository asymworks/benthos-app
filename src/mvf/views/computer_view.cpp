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

#include <ctime>

#include <QDateTime>
#include <QHBoxLayout>
#include <QSettings>
#include <QThreadPool>
#include <QVBoxLayout>

#include "dialogs/transferdialog.hpp"
#include "workers/transferworker.hpp"

#include "computer_view.hpp"

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

void ComputerView::btnTransferClicked()
{
	if (! m_dc)
		return;

	// Create the Transfer Worker
	QThreadPool * tp = QThreadPool::globalInstance();
	TransferWorker * worker = new TransferWorker(m_dc, m_dc->session(), true, false);

	// Create the Progress Dialog
	TransferDialog * dialog = new TransferDialog(this);

	// Connect Signals/Slots
	connect(worker, SIGNAL(finished()), dialog, SLOT(xfrFinished()), Qt::QueuedConnection);
	connect(worker, SIGNAL(progress(unsigned long)), dialog, SLOT(xfrProgress(unsigned long)), Qt::QueuedConnection);
	connect(worker, SIGNAL(started(unsigned long)), dialog, SLOT(xfrStarted(unsigned long)), Qt::QueuedConnection);
	connect(worker, SIGNAL(status(const QString &)), dialog, SLOT(xfrStatus(const QString &)), Qt::QueuedConnection);
	connect(worker, SIGNAL(transferError(const QString &)), dialog, SLOT(xfrError(const QString &)), Qt::QueuedConnection);

	connect(dialog, SIGNAL(cancelled()), worker, SLOT(cancel()), Qt::QueuedConnection);

	// Run the Transfer
	tp->start(worker);
	if (dialog->exec() == QDialog::Accepted)
	{
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
	hbox2->setContentsMargins(16, 8, 16, 8);
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

void ComputerView::setComputer(DiveComputer::Ptr value)
{
	QSettings s;
	s.beginGroup("Settings");
	QVariant fmt = s.value(QString("DTFormat"), QString("MM/dd/yy hh:mm AP"));
	s.endGroup();

	m_dc = value;

	if (! m_dc->name())
		m_lblName->clear();
	else
		m_lblName->setText(QString::fromStdString(m_dc->name().get()));

	m_lblSerial->setText(QString::fromStdString(m_dc->serial()));
	m_lblDives->setText(QString("%1 dives").arg(m_dc->dives()->count()));

	if (! m_dc->last_transfer())
		m_lblLastXfr->clear();
	else
		m_lblLastXfr->setText(QDateTime::fromTime_t(m_dc->last_transfer().get()).toString(fmt.toString()));

	m_lblDriver->setText(QString::fromStdString(m_dc->driver()));

	if (! m_dc->device())
		m_lblDevice->clear();
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

