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

#ifndef TRANSFERWORKER_HPP_
#define TRANSFERWORKER_HPP_

/**
 * @file src/workers/transferworker.hpp
 * @brief Dive Computer Transfer Worker Thread Class
 * @author Jonathan Krauss <jkrauss@asymworks.com>
 */

#include <cstdint>
#include <list>
#include <vector>

#include <QObject>
#include <QRunnable>

#include <benthos/divecomputer/driver.hpp>

#include <benthos/logbook/dive_computer.hpp>
#include <benthos/logbook/profile.hpp>
#include <benthos/logbook/session.hpp>

using namespace benthos::dc;
using namespace benthos::logbook;

typedef std::vector<uint8_t>		dive_buffer_t;
typedef std::list<dive_buffer_t>	dive_data_t;

/**
 * @brief Dive Computer Transfer Worker
 *
 * Runnable that manages a transfer from a dive computer, including data
 * transfer and dive parsing.  The dive computer is specified in the class
 * constructor, which then runs the transfer when the run() method is called.
 * The Transfer Worker provides status updates and parsed dives via the Qt
 * signal/slot mechanism.
 *
 * The data transfer begins with the worker loading the driver specified in
 * the dive computer instance and opening a connection with the requested
 * options.  If the connection opens successfully, the serial number is read
 * to verify that it is the same dive computer.  This check can be skipped with
 * a flag in the constructor.
 *
 * Once the connection has been opened and the computer verified, data is
 * transferred, starting at the token stored in the instance.  The token is then
 * updated and the connection closed.  Updating the token can be skipped with a
 * flag in the constructor.  The transferred data is then parsed and sent to the
 * calling application.  The Transfer Worker does not directly add dives to the
 * logbook; rather, the parsed profiles are passed back to the caller through
 * the parsedDive() slot.  This enables the calling procedure to do further
 * processing and possibly ignore the dive.
 *
 * @note Profile::Ptr must be registered with the Qt metadata system in order
 * for the signal/slot to work correctly with TransferWorker.  To register, add
 * the following line to the application initialization:
 * @code
 * qRegisterMetaType<Profile::Ptr>();
 * @endcode
 */
class TransferWorker: public QObject, public QRunnable
{
	Q_OBJECT

public:

	/**
	 * @brief Class Constructor
	 * @param[in] Dive Computer instance from which to transfer dives
	 * @param[in] Session instance where the dives will be persisted
	 * @param[in] Flag whether to verify the dive computer serial number
	 * @param[in] Flag whether to update the dive computer token
	 * @param[in] Parent object
	 */
	TransferWorker(DiveComputer::Ptr dc, Session::Ptr session, bool checkSerNo = true, bool updateToken = true, QObject * parent = 0);

	//! Class Destructor
	virtual ~TransferWorker();

	//! Run the Transfer
	virtual void run();

public:

	//! @return If the Transfer has been Cancelled
	bool cancelled() const;

public slots:

	/**
	 * @brief Cancel the Transfer
	 *
	 * Causes the transfer to terminate at the next possible opportunity, if
	 * cancelling is supported by the driver.
	 */
	void cancel();

signals:

	/**
	 * @brief Transfer Finished Signal
	 *
	 * Emitted after all transfer and parsing activities are completed; i.e.
	 * when the runnable is finished with all work.
	 */
	void finished();

	/**
	 * @brief Parsed Dive Signal
	 * @param[out] Parsed Profile
	 *
	 * Emitted when a dive has been parsed.  The profile contains the waypoint
	 * data and the associated Dive instance holds dive header information.  Any
	 * mixes that were not found in the logbook are also attached to the Profile
	 * instance object.
	 *
	 * It is the responsibility of the slot to call Session::add() on the
	 * profile and linked objects (if required by the merge policy). prior to
	 * committing the transaction.
	 */
	void parsedDive(Profile::Ptr);

	/**
	 * @brief Data Transfer Progress Signal
	 * @param[out] Number of Bytes Transferred
	 *
	 * This signal is emitted periodically during the data transfer process with
	 * the current number of bytes that have been transferred.
	 */
	void progress(unsigned long);

	/**
	 * @brief Data Transfer Status Signal
	 * @param[out] Status Message
	 *
	 * This signal is emitted periodically during the data transfer process with
	 * status messages pertaining to the current transfer phase.
	 */
	void status(const QString &);

	/**
	 * @brief Data Transfer Started Signal
	 * @param[out] Number of Bytes to Transfer
	 *
	 * This signal is emitted once when the actual data transfer starts to
	 * notify the caller of the number of bytes that will be transferred (i.e.
	 * the argument in the progress() slot will range between 0 and this value).
	 */
	void started(unsigned long);

	/**
	 * @brief Data Transfer Error Signal
	 * @param[out] Error Message
	 *
	 * This signal is emitted in response to an error in the transfer.  The
	 * parameter contains the error message.
	 */
	void transferError(const QString &);

public:

	//! @internal Do not call from client code
	void driver_progress(uint32_t transferred, uint32_t total);

protected:

	//! Parse Dive Data
	void parse_dives(Driver::Ptr driver, const dive_data_t & dives);

private:
	DiveComputer::Ptr			m_dc;
	Session::Ptr				m_session;

	bool						m_checkSN;
	bool						m_updateToken;
	bool						m_cancel;

};

#endif /* TRANSFERWORKER_HPP_ */
