/*******************************************************************************
*                     Copyright (C) 2017 by Christian Meeßen                   *
*                                                                              *
*                          This file is part of VeloDT.                        *
*                                                                              *
*         VeloDT is free software: you can redistribute it and/or modify       *
*     it under the terms of the GNU General Public License as published by     *
*           the Free Software Foundation version 3 of the License.             *
*                                                                              *
*        VeloDT is distributed in the hope that it will be useful, but         *
*          WITHOUT ANY WARRANTY; without even the implied warranty of          *
*       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU       *
*                   General Public License for more details.                   *
*                                                                              *
*      You should have received a copy of the GNU General Public License       *
*        along with VeloDT. If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************/
#ifndef ERMS_H_
#define ERMS_H_

#include <QFile>
#include <QList>
#include <QString>
#include <QTextStream>
#include <QVector>
#include <iostream>
#include <math.h>
#include "ANSIICodes.h"
#include "PhysicalConstants.h"
#include "PointClasses.h"

class EarthReferenceModel {
  QList <double> ERMz;
  QList <double> ERMrho;
  QString ERMtype;
  private:
    bool INIT_AK135();
    bool INIT_PREM();
  public:
    EarthReferenceModel();
    EarthReferenceModel(QString type);
    bool set(QString type);
    double pressure(double z);
    QString type(){return ERMtype;}
    bool writeP();
    bool writeP(double dz);
};

#endif // ERMS_H_
