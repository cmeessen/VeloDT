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
#ifndef V2RHOT_H_
#define V2RHOT_H_

#include <QDateTime>
#include <QStringList>
#include <ctime>
#include <cmath>
#include <stdlib.h>   //exit
#include "ANSIICodes.h"
#include "ERMs.h"
#include "math.h"
#include "PhysicalConstants.h"
#include "PointClasses.h"
#include "Rock.h"


class V2RhoT {
  QString File_In;        // Input file name of Vs grid
  QString File_Out;       // Output file name

  // Iteration properties
  double c_Fdamp;         // Dampening
  double count_avrg;      // Counts average iteration steps
  double T_start;         // Starting temperature
  double threshold;       // Threshold below which Newton iteration stops [degC]
  bool use_t_crust;       // use crustal thickness to calculate P
  bool ArbitraryPoints;   // If 'true' Vs input file is no regular point grid
  QString VelType;        // Velocity type P or S
  QString File_z_topo;    // EarthVision file of topographic elevation
  QString File_t_crust;   // EarthVision file of crustal thickness
  QString PMethod;        // String for pressure calculation method
  double rho_crust;       // Crustal density [kg/m3]
  double rho_mantle;      // Mantle density  [kg/m3]
  double rho_avrg;        // Density used to calculate P, if use_t_crust = False
  double scaleZ;          // Factor that all depth values are multiplied with
  double scaleVs;         // Factor that all Vs values are multiplied with
  bool verbose;           // True for debugging
  bool petrel;            // Output Petrel points with attributes
  Rock * MantleRock;      // The object that hosts the rock properties
  EarthReferenceModel * ERM;  // Calculates pressure from an ERM

  // Input data properties - 1: data_Vs, 2: t_crust, 3:z_topo
  double x_min1, x_max1, y_min1, y_max1, z_min1, z_max1, x_min2, x_max2,
         y_min2, y_max2, x_min3, x_max3, y_min3, y_max3;
  int nX, nY, nZ;

  QList <Point3D> z_topo;
  QList <Point3D> t_crust;
  QList <Point5D> data_V;
  QList <Point5D> data_T;

  bool SetPMethod(QString method);
  double pressure(double x, double y, double z);
  double pressure_crust(double x, double y, double z);
  double pressure_simple(double z);
  void argsError(QString val, bool ok);
  void help();

 public:
  V2RhoT();
  ~V2RhoT();
  QString FileIn() {return File_In;}
  QString FileOut() {return File_Out;}
  bool readFile(QString InName, QString InType);
  bool saveFile(QString OutName);
  void readArgs(int &argc, char *argv[]);
  void usage();
  bool Iterate();
  void Info();
};

#endif //V2RHOT_H_
