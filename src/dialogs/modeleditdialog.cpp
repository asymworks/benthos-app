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
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

#include "mvf/delegates.hpp"
#include "mvf/models.hpp"

#include "modeleditdialog.hpp"

ModelEditDialog::ModelEditDialog(QAbstractItemModel * model, IModelEditPanel * panel,
		Logbook::Ptr logbook, const QModelIndex & index, QWidget * parent)
	: QDialog(parent), m_model(model), m_panel(panel), m_logbook(logbook)
{
	m_mapper = new QDataWidgetMapper(this);
	m_mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
	m_mapper->setModel(m_model);

	QAbstractItemDelegate * d = m_panel->createDelegate(this);
	if (d != NULL)
		m_mapper->setItemDelegate(d);
	else
		m_mapper->setItemDelegate(new CustomDelegate(this));

	m_btnPrev = new QPushButton(tr("Previous"));
	m_btnNext = new QPushButton(tr("Next"));

	//FIXME: HAX so that the search box in MapLocationEditor gets a
	//  returnPressed() event instead of submitting the dialog.
	m_btnPrev->setDefault(false);
	m_btnPrev->setAutoDefault(false);
	m_btnNext->setDefault(false);
	m_btnNext->setAutoDefault(false);

	QHBoxLayout * hbox = new QHBoxLayout();
	hbox->addStretch();
	hbox->addWidget(m_btnPrev);
	hbox->addWidget(m_btnNext);

	QWidget * pnl_widget = dynamic_cast<QWidget *>(m_panel);
	assert(pnl_widget != NULL);

	QVBoxLayout * vbox = new QVBoxLayout();
	vbox->addWidget(pnl_widget);
	vbox->addLayout(hbox);

	setLayout(vbox);

	QAbstractItemModel * m = model;
	QSortFilterProxyModel * p = dynamic_cast<QSortFilterProxyModel *>(m);
	while (p != NULL)
	{
		m = p->sourceModel();
		p = dynamic_cast<QSortFilterProxyModel *>(m);
	}

	CustomTableModel * tm = dynamic_cast<CustomTableModel *>(m);
	if (tm)
		tm->bind(m_logbook->session());

	m_panel->bind(m_logbook->session(), m_mapper);
	connect(m_btnNext, SIGNAL(clicked()), m_mapper, SLOT(toNext()));
	connect(m_btnPrev, SIGNAL(clicked()), m_mapper, SLOT(toPrevious()));
	connect(m_mapper, SIGNAL(currentIndexChanged(int)), this, SLOT(mapperIndexChanged(int)));

	if (index.isValid())
		m_mapper->setCurrentIndex(index.row());
}

ModelEditDialog::~ModelEditDialog()
{
}

void ModelEditDialog::closeEvent(QCloseEvent * event)
{
	//FIXME: Why is closeEvent not getting called?
	printf("ModelEditDialog::closeEvent()\n");
	m_mapper->submit();
}

void ModelEditDialog::mapperIndexChanged(int index)
{
	m_btnPrev->setEnabled(index > 0);
	m_btnNext->setEnabled(index < m_model->rowCount() - 1);

	setWindowTitle(m_panel->title());
}
