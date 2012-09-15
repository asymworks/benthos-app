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

#include <cmath>
#include <cstdio>
#include <stdexcept>

#include <QApplication>
#include <QComboBox>
#include <QDateTime>
#include <QLineEdit>
#include <QLocale>
#include <QModelIndex>
#include <QPainter>
#include <QRegExp>
#include <QSettings>
#include <QStyleOptionViewItem>

#include "delegates.hpp"

CustomDelegate::CustomDelegate(QObject * parent)
	: QStyledItemDelegate(parent)
{
}

CustomDelegate::~CustomDelegate()
{
}

void CustomDelegate::drawBackground(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QStyle * style;
	QStyleOptionViewItemV4 opt(option);
	initStyleOption(& opt, index);

	if (opt.widget == NULL)
		style = QApplication::style();
	else
		style = opt.widget->style();

	style->drawPrimitive(QStyle::PE_PanelItemViewItem, & opt, painter, opt.widget);
}

void CustomDelegate::filter(QStyleOptionViewItem * option, const QModelIndex & index) const
{
}

void CustomDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QStyle * style;
	QStyleOptionViewItemV4 opt(option);
	initStyleOption(& opt, index);

	filter(& opt, index);

	if (opt.widget == NULL)
		style = QApplication::style();
	else
		style = opt.widget->style();

	style->drawControl(QStyle::CE_ItemViewItem, & opt, painter, opt.widget);
}

void CustomDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
	QComboBox * cbxEditor = dynamic_cast<QComboBox *>(editor);

	if (cbxEditor != NULL)
	{
		QVariant text = index.data(Qt::EditRole);
		if (! text.isValid())
			text = QString();

		if (cbxEditor->isEditable())
			cbxEditor->setEditText(text.toString());
		else
			cbxEditor->setCurrentIndex(cbxEditor->findData(text, Qt::EditRole, Qt::MatchFlags()));
	}
	else
		QStyledItemDelegate::setEditorData(editor, index);
}

void CustomDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
	QComboBox * cbxEditor = dynamic_cast<QComboBox *>(editor);

	if (cbxEditor != NULL)
		model->setData(index, cbxEditor->itemData(cbxEditor->currentIndex(), Qt::EditRole));
	else
		QStyledItemDelegate::setModelData(editor, model, index);
}

NoFocusDelegate::NoFocusDelegate(QObject * parent)
	: CustomDelegate(parent)
{
}

NoFocusDelegate::~NoFocusDelegate()
{
}

void NoFocusDelegate::filter(QStyleOptionViewItem * option, const QModelIndex & index) const
{
	option->state &= ~(QStyle::State_HasFocus);
}

NullDelegate::NullDelegate(QObject * parent)
	: CustomDelegate(parent)
{
}

NullDelegate::~NullDelegate()
{
}

void NullDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
	CustomDelegate::setEditorData(editor, index);
}

void NullDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
	QComboBox * cbxEditor = dynamic_cast<QComboBox *>(editor);

	if (cbxEditor != NULL)
		model->setData(index, cbxEditor->itemData(cbxEditor->currentIndex(), Qt::EditRole));
	else if (editor->property("text") == "")
		model->setData(index, QVariant());
	else
		CustomDelegate::setModelData(editor, model, index);
}

RatingDelegate::RatingDelegate(const QImage & star, QObject * parent)
	: NoFocusDelegate(parent), m_star(star)
{
	if (m_star.isNull())
		m_star = QImage(":/icons/star.png");
}

RatingDelegate::~RatingDelegate()
{
}

void RatingDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	painter->save();

	drawBackground(painter, option, index);
	double rating = index.data().toDouble();
	int width = m_star.width();

	int y = (option.rect.height() - m_star.height()) / 2;
	painter->translate(option.rect.left(), option.rect.top());

	for (int i = 0; i <= 5; ++i)
	{
		if (rating >= (i + 1))
			painter->drawImage(i*(width+1), y, m_star);
	}

	painter->restore();
}

void RatingDelegate::setStar(const QImage & image)
{
	m_star = image;
}

QSize RatingDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	return QSize((m_star.width()+2) * 5, m_star.height()+2);
}

const QImage & RatingDelegate::star() const
{
	return m_star;
}

TitleCaseDelegate::TitleCaseDelegate(QObject * parent)
	: NoFocusDelegate(parent)
{
}

TitleCaseDelegate::~TitleCaseDelegate()
{
}

