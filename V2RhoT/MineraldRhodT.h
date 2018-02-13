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
#ifndef MINERALDRHODT_H_
#define MINERALDRHODT_H_

#include <QtGui>
#include <iostream>
#include "ANSIICodes.h"

class MineraldRhodT {
/**
This class hosts the mineral property dRho/dT
It must be calculated numerically to avoid errors far away from the reference
temperature. The values will be stored in arrays.
**/
private:
  // Variables
  double Tmin, Tmax;
  QList <QList <double> > vals;
  QList <double> alpha0, alpha1, alpha2, alpha3;
  int AlphaMode;
  // Functions
  void set_alpha(int idx, double Ol, double Opx, double Cpx, double Sp,
                 double Gnt);
  void fill();             // Calculate dRho/dT tables

public:
  MineraldRhodT();
  bool set_AlphaMode(int mode);
  double dRhodT(double T, int mineral); // Returns dRhodT at T
  void exportTable();
};

#endif // MINERALDRHODT_H_
