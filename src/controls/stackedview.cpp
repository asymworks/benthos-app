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

#include <QAbstractItemView>
#include <QMessageBox>
#include <QSettings>
#include <QVariant>

#include "compositelistview.hpp"
#include "dialogs/modeleditdialog.hpp"

#include "benthositemview.hpp"
#include "stackedview.hpp"

StackedView::StackedView(IModelFactory * mfactory, QWidget * parent)
	: QStackedWidget(parent), m_viewMode(InvalidViewMode),
	  m_viewList(), m_proxyList(), m_filter(),
	  m_model(mfactory->create()), m_logbook()
{
}

StackedView::~StackedView()
{
}

void StackedView::bind(Logbook::Ptr logbook)
{
	m_logbook = logbook;
}

QModelIndex StackedView::currentModelIndex() const
{
	QAbstractItemView * iv = dynamic_cast<QAbstractItemView *>(currentWidget());
	if (iv)
		return iv->currentIndex();

	IBenthosItemView * biv = dynamic_cast<IBenthosItemView *>(currentWidget());
	if (biv)
		return biv->currentIndex();

	return QModelIndex();
}

QModelIndex StackedView::currentModelIndexForMode(ViewMode vm) const
{
	if (! hasViewMode(vm))
		return QModelIndex();

	QAbstractItemView * iv = dynamic_cast<QAbstractItemView *>(m_viewList.at(vm));
	if (iv)
		return iv->currentIndex();

	IBenthosItemView * biv = dynamic_cast<IBenthosItemView *>(m_viewList.at(vm));
	if (biv)
		return biv->currentIndex();

	return QModelIndex();
}

void StackedView::deleteSelection(bool confirm)
{
	if (! selectionModel())
		return;

	QModelIndexList items = selectionModel()->selectedRows();

	if (confirm && (QMessageBox::question(this, tr("Confirm Delete"), tr("Really delete %1 items?").arg(items.count()), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No))
		return;

	qSort(items.begin(), items.end());
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

		m->removeRow(idx.row());
	}

	m_logbook->session()->commit();
}

const QString & StackedView::filter_string() const
{
	return m_filter;
}

bool StackedView::hasViewMode(ViewMode vm) const
{
	return (m_viewList.find(vm) != m_viewList.end());
}

QAbstractItemModel * StackedView::model() const
{
	return m_model;
}

void StackedView::onViewCurrentChanged(const QModelIndex & current, const QModelIndex & previous)
{
	emit currentChanged(current, previous);
}

void StackedView::onViewSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
	emit selectionChanged(selected, deselected);
}

void StackedView::readSettings()
{
	QSettings s;

	// Load the View Mode
	s.beginGroup(metaObject()->className());
	QVariant vm = s.value("mode");
	s.endGroup();

	if (vm.isValid() && hasViewMode((ViewMode)vm.toInt()))
	{
		m_viewMode = (ViewMode)vm.toInt();
		setCurrentWidget(m_viewList[m_viewMode]);
	}
}

void StackedView::refreshView()
{
	currentWidget()->update();
}

QItemSelectionModel * StackedView::selectionModel() const
{
	QAbstractItemView * iv = dynamic_cast<QAbstractItemView *>(currentWidget());
	if (iv)
		return iv->selectionModel();

	IBenthosItemView * biv = dynamic_cast<IBenthosItemView *>(currentWidget());
	if (biv)
		return biv->selectionModel();

	return 0;
}

void StackedView::setFilterString(const QString & filter)
{
	m_filter = filter;

	std::map<ViewMode, QSortFilterProxyModel *>::iterator it;
	for (it = m_proxyList.begin(); it != m_proxyList.end(); it++)
		it->second->setFilterFixedString(filter);
}

void StackedView::setViewMode(ViewMode vm)
{
	if (! hasViewMode(vm))
		throw std::runtime_error("Requested view mode not supported");

	if (m_viewMode != vm)
	{
		m_viewMode = vm;

		setCurrentWidget(m_viewList[vm]);
		writeSettings();

		emit viewModeChanged(vm);
	}
}

void StackedView::showEditor(const QModelIndex & index)
{
	if (! index.isValid() || ! m_logbook)
		return;

	IModelEditPanel * pnl = createEditor();
	QAbstractItemModel * mdl = m_proxyList[m_viewMode];
	ModelEditDialog d(mdl, pnl, m_logbook, index);
	d.exec();

	m_logbook->session()->commit();
}

StackedView::ViewMode StackedView::view_mode() const
{
	return m_viewMode;
}

void StackedView::writeSettings()
{
	QSettings s;

	// Store the View Mode
	s.beginGroup(metaObject()->className());
	s.setValue("mode", QVariant::fromValue<int>(m_viewMode));
	s.endGroup();
}
