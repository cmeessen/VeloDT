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
#include "Rock.h"

#define _USE_MATH_DEFINES
using namespace std;

Rock::Rock() {
  /**
  Rock object contains lists with 5 items
  The items per list define properties for:
  1 - Olivine
  2 - Orthopyroxene
  3 - Clinopyroxene
  4 - Spinel
  5 - Garnet
  **/
  UseCustomComposition = false;
  verbose = false;

  // Create dRho/dT tables
  dRhodT = new MineraldRhodT;

  /*
  Define how alpha is Calculated
  0 - Alpha = const
  1 - Alpha(T)
  2 - Alpha(P,T) ? Can be implemented ?
  */
  set_AlphaMode(0);

  // Generate property lists
  // Lists with _PT refer to pressure and temperature dependent properties
  for(int i=0; i<5; i++) {
    Composition.append(0.0);
    minerals_rho.append(0.0);
    minerals_rhoXFe.append(0.0);
    minerals_K.append(0.0);
    minerals_mu.append(0.0);
    minerals_K_PT.append(0.0);
    minerals_mu_PT.append(0.0);
    minerals_dKdT.append(0.0);
    minerals_dKdP.append(0.0);
    minerals_dKdPdX.append(0.0);
    minerals_dmudT.append(0.0);
    minerals_dmudP.append(0.0);
    minerals_drhodX.append(0.0);
    minerals_dKdX.append(0.0);
    minerals_dmudX.append(0.0);
    minerals_alpha0.append(0.0);
    minerals_alpha1.append(0.0);
    minerals_alpha2.append(0.0);
    minerals_alpha3.append(0.0);
    minerals_alpha_T.append(0.0);
  }

  // Initiate default composition
  // order is Ol, Opx, Cpx, Sp, Gnt
  set_Comp_init(0.67, 0.225, 0.045, 0.0, 0.06);
  set_XFe(0.0);

  // Mineral database
  set_MineralPropertyDB("Cammarano");
  set_T0(273.15);
  set_P0(0.0);

  // Anelasticity after Sobolev et al. (1996)
  Qmode = "Sobolev et al. (1996)";
  c_a = 0.15;
  c_A = 0.148;
  c_H = 500000.;
  c_V = 0.00002;
  c_omega = 2.*M_PI*0.05;
  dT_alpha = 1.0;

}

Rock::~Rock() {
  delete dRhodT;
}

bool Rock::set_AlphaMode(int mode) {
  switch(mode) {
    case 0:
      // Alpha = const.
      AlphaMode = mode;
      dRhodT->set_AlphaMode(mode);
      return true;
    case 1:
      // Alpha(T)
      AlphaMode = mode;
      dRhodT->set_AlphaMode(mode);
      return true;
    case 2:
      // Alpha(P,T)
      cout << PRINT_ERROR "Alpha(P,T) not implemented" << endl;
      return false;
    default:
      cout << PRINT_ERROR "Wrong alpha mode " << mode << endl;
      return false;
  }
}

