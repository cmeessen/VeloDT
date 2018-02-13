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
#ifndef V2T_H_
#define V2T_H_

#include <ctime>
#include <QString>
#include <QList>
#include <QDateTime>
#include <QStringList>
#include <stdlib.h>  //exit
#include "ANSIICodes.h"
#include "PointClasses.h"
#include "math.h"
#include "ERMs.h" // Stores ERMs
#include "PhysicalConstants.h"


class V2T {
  QString File_In;        // Input file name of Vs grid
  QString File_Out;       // Output file name

  // Data for pressure calculation
  QString PMethod;        // Defines the method used for pressure calculation
  bool use_t_crust;       // Define whether to use crustal thickness information
  bool ArbitraryPoints;   // If 'true' Vs input file is no regular point grid
  bool outVs;             // Writes VsObs and VsCalc into the output file
  QString File_z_topo;    // EarthVision file of topographic elevation
  QString File_t_crust;   // EarthVision file of crustal thickness
  double rho_crust;       // Crustal density [kg/m3]
  double rho_mantle;      // Mantle density  [kg/m3]
  double rho_avrg;        // Density used to calculate P, if use_t_crust = False
  double scaleZ;          // Mutliply all depth with this factor, default = 1
  double scaleVs;         // Multiply all vs with this factor, default = 1
  bool verbose;           // True = display parameters during calculation
  EarthReferenceModel * ERM;

  // Properties for Newton iteration method
  double threshold;       // Threshold below which Newton iteration stops [degC]

  // Constants
  double c_bV;            // 1/km
  double c_m;             // km/s/degC
  double c_c;             // km/s
  double c_A;             // km/s
  double c_E;             // kJ/mol
  double c_Va;            // m3/mol
  double c_K;             // degC to Kelvin

  // Input data properties - 1: data_Vs, 2: t_crust, 3:z_topo
  double x_min1, x_max1, y_min1, y_max1, z_min1, z_max1;
  double x_min2, x_max2, y_min2, y_max2;
  double x_min3, x_max3, y_min3, y_max3;
  int nX, nY, nZ;

  QList <Point3D> z_topo;
  QList <Point3D> t_crust;
  QList <Point4D> data_Vs;
  QList <Point4D> data_T;
  QList <double> data_Vcalc;
  QList <double> ERMz;    // List for depth values of the Earth reference model
  QList <double> ERMrho;  // List for density values of the ERM

  public:
    V2T();
    bool newton();
    bool readFile(QString InName, QString InType);
    bool saveFile(QString OutName);
    void readArgs(int &argc, char *argv[]);
    bool test_data();
    void Info();
    void usage();

    QString FileIn(){return File_In;}
    QString FileZTopo(){return File_z_topo;}
    QString FileTCrust(){return File_t_crust;}
    QString FileOut(){return File_Out;}
    bool UseCrust(){return use_t_crust;}

  private:
    void usage_extended();
    void argsError(QString val, bool ok);
    bool SetPMethod(QString method);
    double ftheta(double VsS, double P, double T);
    double dfdtheta(double P, double T);
    double pressure(double x, double y, double z);
    double pressure_simple(double z);
    double pressure_crust(double x, double y, double z);
    void WRITE_P(QString method);
};

#endif // V2T_H_
