#------------------------------------------------------------------------------
# Master CMake File for the Benthos Dive Log Application (benthos)
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

cmake_minimum_required( VERSION 2.6 )
project( benthos )

# Application Version Information
set(BENTHOS_MAJOR_VERSION 0)
set(BENTHOS_MINOR_VERSION 1)
set(BENTHOS_PATCH_VERSION 1)
set(BENTHOS_VERSION
	${BENTHOS_MAJOR_VERSION}.${BENTHOS_MINOR_VERSION}.${BENTHOS_PATCH_VERSION})

# Application Name/Copyright Info
set(BENTHOS_APP_TITLE "Benthos")
set(BENTHOS_ORG_TITLE "Asymworks, LLC")
set(BENTHOS_COPYRIGHT "Copyright (c) 2011-2013 Asymworks, LLC.  All Rights Reserved.")

# Qt Settings Information
set(BENTHOS_APP_NAME benthos)
set(BENTHOS_APP_VERSION ${BENTHOS_VERSION})
set(BENTHOS_ORG_NAME asymworks)
set(BENTHOS_ORG_DOMAIN asymworks.com)

# Benthos Logbook Information
set(BENTHOS_DB_CREATOR ${BENTHOS_APP_NAME})
set(BENTHOS_DB_VERSION 1)

# Directory Information
SET(BENTHOS_DATADIR		"${CMAKE_INSTALL_PREFIX}/share/benthos")

# Run Subdirectories
add_subdirectory( src )

# Install top-level files
install(FILES ${CMAKE_SOURCE_DIR}/LICENSE DESTINATION share/benthos/)
install(FILES ${CMAKE_SOURCE_DIR}/README DESTINATION share/benthos/)