bool Rock::set_MineralPropertyDB(QString db) {
  if(db=="Goes") {
    // Original database by Goes et al. (2000)
    set_Rho(3222.0, 3198.0, 3280.0, 3578.0, 3565.0);
    set_drhodX(1182.0, 804.0, 377.0, 702.0, 758.0);
    set_K(1.29E+11, 1.11E+11, 1.05E+11, 1.98E+11, 1.73E+11);
    set_dKdT(-1.60E+07, -1.20E+07, -1.30E+07, -2.80E+07, -2.10E+07);
    set_dKdP(4.2, 6.0, 6.2, 5.7, 4.9);
    set_dKdPdX(0, 0, -1.9, 0, 0);
    set_dKdX(0.0, -10E9, 13E9, 12E9, 7E9);
    set_mu(8.20E+10, 8.10E+10, 6.70E+10, 1.08E+11, 9.20E+10);
    set_dmudT(-1.40E+07, -1.10E+07, -1.00E+07, -1.20E+07, -1.00E+07);
    set_dmudP(1.4, 2.0, 1.7, 0.8, 1.4);
    set_dmudX(-30E9, -29E9, -6E9, -24E9, -7E9);
    set_alpha0(2.01E-5, 3.871E-5, 3.206E-5, 6.969E-5, 9.91E-6);
    set_alpha1(1.39E-8, 4.46E-9, 8.11E-9, -1.08E-9, 1.165E-8);
    set_alpha2(0.001627, 0.000343, 0.001347, -0.030799, 0.010624);
    set_alpha3(-0.338, -1.7278, -1.8167, 5.0395, -2.5);
    MineralPropertyDB = "Goes et al. (2000)";
    return true;
  } else if(db=="Cammarano") {
    // Newer database by Cammarano et al. (2003)
    set_Rho(3222, 3215, 3277, 3578, 3565);
    set_drhodX(1182, 799, 380, 702, 0);
    set_K(129E9, 109E9, 105E9, 198E9, 171E9);
    set_dKdT(-17E6, -27E6, -13E6, -28E6, -19E6);
    set_dKdP(4.2, 7, 6.2, 5.7, 4.4);
    set_dKdPdX(0, 0, -1.9, 0, 0);
    set_dKdX(0, 20E9, 12E9, 12E9, 0);
    set_mu(81E9, 75E9, 67E9, 108E9, 92E9);
    set_dmudT(-14E6, -12E6, -10E6, -12E6, -10E6);
    set_dmudP(1.4, 1.6, 1.7, 0.8, 1.4);
    set_dmudX(-31E9, 10E9, -6E9, -24E9, 0);
    set_alpha0(0.0000201, 0.000044135, 0.000053, 0.00006969, 0.00000991);
    set_alpha1(1.39E-08, 6.61E-09, 5.92E-09, -1.08E-09, 1.17E-08);
    set_alpha2(0.001627, -0.00575625, -0.0122, -0.030799, 0.0106);
    set_alpha3(-0.338, -0.08385, 0.672, 5.0395, -2.5);
    MineralPropertyDB = "Cammarano et al. (2003), Sp: Goes et al. (2000)";
    return true;
  }
  return false;
}

QString Rock::get_AlphaModeStr() {
  switch(AlphaMode) {
    case 0:
      return QString("Constant");
    case 1:
      return QString("Alpha(T)");
    default:
      return QString("Undefined");
  }
}

void Rock::set_Comp_init(double Ol, double Opx, double Cpx, double Sp,
                   double Gnt) {
  if(Ol + Opx + Cpx + Sp + Gnt == 1.0) {
    Composition[0] = Ol;
    Composition[1] = Opx;
    Composition[2] = Cpx;
    Composition[3] = Sp;
    Composition[4] = Gnt;
  } else {
    cout << PRINT_ERROR "Composition sum is " << Ol + Opx + Cpx + Sp + Gnt
         << ". Must equal 1.0.\n";
    exit(1);
  }
}

void Rock::set_Comp(double Ol, double Opx, double Cpx, double Sp, double Gnt) {
  if(Ol + Opx + Cpx + Sp + Gnt == 1.0) {
    UseCustomComposition = true;
    Composition[0] = Ol;
    Composition[1] = Opx;
    Composition[2] = Cpx;
    Composition[3] = Sp;
    Composition[4] = Gnt;
  } else {
    cout << PRINT_ERROR "Composition sum is " << Ol + Opx + Cpx + Sp + Gnt
         << ". Must equal 1.0.\n";
    exit(1);
  }
}

void Rock::set_Comp(int i) {
  if(i == 0) {
    // Default composition after Goes et al (2000)
    set_Comp(0.67, 0.225, 0.045, 0.0, 0.06);
  } else if(i == 1) {
    // On-cratonic Shapiro and Ritzwoller (2004)
    set_Comp(0.83, 0.15, 0.0, 0.0, 0.02);
  } else if(i == 2) {
    // Off-cratonic Shapiro and Ritzwoller (2004)
    set_Comp(0.68, 0.18, 0.11, 0.0, 0.03);
  } else if(i == 3) {
    // Oceanic Shapiro and Ritzwoller (2004)
    set_Comp(0.75, 0.21, 0.035, 0.005, 0.0);
  } else {
    cout << PRINT_ERROR "Unknown composition " << i << "\n. Exit.\n";
    exit(1);
  }
}

void Rock::set_K(double Ol, double Opx, double Cpx, double Sp, double Gnt) {
  minerals_K[0] = Ol;
  minerals_K[1] = Opx;
  minerals_K[2] = Cpx;
  minerals_K[3] = Sp;
  minerals_K[4] = Gnt;
}

