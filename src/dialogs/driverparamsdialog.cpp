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

#include <QDialogButtonBox>
#include <QFont>
#include <QFrame>
#include <QVBoxLayout>

#include "controls/propertygrid.hpp"
#include "driverparamsdialog.hpp"

DriverParamsDialog::DriverParamsDialog(DriverClass::Ptr cls, const std::string & args, QWidget * parent)
	: QDialog(parent), m_model(0), m_view(0)
{
	// Create the Model Instance
	m_model = new DriverParamsModel(cls, args);

	// Create the Property Grid
	m_view = new PropertyGridView;
	m_view->setModel(m_model);

	connect(m_view, SIGNAL(currentIndexChanged(const QModelIndex &)), this, SLOT(currentIndexChanged(const QModelIndex &)));

	// Create the Tip Frame
	QFrame * tipFrame = new QFrame;
	tipFrame->setFrameStyle(QFrame::StyledPanel | QFrame::Plain);

	m_lblTitle = new QLabel;
	m_lblTitle->setStyleSheet("QLabel { font-weight: bold }");

	m_lblTip = new QLabel;
	m_lblTip->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	m_lblTip->setMinimumWidth(300);
	m_lblTip->setMinimumHeight(70);
	m_lblTip->setWordWrap(true);

	QVBoxLayout * tipbox = new QVBoxLayout;
	tipbox->addWidget(m_lblTitle);
	tipbox->addWidget(m_lblTip);
	tipFrame->setLayout(tipbox);

	// Create the Button Box
	QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

	// Create the Layout
	QVBoxLayout * vbox = new QVBoxLayout;
	vbox->addWidget(m_view, 1);
	vbox->addWidget(tipFrame);
	vbox->addWidget(buttonBox);

	setLayout(vbox);
	setWindowTitle(tr("Configure Connection"));
}

DriverParamsDialog::~DriverParamsDialog()
{
}

void DriverParamsDialog::currentIndexChanged(const QModelIndex & index)
{
	if (! index.isValid())
	{
		m_lblTitle->clear();
		m_lblTip->clear();
	}
	else
	{
		int r = index.row();

		m_lblTitle->setText(m_model->data(m_model->index(r, 0), Qt::DisplayRole).toString());
		m_lblTip->setText(m_model->data(m_model->index(r, 0), Qt::UserRole+0).toString());
	}
}

std::string DriverParamsDialog::param_string() const
{
	return m_model->toString();
}
