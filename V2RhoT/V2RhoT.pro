################################################################################
#                     Copyright (C) 2017 by Christian Meeßen                   #
#                                                                              #
#                          This file is part of VeloDT.                        #
#                                                                              #
#         VeloDT is free software: you can redistribute it and/or modify       #
#     it under the terms of the GNU General Public License as published by     #
#           the Free Software Foundation version 3 of the License.             #
#                                                                              #
#        VeloDT is distributed in the hope that it will be useful, but         #
#          WITHOUT ANY WARRANTY; without even the implied warranty of          #
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU       #
#                   General Public License for more details.                   #
#                                                                              #
#      You should have received a copy of the GNU General Public License       #
#        along with VeloDT. If not, see <http://www.gnu.org/licenses/>.        #
################################################################################
TEMPLATE = app
TARGET = V2RhoT
CONFIG -= app_bundle
DEPENDPATH += .
DEFINES +=
INCLUDEPATH += . ../ ../common
LIBS +=

# Input
HEADERS += V2RhoT.h \
           Rock.h \
           MineraldRhodT.h \
           ../common/ANSIICodes.h \
           ../common/PointClasses.h \
           ../common/PhysicalConstants.h \
           ../common/ERMs.h
SOURCES += V2RhoT.cpp \
           Rock.cpp \
           MineraldRhodT.cpp \
           ../common/PointClasses.cpp \
           ../common/ERMs.cpp
RESOURCES +=