void Rock::set_mu(double Ol, double Opx, double Cpx, double Sp, double Gnt) {
  minerals_mu[0] = Ol;
  minerals_mu[1] = Opx;
  minerals_mu[2] = Cpx;
  minerals_mu[3] = Sp;
  minerals_mu[4] = Gnt;
}

void Rock::set_dKdT(double Ol, double Opx, double Cpx, double Sp, double Gnt) {
  minerals_dKdT[0] = Ol;
  minerals_dKdT[1] = Opx;
  minerals_dKdT[2] = Cpx;
  minerals_dKdT[3] = Sp;
  minerals_dKdT[4] = Gnt;
}

void Rock::set_dKdP(double Ol, double Opx, double Cpx, double Sp, double Gnt) {
  minerals_dKdP[0] = Ol;
  minerals_dKdP[1] = Opx;
  minerals_dKdP[2] = Cpx;
  minerals_dKdP[3] = Sp;
  minerals_dKdP[4] = Gnt;
}

void Rock::set_dKdPdX(double Ol, double Opx, double Cpx, double Sp, double Gnt) {
  minerals_dKdPdX[0] = Ol;
  minerals_dKdPdX[1] = Opx;
  minerals_dKdPdX[2] = Cpx;
  minerals_dKdPdX[3] = Sp;
  minerals_dKdPdX[4] = Gnt;
}

void Rock::set_dmudT(double Ol, double Opx, double Cpx, double Sp, double Gnt) {
  minerals_dmudT[0] = Ol;
  minerals_dmudT[1] = Opx;
  minerals_dmudT[2] = Cpx;
  minerals_dmudT[3] = Sp;
  minerals_dmudT[4] = Gnt;
}

void Rock::set_dmudP(double Ol, double Opx, double Cpx, double Sp, double Gnt) {
  minerals_dmudP[0] = Ol;
  minerals_dmudP[1] = Opx;
  minerals_dmudP[2] = Cpx;
  minerals_dmudP[3] = Sp;
  minerals_dmudP[4] = Gnt;
}

void Rock::set_drhodX(double Ol, double Opx, double Cpx, double Sp, double Gnt) {
  minerals_drhodX[0] = Ol;
  minerals_drhodX[1] = Opx;
  minerals_drhodX[2] = Cpx;
  minerals_drhodX[3] = Sp;
  minerals_drhodX[4] = Gnt;
}

void Rock::set_dKdX(double Ol, double Opx, double Cpx, double Sp, double Gnt) {
  minerals_dKdX[0] = Ol;
  minerals_dKdX[1] = Opx;
  minerals_dKdX[2] = Cpx;
  minerals_dKdX[3] = Sp;
  minerals_dKdX[4] = Gnt;
}

void Rock::set_dmudX(double Ol, double Opx, double Cpx, double Sp, double Gnt) {
  minerals_dmudX[0] = Ol;
  minerals_dmudX[1] = Opx;
  minerals_dmudX[2] = Cpx;
  minerals_dmudX[3] = Sp;
  minerals_dmudX[4] = Gnt;
}

void Rock::set_alpha0(double Ol, double Opx, double Cpx, double Sp, double Gnt) {
  minerals_alpha0[0] = Ol;
  minerals_alpha0[1] = Opx;
  minerals_alpha0[2] = Cpx;
  minerals_alpha0[3] = Sp;
  minerals_alpha0[4] = Gnt;
}

void Rock::set_alpha1(double Ol, double Opx, double Cpx, double Sp, double Gnt) {
  minerals_alpha1[0] = Ol;
  minerals_alpha1[1] = Opx;
  minerals_alpha1[2] = Cpx;
  minerals_alpha1[3] = Sp;
  minerals_alpha1[4] = Gnt;
}

void Rock::set_alpha2(double Ol, double Opx, double Cpx, double Sp, double Gnt) {
  minerals_alpha2[0] = Ol;
  minerals_alpha2[1] = Opx;
  minerals_alpha2[2] = Cpx;
  minerals_alpha2[3] = Sp;
  minerals_alpha2[4] = Gnt;
}

void Rock::set_alpha3(double Ol, double Opx, double Cpx, double Sp, double Gnt) {
  minerals_alpha3[0] = Ol;
  minerals_alpha3[1] = Opx;
  minerals_alpha3[2] = Cpx;
  minerals_alpha3[3] = Sp;
  minerals_alpha3[4] = Gnt;
}

