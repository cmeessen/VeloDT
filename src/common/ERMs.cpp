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
#include "ERMs.h"

using std::cout;
using std::endl;

const int AK135_LEN = 27;
const int PREM_LEN = 107;

double ARR_AK135z[AK135_LEN] = {0, 3, 3, 3.3, 3.3, 10, 10, 18, 18, 43, 80, 80,
  120, 120, 165, 210, 210, 260, 310, 360, 410, 410, 460, 510, 560, 610, 660};
double ARR_AK135rho[AK135_LEN] = {1.02, 1.02, 2, 2, 2.6, 2.6, 2.92, 2.92, 3.641,
  3.5801, 3.502, 3.502, 3.4268, 3.4268, 3.3711, 3.3243, 3.3243, 3.3663, 3.411,
  3.4577, 3.5068, 3.9317, 3.9273, 3.9233, 3.9218, 3.9206, 3.9201};

double ARR_PREMz[PREM_LEN] = {0, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
  14, 15, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24.4, 24.4, 25, 26, 27, 28, 29,
  30, 31, 32, 33, 34, 35, 40, 45, 50, 60, 70, 80, 80, 90, 100, 110, 120, 130,
  140, 150, 160, 170, 180, 190, 200, 210, 220, 220, 230, 240, 250, 260, 270,
  280, 290, 300, 310, 320, 330, 340, 350, 360, 370, 380, 390, 400, 400, 410,
  420, 430, 440, 450, 460, 470, 480, 490, 500, 510, 520, 530, 540, 550, 560,
  570, 580, 590, 600, 600, 610, 620, 630, 640, 650, 660};
double ARR_PREMrho[PREM_LEN] = {1.02, 1.02, 1.02, 1.02, 2.6, 2.6, 2.6, 2.6, 2.6,
  2.6, 2.6, 2.6, 2.6, 2.6, 2.6, 2.6, 2.6, 2.9, 2.9, 2.9, 2.9, 2.9, 2.9, 2.9,
  2.9, 2.9, 2.9, 3.38075, 3.38068, 3.38057, 3.38047, 3.38036, 3.38025, 3.38014,
  3.38003, 3.37992, 3.37981, 3.3797, 3.3796, 3.37905, 3.37851, 3.37797, 3.37688,
  3.37579, 3.37471, 3.37471, 3.37362, 3.37253, 3.37145, 3.37036, 3.36927,
  3.36818, 3.3671, 3.36601, 3.36492, 3.36384, 3.36275, 3.36166, 3.36058,
  3.35949, 3.43577, 3.44175, 3.44772, 3.45369, 3.45966, 3.46563, 3.4716,
  3.47758, 3.48355, 3.48952, 3.49549, 3.50146, 3.50743, 3.51341, 3.51938,
  3.52535, 3.53132, 3.53729, 3.54326, 3.72375, 3.73635, 3.74895, 3.76156,
  3.77416, 3.78677, 3.79937, 3.81197, 3.82458, 3.83718, 3.84978, 3.86239,
  3.87499, 3.88759, 3.9002, 3.9128, 3.92541, 3.93801, 3.95061, 3.96322, 3.97582,
  3.97582, 3.97815, 3.98048, 3.98281, 3.98514, 3.98746, 3.98979};

EarthReferenceModel::EarthReferenceModel() {
  ERMtype = "Undefined";
}

EarthReferenceModel::EarthReferenceModel(QString type) {
  EarthReferenceModel();
  set(type);
}

bool EarthReferenceModel::INIT_AK135() {
  // Initialise AK135 from array
  for (int i = 0; i < AK135_LEN; i++) {
    ERMz.append(ARR_AK135z[i]);
    ERMrho.append(ARR_AK135rho[i]);
  }
  ERMtype = "AK135";
  return true;
}

bool EarthReferenceModel::INIT_PREM() {
  // Inititalise PREM from array
  for (int i = 0; i < PREM_LEN; i++) {
    ERMz.append(ARR_PREMz[i]);
    ERMrho.append(ARR_PREMrho[i]);
  }
  ERMtype = "PREM";
  return true;
}

double EarthReferenceModel::pressure(double z) {
  /**
  Performs linear 1D pressure calculation with the given ERM
  Input: z - Depth in m a.s.l.
  Returns: Pcalc - Pressure at z in Pa
  **/
  int i = 0;
  double Pcalc, z_abs, z1, z2, Rho1, Rho2, dz;
  bool DepthCalculated = false;
  Pcalc = 0.;
  z_abs = fabs(z);
  while (!DepthCalculated) {
    z1 = ERMz[i]*1000;
    z2 = ERMz[i+1]*1000;
    Rho1 = ERMrho[i]*1000;
    Rho2 = ERMrho[i+1]*1000;
    if ( z_abs < z2 ) {
      Rho2 = Rho1 + (z_abs - z1)/(z2 - z1)*(Rho2 - Rho1);
      dz = z_abs - z1;
      DepthCalculated = true;
    } else {
      dz = z2 - z1;
    }
    Pcalc += dz*(Rho1 + (Rho2 - Rho1)/2)*c_g;
    i += 1;
  }
  return Pcalc;
}

bool EarthReferenceModel::set(QString type) {
  // Define a reference model
  if (type == "AK135") {
    INIT_AK135();
  } else if (type == "PREM") {
    INIT_PREM();
  } else {
    std::cout << "Unknown reference model " << type.toUtf8().data() << endl;
    return false;
  }
  return true;
}

bool EarthReferenceModel::writeP() {
  return writeP(10000);
}

bool EarthReferenceModel::writeP(double dz) {
  // Write pressure values to a file
  double zmin = -200000;
  double zmax = 0;
  double nzfloat = (zmax - zmin)/dz + 1;
  cout << "Calculating pressures" << endl
       << "ERM type is " << ERMtype.toUtf8().data() << endl;
  if (fmod(nzfloat, 1) == 0.) {
    // Calculate pressures
    int nz = static_cast<int>(nzfloat);
    QVector<double> depths;
    QVector<double> pressures;
    float z;
    for (int i=0; i < nz; i++) {
      z = zmin + i*dz;
      depths.append(z);
      pressures.append(pressure(z));
    }
    // Write to file
    QString OutName = "PressureAK135.txt";
    cout << "Writing temperature file " << OutName.toUtf8().data() << endl;
    QFile tmp(OutName);
    if (!tmp.open(QIODevice::WriteOnly | QIODevice::Text)) {
      cout << PRINT_ERROR "Could not open file " << OutName.toUtf8().data()
           << endl;
      return false;
    }
    QString T_header = "# Depth/m Pressure/Pa";
    QTextStream fout(&tmp);
    fout.setRealNumberPrecision(5);
    fout.setRealNumberNotation(QTextStream::FixedNotation);
    fout << T_header.toUtf8().data() << endl;
    for (int i=0; i < nz; i++) {
      fout << depths[i] << ' ' << pressures[i] << endl;
    }
    tmp.close();
    cout << "Pressure written to" << OutName.toUtf8().data() << "." << endl;
    return true;
  } else {
    cout << PRINT_ERROR "Invalid depth interval. 200000 must be dividable by dz"
         << endl;
    return false;
  }
}
