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

#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "mvf/delegates.hpp"
#include "mvf/models.hpp"

#include "modeladddialog.hpp"

ModelAddDialog::ModelAddDialog(QAbstractItemModel * model, IModelEditPanel * panel,
		Logbook::Ptr logbook, QWidget * parent)
	: QDialog(parent), m_model(model), m_panel(panel), m_logbook(logbook)
{
	m_mapper = new QDataWidgetMapper(this);
	m_mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
	m_mapper->setModel(m_model);

	QAbstractItemDelegate * d = m_panel->createDelegate(this);
	if (d != NULL)
		m_mapper->setItemDelegate(d);
	else
		m_mapper->setItemDelegate(new CustomDelegate(this));

	m_btnAdd    = new QPushButton(tr("Add"));
	m_btnCancel = new QPushButton(tr("Cancel"));

	//FIXME: HAX so that the Add Site button gets a trigger on a return
	// press rather than the submit button
	m_btnAdd->setDefault(false);
	m_btnAdd->setAutoDefault(false);
	m_btnCancel->setDefault(false);
	m_btnCancel->setAutoDefault(false);

	QHBoxLayout * hbox = new QHBoxLayout();
	hbox->addStretch();
	hbox->addWidget(m_btnAdd);
	hbox->addWidget(m_btnCancel);

	QWidget * pnl_widget = dynamic_cast<QWidget *>(m_panel);
	assert(pnl_widget != NULL);

	QVBoxLayout * vbox = new QVBoxLayout();
	vbox->addWidget(pnl_widget);
	vbox->addLayout(hbox);

	setLayout(vbox);

	CustomTableModel * tm = dynamic_cast<CustomTableModel *>(model);
	if (tm)
		tm->bind(m_logbook->session());

	m_panel->bind(m_logbook->session(), m_mapper);
	connect(m_btnAdd, SIGNAL(clicked()), this, SLOT(accept()));
	connect(m_btnCancel, SIGNAL(clicked()), this, SLOT(reject()));

	m_mapper->toFirst();

	setWindowTitle(m_panel->title());
}

ModelAddDialog::~ModelAddDialog()
{
}

void ModelAddDialog::submit()
{
	m_mapper->submit();
}
