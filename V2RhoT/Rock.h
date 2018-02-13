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
#ifndef ROCK_H_
#define ROCK_H_

#include <QtGui>
#include <iostream>   //cout
#include <iomanip>    //setwidth for cout
#include <stdlib.h>   //exit
#include "ANSIICodes.h"
#include "MineraldRhodT.h"
#include "PhysicalConstants.h"

class Rock {
private:
  // Variables
  bool verbose;
  int AlphaMode;
  QString MineralPropertyDB;
  MineraldRhodT * dRhodT; // Table that stores dRho/dT(T)
  QList <double> Composition;
  QList <double> minerals_rho;
  QList <double> minerals_rhoXFe;
  QList <double> minerals_K;
  QList <double> minerals_mu;
  QList <double> minerals_K_PT;
  QList <double> minerals_mu_PT;
  QList <double> minerals_dKdT;
  QList <double> minerals_dKdP;
  QList <double> minerals_dKdPdX;
  QList <double> minerals_dmudT;
  QList <double> minerals_dmudP;
  QList <double> minerals_drhodX;
  QList <double> minerals_dKdX;
  QList <double> minerals_dmudX;
  QList <double> minerals_alpha0;
  QList <double> minerals_alpha1;
  QList <double> minerals_alpha2;
  QList <double> minerals_alpha3;
  QList <double> minerals_alpha_T;
  QList <double> minerals_drhodT;
  double rock_XFe;
  double c_T0, c_P0;
  // P/T independent sums of anharmonic dV/dT
  double anh_sum1, anh_sum2, anh_mu_reuss;
  double dT_alpha;               // dT used to calculate drho/dT(T)
  bool UseCustomComposition;     // TRUE if custom rock composition is used

  // Anelasticity parameters
  QString Qmode;
  double c_a, c_A, c_H, c_V, c_omega;

  // P/T independent parameters for calculation of d<mu>/dT for s-waves
  double rock_dmudT;
  double rock_dKdT;
  double rock_mu_reuss;

  // P/T dependent parameters
  double rock_T;
  double rock_P;
  double rock_K_PT;
  double rock_mu_PT;
  double rock_rho_PT;
  double rock_Qmu_T;
  double rock_QP_T;
  double rock_dMdT_PT;
  double rock_Vsyn_PT;
  double rock_dVdTsyn_PT;
  double rock_drhodT_T;

  // Functions
  void printline(int width, QString title, QString unit,
                 QList <double> object);
  void alpha();
  void K_PT();
  void mu_PT();
  void rho_PT();
  void Qmu_T();
  void QP_T();
  void drhodT_T();
  void dmudT();
  void dMdT();
  void Vsyn_PT(QString VelType);
  void dVdTsyn_PT(QString VelType);

public:
  Rock();
  ~Rock();
  void printProperties();
  void printComposition();
  void writedRdT();
  void setVerbose(bool val){verbose=val;}

  // Defining properties
  bool set_AlphaMode(int mode);
  bool set_MineralPropertyDB(QString db);
  bool set_XFe(double val);
  void set_T0(double val){c_T0=val;}
  void set_P0(double val){c_P0=val;}
  void set_omega(QString VelType);
  void set_omega(double f){c_omega=2.*M_PI*f;}
  void set_Comp_init(double Ol, double Opx, double Cpx, double Sp,
                     double Gnt);
  void set_Comp(double Ol, double Opx, double Cpx, double Sp, double Gnt);
  void set_Comp(int i);
  void set_Rho(double Ol, double Opx, double Cpx, double Sp, double Gnt);
  void set_K(double Ol, double Opx, double Cpx, double Sp, double Gnt);
  void set_mu(double Ol, double Opx, double Cpx, double Sp, double Gnt);
  void set_dKdT(double Ol, double Opx, double Cpx, double Sp, double Gnt);
  void set_dKdP(double Ol, double Opx, double Cpx, double Sp, double Gnt);
  void set_dKdPdX(double Ol, double Opx, double Cpx, double Sp, double Gnt);
  void set_dmudT(double Ol, double Opx, double Cpx, double Sp, double Gnt);
  void set_dmudP(double Ol, double Opx, double Cpx, double Sp, double Gnt);
  void set_drhodX(double Ol, double Opx, double Cpx, double Sp, double Gnt);
  void set_dKdX(double Ol, double Opx, double Cpx, double Sp, double Gnt);
  void set_dmudX(double Ol, double Opx, double Cpx, double Sp, double Gnt);
  void set_alpha0(double Ol, double Opx, double Cpx, double Sp, double Gnt);
  void set_alpha1(double Ol, double Opx, double Cpx, double Sp, double Gnt);
  void set_alpha2(double Ol, double Opx, double Cpx, double Sp, double Gnt);
  void set_alpha3(double Ol, double Opx, double Cpx, double Sp, double Gnt);

  // Calculating rock properties
  bool calc_prop_PT(double Pressure, double Temperature, QString VelType);
  bool calc_prop(QString VelType);
  void fill_minerals_drhodT();
  bool setQ(int mode);

  // Obtaining values
  int get_AlphaMode(){return AlphaMode;}
  QString get_AlphaModeStr();
  QString get_MineralPropertyDB(){return MineralPropertyDB;}
  double get_Vsyn_PT(){return rock_Vsyn_PT;}
  double get_dVdTsyn_PT(){return rock_dVdTsyn_PT;}
  double getComposition(int idx){return Composition[idx];}
  double getRho(){return rock_rho_PT;}
  double getOmega(){return c_omega;}
  QString getQ(){return Qmode;}
  double getXFe(){return rock_XFe;}
  bool CustomComposition(){return UseCustomComposition;}
};

#endif // ROCK_H_
