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

#ifndef DELEGATES_HPP_
#define DELEGATES_HPP_

/**
 * @file src/mvf/delegates.hpp
 * @brief Custom Delegate Classes
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QStyledItemDelegate>

#include "util/units.hpp"

/**
 * @brief Base Custom Delegate Class
 *
 * Provides helper methods for drawing styled items since QStyledItemDelegate
 * inexplicably does not implement the protected QItemDelegate draw*()
 * methods. Also implements setEditorData() and setModelData() for combo
 * boxes, which do not work properly with QDataWidgetMapper.
 *
 * CustomDelegate also provides a hook function to allow subclasses to perform
 * filtering on the display parameters (contained in a QStyleOptionViewItemV4
 * instance) before the widget is drawn. See the filter() method documentation
 * for more information.
 */
class CustomDelegate: public QStyledItemDelegate
{
public:

	//! Class Constructor
	CustomDelegate(QObject * parent = 0);

	//! Class Destructor
	virtual ~CustomDelegate();

	//! Draw the background of a styled item
	virtual void drawBackground(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

	//! Reimplemented from QStyledItemDelegate, calls filter()
	virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

	/**
	 * @brief Set Editor Data
	 *
	 * Overrides the default implementation to catch QComboBox editors and
	 * handle them properly with QDataWidgetMapper.  See bug report at
	 * http://bugreports.qt.nokia.com/browse/QTBUG-428
	 */
	virtual void setEditorData(QWidget * editor, const QModelIndex & index) const;

	/**
	 * @brief Set Model Data
	 *
	 * Overrides the default implementation to catch QComboBox editors and
	 * handle them properly with QDataWidgetMapper.  See bug report at
	 * http://bugreports.qt.nokia.com/browse/QTBUG-428
	 */
	virtual void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;

protected:

	/**
	 * @brief Inline Option Filter
	 *
	 * This method can be used to tweak the QStyleOptionViewItemV4 after it
	 * has been initialized with the view data, directly before it is drawn
	 * using style.drawWidget(). This method operates directly on the option
	 * parameter.
	 *
	 * The current model index is also provided so that the filter function
	 * can use context to alter filter behavior. The default behavior
	 * provides no filtering.
	 *
	 * The primary advantage to this approach is that it moves the content
	 * display filtering out of the model (QAbstractItemModel::data()) into
	 * the presentation layer so that the same model data can be displayed in
	 * various ways by different views.
	 */
	virtual void filter(QStyleOptionViewItem * option, const QModelIndex & index) const;

};

/**
 * @brief No-Focus Delegate
 *
 * Custom delegate which suppresses drawing of the focus rectangle.  Used for
 * multi-column list views when it is desired to select the entire row without
 * drawing a per-column focus rectangle.
 */
class NoFocusDelegate: public CustomDelegate
{
public:

	//! Class Constructor
	NoFocusDelegate(QObject* parent = 0);

	//! Class Destructor
	virtual ~NoFocusDelegate();

protected:

	/**
	 * @brief Inline Option Filter
	 *
	 * Removes the State_HasFocus flag from the given option parameter.  This
	 * will suppress painting of the focus rectangle.
	 */
	virtual void filter(QStyleOptionViewItem * option, const QModelIndex & index) const;

};

/**
 * @brief Null-Storing Delegate
 *
 * This delegate implements a filter between the editor and model to store
 * blank values as NULL in the model.
 */
class NullDelegate: public CustomDelegate
{
public:

	//! Class Constructor
	NullDelegate(QObject * parent = 0);

	//! Class Destructor
	virtual ~NullDelegate();

	/**
	 * @brief Set Editor Data
	 *
	 * Overrides the default implementation to catch QComboBox editors and
	 * handle them properly with QDataWidgetMapper.  See bug report at
	 * http://bugreports.qt.nokia.com/browse/QTBUG-428
	 */
	virtual void setEditorData(QWidget * editor, const QModelIndex & index) const;

	/**
	 * @brief Set Model Data
	 *
	 * Overrides the default implementation to catch QComboBox editors and
	 * handle them properly with QDataWidgetMapper.  See bug report at
	 * http://bugreports.qt.nokia.com/browse/QTBUG-428
	 */
	virtual void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;

};

/**
 * @brief Star Rating Delegate
 */
class RatingDelegate: public NoFocusDelegate
{
public:

	//! Class Constructor
	RatingDelegate(const QImage & star = QImage(), QObject * parent = 0);

	//! Class Destructor
	virtual ~RatingDelegate();

