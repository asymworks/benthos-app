#------------------------------------------------------------------------------
# CMake File for the Benthos Dive Log Application (benthos)
#------------------------------------------------------------------------------
#
# Copyright (C) 2011 Asymworks, LLC.  All Rights Reserved.
# www.asymworks.com / info@asymworks.com
#
# This file is part of the Benthos Dive Log Package (benthos-log.com)
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA.
#

# C/C++ Source Files
set( BENTHOS_SRCS
	main.cpp
	mainwindow.cpp
	controls/compositelistview.cpp
	controls/maplocationeditor.cpp
	controls/multicolumnlistview.cpp
	controls/navtree.cpp
	controls/propertygrid.cpp
	controls/quantityedit.cpp
	controls/ratingedit.cpp
	controls/searchedit.cpp
	controls/stackedview.cpp
	controls/tileview.cpp
	controls/tileviewsorter.cpp
	controls/wizardcombobox.cpp
	dialogs/driverparamsdialog.cpp
	dialogs/modeladddialog.cpp
	dialogs/modeleditdialog.cpp
	dialogs/transferdialog.cpp
	mvf/countrymodel.cpp
	mvf/delegates.cpp
	mvf/modelcolumn.cpp
	mvf/models.cpp
	mvf/delegates/driverparams_delegate.cpp
	mvf/delegates/logbook_delegate.cpp
	mvf/delegates/site_tiledelegate.cpp
	mvf/delegates/tiledelegate.cpp
	mvf/models/dive_model.cpp
	mvf/models/drivermodels_model.cpp
	mvf/models/driverparams_model.cpp
	mvf/models/logbook_item.cpp
	mvf/models/logbook_model.cpp
	mvf/models/site_model.cpp
	mvf/models/sys/udevserialportmodel.cpp
	mvf/views/computer_view.cpp
	mvf/views/dive_editpanel.cpp
	mvf/views/dive_profileview.cpp
	mvf/views/dive_stackedview.cpp
	mvf/views/profile_alarmitem.cpp
	mvf/views/profile_plot.cpp
	mvf/views/profile_table.cpp
	mvf/views/profile_view.cpp
	mvf/views/site_editpanel.cpp
	mvf/views/site_mapview.cpp
	mvf/views/site_stackedview.cpp
	util/qcustomplot.cpp
	util/qticonloader.cpp
	util/units.cpp
	wizards/addcomputerwizard.cpp
	wizards/addcomputer/configpage.cpp
	wizards/addcomputer/intropage.cpp
	workers/transferworker.cpp
)

# C/C++ Header Files for moc
set( BENTHOS_MOC_HDRS
	mainwindow.hpp
	controls/compositelistview.hpp
	controls/maplocationeditor.hpp
	controls/multicolumnlistview.hpp
	controls/navtree.hpp
	controls/propertygrid.hpp
	controls/quantityedit.hpp
	controls/ratingedit.hpp
	controls/searchedit.hpp
	controls/stackedview.hpp
	controls/tileview.hpp
	controls/tileviewsorter.hpp
	controls/wizardcombobox.hpp
	dialogs/driverparamsdialog.hpp
	dialogs/modeladddialog.hpp
	dialogs/modeleditdialog.hpp
	dialogs/transferdialog.hpp
	mvf/models.hpp
	mvf/models/sys/udevserialportmodel.hpp
	mvf/views/computer_view.hpp
	mvf/views/dive_editpanel.hpp
	mvf/views/dive_profileview.hpp
	mvf/views/dive_stackedview.hpp
	mvf/views/profile_alarmitem.hpp
	mvf/views/profile_plot.hpp
	mvf/views/profile_view.hpp
	mvf/views/site_editpanel.hpp
	mvf/views/site_mapview.hpp
	mvf/views/site_stackedview.hpp
	util/qcustomplot.h
	wizards/addcomputerwizard.hpp
	wizards/addcomputer/configpage.hpp
	wizards/addcomputer/intropage.hpp
	workers/transferworker.hpp
)

# Resource Files
set( BENTHOS_RCS
	${CMAKE_SOURCE_DIR}/res/flags.qrc
	${CMAKE_SOURCE_DIR}/res/graphics.qrc
	${CMAKE_SOURCE_DIR}/res/icons.qrc
	${CMAKE_SOURCE_DIR}/res/mapview.qrc
)

# Enable Warnings, C++0x, etc.
add_definitions( -Wall -g )

# Configure Header
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/config.hpp @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Boost Headers Required
find_package( Boost 1.45 REQUIRED COMPONENTS filesystem locale system )

include_directories(
	${Boost_INCLUDE_DIR}
)

# Find UDev
find_path( UDEV_INCLUDE_DIR libudev.h )
find_library( UDEV_LIBS udev )
set( UDEV_FOUND false )
if( UDEV_INCLUDE_DIR AND UDEV_LIBS )
	set( UDEV_FOUND true )
endif( UDEV_INCLUDE_DIR AND UDEV_LIBS )

include_directories(${UDEV_INCLUDE_DIR})

# Find liblogbook
find_package(PkgConfig)
if (NOT PKG_CONFIG_FOUND)
	message(FATAL ERROR "pkg-config was not found.  Please install pkg-config before building ${BENTHOS_APP_NAME}")
endif (NOT PKG_CONFIG_FOUND)

pkg_check_modules(BENTHOS_LOGBOOK REQUIRED benthos-logbook)
if (NOT BENTHOS_LOGBOOK_FOUND)
	message(FATAL_ERROR "benthos-logbook not found.  Please install benthos-logbook before building ${BENTHOS_APP_NAME}")
endif (NOT BENTHOS_LOGBOOK_FOUND)

pkg_check_modules(BENTHOS_DC REQUIRED benthos-dc)
if (NOT BENTHOS_DC_FOUND)
	message(FATAL_ERROR "benthos-dc not found.  Please install benthos-dc before building ${BENTHOS_APP_NAME}")
endif (NOT BENTHOS_DC_FOUND)

add_definitions(${BENTHOS_LOGBOOK_CFLAGS_OTHER} ${BENTHOS_DC_CFLAGS_OTHER})
include_directories(${BENTHOS_LOGBOOK_INCLUDE_DIRS} ${BENTHOS_DC_INCLUDE_DIRS})

# Include QtNetwork and QtWebKit
set( QT_USE_QTNETWORK TRUE )
set( QT_USE_QTWEBKIT TRUE )

# Find Qt4
find_package( Qt4 REQUIRED )

# Setup Qt4
include( ${QT_USE_FILE} )

qt4_add_resources( BENTHOS_RC_SRCS ${BENTHOS_RCS} )
qt4_wrap_cpp( BENTHOS_MOC_SRCS ${BENTHOS_MOC_HDRS} )
include_directories( ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )

# Build Benthos
add_executable( benthos 
	${BENTHOS_SRCS} 
	${BENTHOS_MOC_SRCS} 
	${BENTHOS_RC_SRCS} 
)

# Link Benthos
target_link_libraries( benthos 
	${QT_LIBRARIES} 
	${Boost_LIBRARIES}
	${BENTHOS_LOGBOOK_LDFLAGS} 
	${BENTHOS_DC_LDFLAGS}
	${UDEV_LIBS}
)