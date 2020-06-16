/*******************************************************************************
*                     Copyright (C) 2018 by Christian Meeßen                   *
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
#ifndef T2RHO_H_
#define T2RHO_H_

#include <QList>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QRegExp>
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include "PhysicalConstants.h"
#include "PointClasses.h"
#include "ERMs.h"

class T2Rho {
  QString PMethod;
  QString file_in;
  QString file_out;
  EarthReferenceModel * ERM;
  // Mineral properties
  QList<double> m_rho;
  QList<double> m_drhodX;
  QList<double> m_K;
  QList<double> m_dKdT;
  QList<double> m_dKdP;
  QList<double> m_dKdPdX;
  QList<double> m_dKdX;
  QList<double> m_alpha0;
  QList<Point5D> data_out;
  // Rock properties
  double r_XFe;
  QList<double> r_comp;
  // Functions
  bool SetPMethod(QString method);
  void argsError(QString val, bool ok);
  void setComp(QList<double> composition);
  void setComp(int c);

 public:
  T2Rho();
  void usage();
  void info();
  void readArgs(int &argc, char *argv[]);
  void readFile();
  void writeFile();
};

#endif  // T2RHO_H_
