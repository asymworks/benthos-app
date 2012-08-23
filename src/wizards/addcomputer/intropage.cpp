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

#include <stdexcept>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

#include <dialogs/driverparamsdialog.hpp>

#include <mvf/models/drivermodels_model.hpp>
#include <mvf/models/sys/udevserialportmodel.hpp>

#include <benthos/divecomputer/arglist.hpp>
#include <benthos/divecomputer/driver.hpp>

#include "intropage.hpp"
#include "pages.hpp"

using namespace wizards::addcomputer;

IntroPage::IntroPage(QWidget * parent)
	: QWizardPage(parent)
{
	m_registry = PluginRegistry::Instance();

	createLayout();

	setTitle(tr("Add a Dive Computer"));
	setSubTitle(tr("Select the type of Dive Computer to add.  To configure driver parameters use the 'Configure Connection' button."));

	registerField("driver", m_cbxType);
	registerField("device", m_cbxDevice);

	// Hidden Fields
	registerField("params", m_txtParams);
	registerField("serial", m_txtSerial);
	registerField("manuf", m_txtManuf);
	registerField("model", m_txtModel);
}

IntroPage::~IntroPage()
{
}

void IntroPage::btnParamsClicked()
{
	std::string params = paramString();
	DriverClass::Ptr dc = driverClass();
	DriverParamsDialog * dialog = new DriverParamsDialog(dc, params);
	if (dialog->exec() == QDialog::Accepted)
	{
		m_params = dialog->param_string(dc->model_parameter());
		if (! dc->model_parameter().empty())
		{
			ArgumentList al(dialog->param_string());
			boost::optional<int> mdl_id = al.read_integer(dc->model_parameter());

			if (mdl_id)
				m_cbxModel->setCurrentIndex(m_cbxModel->findData(mdl_id.get()));
		}
	}
}

void IntroPage::createLayout()
{
	QSortFilterProxyModel * mdl = new QSortFilterProxyModel;
	mdl->setDynamicSortFilter(true);
	mdl->setSourceModel(new UDevSerialPortModel);
	mdl->setSortRole(Qt::DisplayRole);
	mdl->setSortCaseSensitivity(Qt::CaseInsensitive);
	mdl->sort(0, Qt::AscendingOrder);

	m_cbxType = new WizardComboBox();
	QLabel * lblType = new QLabel(tr("Dive Computer &Driver:"));
	lblType->setBuddy(m_cbxType);

	m_cbxModel = new WizardComboBox();
	m_lblModel = new QLabel(tr("Dive Computer &Model:"));
	m_lblModel->setBuddy(m_cbxModel);

	m_cbxDevice = new WizardComboBox();
	m_cbxDevice->setModel(mdl);
	m_lblDevice = new QLabel(tr("&Serial Port:"));
	m_lblDevice->setBuddy(m_cbxDevice);

	m_txtParams = new QLineEdit(this);
	m_txtParams->setHidden(true);

	m_txtSerial = new QLineEdit(this);
	m_txtSerial->setHidden(true);

	m_txtManuf = new QLineEdit(this);
	m_txtManuf->setHidden(true);

	m_txtModel = new QLineEdit(this);
	m_txtModel->setHidden(true);

	QPushButton * btnParams = new QPushButton(tr("Configure Connection"));
	btnParams->setToolTip(tr("Edit Driver Arguments"));
	connect(btnParams, SIGNAL(clicked()), this, SLOT(btnParamsClicked()));

	std::list<plugin_manifest_t>::const_iterator it;
	for (it = m_registry->plugins().begin(); it != m_registry->plugins().end(); it++)
	{
		std::list<driver_manifest_t>::const_iterator itd;
		for (itd = it->plugin_drivers.begin(); itd != it->plugin_drivers.end(); itd++)
			m_cbxType->addItem(QString::fromStdString(itd->driver_desc), QVariant(QString::fromStdString(itd->driver_name)));
	}

	updateModels(0);
	connect(m_cbxType, SIGNAL(currentIndexChanged(int)), this, SLOT(updateModels(int)));

	QGridLayout * gbox = new QGridLayout();
	gbox->addWidget(lblType, 0, 0);
	gbox->addWidget(m_cbxType, 0, 1);
	gbox->addWidget(m_lblModel, 1, 0);
	gbox->addWidget(m_cbxModel, 1, 1);
	gbox->addWidget(m_lblDevice, 2, 0);
	gbox->addWidget(m_cbxDevice, 2, 1);
	gbox->setColumnStretch(1, 1);

	QHBoxLayout * hbox = new QHBoxLayout();
	hbox->addStretch();
	hbox->addWidget(btnParams);

	QVBoxLayout * vbox = new QVBoxLayout();
	vbox->addLayout(gbox);
	vbox->addStretch();
	vbox->addLayout(hbox);

	setLayout(vbox);
}

