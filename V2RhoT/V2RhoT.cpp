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
#include "V2RhoT.h"

#define _USE_MATH_DEFINES
#define EOL "\n"

using namespace std;

const QString compilationTime = QString("%1 %2").arg(__DATE__).arg(__TIME__);

V2RhoT::V2RhoT() {
  ArbitraryPoints = false;
  use_t_crust = false;
  verbose = false;
  petrel = false;
  count_avrg = 0;
  c_Fdamp = 0.025;
  scaleZ = 1.;
  scaleVs = 1.;

  PMethod = "AK135";
  rho_crust = 2800.;
  rho_mantle = 3300.;
  rho_avrg = 3000.;
  threshold = 0.1;

  T_start = 273.15;
  MantleRock = new Rock;
  ERM = new EarthReferenceModel;
}

V2RhoT::~V2RhoT() {
  delete MantleRock;
  delete ERM;
}

void V2RhoT::Info() {
  cout << "******************************************************************\n"
       << endl
       << "          Seismic Velocity to Temperature Conversion\n"
       << endl
       << "******************************************************************\n"
       << "For details see Goes et al. (2000): \"Shallow mantle temperatures\n"
       << "under Europe from P and S wave tomography\"\n"
       << endl
       << "Conversion settings\n"
       << "-------------------\n"
       << "Iteration threshold: " << threshold << " K\n"
       << "AlphaMode          : " << MantleRock->get_AlphaModeStr()
                                     .toUtf8().data() << endl
       << "Mineral properties : " << MantleRock->get_MineralPropertyDB()
                                     .toUtf8().data() << endl
       << endl
       << "Input\n"
       << "-----\n"
       << "Vs                 : " << File_In.toUtf8().data() << "\n";
  if(use_t_crust){
  cout << "Topography         : " << File_z_topo.toUtf8().data() << "\n"
       << "Crustal thickness  : " << File_t_crust.toUtf8().data() << "\n";
  }
  cout << endl
       << "Output\n"
       << "------\n"
       << "Temperatures       : " << File_Out.toUtf8().data() << "\n"
       << endl;
  if(use_t_crust) {
  cout << "Densities\n"
       << "---------\n"
       << "Crust              : "<< rho_crust << " kg/m3\n"
       << "Mantle             : "<< rho_mantle << " kg/m3\n"
       << "Average            : "<< rho_avrg << " kg/m3\n"
       << endl;
  }
  cout << "Anelasticity parameters\n"
       << "-----------------------\n"
       << "Q                 : " << MantleRock->getQ().toUtf8().data() << endl
       << endl;

  if(MantleRock->CustomComposition() && !verbose) {
    cout << "Using custom rock composition\n"
         << "-----------------------------\n";
    MantleRock->printComposition();
  }
  if(verbose){
    cout << "Mantle rock composition" << endl
         << "-----------------------" << endl;
    MantleRock->printComposition();
  }

  cout << "Other\n"
       << "-----\n"
       << "Velocity type     : " << VelType.toUtf8().data() << endl
       << "Omega             : " << MantleRock->getOmega()/2./M_PI << " Hz\n"
       << "Verbose           : " << (verbose ? "true" : "false") << endl
       << "z-scaling factor  : " << scaleZ << endl
       << "V-scaling factor  : " << scaleVs << endl
       << "Dampening         : " << c_Fdamp << endl
       << endl;
}

void V2RhoT::usage() {
  cout << endl
       << "Usage: V2RhoT FileIn FileOut -type <P,S> [options][-h]" << endl
       << endl;
  exit(0);
}

