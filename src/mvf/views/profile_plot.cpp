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

#include <QHBoxLayout>
#include <QSettings>
#include <QVBoxLayout>

#include <boost/locale.hpp>

#include <benthos/logbook/logging.hpp>
#include <benthos/logbook/session.hpp>

#include "profile_alarmitem.hpp"
#include "profile_plot.hpp"

ProfilePlotView::ProfilePlotView(QWidget * parent)
	: QWidget(parent), m_lblProfile(0), m_cbxProfile(0), m_cbxAuxKeys(0),
	  m_pltDepth(0), m_pltAux(0), m_curDive(), m_curProfile(), m_auxKey()
{
	createLayout();

	QVariant vkey;
	QSettings s;
	s.beginGroup("ProfileView");
	vkey = s.value("AuxKey");
	s.endGroup();

	if (vkey.isValid())
		setAuxKey(vkey.toString());
}

ProfilePlotView::~ProfilePlotView()
{
}

QString ProfilePlotView::alarmLabel(const std::string & name)
{
	std::string lname(name);
	std::transform(lname.begin(), lname.end(), lname.begin(), tolower);

	if (lname == "ascent")
		return tr("Ascent Rate");
	if (lname == "bookmark")
		return tr("Bookmark");

	std::string name2(name);
	std::replace(name2.begin(), name2.end(), '_', ' ');
	return QString::fromStdString(boost::locale::to_title(name2));
}

void ProfilePlotView::cbxAuxKeysIndexChanged(int index)
{
	setAuxKey(m_cbxAuxKeys->itemData(index, Qt::UserRole).toString());
}

void ProfilePlotView::cbxProfileIndexChanged(int index)
{
	if (! m_curDive)
		return;

	bool ok;
	int pid = m_cbxProfile->itemData(index, Qt::UserRole).toInt(& ok);

	if (ok)
	{
		IProfileFinder::Ptr pf = boost::dynamic_pointer_cast<IProfileFinder>(m_curDive->session()->finder<Profile>());
		setProfile(pf->find(pid));
	}
	else
	{
		setProfile(Profile::Ptr());
	}
}

