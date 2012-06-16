/*
 * tiledelegate.hpp
 *
 *  Created on: May 6, 2012
 *      Author: lowsnr
 */

#ifndef TILEDELEGATE_HPP_
#define TILEDELEGATE_HPP_

/**
 * @file src/mvf/tiledelegate.hpp
 * @brief Base Tile Delegate Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <QBrush>
#include <QFont>
#include <QModelIndex>
#include <QPainter>
#include <QPen>
#include <QSize>
#include <QSortFilterProxyModel>
#include <QStyleOptionViewItem>

#include "mvf/delegates.hpp"
#include "mvf/models.hpp"

/**
 * @brief Tile Item Delegate
 *
 * Delegate base class for tiled item views.  Contains helper methods for
 * accessing the row instance associated with the current index as well as
 * primitive drawing helpers.
 *
 * Subclasses must implement the contentSize() and paintContent() methods.
 */
class TileDelegate: public NoFocusDelegate
{
public:

	//! Class Constructor
	TileDelegate(QObject * parent = 0);

	//! Class Destructor
	virtual ~TileDelegate();

	//! @return Background Brush
	const QBrush & brush() const;

	//! @return Image Size
	int imgSize() const;

	//! @return Content Margin
	int margin() const;

	//! Paint the Tile
	virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;

	//! @return Content Pen
	const QPen & pen() const;

	//! @brief Set the Background Brush
	void setBrush(const QBrush & brush);

	//! @brief Set the Image Size
	void setImgSize(int value);

	//! @brief Set the Content Margin
	void setMargin(int value);

	//! @brief Set the Content Pen
	void setPen(const QPen & pen);

	//! @brief Set the Text Font
	void setTextFont(const QFont & font);

	//! @brief Set the Title Font
	void setTitleFont(const QFont & font);

	//! @return Tile Size Hint
	virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;

	//! @return Text Font
	const QFont & textFont() const;

	//! @return Title Font
	const QFont & titleFont() const;

protected:

	//! @brief Draw a Quantity with Units
	static QString formatUnits(quantity_t quantity, double value, bool showAbbr = true);

	//! @return Row Object
	template <class T>
	static boost::shared_ptr<T> rowObject(const QModelIndex & index)
	{
		QModelIndex idx(index);
		QAbstractItemModel * m = (QAbstractItemModel *)idx.model();
		QSortFilterProxyModel * p = dynamic_cast<QSortFilterProxyModel *>(m);
		while (p != NULL)
		{
			idx = p->mapToSource(idx);
			m = p->sourceModel();
			p = dynamic_cast<QSortFilterProxyModel *>(m);
		}

		LogbookQueryModel<T> * mdl = dynamic_cast<LogbookQueryModel<T> *>(m);

		if (mdl == NULL)
			return boost::shared_ptr<T>();
		return mdl->item(idx);
	}

	//! @return Content Size
	virtual QSize contentSize(const QStyleOptionViewItem & option, const QModelIndex & index) const = 0;

	//! @brief Paint Content
	virtual void paintContent(QPainter * painter, const QRect & rect, const QModelIndex & index) const = 0;

protected:
	int			m_margin;
	int			m_imgsize;
	QFont		m_textFont;
	QFont		m_titleFont;
	QBrush		m_brush;
	QPen		m_pen;

};

#endif /* TILEDELEGATE_HPP_ */