void V2RhoT::help() {
  cout << endl
       << "usage: "
       << "V2RhoT File_In File_Out -type <P,S> [options]\n"
       << endl
       << "  Required input parameters:\n"
       << "  --------------------------\n"
       << "  File_In          Path and name of grid file containing x y z Vs\n"
       << "  File_Out         Output file name and path\n"
       << "  type             Specify S or P for input wave type\n"
       << endl
       << "  Units\n"
       << "  -----\n"
       << "  depth     m      Values below MSL must be negative\n"
       << "  velocity  m/s\n"
       << endl
       << "  Option    Value Default Description\n"
       << "  ------    ----- ------- -----------\n"
       << "  -AlphaT                 Activates T-dependency of alpha. Default\n"
       << "                          is constant alpha.\n"
       << "  -compc    vals          Custom rock composition, see example 2\n"
       << "                          -comp Ol Opx Cpx Sp Gnt\n"
       << "  -compp    val         0 Use predefined rock compositions:\n"
       << "                          0 - Garnet Lherzolite after (Jordan,\n"
       << "                              1979; Goes et al, 2000)\n"
       << "                          1 - On-cratonic (Shapiro and Ritzwoller, 2004)\n"
       << "                          2 - Off-cratonic (Shapiro and Ritzwoller, 2004)\n"
       << "                          3 - Oceanic (Shapiro and Ritzwoller, 2004)\n"
       << "  -ERM      string  AK135 P calculation with AK135 or PREM\n"
       << "  -f        val    1/0.02 Define custom wave frequency in Hz.\n"
       << "  -fdamp    val     0.025 Iteration dampening\n"
       << "  -minDB    1 or 2      1 Mineral property database by\n"
       << "                          1 - Cammarano et al. (2003)\n"
       << "                          2 - Goes et al. (2000)\n"
       << "  -petrel                 Output is written as Petrel Points with  Attribtues\n"
       << "  -prop                   Print mineral properties\n"
       << "  -Q        1 or 2      1 Anelasticity paramters Q\n"
       << "                          1 - Sobolev et al. (1996)\n"
       << "                          2 - Berckhemer et al. (1982)\n"
       << "  -rc       val      2890 Crustal density in kg/m3\n"
       << "  -rm       val      3300 Mantle density in kg/m3\n"
       << "  -ra       val      3100 Average density in kg/m3\n"
       << "  -scaleZ   val         1 Scale every z-value in File_In by this value\n"
       << "  -scaleV   val         1 Scale every Vs-value in File_In by this value\n"
       << "  -scatter                Use scattered data as input instead of regular grid\n"
       << "  -t        val       0.1 Threshold in K where Temperature iteration stops\n"
       << "  -Tstart   val    273.15 Iteration starting temperature\n"
       << "  -t_crust  path          EarthVision file for crustal thickness\n"
       << "  -writedRdT              Writes used dRho/dT tables for minerals to a text file\n"
       << "  -xfe      val         0 Define iron content of the rock in mole fraction\n"
       << "  -z_topo   path          EarthVision file for topographic elevation\n"
       << "\n"
       << "Example 1:\n"
       << "V2RhoT Vs.dat T.dat -type P -rc 2870 -rm 3300 -t_crust crust.dat -z_topo  topo.dat\n"
       << endl
       << "Example 2:\n"
       << "V2RhoT Vs.dat T.dat -type S -compc 0.82 0.144 0.0 0.0 0.036\n"
       << endl;
  exit(0);
}

void V2RhoT::argsError(QString val, bool ok) {
  if(!ok) {
    cout << PRINT_ERROR "Invalid value in " << val.toUtf8().data() << endl;
    exit(1);
  }
}