void ProfilePlotView::createLayout()
{
	m_pltDepth = new QCustomPlot;
	m_pltDepth->setInteraction(QCustomPlot::iSelectItems, true);
	m_pltDepth->setInteraction(QCustomPlot::iMultiSelect, false);

	/*
	 * Calculate Left-hand Margin (identical for both plots)
	 */
	QFontMetrics fm(m_pltDepth->xAxis->labelFont());
	int lMargin = fm.width('0') * 4 + fm.width('.') + fm.height() * 5 / 2;
	int bMargin = fm.height() * 7 / 2;

	/*
	 * Setup Depth Plot Margins
	 */
	m_pltDepth->setAutoMargin(false);
	m_pltDepth->setMargin(lMargin, 8, 8, bMargin);

	/*
	 * Setup Depth Plot Axes
	 */
	m_pltDepth->xAxis->setAutoTicks(false);
	m_pltDepth->xAxis->setBasePen(QColor(192, 192, 192, 255));
	m_pltDepth->xAxis->setLabel("Time (minutes)");
	m_pltDepth->xAxis->setSubTickPen(Qt::NoPen);
	m_pltDepth->xAxis->setTickLength(0, 4);
	m_pltDepth->xAxis->setTickPen(QColor(192, 192, 192, 255));

	m_pltDepth->yAxis->setAutoTicks(false);
	m_pltDepth->yAxis->setBasePen(QColor(192, 192, 192, 255));
	m_pltDepth->yAxis->setRangeReversed(true);
	m_pltDepth->yAxis->setSubTickPen(Qt::NoPen);
	m_pltDepth->yAxis->setTickLength(0, 4);
	m_pltDepth->yAxis->setTickPen(QColor(192, 192, 192, 255));

	m_pltDepth->xAxis2->setBasePen(QColor(192, 192, 192, 255));
	m_pltDepth->xAxis2->setSubTickPen(Qt::NoPen);
	m_pltDepth->xAxis2->setTickLabels(false);
	m_pltDepth->xAxis2->setTickPen(Qt::NoPen);
	m_pltDepth->xAxis2->setVisible(true);

	m_pltDepth->yAxis2->setBasePen(QColor(192, 192, 192, 255));
	m_pltDepth->yAxis2->setSubTickPen(Qt::NoPen);
	m_pltDepth->yAxis2->setTickLabels(false);
	m_pltDepth->yAxis2->setTickPen(Qt::NoPen);
	m_pltDepth->yAxis2->setVisible(true);

	connect(m_pltDepth, SIGNAL(beforeReplot()), this, SLOT(pltDepthBeforeReplot()));

	/*
	 * Setup Aux Plot Margins
	 */
	m_pltAux = new QCustomPlot;
	m_pltAux->setAutoMargin(false);
	m_pltAux->setMinimumSize(400, 80);
	m_pltAux->setMargin(lMargin, 8, 8, 8);

	/*
	 * Setup Aux Plot Axes
	 */
	m_pltAux->xAxis->setAutoTicks(false);
	m_pltAux->xAxis->setBasePen(QColor(192, 192, 192, 255));
	m_pltAux->xAxis->setSubTickPen(Qt::NoPen);
	m_pltAux->xAxis->setTickLabels(false);
	m_pltAux->xAxis->setTickPen(Qt::NoPen);

	m_pltAux->yAxis->setAutoTicks(false);
	m_pltAux->yAxis->setBasePen(QColor(192, 192, 192, 255));
	m_pltAux->yAxis->setSubTickPen(Qt::NoPen);
	m_pltAux->yAxis->setTickLength(0, 4);
	m_pltAux->yAxis->setTickPen(QColor(192, 192, 192, 255));

	m_pltAux->xAxis2->setBasePen(QColor(192, 192, 192, 255));
	m_pltAux->xAxis2->setSubTickPen(Qt::NoPen);
	m_pltAux->xAxis2->setTickLabels(false);
	m_pltAux->xAxis2->setTickPen(Qt::NoPen);
	m_pltAux->xAxis2->setVisible(true);

	m_pltAux->yAxis2->setBasePen(QColor(192, 192, 192, 255));
	m_pltAux->yAxis2->setSubTickPen(Qt::NoPen);
	m_pltAux->yAxis2->setTickLabels(false);
	m_pltAux->yAxis2->setTickPen(Qt::NoPen);
	m_pltAux->yAxis2->setVisible(true);

	/*
	 * Setup Combo Boxes
	 */
	m_cbxAuxKeys = new QComboBox;
	m_cbxProfile = new QComboBox;
	m_lblProfile = new QLabel(tr("Select &Profile:"));
	m_lblProfile->setBuddy(m_cbxProfile);

	m_lblProfile->setVisible(false);
	m_cbxProfile->setVisible(false);

	connect(m_cbxAuxKeys, SIGNAL(currentIndexChanged(int)), this, SLOT(cbxAuxKeysIndexChanged(int)));
	connect(m_cbxProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(cbxProfileIndexChanged(int)));

	/*
	 * Setup Layout
	 */
	QHBoxLayout * hbox1 = new QHBoxLayout;
	hbox1->addWidget(m_lblProfile);
	hbox1->addWidget(m_cbxProfile);

	QHBoxLayout * hbox2 = new QHBoxLayout;
	hbox2->setContentsMargins(0, 0, 0, 0);
	hbox2->addSpacing(lMargin * 3 / 4);
	hbox2->addWidget(m_cbxAuxKeys);
	hbox2->addStretch();

	QVBoxLayout * vbox = new QVBoxLayout;
	vbox->setContentsMargins(0, 0, 0, 0);
	vbox->addLayout(hbox1);
	vbox->addWidget(m_pltDepth, 1);
	vbox->addLayout(hbox2);
	vbox->addWidget(m_pltAux);
	setLayout(vbox);
}