void Rock::set_omega(QString VelType) {
  if (VelType == "S") {
    set_omega(1);
  } else {
    set_omega(0.02);
  }
}

void Rock::set_omega(double f) {
  c_frequency = f;
  c_omega = 2.*M_PI*c_frequency;
}

bool Rock::setQ(int mode) {
  // Defines which Q-mode to be used
  if(mode == 1) {
    Qmode = "Sobolev et al. (1996)";
    c_a = 0.15;
    c_A = 0.148;
    c_H = 500000;
    c_V = 0.00002;
    cout << endl
         << "Using anelastic parameters according to Sobolev et a. (1996).\n";
    return true;
  } else if(mode == 2) {
    Qmode = "Berckhemer et al. (1982)";
    c_a = 0.25;
    c_A = 0.0002;
    c_H = 584000;
    c_V = 0.000021;
    cout << endl
         << "Using anelastic paramters according to Berckhemer et al. (1982).\n";
    return true;
  } else {
    cout << PRINT_ERROR "Wrong parameter for Q-mode: " << mode
         << ". Must be 1 or 2.\n";
    return false;
  }
}

void Rock::set_Rho(double Ol, double Opx, double Cpx, double Sp, double Gnt) {
  minerals_rho[0] = Ol;
  minerals_rho[1] = Opx;
  minerals_rho[2] = Cpx;
  minerals_rho[3] = Sp;
  minerals_rho[4] = Gnt;
}

void Rock::printline(int width, QString title, QString unit,
                     QList <double> object) {
  cout << left << setw(width) << setfill(' ') << title.toUtf8().data();
  cout << left << setw(10) << setfill(' ') << unit.toUtf8().data();
  for(int i=0; i<5; i++) {
    cout << right << setw(10) << setfill(' ') << object[i];
  }
  cout << endl;
}

void Rock::printComposition() {
  QList <QString> header;
  int width = 15;
  header << ("Ol");
  header << ("Opx");
  header << ("Cpx");
  header << ("Sp");
  header << ("Gnt");

  cout << endl;
  cout << left << setw(width) << setfill(' ') << "Property";
  cout << left << setw(10) << setfill(' ') << "Unit";
  for(int i=0; i<5; i++) {
    cout << right << setw(10) << setfill(' ') << header[i].toUtf8().data();
  }
  cout << endl;
  cout << setw(75) << setfill('-') << "" << endl;
  printline(width, "Composition", "rel.", Composition);
  cout << endl;
}

void Rock::writedRdT() {
  dRhodT->exportTable();
  exit(0);
}

void Rock::printProperties() {
  QList <QString> header;
  int width=15;
  header << ("Ol");
  header << ("Opx");
  header << ("Cpx");
  header << ("Sp");
  header << ("Gnt");

  cout << endl
       << "Mineral properties from " << MineralPropertyDB.toUtf8().data()
       << endl
       << endl
       << left << setw(width) << setfill(' ') << "Property"
       << left << setw(10) << setfill(' ') << "Unit";
  for(int i=0; i<5; i++) {
    cout << right << setw(10) << setfill(' ') << header[i].toUtf8().data();
  }
  cout << endl;
  cout << setw(75) << setfill('-') << "" << endl;
  printline(width, "Composition", "rel.", Composition);
  printline(width, "Density", "kg/m3", minerals_rho);
  printline(width, "K", "Pa", minerals_K);
  printline(width, "mu", "Pa", minerals_mu);
  printline(width, "dK/dT", "Pa/K", minerals_dKdT);
  printline(width, "dmu/dT", "Pa/K", minerals_dmudT);
  printline(width, "dK/dP", "Pa/Pa", minerals_dKdP);
  printline(width, "dK/dP/dX", "Pa/Pa", minerals_dKdPdX);
  printline(width, "dmu/dP", "Pa/Pa", minerals_dmudP);
  printline(width, "dRho/dX", "kg/m3", minerals_drhodX);
  printline(width, "dK/dX", "Pa/K", minerals_dKdX);
  printline(width, "dmu/dX", "Pa/K", minerals_dmudX);
  printline(width, "alpha0", "K^-1", minerals_alpha0);
  printline(width, "alpha1", "K^-2", minerals_alpha1);
  printline(width, "alpha2", "", minerals_alpha2);
  printline(width, "alpha3", "K", minerals_alpha3);
  cout << endl;
  exit(0);
}