void V2RhoT::readArgs(int &argc, char *argv[]) {
  /**
  Input convention
  V2RhoT FileIn FileOut -rc val -rm val -ra val -t_crust path -z_topo path
  -v -t val
  **/
  bool ok, okCrust, okTopo, UseCustomOmega, definedPMethod;
  double CustomFreq = -1.;
  QList <double> comp_input;

  ok = false;
  okCrust = false;
  okTopo = false;
  UseCustomOmega = false;
  definedPMethod = false;

  QStringList arg;
  for(int i=0; i<argc; i++) {
    arg << argv[i];
  }

  if( (argc>1) && (arg[1]=="-h" || arg[1]=="-help") ) {
    help();
  } else if( (argc>1) && (arg[1]=="-prop")) {
    MantleRock->printProperties();
  } else if( (argc>1) && (arg[1]=="-writedRdT")) {
    MantleRock->writedRdT();
  } else if( argc>1 && arg[1]=="--WRITEP_AK135") {
    ERM->set("AK135");
    ok = ERM->writeP();
    argsError(arg[1], ok);
    exit(0);
  } else if( argc>1 && arg[1]=="--BUILD") {
    cout << endl << "Build date: " << compilationTime.toUtf8().data() << endl
         << endl;
    exit(0);
  }else if(argc<4) {
    cout << endl << PRINT_ERROR "Not enough arguments!";
    usage();
  } else {
    File_In = arg[1].toUtf8().data();
    File_Out = arg[2].toUtf8().data();

    for(int i=3; i<argc; i++) {
      if(arg[i]=="-type") {
        if(arg[i+1]=="P" || arg[i+1]=="S") {
          VelType = arg[i+1];
          i++;
        } else {
          ok = false;
          argsError(arg[i],ok);
        }
      } else if(arg[i]=="-AlphaT") {
        MantleRock->set_AlphaMode(1);
      } else if(arg[i]=="-compc") {
        for(int j=1; j<6; j++) {
          comp_input.append(arg[i+j].toDouble(&ok));
          argsError(arg[i+j], ok);
        }
        MantleRock->set_Comp(comp_input[0], comp_input[1], comp_input[2],
                             comp_input[3], comp_input[4]);
        i+=5;
      } else if(arg[i]=="-compp") {
        MantleRock->set_Comp(arg[i+1].toDouble(&ok));
        argsError(arg[i+1], ok);
        i++;
      } else if(arg[i]=="-ERM") {
        ok = SetPMethod(arg[i+1]);
        argsError(arg[i], ok);
        definedPMethod = true;
        i++;
      } else if(arg[i]=="-f") {
        UseCustomOmega = true;
        CustomFreq = arg[i+1].toDouble(&ok);
        argsError(arg[i], ok);
        i++;
      } else if(arg[i]=="-fdamp") {
        c_Fdamp = arg[i+1].toDouble(&ok);
        argsError(arg[i], ok);
        i++;
      } else if(arg[i]=="-minDB"){
        int DBarg = arg[i+1].toInt(&ok);
        argsError(arg[i], ok);
        if(DBarg==1){
          ok = MantleRock->set_MineralPropertyDB("Cammarano");
        } else if(DBarg==2){
          ok = MantleRock->set_MineralPropertyDB("Goes");
        }
        argsError(arg[i], ok);
        i++;
      } else if(arg[i]=="-petrel") {
        petrel=true;
      } else if(arg[i]=="-Q") {
        int Qarg = arg[i+1].toInt(&ok);
        argsError(arg[i], ok);
        ok = MantleRock->setQ(Qarg);
        argsError(arg[i], ok);
        i++;
      } else if(arg[i]=="-rc") {
        rho_crust = arg[i+1].toDouble(&ok);
        argsError(arg[i], ok);
        i++;
      } else if(arg[i]=="-rm") {
        rho_mantle = arg[i+1].toDouble(&ok);
        argsError(arg[i], ok);
        i++;
      } else if(arg[i]=="-ra") {
        rho_avrg = arg[i+1].toDouble(&ok);
        SetPMethod("simple");
        argsError(arg[i], ok);
        i++;
      } else if(arg[i]=="-scaleZ") {
        scaleZ = arg[i+1].toDouble(&ok);
        argsError(arg[i], ok);
        i++;
      } else if(arg[i]=="-scaleV") {
        scaleVs = arg[i+1].toDouble(&ok);
        argsError(arg[i], ok);
        i++;
      } else if(arg[i]=="-scatter") {
        ArbitraryPoints = true;
      } else if(arg[i]=="-Tstart") {
        T_start = arg[i+1].toDouble(&ok);
        argsError(arg[i], ok);
        i++;
      } else if(arg[i]=="-t_crust") {
        File_t_crust = arg[i+1];
        okCrust = true;
        SetPMethod("crust");
        definedPMethod = true;
        i++;
      } else if(arg[i]=="-t") {
        threshold = arg[i+1].toDouble(&ok);
        argsError(arg[i], ok);
      } else if(arg[i]=="-v") {
        verbose = true;
        MantleRock->setVerbose(verbose);
      } else if(arg[i]=="-xfe") {
        double ArgXFe = arg[i+1].toDouble(&ok);
        argsError(arg[i], ok);
        ok = MantleRock->set_XFe(ArgXFe);
        argsError(arg[i], ok);
        i++;
      } else if(arg[i]=="-z_topo") {
        File_z_topo = arg[i+1];
        okTopo = true;
        SetPMethod("crust");
        definedPMethod = true;
      } else {
        cout << PRINT_ERROR "Unknown argument " << arg[i].toUtf8().data()
             << endl << endl;
        exit(1);
      }
    }
  }

  // Some logical checks
  //if(ArbitraryPoints)
  //{
  //  printf("ArbitraryPoints: TRUE\n");
  //}
  //if(use_t_crust)
  //{
  //  printf("use_t_crust: TRUE\n");
  //}
  if(!definedPMethod) {
    // If not defined by user initiate the default method
    ERM->set(PMethod);
  }
  if(ArbitraryPoints && use_t_crust) {
    cout << PRINT_ERROR "Defined crustal thickness/topography using scattered"
         << " data. Exit.\n";
    exit(1);
  }
  if(okTopo ^ okCrust) {
    cout << PRINT_ERROR "Not enough arguments:" << endl;
    if(okTopo) {
      cout << "No crustal thickness file defined!" << endl;
    } else {
      cout << "No topographic elevation file defined!" << endl;
    }
    exit(1);
  }
  if(use_t_crust && (okTopo || okCrust)) {
    cout << PRINT_ERROR "Pressure calculation method set to 'crust' but ";
    if(okTopo) {
      cout << "crustal thickness not defined!" << endl;
    } else {
      cout << "topography not defined!" << endl;
    }
    exit(1);
  }

  // Define Omega
  if(UseCustomOmega) {
    MantleRock->set_omega(CustomFreq);
  } else {
    // If no custom frequency given define omega according to Goes et al. (2000)
    // f_p = 1Hz or f_s = 0.02Hz
    MantleRock->set_omega(VelType);
  }
}

