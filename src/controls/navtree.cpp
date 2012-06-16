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

#include <QAbstractItemView>

#include "mvf/delegates/logbook_delegate.hpp"

#include "navtree.hpp"

// Requires C++0x
const QString NavTree::stylesheet = QString(R"END(
QTreeView
{
    background-color: white;
    border-style: none;
}

QTreeView::item
{
}

QTreeView::branch:has-children:!adjoins-item,
QTreeView::branch:has-children:adjoins-item
{
    border-style: none;
}

QTreeView::branch:!has-children:!adjoins-item,
QTreeView::branch:!has-children:adjoins-item
{
    border-style: none;
}

QTreeView::branch:closed:has-children:!has-siblings,
QTreeView::branch:closed:has-children:has-siblings 
{
}

QTreeView::branch:open:has-children:!has-siblings,
QTreeView::branch:open:has-children:has-siblings 
{
}
)END");

NavTree::NavTree(QWidget * parent)
	: QTreeView(parent)
{
	setHeaderHidden(true);
	setItemDelegate(new LogbookDelegate(this));
	setItemsExpandable(false);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setStyleSheet(stylesheet);
}

NavTree::~NavTree()
{
}