void Rock::alpha() {
  // Calculates alpha(rock_T)
  if(verbose){
    cout << endl << "Calculate alpha(T)" << endl
         << "> AlphaMode " << AlphaMode << endl;
  }
  switch(AlphaMode) {
    case 0:
      if(verbose){cout << "> alpha=const." << endl;}
      for(int i=0; i<5; i++) {
        minerals_alpha_T[i] = minerals_alpha0[i];
      }
      break;
    case 1:
      if(verbose){cout << "> alpha(T)" << endl;}
      for(int i=0; i<5; i++) {
        minerals_alpha_T[i] = minerals_alpha0[i] + minerals_alpha1[i]*rock_T
        + minerals_alpha2[i]/rock_T + minerals_alpha3[i]/rock_T/rock_T;
      }
      break;
  }
}

bool Rock::set_XFe(double val) {
  // Set iron content of rock
  rock_XFe = val;
  return true;
}

void Rock::rho_PT() {
  //Sets VRH average rock density
  QList <double> rho_minerals_PT;
  double rho_avrg_PT;
  // Calculate rho(P,T) for each mineral
  rho_avrg_PT = 0.0;
  if(verbose){cout << endl << "Calculate rho_PT" << endl;}
  for(int i=0; i<5; i++) {
    if(verbose){
      cout << "Mineral index i     " << i << endl
           << "> minerals_rhoXFe[i]  " << minerals_rhoXFe[i] << endl
           << "> minerals_alpha_T[i] " << minerals_alpha_T[i] << endl
           << "> rock_T              " << rock_T << endl
           << "> c_T0                " << c_T0 << endl
           << "> rock_P              " << rock_P << endl
           << "> c_P0                " << c_P0 << endl
           << "> minerals_K[i]       " << minerals_K[i] << endl
           << "> Composition[i]      " << Composition[i] << endl
           << "> rho_minerals_PT[i]  " << rho_minerals_PT[i] << endl;
    }
    rho_minerals_PT.append(minerals_rhoXFe[i]*(1. - minerals_alpha_T[i]*(rock_T
      - c_T0) + (rock_P - c_P0)/minerals_K_PT[i]));
    rho_avrg_PT = rho_avrg_PT + Composition[i]*rho_minerals_PT[i];
  }
  rock_rho_PT = rho_avrg_PT;
}

void Rock::mu_PT() {
  //Returns VRH averaged rock property <mu>
  double mu_voigt, mu_reuss;
  if(verbose){cout << endl << "Calculate mu(P,T)\n";}
  // Calculate mu(P,T) for each mineral
  for(int i=0; i<5; i++) {
    minerals_mu_PT[i] = minerals_mu[i] + (rock_T - c_T0)*minerals_dmudT[i]
                        + (rock_P - c_P0)*minerals_dmudP[i]
                        + rock_XFe*minerals_dmudX[i];
    if(verbose){
      cout << "> mu[" << i << "]:           " << minerals_mu_PT[i] << endl;
    }
  }
  // Calculate mu_voigt and mu_reuss
  mu_voigt = 0.0;
  mu_reuss = 0.0;
  for(int i=0; i<5; i++) {
    mu_voigt = mu_voigt + Composition[i]*minerals_mu_PT[i];
    mu_reuss = mu_reuss + Composition[i]/minerals_mu_PT[i];
  }
  mu_reuss = 1./mu_reuss;
  rock_mu_reuss = mu_reuss;
  rock_mu_PT = (mu_voigt + mu_reuss)/2.;
}