bool V2RhoT::readFile(QString InName, QString InType) {
  // InType "topo", "crust" or "vox"
  double x,y,z,val;
  bool okx, oky, okz, okval, okGrid;

  okGrid = false; // Used to check if grid size was extracted from input voxel

  cout << "Reading file: " << InName.toUtf8().data() << endl;
  QFile file(InName);

  x_min1 = 1.7E308;
  x_min2 = x_min1;
  x_min3 = x_min1;
  y_min1 = x_min1;
  y_min2 = x_min1;
  y_min3 = x_min1;
  z_min1 = x_min1;

  x_max1 = -x_min1;
  x_max2 = x_max1;
  x_max3 = x_max1;
  y_max1 = x_max1;
  y_max2 = x_max1;
  y_max3 = x_max1;

  int n = 0;
  if(file.open(QIODevice::ReadOnly)) {
  QTextStream stream( &file );

  while(!stream.atEnd()) {
    n++;
    QString t = stream.readLine().simplified();

    if(!t.startsWith("#") && (InType == "topo" || InType == "crust")) {
      /// Topography or crustal thickness
      QStringList vals = t.split(" ");
      if(vals.count() != 6) {
        file.close();
        cout << PRINT_ERROR "In header of " << InName.toUtf8().data()
             << " - grid size." << endl;
        exit(1);
      } else {
        x = vals[0].toDouble(&okx);
        y = vals[1].toDouble(&oky);
        z = vals[2].toDouble(&okz);
        if(okx && oky && okz) {
          Point3D p3D(x,y,z);
          if(InType == "crust") {
            t_crust.append(p3D);
          } else if (InType == "topo") {
            z_topo.append(p3D);
          }
        } else {
          file.close();
          cout << PRINT_ERROR "In value conversion line " << n << endl;
          return false;
        }
        //Determine min/max
        if(InType == "crust") {
          if(x < x_min2) {
            x_min2 = x;
          } else if(x > x_max2) {
            x_max2 = x;
          }
          if(y < y_min2) {
            y_min2 = y;
          } else if(y > y_max2) {
            y_max2 = y;
          }
        } else if (InType =="topo") {
            if(x < x_min3) {
              x_min3 = x;
            } else if(x > x_max3) {
              x_max3 = x;
            }
            if(y < y_min3) {
              y_min3 = y;
            } else if(y > y_max3) {
              y_max3 = y;
            }
          }
        }
      } else if(t.startsWith("# Grid_size:") && InType == "vox") {
        // velocity grid
        t.remove(0,12);
        QStringList gridSize = t.split("x", QString::SkipEmptyParts);
        nX = gridSize[0].toInt(&okx);
        nY = gridSize[1].toInt(&oky);
        nZ = gridSize[2].toInt(&okz);
        if(!okx || !oky || !okz) {
          cout << PRINT_ERROR "In header of " << InName.toUtf8().data()
               << " - grid size.";
          exit(1);
        }
        okGrid = true;
      } else if(!t.startsWith("#") && InType == "vox") {
        QStringList vals = t.split(" ");
        if(vals.count() != 4) {
          file.close();
          cout << PRINT_ERROR "In line " << n << ": unkown amount of columns."
               << endl;
          exit(1);
        } else {
          x = vals[0].toDouble(&okx);
          y = vals[1].toDouble(&oky);
          z = scaleZ*vals[2].toDouble(&okz);
          val = scaleVs*vals[3].toDouble(&okval);
        if(val < 50) {
          cout << endl << endl
               << PRINT_WARNING "Imported velocity is < 50 m/s! Maybe imported "
               << "velocities are in km/s?\n"
               << "To convert to m/s use option -scaleV 1000\n"
               << endl;
          exit(1);
        } else if(okx && oky && okz && okval) {
          Point5D p5D(x,y,z,val,0.0);
          data_V.append(p5D);
        } else {
          file.close();
          cout << PRINT_ERROR "In value conversion line " << n << "\n";
          exit(1);
        }
        // Get minima and maxima
        if(x < x_min1) {
          x_min1 = x;
        } else if(x > x_max1) {
          x_max1 = x;
        }
        if(y < y_min1) {
          y_min1 = y;
        } else if(y > y_max1) {
          y_max1 = y;
        }
        if(z < z_min1) {
          z_min1 = z;
        } else if(z > z_max1) {
          z_max1 = z;
        }
      }
    }
  }

  if(!ArbitraryPoints && !okGrid) {
    cout << PRINT_WARNING "Could not find grid information. Set output to "
         << "scattered data.\n";
    ArbitraryPoints = true;
  }

  file.close();
  return true;
  }
  cout << PRINT_ERROR "File " << InName.toUtf8().data() << " not found\n";
  exit(1);
}