	//! Paint the Rating
	virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

	//! Set the Star Image
	void setStar(const QImage & image);

	//! Return the Size Hint
	virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;

	//! @return Star Image
	const QImage & star() const;

protected:
	QImage			m_star;

};

/**
 * @brief Title-Casing Delegate
 *
 * Provides a custom text filter delegate which replaces underscores in the
 * string value with spaces and title-cases the result.  Equivalent Python is
 * str.replace('_', ' ').title()
 */
class TitleCaseDelegate: public NoFocusDelegate
{
public:

	//! Class Constructor
	TitleCaseDelegate(QObject * parent = 0);

	//! Class Destructor
	virtual ~TitleCaseDelegate();

	//! Return the Display Text
	virtual QString displayText(const QVariant & value, const QLocale & locale) const;
};

/**
 * @brief Position (Latitude/Longitude) Delegate
 */
class PositionDelegate: public NoFocusDelegate
{
public:

	//! Class Constructor
	PositionDelegate(QObject * parent = 0);

	//! Class Destructor
	virtual ~PositionDelegate();

	//! Return the Display Text
	virtual QString displayText(const QVariant & value, const QLocale & locale) const;

};

/**
 * @brief Unit Formatting Delegate
 *
 * Formats the display text with a unit suffix, providing inline conversion
 * if necessary.
 */
class UnitDelegate: public NoFocusDelegate
{
public:

	//! Class Constructor
	UnitDelegate(quantity_t quantity, const char * _default = NULL, QObject * parent = 0);

	//! Class Destructor
	virtual ~UnitDelegate();

	//! Return the Display Text
	virtual QString displayText(const QVariant & value, const QLocale & locale) const;

protected:
	quantity_t			m_quantity;
	const char *		m_unit;

};

/**
 * @brief Depth Formatting Delegate
 */
class DepthDelegate: public UnitDelegate
{
public:
	DepthDelegate(QObject * parent = 0) : UnitDelegate(qtDepth, "Meters", parent) { }
	virtual ~DepthDelegate() { }
};

/**
 * @brief Temperature Formatting Delegate
 */
class TemperatureDelegate: public UnitDelegate
{
public:
	TemperatureDelegate(QObject * parent = 0) : UnitDelegate(qtTemperature, "Celsius", parent) { }
	virtual ~TemperatureDelegate() { }
};

/**
 * @brief Pressure Formatting Delegate
 */
class PressureDelegate: public UnitDelegate
{
public:
	PressureDelegate(QObject * parent = 0) : UnitDelegate(qtPressure, "Bar", parent) { }
	virtual ~PressureDelegate() { }
};

/**
 * @brief Volume Formatting Delegate
 */
class VolumeDelegate: public UnitDelegate
{
public:
	VolumeDelegate(QObject * parent = 0) : UnitDelegate(qtVolume, "Liters", parent) { }
	virtual ~VolumeDelegate() { }
};

/**
 * @brief Weight Formatting Delegate
 */
class WeightDelegate: public UnitDelegate
{
public:
	WeightDelegate(QObject * parent = 0) : UnitDelegate(qtWeight, "Kilograms", parent) { }
	virtual ~WeightDelegate() { }
};

/**
 * @brief Date/Time Formatting Delegate
 */
class DateTimeDelegate: public NoFocusDelegate
{
public:

	//! Class Constructor
	DateTimeDelegate(QObject * parent = 0);

	//! Class Destructor
	virtual ~DateTimeDelegate();

	//! Return the Display Text
	virtual QString displayText(const QVariant & value, const QLocale & locale) const;

};

/**
 * @brief Minutes Delegate
 */
class MinutesDelegate: public NoFocusDelegate
{
public:

	//! Class Constructor
	MinutesDelegate(QObject * parent = 0);

	//! Class Destructor
	virtual ~MinutesDelegate();

	//! Return the Display Text
	virtual QString displayText(const QVariant & value, const QLocale & locale) const;

};

/**
 * @brief Per-Mil Delegate
 */
class PerMilDelegate: public NoFocusDelegate
{
public:

	//! Class Constructor
	PerMilDelegate(QObject * parent = 0);

	//! Class Destructor
	virtual ~PerMilDelegate();

	//! Return the Display Text
	virtual QString displayText(const QVariant & value, const QLocale & locale) const;

	//! @brief Set Editor Data
	virtual void setEditorData(QWidget * editor, const QModelIndex & index) const;

	//! @brief Set Model Data
	virtual void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;

};

#endif /* DELEGATES_HPP_ */
