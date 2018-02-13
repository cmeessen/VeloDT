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
#include "MineraldRhodT.h"

using namespace std;

MineraldRhodT::MineraldRhodT() {
  AlphaMode = 0;
  Tmin = 273.;
  Tmax = 2273.;
  set_alpha(0, 0.0000201, 0.00003871, 0.00003206, 0.00006969, 0.00000991);
  set_alpha(1, 0.0000000139, 0.00000000446, 0.00000000811, -0.00000000108,
            0.00000001165);
  set_alpha(2, 0.001627, 0.000343, 0.001347, -0.030799, 0.010624);
  set_alpha(3, -0.338, -1.7278, -1.8167, 5.0395, -2.5);
  fill();
}

bool MineraldRhodT::set_AlphaMode(int mode) {
  switch(mode) {
    case 0:
      // Alpha = const.
      AlphaMode = mode;
      return true;
    case 1:
      // Alpha(T)
      AlphaMode = mode;
      return true;
    case 2:
      // Alpha(P,T)
      cout << PRINT_ERROR "Alpha(P,T) not implemented" << endl;
      return false;
    default:
      cout << PRINT_ERROR "Wrong alpha mode" << mode << endl;
      return false;
  }
}

void MineraldRhodT::set_alpha(int idx, double Ol, double Opx, double Cpx,
                              double Sp, double Gnt) {
  if(idx==0) {
    alpha0.append(Ol);
    alpha0.append(Opx);
    alpha0.append(Cpx);
    alpha0.append(Sp);
    alpha0.append(Gnt);
  } else if(idx==1) {
    alpha1.append(Ol);
    alpha1.append(Opx);
    alpha1.append(Cpx);
    alpha1.append(Sp);
    alpha1.append(Gnt);
  } else if(idx==2) {
    alpha2.append(Ol);
    alpha2.append(Opx);
    alpha2.append(Cpx);
    alpha2.append(Sp);
    alpha2.append(Gnt);
  } else if(idx==3) {
    alpha3.append(Ol);
    alpha3.append(Opx);
    alpha3.append(Cpx);
    alpha3.append(Sp);
    alpha3.append(Gnt);
  } else {
    cout << PRINT_ERROR "set_alpha() wrong idx\n";
  }
}

void MineraldRhodT::fill() {
  /**
  Calculates dRhodT for T from Tmin to Tmax in steps of 1K

  Rho_i0  - density at iteration step n
  Rho_i1  - density at iteration step n+1
  alpha_i - vol. exp. coefficient at temperature step n

  Structure of vals:

  i    [ [T_i,   dRhodT_Ol, dRrhodT_Opx, dRrhodT_Cpx, dRrhodT_Sp, dRrhodT_Gnt],
  i+1  [ [T_i+1, dRhodT_Ol, dRrhodT_Opx, dRrhodT_Cpx, dRrhodT_Sp, dRrhodT_Gnt],
  i+..   [...] ]

  Temperature starts at 273.0K
  **/
  double T_i, dT, i_max;
  QList <double> Rho_i0, Rho_i1, alpha_i, dRhodT_i, vals_T;

  dT = 1;   // DO NOT CHANGE! Temperature step is 1K
  i_max = Tmax - Tmin;

  // Fill values for every mineral after Goes et al (2000)
  // Initial values are for T=0degC=273K
  Rho_i0.append(3222.); //Ol
  Rho_i0.append(3198.); //Opx
  Rho_i0.append(3280.); //Cpx
  Rho_i0.append(3578.); //Sp
  Rho_i0.append(3565.); //Gnt

  for(int i=0; i<5; i++) {
    Rho_i1.append(0.);
    alpha_i.append(0.);
    dRhodT_i.append(0.);
    vals_T.append(0.);
  }
  vals_T.append(0.); // vals_T needs one additional item for the temperature

  // Calculate
  for(int i=0; i<i_max; i++) {
    T_i = Tmin + (double) i;
    vals_T[0] = T_i;

    for(int j=0; j<5; j++) {
      if(AlphaMode==0) {
        alpha_i[j] = alpha0[j];
      } else if(AlphaMode==1) {
        alpha_i[j] = alpha0[j] + alpha1[j]*T_i + alpha2[j]/T_i
                     + alpha3[j]/T_i/T_i;
      }
      Rho_i1[j] = Rho_i0[j]/(1. + alpha_i[j]*dT);
      vals_T[j+1] = (Rho_i1[j] - Rho_i0[j])/dT;
      Rho_i0[j] = Rho_i1[j];
    }
    vals.append(vals_T);
  }
}

double MineraldRhodT::dRhodT(double T, int mineral) {
  /**
  Returns dRho/dT at T. T must be in Kelvin!
  If T lies between two stored T-values, dRho/dT will be interpolated linearly
  int mineral:
   0 - Ol
   1 - Opx
   2 - Cpx
   3 - Sp
   4 - Gnt
  **/

  int T_low;
  double dRdT_low, dRdT_high;

  if (T < Tmin || T > Tmax) {
    cout << PRINT_WARNING "Iteration failed. Temperature out of bounds T="
         << T << " K\n";
    return 999;
  } else if ( fmod(T,1) == 0 && T < 2273.) {
    // If temperature is already in table
    return vals[(int) T - 273][mineral+1];
  } else {
    T_low = (int) (T - fmod(T,1) - 273);
    dRdT_low = vals[T_low][mineral+1];
    dRdT_high = vals[T_low+1][mineral+1];
    return (T - (double) T_low)*(dRdT_high - dRdT_low) + dRdT_low;
  }
}

void MineraldRhodT::exportTable() {
  QString OutName = "dRhodT.txt";
  QString T_header = QString("# T / K\tOl / kg/m3/K\tOpx / kg/m3/K\tCpx / "
                             "kg/m3/K\tSp / kg/m3/K\tGnt / kg/m3/K");
  cout << "Writing dRho/dT to " << OutName.toUtf8().data() << endl;
  QFile tmp(OutName);

  if(!tmp.open(QIODevice::WriteOnly | QIODevice::Text)) {
    cout << PRINT_ERROR "Could not open file " << OutName.toUtf8().data()
         << endl;
    exit(1);
  }

  QTextStream fout(&tmp);
  fout.setRealNumberPrecision(5);
  fout.setFieldAlignment(QTextStream::AlignRight);
  fout.setRealNumberNotation(QTextStream::FixedNotation);
  fout << T_header.toUtf8().data() << endl;

  for(int i=0; i<vals.length(); i++) {
    fout << vals[i][0];  //T
    fout << "\t";
    fout << vals[i][1];  //Ol
    fout << "\t";
    fout << vals[i][2];  //Opx
    fout << "\t";
    fout << vals[i][3];  //Cpx
    fout << "\t";
    fout << vals[i][4];  //Sp
    fout << "\t";
    fout << vals[i][5];  //Gnt
    fout << "\t";
    fout << endl;
  }

  tmp.close();
}