bool V2RhoT::saveFile(QString OutName) {
  QString T_header, Info_header, usecrust;

  // Create time stamp
  QDateTime currentDateTime = QDateTime::currentDateTime();
  QString timestamp = currentDateTime.toString();

  if(use_t_crust) {
    usecrust = "yes";
  } else {
    usecrust = "no";
  }

  Info_header  = QString("# Transformation settings:\n");
  Info_header += QString("# Date created: %1\n").arg(timestamp);
  Info_header += QString("# Input file: %1\n").arg(File_In);
  Info_header += QString("# Mantle composition:\n");
  Info_header += QString("# Ol - %1\n").arg(MantleRock->getComposition(0), 5, 'f', 2);
  Info_header += QString("# Opx - %1\n").arg(MantleRock->getComposition(1), 5, 'f', 2);
  Info_header += QString("# Cpx - %1\n").arg(MantleRock->getComposition(2), 5, 'f', 2);
  Info_header += QString("# Sp - %1\n").arg(MantleRock->getComposition(3), 5, 'f', 2);
  Info_header += QString("# Gnt - %1\n").arg(MantleRock->getComposition(4), 5, 'f', 2);
  Info_header += QString("# Iron content XFe: %1\n").arg(MantleRock->getXFe(),3,'f',2);
  Info_header += QString("# Pressure calculation method: %1\n").arg(PMethod);
  if(use_t_crust) {
    Info_header += QString("# Use crustal thickness for pressure calculation: %1\n").arg(usecrust);
    Info_header += QString("# Density crust/mantle/average: %1%2%3\n")
      .arg(rho_crust,7,'f',1).arg(rho_mantle,7,'f',1).arg(rho_avrg,7,'f',1);
  }
  Info_header += QString("# Alpha: %1\n").arg(MantleRock->get_AlphaModeStr());
  if(use_t_crust) {
    Info_header += QString("# Topography: %1\n").arg(File_z_topo);
    Info_header += QString("# Crustal thickness: %1\n").arg(File_t_crust);
  }
  Info_header += QString("# Wave frequency (Omega) / Hz: %1\n").arg(MantleRock->getOmega()/2/M_PI);
  Info_header += QString("# Dampening factor: %1\n").arg(c_Fdamp);
  Info_header += QString("# Iteration starting temperature / K: %1\n").arg(T_start);
  Info_header += QString("# Anelasticity parameters: %1\n").arg(MantleRock->getQ());
  Info_header += QString("# Average iteration steps: %1\n").arg(count_avrg,0,'f',1);

  if(petrel) {
    T_header  = QString("# Petrel Points with attributes\n");
    T_header += QString("# Unit in X and Y direction: m\n");
    T_header += QString("# Unit in depth: m\n");
    T_header += Info_header;
    T_header += QString("VERSION 1\n");
    T_header += QString("BEGIN HEADER\n");
    T_header += QString("X\n");
    T_header += QString("Y\n");
    T_header += QString("Z\n");
    T_header += QString("FLOAT,V%1 / km/s\n").arg(VelType);
    T_header += QString("FLOAT,T / degC\n");
    T_header += QString("FLOAT,Rho / kg/m3\n");
    T_header += QString("END HEADER");
  } else if (ArbitraryPoints) {
    T_header  =  QString("# Point data\n");
    T_header += Info_header;
    T_header += QString("# Columns:\n");
    T_header += QString("# 1 - X\n");
    T_header += QString("# 2 - Y\n");
    T_header += QString("# 3 - Z [m]\n");
    T_header += QString("# 4 - V_%1 [m/s]\n").arg(VelType);
    T_header += QString("# 5 - T [degC]\n");
    T_header += QString("# 6 - Rho [kg/m3]");
  } else {
    T_header  = QString("# Type: GMS GridPoints\n");
    T_header += QString("# Version: 2\n");
    T_header += QString("# Description:\n");
    T_header += QString("# Format: free\n");
    T_header += QString("# Field: 1 X [m]\n");
    T_header += QString("# Field: 2 Y [m]\n");
    T_header += QString("# Field: 3 Z [m]\n");
    T_header += QString("# Field: 4 V_%1 [m/s]\n").arg(VelType);
    T_header += QString("# Field: 5 T [degC]\n");
    T_header += QString("# Field: 6 Rho [kg/m3]\n");
    T_header += QString("# Projection: Local Rectangular\n");
    T_header += QString("# Information from grid:\n");
    T_header += QString("# Grid_size: %1 x %2 x %3\n").arg(nX).arg(nY).arg(nZ);
    T_header += QString("# Grid_X_range: %1 to %2\n").arg(x_min1,0,'f').arg(x_max1,0,'f');
    T_header += QString("# Grid_Y_range: %1 to %2\n").arg(y_min1,0,'f').arg(y_max1,0,'f');
    T_header += QString("# Grid_Z_range: %1 to %2\n").arg(z_min1,0,'f').arg(z_max1,0,'f');
    T_header += Info_header;
    T_header += QString("# End:");
  }

  cout << "Writing temperature file " << OutName.toUtf8().data() << endl;
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
  for(int i=0; i<data_T.length(); i++) {
    fout << data_T[i].x();
    fout << "\t";
    fout << data_T[i].y();
    fout << "\t";
    fout << data_T[i].z();
    fout << "\t";
    fout << data_V[i].v();
    fout << "\t";
    fout.setRealNumberPrecision(1);
    fout << data_T[i].v();
    fout << "\t";
    fout << data_T[i].prop();
    fout.setRealNumberPrecision(5);
    fout << endl;
  }

  tmp.close();
  return true;
}