void ProfilePlotView::loadAuxPlotData(const std::string & key)
{
	/*
	 * Clear the Existing Plot Data
	 */
	m_pltAux->clearGraphs();
	m_pltAux->yAxis->setLabel(QString());
	m_pltAux->yAxis->setTickVector(QVector<double>());
	m_pltAux->replot();

	/*
	 * Check the Key is Valid
	 */
	if (! m_curProfile || key.empty())
		return;

	if (m_curProfile->keys().find(key) == m_curProfile->keys().end())
	{
		logging::getLogger("gui.plot")->warning("Unknown Profile Data Key: " + key);
		return;
	}

	/*
	 * Load the Value Quantity Type and Units
	 */
	unit_t unit;
	bool hasUnit = false;

	try
	{
		quantity_t q = profileKeyQuantity(key);
		if (q != qtUnknown)
			unit = unitForQuantity(q);

		hasUnit = true;
	}
	catch (std::runtime_error & e)
	{
	}

	/*
	 * Create the Plot Vector
	 */
	QVector<double> time;
	QVector<double> value;
	std::list<waypoint>::const_iterator it;
	for (it = m_curProfile->profile().begin(); it != m_curProfile->profile().end(); it++)
	{
		time.push_back(it->time / 60.0f);

		if (! hasUnit)
			value.push_back(it->data.at(key));
		else
			value.push_back(unit.conv->fromNative(it->data.at(key)));
	}

	/*
	 * Setup the Aux Plot
	 */
	if (hasUnit)
		m_pltAux->yAxis->setLabel(QString::fromStdWString(unit.abbr));

	m_pltAux->addGraph();
	m_pltAux->graph(0)->setPen(QColor(64, 64, 64, 255));
	m_pltAux->graph(0)->setData(time, value);
	m_pltAux->graph(0)->rescaleAxes();
	setupAuxAxis();
	m_pltAux->replot();
}

void ProfilePlotView::pltDepthBeforeReplot()
{
	setupAlarms();
}

QString ProfilePlotView::profileKeyLabel(const std::string & key)
{
	std::string lkey(key);
	std::transform(lkey.begin(), lkey.end(), lkey.begin(), tolower);

	if (lkey == "depth")
		return tr("Depth");
	if ((lkey == "temp") or (lkey == "temperature"))
		return tr("Temp");
	if ((lkey == "px") or (lkey == "pressure"))
		return tr("Pressure");
	if (lkey == "rbt")
		return tr("RBT");
	if (lkey == "ndl")
		return tr("NDL");
	if (lkey == "heartrate")
		return tr("Heartrate");
	if ((lkey == "heading") or (lkey == "hdg"))
		return tr("Heading");
	if (lkey == "bearing")
		return tr("Bearing");

	std::string key2(key);
	std::replace(key2.begin(), key2.end(), '_', ' ');
	return QString::fromStdString(boost::locale::to_title(key2));
}

quantity_t ProfilePlotView::profileKeyQuantity(const std::string & key)
{
	std::string lkey(key);
	std::transform(lkey.begin(), lkey.end(), lkey.begin(), tolower);

	if (lkey == "depth")
		return qtDepth;
	if ((lkey == "temp") or (lkey == "temperature"))
		return qtTemperature;
	if ((lkey == "px") or (lkey == "pressure"))
		return qtPressure;
	if ((lkey == "rbt") or (lkey == "ndl"))
		return qtTime;
	if (lkey == "heartrate")
		return qtHeartrate;
	if ((lkey == "heading") or (lkey == "bearing") or (lkey == "hdg"))
		return qtHeading;

	return qtUnknown;
}

QString ProfilePlotView::profileLabel(Profile::Ptr profile)
{
	if (profile->name())
		return QString::fromStdString(profile->name().get());

	if (profile->computer())
	{
		if (profile->computer()->name())
			return QString::fromStdString(profile->computer()->name().get());

		if (profile->computer()->model())
		{
			if (profile->computer()->manufacturer())
			{
				return QString("%1 %2")
					.arg(QString::fromStdString(profile->computer()->manufacturer().get()))
					.arg(QString::fromStdString(profile->computer()->model().get()));
			}
			else
			{
				return QString::fromStdString(profile->computer()->model().get());
			}
		}
	}

	return "Unknown Profile";
}

void ProfilePlotView::setAuxKey(const QString & key)
{
	m_auxKey = key;

	QSettings s;
	s.beginGroup("ProfileView");
	s.setValue("AuxKey", key);
	s.endGroup();

	loadAuxPlotData(key.toStdString());
}

