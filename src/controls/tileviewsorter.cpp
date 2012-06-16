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

#include <QHBoxLayout>
#include <QLabel>
#include <QSortFilterProxyModel>

#include "mvf/models.hpp"
#include "tileviewsorter.hpp"

// Requires C++0x
const QString TileViewSorter::stylesheet = QString(R"END(
#sortFrame {
}

#sortFrame QPushButton {
	padding: 1px;
	padding-left: 3px;
	padding-right: 3px;
	min-width: 50px;
})END");

TileViewSorter::TileViewSorter(const std::list<std::string> & columns, QWidget * parent)
	: QFrame(parent), m_view(0), m_sortColumns(columns.begin(), columns.end()),
	  m_sortColumn(-1), m_sortOrder(Qt::AscendingOrder), m_colButtons(), m_dirButtons(),
	  m_bgSortCol(0), m_bgSortDir(0)
{
	m_bgSortCol = new QButtonGroup(this);
	m_bgSortDir = new QButtonGroup(this);

	connect(m_bgSortCol, SIGNAL(buttonClicked(int)), this, SLOT(setSortColumn(int)));
	connect(m_bgSortDir, SIGNAL(buttonClicked(int)), this, SLOT(setSortOrderByIndex(int)));

	std::list<std::string>::const_iterator it;
	for (it = columns.begin(); it != columns.end(); it++)
	{
		QPushButton * btn = new QPushButton(QString(it->c_str()), this);
		btn->setCheckable(true);
		btn->setChecked(false);
		btn->setFocusPolicy(Qt::NoFocus);

		m_colButtons.push_back(std::pair<std::string, QPushButton *>(* it, btn));
		m_bgSortCol->addButton(btn, -1);
	}

	QPushButton * btnA = new QPushButton(tr("Asc"), this);
	btnA->setCheckable(true);
	btnA->setChecked(false);
	btnA->setFocusPolicy(Qt::NoFocus);

	QPushButton * btnD = new QPushButton(tr("Desc"), this);
	btnD->setCheckable(true);
	btnD->setChecked(false);
	btnD->setFocusPolicy(Qt::NoFocus);

	m_dirButtons.push_back(std::pair<Qt::SortOrder, QPushButton *>(Qt::AscendingOrder, btnA));
	m_dirButtons.push_back(std::pair<Qt::SortOrder, QPushButton *>(Qt::DescendingOrder, btnD));

	m_bgSortDir->addButton(btnA, Qt::AscendingOrder);
	m_bgSortDir->addButton(btnD, Qt::DescendingOrder);

	QFrame * sep = new QFrame(this);
	sep->setFrameShadow(QFrame::Sunken);
	sep->setFrameShape(QFrame::VLine);

	setObjectName("sortFrame");
	setFrameShape(QFrame::NoFrame);
	setContentsMargins(8, 8, 8, 8);
	setStyleSheet(stylesheet);

	QHBoxLayout * hbox = new QHBoxLayout;
	hbox->setContentsMargins(0, 0, 0, 0);
	hbox->addStretch();
	hbox->addWidget(new QLabel(tr("Sort By:")));
	hbox->addSpacing(8);

	std::list<std::pair<std::string, QPushButton *> >::iterator it2;
	for (it2 = m_colButtons.begin(); it2 != m_colButtons.end(); it2++)
		hbox->addWidget(it2->second);

	hbox->addWidget(sep);
	hbox->addWidget(btnA);
	hbox->addWidget(btnD);

	setLayout(hbox);
}

TileViewSorter::~TileViewSorter()
{
}

void TileViewSorter::setSortColumn(int column)
{
	m_sortColumn = column;

	if (m_view)
	{
		QSortFilterProxyModel * mdl = dynamic_cast<QSortFilterProxyModel *>(m_view->model());
		mdl->sort(m_sortColumn, m_sortOrder);
	}

	QAbstractButton * b = m_bgSortCol->button(column);
	if (b)
	{
		b->clearFocus();
		b->setChecked(true);
	}

	emit sortChanged(m_sortColumn, m_sortOrder);
}

void TileViewSorter::setSortOrder(Qt::SortOrder order)
{
	m_sortOrder = order;

	if (m_view)
	{
		QSortFilterProxyModel * mdl = dynamic_cast<QSortFilterProxyModel *>(m_view->model());
		mdl->sort(m_sortColumn, m_sortOrder);
	}

	QAbstractButton * b = m_bgSortDir->button(order);
	if (b)
	{
		b->clearFocus();
		b->setChecked(true);
	}

	emit sortChanged(m_sortColumn, m_sortOrder);
}

void TileViewSorter::setSortOrderByIndex(int index)
{
	setSortOrder((Qt::SortOrder)index);
}

void TileViewSorter::setView(QAbstractItemView * v)
{
	if (! v->model())
		throw std::runtime_error("Linked View model must not be NULL");
	QSortFilterProxyModel * proxy = dynamic_cast<QSortFilterProxyModel *>(v->model());
	if (! proxy)
		throw std::runtime_error("Linked View model must be a QSortFilterProxyModel");

	// Find the Source Model
	QAbstractItemModel * mdl = proxy;
	while (QAbstractProxyModel * apm = dynamic_cast<QAbstractProxyModel *>(mdl))
		mdl = apm->sourceModel();

	// Ensure the Source Model is a CustomTableModel
	CustomTableModel * model = dynamic_cast<CustomTableModel *>(mdl);
	if (! model)
		throw std::runtime_error("Linked View source model must be a CustomTableModel subclass");

	// Update the Sorter
	m_view = v;
	m_sortColumn = proxy->sortColumn();
	m_sortOrder = proxy->sortOrder();

	std::list<std::pair<std::string, QPushButton *> >::iterator it;
	for (it = m_colButtons.begin(); it != m_colButtons.end(); it++)
	{
		int idx = model->findColumn(QString(it->first.c_str()));
		if (idx == -1)
			continue;

		m_bgSortCol->setId(it->second, idx);

		it->second->setChecked(idx == m_sortOrder);
		it->second->setText(model->columns()[idx]->label());
	}

	std::list<std::pair<Qt::SortOrder, QPushButton *> >::iterator it2;
	for (it2 = m_dirButtons.begin(); it2 != m_dirButtons.end(); it2++)
		it2->second->setChecked(m_sortOrder == it2->first);
}

int TileViewSorter::sortColumn() const
{
	return m_sortColumn;
}

Qt::SortOrder TileViewSorter::sortOrder() const
{
	return m_sortOrder;
}

QAbstractItemView * TileViewSorter::view() const
{
	return m_view;
}