bool V2RhoT::SetPMethod(QString method) {
  /**
  Required because AK135 and PREM need to be initialised
  **/
  if (method == "AK135" || method == "PREM") {
    cout << "Setting method to AK135 or PREM\n";
    ERM->set(method);
  } else if (method == "crust") {
    use_t_crust = true;
    delete ERM;
  } else if (method != "simple") {
    cout << PRINT_ERROR << "Undefined method for pressure calculation!\n";
    return false;
    delete ERM;
  }
  PMethod = method;
  return true;
}

double V2RhoT::pressure(double x, double y, double z) {
  if(PMethod=="AK135" || PMethod=="PREM") {
    return ERM->pressure(z);
  } else if(use_t_crust) {
    return pressure_crust(x, y, z);
  } else {
    return pressure_simple(z);
  }
}

double V2RhoT::pressure_simple(double z){
  // Simple lithostatic pressure with average density
  return rho_avrg * c_g * abs(z);
}

double V2RhoT::pressure_crust(double x, double y, double z) {
  /**
   Calculates the pressure [Pa] at point x,y for the given properties
   x             x-coordinate [m]
   y             y-coordinate [m]
   z             z-coordinate [m]
   data_z_topo   Array containing topographic elevation (X, Y, Z [m])
   data_t_crust  Array containing crustal thickness (X, Y, Thickness [m])
   rho_crust     Crustal density
   rho_mantle    Mantle density
  **/

  int i_crust, i_topo;
  double P_crust, t_mantle, P_mantle;

  // Get index in data_t_crust and data_z_topo
  int i=0;
  while( (t_crust[i].x()!=x) && (t_crust[i].y()!=y)) {i++;}
  i_crust = i;
  i = 0;
  while( (z_topo[i].x()!=x) && (z_topo[i].y()!=y)) {i++;}
  i_topo=i;

  P_crust = rho_crust*c_g*t_crust[i_crust].z();              // Pressure crust
  t_mantle = z_topo[i_topo].z() - t_crust[i_crust].z() - z;  // Mantle thickness

  if (t_mantle < 0) {
    cout << "Error: Mantle thickness < 0\nExit\n";
    exit(1);
  }

  P_mantle = rho_mantle*c_g*t_mantle;

  return P_crust + P_mantle;
}