void ProfilePlotView::setDive(Dive::Ptr dive)
{
	m_cbxProfile->clear();
	m_cbxProfile->setVisible(false);
	m_lblProfile->setVisible(false);

	m_curDive = dive;
	if (! m_curDive)
	{
		setProfile(Profile::Ptr());
		return;
	}

	//TODO: Use Dive::Profiles collection
	IProfileFinder::Ptr pf = boost::dynamic_pointer_cast<IProfileFinder>(m_curDive->session()->finder<Profile>());
	std::vector<Profile::Ptr> pl = pf->findByDive(m_curDive->id());

	if (pl.empty())
	{
		setProfile(Profile::Ptr());
		return;
	}

	/*
	 * Load the list of Profiles
	 */
	int defidx = -1;
	int idx = 0;
	Profile::Ptr def_p;
	std::vector<Profile::Ptr>::iterator it;
	for (it = pl.begin(); it != pl.end(); it++)
	{
		++idx;
		m_cbxProfile->addItem(profileLabel(* it), QVariant((qlonglong)(* it)->id()));

		if ((* it)->computer() == m_curDive->computer())
		{
			defidx = idx;
			def_p = * it;
		}
	}

	if (! def_p && (pl.size() == 1))
	{
		def_p = * pl.begin();
		defidx = 0;
	}

	if (m_cbxProfile->count() > 1)
	{
		m_lblProfile->setVisible(true);
		m_cbxProfile->setVisible(true);
	}

	/*
	 * Load the Profile
	 */
	m_cbxProfile->setCurrentIndex(defidx);
	setProfile(* pl.begin());
}

void ProfilePlotView::setProfile(Profile::Ptr profile)
{
	m_pltDepth->clearGraphs();
	m_pltDepth->clearItems();

	m_cbxAuxKeys->clear();
	m_cbxAuxKeys->setEnabled(false);

	m_curProfile = profile;
	if (! m_curProfile)
	{
		loadAuxPlotData(std::string());
		return;
	}

	/*
	 * Check for the "depth" key and remove it
	 */
	std::set<std::string> keys(m_curProfile->keys());
	bool hasDepth = (keys.find("depth") != keys.end());
	if (! hasDepth)
		logging::getLogger("gui.plot")->warning("Profile does not have depth data");
	else
		keys.erase("depth");

	/*
	 * Load the Aux Key Combo Box
	 */
	std::set<std::string>::const_iterator itk;
	for (itk = keys.begin(); itk != keys.end(); itk++)
	{
		QString key(QString::fromStdString(* itk));
		QString name(profileKeyLabel(* itk));
		m_cbxAuxKeys->addItem(name, key);
	}

	/*
	 * Load the Depth Profile
	 */
	if (hasDepth)
	{
		bool hasUnit = false;
		unit_t unit;

		try
		{
			unit = unitForQuantity(qtDepth);
			hasUnit = true;
		}
		catch (std::runtime_error & e)
		{
		}

		QVector<double> time;
		QVector<double> depth;

		unsigned int lastAlarm = 0;
		AlarmPlotItem * curAlarm = 0;

		std::list<waypoint>::const_iterator it;
		for (it = m_curProfile->profile().begin(); it != m_curProfile->profile().end(); it++)
		{
			time.push_back(it->time / 60.0f);
			if (! hasUnit)
				depth.push_back(it->data.at("depth"));
			else
				depth.push_back(unit.conv->fromNative(it->data.at("depth")));

			if (it->alarms.size() > 0)
			{
				std::set<std::string>::const_iterator ait;

				// Group alarms within the same 60 seconds into one item
				if (((it->time - lastAlarm) > 60) || ! curAlarm)
				{
					if (curAlarm)
					{
						curAlarm->finalize();
						m_pltDepth->addItem(curAlarm);
					}

					curAlarm = new AlarmPlotItem(m_pltDepth);
					lastAlarm = it->time;
				}

				for (ait = it->alarms.begin(); ait != it->alarms.end(); ait++)
					curAlarm->addAlarm(it->time, alarmLabel(* ait), QString());
			}
		}

		if (curAlarm)
		{
			curAlarm->finalize();
			m_pltDepth->addItem(curAlarm);
		}

		if (! hasUnit)
			m_pltDepth->yAxis->setLabel(tr("Depth"));
		else
			m_pltDepth->yAxis->setLabel(tr("Depth (%1)").arg(QString(unit.name).toLower()));

		QLinearGradient lg(0, 0, 0, 1);
		lg.setCoordinateMode(QGradient::StretchToDeviceMode);
		lg.setColorAt(0, QColor(255, 255, 255, 255));
		lg.setColorAt(0.6, QColor(0, 64, 112, 255));

		m_pltDepth->addGraph();
		m_pltDepth->graph(0)->setBrush(lg);
		m_pltDepth->graph(0)->setPen(QColor(192, 192, 192, 255));
		m_pltDepth->graph(0)->setData(time, depth);
		m_pltDepth->graph(0)->rescaleAxes();
		setupTimeAxis();
		setupDepthAxis();
		setupAlarms();
		m_pltDepth->replot();
	}

	/*
	 * Initialize the Aux Plot
	 */
	if ((m_cbxAuxKeys->count() == 1) || (keys.find(m_auxKey.toStdString()) == keys.end()))
	{
		setAuxKey(m_cbxAuxKeys->itemData(0).toString());
	}
	else if (keys.find(m_auxKey.toStdString()) != keys.end())
	{
		m_cbxAuxKeys->setCurrentIndex(m_cbxAuxKeys->findText(m_auxKey, Qt::MatchFlags()));
		loadAuxPlotData(m_auxKey.toStdString());
	}
}