void Rock::K_PT() {
  // Returns VRH averaged rock property <K>
  double K_voigt, K_reuss;

  if(verbose){cout << endl << "Calculate K(P,T)\n";}
  // Calculate K(P,T) for each mineral
  for(int i=0; i<5; i++) {
    minerals_K_PT[i] = minerals_K[i] + (rock_T - c_T0)*minerals_dKdT[i]
                       + (rock_P - c_P0)*(minerals_dKdP[i]
                       + rock_XFe*minerals_dKdPdX[i])
                       + rock_XFe*minerals_dKdX[i];
    if(verbose){
      cout << "K[" << i << "]:            " << minerals_K_PT[i] << endl;
    }
  }

  // Calculate K_voigt and K_reuss
  K_voigt = 0.0;
  K_reuss = 0.0;

  for(int i=0; i<5; i++) {
    K_voigt = K_voigt + Composition[i]*minerals_K_PT[i];
    K_reuss = K_reuss + Composition[i]/minerals_K_PT[i];
  }

  K_reuss = 1./K_reuss;
  rock_K_PT = (K_voigt + K_reuss)/2.;
}

void Rock::Qmu_T() {
  // Calculate Q_mu, Eqn A6
  double val, Estar;
  if(verbose){cout << endl << "Calculate Qmu_T" << endl;}
  Estar = c_H + rock_P*c_V;
  val = c_A*pow(c_omega, c_a)*exp(c_a*Estar/c_R/rock_T);
  if(verbose){cout << "> Qmu(T) = " << val << endl;}
  rock_Qmu_T = val;
}

void Rock::QP_T() {
  // Calculate Q_P, Eqn A7
  double val, Estar, L;
  if(verbose){cout << endl << "Calculate QP_T" << endl;}
  Estar = c_H + rock_P*c_V;
  val = c_A*pow(c_omega,c_a)*exp(c_a*Estar/c_R/rock_T);
  L = 4.*rock_mu_PT/(3.*rock_K_PT + 4.*rock_mu_PT);
  rock_QP_T = rock_Qmu_T/L;
  if(verbose){cout << "> QP(T) = " << rock_QP_T << endl;}
}

void Rock::dmudT() {
  double anh_mu_reuss = 0;
  if(verbose){cout << endl << "Calculate dmudT" << endl;}
  for(int i=0; i<5; i++) {
    anh_mu_reuss = anh_mu_reuss + Composition[i]/minerals_mu_PT[i];
  }
  anh_mu_reuss = 1./anh_mu_reuss;
  rock_dmudT = anh_sum1 + anh_sum2/pow(anh_mu_reuss, 2);
}

void Rock::dMdT() {
  // Calculate d<M>/dT (P,T) for P-waves, Eqn. A5b
  double M_reuss = 0.0;
  anh_sum2 = 0.0;
  if(verbose){cout << "Calculate d<M>/dT\n";}
  for(int i=0; i<5; i++) {
    M_reuss = M_reuss + Composition[i]/(minerals_K_PT[i] + + 4./3*minerals_mu_PT[i]);
    anh_sum2 = anh_sum2 + (Composition[i]/(minerals_K_PT[i]
                           + 4./3*minerals_mu_PT[i])*(minerals_dKdT[i]
                           + 4./3*minerals_dmudT[i]));
  }
  M_reuss = 1./M_reuss;

  if(verbose) {
    cout << "M_reuss:          " << M_reuss << endl
         << "anh_sum1:         " << anh_sum1 << endl
         << "anh_sum2:         " << anh_sum2 << endl;
  }

  rock_dMdT_PT = anh_sum1 + pow(M_reuss, -2)*anh_sum2;
}

void Rock::Vsyn_PT(QString VelType) {
  // Calculate synthethic velocity, Eqn A8
  double Vanh, Vanel;

  if(verbose){cout << endl << "Calculate Vsyn_PT:\n";}
  if(VelType=="S") {
    Vanh = sqrt(rock_mu_PT/rock_rho_PT);
    Vanel = 1. - 2./rock_Qmu_T/tan(M_PI*c_a/2.);
  } else {
    if(verbose){cout << "VelType:         P\n";}
    Vanh = sqrt((rock_K_PT + 4./3*rock_mu_PT)/rock_rho_PT);
    Vanel = 1. - 2./rock_QP_T/tan(M_PI*c_a/2.);
  }

  if(verbose) {
    cout << "rock_K_PT:       " << rock_K_PT << endl
         << "rock_mu_PT:      " << rock_mu_PT << endl
         << "rock_rho_PT:     " << rock_rho_PT << endl
         << "Vanh:            " << Vanh << endl
         << "Vanel:           " << Vanel << endl;
  }

  rock_Vsyn_PT = Vanh*Vanel;
}