DriverClass::Ptr IntroPage::driverClass() const
{
	QString driver_name = m_cbxType->currentItemData().toString();
	if (driver_name.isEmpty())
		return DriverClass::Ptr();

	PluginRegistry::Ptr reg = PluginRegistry::Instance();
	try
	{
		return reg->loadDriver(driver_name.toStdString());
	}
	catch (std::exception & e)
	{
		throw std::runtime_error(QString("Failed to load driver '%1': %2")
			.arg(driver_name)
			.arg(QString::fromStdString(e.what())).toStdString());
	}
}

int IntroPage::nextId() const
{
	return pgConfig;
}

std::string IntroPage::paramString() const
{
	DriverClass::Ptr dc = driverClass();
	if (dc->model_parameter().empty())
		return m_params;

	std::string mp = dc->model_parameter() + "=";
	QVariant mdl = m_cbxModel->currentItemData();
	if (mdl.isValid() && ! mdl.isNull())
		mp = mp + mdl.toString().toStdString();

	return m_params.empty() ? mp : m_params + ":" + mp;
}

void IntroPage::updateModels(int index)
{
	m_cbxModel->clear();
	if ((index < 0) || (index >= m_cbxType->count()))
		return;

	QString driver_name = m_cbxType->itemData(index, Qt::UserRole).toString();
	if (driver_name.isEmpty() || driver_name.isNull())
		return;

	const driver_manifest_t * dm = m_registry->findDriver(driver_name.toStdString());
	if (! dm)
	{
		m_cbxModel->setEnabled(false);
		m_cbxDevice->setEnabled(false);

		m_lblModel->setEnabled(false);
		m_lblDevice->setEnabled(false);

		return;
	}

	if (dm->model_param.empty())
	{
		m_lblModel->setEnabled(false);
		m_cbxModel->setEnabled(false);
		m_cbxModel->addItem("Auto-Detect", QVariant(-1));
	}
	else
	{
		m_lblModel->setEnabled(true);
		m_cbxModel->setEnabled(true);
		m_cbxModel->setModel(new DriverModelsModel(driverClass()));
	}

	if (dm->driver_intf != "serial")
	{
		m_lblDevice->setEnabled(false);
		m_cbxDevice->setCurrentIndex(-1);
		m_cbxDevice->setEnabled(false);
	}
	else
	{
		m_lblDevice->setEnabled(true);
		m_cbxDevice->setEnabled(true);
	}
}

bool IntroPage::validatePage()
{
	m_txtSerial->clear();
	m_txtManuf->clear();
	m_txtModel->clear();

	/*
	 * Test the connection parameters by opening a connection to the Dive
	 * Computer using the settings on this page.
	 */
	QString driver_name = m_cbxType->currentItemData().toString();
	QString device_path = m_cbxDevice->currentItemData().toString();
	std::string params = paramString();

	DriverClass::Ptr dclass;
	Driver::Ptr dev;

	// Load the Driver Class
	try
	{
		dclass = driverClass();
	}
	catch (std::exception & e)
	{
		QMessageBox::critical(this, tr("Unknown Driver"), QString::fromStdString(e.what()));
		return false;
	}

	// Ignore Device Path if the interface is IrDA
	if (dclass->interface() != "serial")
	{
		m_cbxDevice->setCurrentIndex(-1);
		device_path.clear();
	}

	// Open the Connection
	try
	{
		dev = dclass->open(device_path.toStdString(), params);
	}
	catch (std::exception & e)
	{
		dev.reset();

		setCursor(Qt::ArrowCursor);
		QMessageBox mb(QMessageBox::Warning, tr("Connection Error"), QString());
		mb.setText(tr("Failed to connect to Dive Computer.  Continue anyway?"));
		mb.setDetailedText(QString::fromStdString(e.what()));
		mb.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		mb.setDefaultButton(QMessageBox::No);
		int ret = mb.exec();

		if (ret == QMessageBox::No)
			return false;

		if (ret == QMessageBox::Cancel)
			wizard()->reject();
	}

	// Save the Computer Information
	m_txtParams->setText(QString::fromStdString(params));

	if (dev)
	{
		m_txtSerial->setText(QString("%1").arg(dev->serial_number()));
		m_txtManuf->setText(QString::fromStdString(dev->manufacturer()));
		m_txtModel->setText(QString::fromStdString(dev->model_name()));
	}

	return QWizardPage::validatePage();
}
