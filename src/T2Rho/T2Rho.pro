################################################################################
#                     Copyright (C) 2018 by Christian Meeßen                   #
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
DESTDIR = ../../bin
TARGET = T2Rho
CONFIG -= app_bundle

INCLUDEPATH += ../../include/common ../../include/T2Rho

LIBS += -L../common -lcommon

SOURCES += T2Rho.cpp

HEADERS += ../../include/T2Rho/T2Rho.h