void ProfilePlotView::setupAlarms()
{
	for (int i = 0; i < m_pltDepth->itemCount(); ++i)
	{
		AlarmPlotItem * pi = dynamic_cast<AlarmPlotItem *>(m_pltDepth->item(i));
		if (! pi)
			continue;

		int tavg = (pi->start_time() + pi->end_time()) / 2;

		// Map to Pixel Coordinates
		int pxL = (int)floor(m_pltDepth->xAxis->coordToPixel(tavg / 60.0f)) - pi->pixmap().width() / 2;
		int pxR = (int)floor(m_pltDepth->xAxis->coordToPixel(tavg / 60.0f)) + pi->pixmap().width() / 2;

		int pxB = (int)floor(m_pltDepth->yAxis->coordToPixel(m_pltDepth->yAxis->range().upper)) - 8;
		int pxT = pxB - pi->pixmap().height();

		pi->topLeft->setType(QCPItemPosition::ptAbsolute);
		pi->topLeft->setCoords(pxL, pxT);
		pi->bottomRight->setType(QCPItemPosition::ptAbsolute);
		pi->bottomRight->setCoords(pxR, pxB);
	}
}

void ProfilePlotView::setupAuxAxis()
{
	QCPRange range = m_pltAux->yAxis->range();
	double step = tickStep(range, 2);
	double min = qFloor(range.lower / step) * step;
	double max = qCeil(range.upper / step) * step;

	QVector<double> ticks;
	ticks << min;
	ticks << (min + max) / 2;
	ticks << max;

	m_pltAux->yAxis->setRange(min, max);
	m_pltAux->yAxis->setTickVector(ticks);

	m_pltAux->xAxis->setRange(m_pltDepth->xAxis->range());
	m_pltAux->xAxis->setTickVector(m_pltDepth->xAxis->tickVector());
}

void ProfilePlotView::setupDepthAxis()
{
	double step = tickStep(m_pltDepth->yAxis->range(), 4);

	static int extTicks = 1;
	QVector<double> ticks;
	int nTicks = (int)qCeil(m_pltDepth->yAxis->range().upper / step) + 1 + extTicks;
	for (int i = 0; i < nTicks; ++i)
		ticks << i * step;

	m_pltDepth->yAxis->setRange(0, (nTicks - 1) * step);
	m_pltDepth->yAxis->setTickVector(ticks);
}

void ProfilePlotView::setupTimeAxis()
{
	double step = tickStep(m_pltDepth->xAxis->range());

	QVector<double> ticks;
	double curTick = 0;
	while (curTick <= m_pltDepth->xAxis->range().upper)
	{
		ticks << curTick;
		curTick += step;
	}

	m_pltDepth->xAxis->setTickVector(ticks);
}

double ProfilePlotView::tickStep(const QCPRange & range, int nSteps)
{
	double step = range.size() / (double)(nSteps+1e-10);
	double factor = qPow(10.0, qFloor(qLn(step) / qLn(10.0)));
	double mantissa = step / factor;

	if (mantissa >= 7)
	{
		factor *= 10.0;
		step = 1;
	}
	else if (mantissa >= 5)
		step = 5;
	else if (mantissa >= 2)
		step = 2;
	else if (mantissa >= 1)
		step = 1;
	else
	{
		factor /= 10.0;
		step = 5;
	}

	return step * factor;
}

unit_t ProfilePlotView::unitForQuantity(quantity_t q) const
{
	QSettings s;
	s.beginGroup("Settings");
	QVariant uname = s.value(QString("Unit%1").arg(q));
	s.endGroup();

	try
	{
		if (! uname.isValid())
			return findUnit(q, 0);
		else
			return findUnit(q, (const char *)uname.toByteArray());
	}
	catch (std::runtime_error & e)
	{
	}

	return unit_t();
}