bool V2RhoT::Iterate() {
  Point5D p5D;
  int n_V, counter, progress;
  double deltaT, T_n, T_n1;
  double V, x, y, z, P, Vsyn, dVdTsyn;
  QList <double> count_total;

  cout << endl
       << "************************\n"
       << "Start iteration\n"
       << "************************\n"
       << "Threshold: " << threshold << " K\n"
       << "T_start: " << T_start << " K\n";

  n_V = data_V.length();
  for(int i=0; i<n_V; i++) {
    V = data_V[i].v();
    x = data_V[i].x();
    y = data_V[i].y();
    z = data_V[i].z();
    P = pressure(x, y, z);
    MantleRock->calc_prop(VelType);   // Calculate all P/T independent rock properties
    T_n = T_start;    // Temperature at step n
    T_n1 = 0.;        // Temperature at step n+1
    counter = 0;
    deltaT = threshold + 1;
    while(deltaT > threshold) {
      // Calculate rock properties
      if(verbose){
        cout << endl << endl
             << "Point " << i+1 << ", Step " << counter << endl
             << "X                " << x << endl
             << "Y                " << y << endl
             << "Z                " << z << endl
             << "Step T           " << T_n << endl
             << "Delta T          " << deltaT << endl
             << "Measured V       " << V << endl;
      }
      MantleRock->calc_prop_PT(P, T_n, VelType);
      Vsyn = MantleRock->get_Vsyn_PT();
      dVdTsyn = MantleRock->get_dVdTsyn_PT();
      T_n1 = T_n + c_Fdamp*(V - Vsyn)/dVdTsyn;
      deltaT = abs(T_n - T_n1);
      counter = counter + 1;
      if(counter>10000) {
        cout << "Too many iterations at point " << i << endl
             << "X(" << x << ") Y(" << y <<") Z(" << z << ") V(" << V << ")\n"
             << "Set T=-1\n";
        T_n1 = 272.15;
        break;
      }
      T_n = T_n1;
    }

    if(verbose) {
      cout << "Iteration finished, deltaT = " << deltaT
           << ", T = " << T_n1 << endl;
    }

    p5D.setV(T_n1-273.15);
    p5D.setX(x);
    p5D.setY(y);
    p5D.setZ(z);
    p5D.setProp(MantleRock->getRho());
    data_T.append(p5D);
    count_total.append(double(counter));

    progress = (int)round( (double(i)/double(n_V)) * 100.);
    if(progress % 5 == 0) {
      printf("\rProgress: %i  ", progress);
    }
  }
  printf("\rProgress: 100\n");

  // Calculate average counts
  count_avrg = 0;
  for(int i=0; i<n_V; i++) {
    count_avrg = count_avrg + count_total[i];
  }
  count_avrg = count_avrg/n_V;
  cout << "Average iteration steps: " << count_avrg << endl;
  return true;
}

//##############################################################################
// Code
//##############################################################################
int main(int argc, char *argv[]) {
  V2RhoT VelTemp;
  if(argc > 0) {
    VelTemp.readArgs(argc,argv);
    VelTemp.Info();
    VelTemp.readFile(VelTemp.FileIn(),"vox");
    VelTemp.Iterate();
    VelTemp.saveFile(VelTemp.FileOut());
  } else {
    VelTemp.usage();
  }
}