void Rock::drhodT_T() {
  // Calculate change of density with temperature
  double result;
  QList <double> mineral_drhodT_T;

  result = 0.;
  for(int i=0; i<5; i++) {
    mineral_drhodT_T.append(dRhodT->dRhodT(rock_T,i));
    // Calculate average drho/dT for rock
    result = result + Composition[i]*mineral_drhodT_T[i];
  }
  rock_drhodT_T = result;
}

void Rock::dVdTsyn_PT(QString VelType) {
  // Calculate dV/dT_syn, Eqn. A9 (anel) and A4 (anh)
  double dVdTanel, dVdTanh;
  if(VelType=="S") {
    dVdTanel = c_A*c_H/rock_Qmu_T/2./c_R/rock_T/rock_T/tan(M_PI*c_a/2.);
    dVdTanh = (rock_dmudT - pow(rock_Vsyn_PT, 2)*rock_drhodT_T)
              /(2.*rock_rho_PT*rock_Vsyn_PT);
  } else {
    dVdTanel = c_A*c_H/rock_QP_T/2./c_R/rock_T/rock_T/tan(M_PI*c_a/2.);
    dVdTanh = (rock_dMdT_PT - pow(rock_Vsyn_PT,2)*rock_drhodT_T)/(2.*rock_rho_PT*rock_Vsyn_PT);
  }

  if(verbose) {
    cout << "Calculate dV/dT_syn\n"
         << "dVdTanel:          " << dVdTanel << endl
         << "dVdTanh:           " << dVdTanh << endl;
  }

  rock_dVdTsyn_PT = dVdTanel + dVdTanh;
}

bool Rock::calc_prop_PT(double Pressure, double Temperature, QString VelType) {
  // Calculate all rock properties for given P/T conditions and store them in
  // the object
  rock_T = Temperature;
  rock_P = Pressure;
  alpha();
  K_PT();
  rho_PT();
  mu_PT();
  Qmu_T();

  if(VelType=="P"){ QP_T();}

  Vsyn_PT(VelType);

  if(VelType=="S"){ dmudT();}
  else            { dMdT();}

  drhodT_T();
  dVdTsyn_PT(VelType);

  if(verbose) {
    cout << endl
         << "Calculated properties:" << endl
         << "rock_T:          " << rock_T << endl
         << "rock_P:          " << rock_P << endl
         << "rock_rho_PT:     " << rock_rho_PT << endl
         << "rock_mu_PT:      " << rock_mu_PT << endl
         << "rock_K_PT:       " << rock_K_PT << endl
         << "rock_Qmu_T:      " << rock_Qmu_T << endl
         << "rock_QP_T:       " << rock_QP_T << endl
         << "rock_Vsyn_PT:    " << rock_Vsyn_PT << endl
         << "rock_dmudT:      " << rock_dmudT << endl
         << "rock_dMdT_PT:    " << rock_dMdT_PT << endl
         << "rock_drhodT_T:   " << rock_drhodT_T << endl
         << "rock_dVdTsyn_PT: " << rock_dVdTsyn_PT << endl;
  }
  return true;
}

bool Rock::calc_prop(QString VelType) {
  // Calculates P/T independent parameters for Eqn A5b

  if(verbose){
    cout << "----------------------------------------------" << endl
         << "Calculating P/T independent parameters" << endl
         << "----------------------------------------------" << endl;
  }

  // Mineral density including iron content
  for(int i=0; i<5; i++) {
    minerals_rhoXFe[i] = minerals_rho[i] + minerals_drhodX[i]*rock_XFe;
    if(verbose){
      cout << "minerals_rho[i]    " << minerals_rho[i] << endl
           << "minerals_drhodX[i] " << minerals_drhodX[i] << endl
           << "rock_XFe           " << rock_XFe << endl
           << "minerals_rhoXFe[i] " << minerals_rhoXFe[i] << endl;
    }
  }

  anh_sum1 = 0.;
  if(VelType=="S") {
    for(int i=0; i<5; i++) {
      anh_sum1 = anh_sum1 + Composition[i]*minerals_dmudT[i];
    }
  } else {
    for(int i=0; i<5; i++) {
      anh_sum1 = anh_sum1 + Composition[i]*(minerals_dKdT[i]
                 + 4./3.*minerals_dmudT[i]);
    }
  }
  if(verbose){cout << "----------------------------------------------" << endl;}
  return true;
}