QString TitleCaseDelegate::displayText(const QVariant & value, const QLocale & locale) const
{
	QString val = value.toString().replace('_', ' ');
	if (val.isNull() || val.isEmpty())
		return QString();

	QRegExp rx("(^|\\b+)(\\w)");
	int i = 0;
	while ((i = rx.indexIn(val, i)) != -1)
	{
		val[i+rx.cap(1).length()] = rx.cap(2).toUpper().at(0);
		i += rx.matchedLength();
	}

	return val;
}

PositionDelegate::PositionDelegate(QObject * parent)
	: NoFocusDelegate(parent)
{
}

PositionDelegate::~PositionDelegate()
{
}

QString PositionDelegate::displayText(const QVariant & value, const QLocale & locale) const
{
	double lng = value.toPointF().x();
	double lat = value.toPointF().y();
	QString latdir(lat > 0 ? "N" : "S");
	QString lngdir(lng > 0 ? "E" : "W");

	return QString("%1 %2, %3 %4").arg(fabs(lat), 0, 'f', 3).arg(latdir).arg(fabs(lng), 0, 'f', 3).arg(lngdir);
}

UnitDelegate::UnitDelegate(quantity_t quantity, const char * _default, QObject * parent)
	: NoFocusDelegate(parent), m_quantity(quantity), m_unit(_default)
{
}

UnitDelegate::~UnitDelegate()
{
}

QString UnitDelegate::displayText(const QVariant & value, const QLocale & locale) const
{
	unit_t u;

	//! Lookup the Unit Abbreviation
	QSettings s;
	s.beginGroup("Settings");
	QVariant uname = s.value(QString("Unit%1").arg(m_quantity));
	s.endGroup();

	try
	{
		if (! uname.isValid())
			u = findUnit(m_quantity, m_unit);
		else
			u = findUnit(m_quantity, (const char *)uname.toByteArray());
	}
	catch (std::runtime_error & e)
	{
		return NoFocusDelegate::displayText(value, locale);
	}

	//! Convert and Format
	return QString("%1 %2").arg(u.conv->fromNative(value.toDouble()), 0, 'f', 1).arg(QString::fromStdWString(u.abbr));
}

DateTimeDelegate::DateTimeDelegate(QObject * parent)
	: NoFocusDelegate(parent)
{
}

DateTimeDelegate::~DateTimeDelegate()
{
}

QString DateTimeDelegate::displayText(const QVariant & value, const QLocale & locale) const
{
	QSettings s;
	s.beginGroup("Settings");
	QVariant fmt = s.value(QString("DTFormat"), QString("MM/dd/yy hh:mm AP"));
	s.endGroup();

	return value.toDateTime().toString(fmt.toString());
}

MinutesDelegate::MinutesDelegate(QObject * parent)
	: NoFocusDelegate(parent)
{
}

MinutesDelegate::~MinutesDelegate()
{
}

QString MinutesDelegate::displayText(const QVariant & value, const QLocale & locale) const
{
	int mins = value.toInt();
	int h = mins / 60;
	int m = mins - h * 60;

	char buf[255];
	sprintf(buf, "%d:%02d", h, m);
	return QString(buf);
}

PerMilDelegate::PerMilDelegate(QObject * parent)
	: NoFocusDelegate(parent)
{
}

PerMilDelegate::~PerMilDelegate()
{
}

QString PerMilDelegate::displayText(const QVariant & value, const QLocale & locale) const
{
	int permil = value.toUInt();
	return QString("%1.%2 %%").arg(permil / 10).arg(permil % 10);
}

void PerMilDelegate::setEditorData(QWidget * editor, const QModelIndex & index) const
{
	QLineEdit * le = dynamic_cast<QLineEdit *>(editor);
	if (le)
	{
		int permil = index.data(Qt::EditRole).toUInt();
		le->setText(QString("%1.%2 %%").arg(permil / 10).arg(permil % 10));
	}
	else
	{
		NoFocusDelegate::setEditorData(editor, index);
	}
}

void PerMilDelegate::setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const
{
	QLineEdit * le = dynamic_cast<QLineEdit *>(editor);
	if (le)
	{
		QString spct = le->text();
		double pct = spct.toDouble();
		model->setData(index, QVariant((int)(round(pct * 10))), Qt::EditRole);
	}
	else
	{
		NoFocusDelegate::setModelData(editor, model, index);
	}
}
