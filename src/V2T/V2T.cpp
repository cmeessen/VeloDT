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
#include "V2T.h"

using namespace std;

V2T::V2T() {
  PMethod = "AK135";
  use_t_crust = false;
  ArbitraryPoints = false;
  outVs = false;
  ERM = new EarthReferenceModel;
  rho_crust = 2890;
  rho_mantle = 3300;
  rho_avrg = 3100;
  verbose = false;
  threshold = 0.1;
  scaleZ = 1;
  scaleVs = 1;
  c_bV = 3.84E-4;  // 1/km
  c_m  = -2.8E-4;  // km/s
  c_c  = 4.72;     // km/s
  c_A  = -1.8E13;  // km/s
  c_E  = 409.0;    // kJ/mol
  c_Va = 10E-6;    // m3/mol
  c_K  = 273.15;
}

void V2T::Info() {
  cout << "*****************************************************************\n"
       << endl
       << "            S-Wave Velocity to Temperature Conversion\n"
       << endl
       << "*****************************************************************\n"
       << "For details see Priestley and McKenzie (2006):\n"
       << "\"The thermal structure of the lithosphere from shear wave\n"
       << "velocities\"\n"
       << "*****************************************************************\n"
       << endl
       << "Conversion settings\n"
       << "-------------------\n"
       << "Pressure calcuation " << PMethod.toUtf8().data() << endl
       << "Iteration threshold " << threshold << endl
       << endl
       << "Input\n"
       << "-----\n"
       << "Vs                  " << File_In.toUtf8().data() << endl;
  if (use_t_crust) {
    cout << "Topography        " << File_z_topo.toUtf8().data() << endl
         << "Crustal thickness " << File_t_crust.toUtf8().data() << endl;
  }
  cout << endl
       << "Output\n"
       << "------\n"
       << "Temperatures        " << File_Out.toUtf8().data() << endl << endl;
  if (use_t_crust || (PMethod == "simple")) {
    cout << "Densities\n"
         << "---------\n";
  }
  if (use_t_crust) {
    cout << "Crust             " << rho_crust << "kg/m3\n"
         << "Mantle            " << rho_mantle << "kg/m3\n";
  }
  if (PMethod == "simple") {
    cout << "Average           " << rho_avrg << " kg/m3\n\n";
  }
  cout << "Other\n"
       << "-----\n"
       << "Verbose             " << (verbose ? "true" : "false") << endl
       << "z-scaling factor    " << scaleZ << endl
       << "Vs-scaling factor   " << scaleVs << "\n\n";
}

void V2T::usage() {
  cout << endl
       << "usage: "
       << "V2T File_In File_Out [options]\n"
       << endl
       << "  Required input parameters:\n"
       << "  --------------------------\n"
       << "  File_In          Path and name of grid file containing x y z Vs\n"
       << "  File_Out         Output file name and path\n"
       << endl
       << "  Option    Value Default Description\n"
       << "  ------    ----- ------- -----------\n"
       << "  -h                      This information\n"
       << "  --help                  Extended information\n"
       << "  -ERM      string  AK135 P calculation method AK135, PREM or simple\n"
       << "  -outVs                  Writes VsObs and VsCalc to output file\n"
       << "  -rc       val      2890 Crustal density in kg/m3\n"
       << "  -rm       val      3300 Mantle density in kg/m3\n"
       << "  -ra       val      3100 Average density in kg/m3 used in '-ERM simple'\n"
       << "  -scaleZ   val         1 Scale every z-value by this value\n"
       << "  -scaleVs  val         1 Scale every Vs-value by this value\n"
       << "  -t_crust  path          EarthVision file for crustal thickness\n"
       << "  -z_topo   path          EarthVision file for topogrpahy\n"
       << "  -t        val       0.1 Threshold for Newton iterations\n"
       << "  -scatter                Use scattered data as input\n"
       << "  -v                      For debugging\n"
       << endl
       << endl;
  exit(0);
}

void V2T::usage_extended() {
  cout << "*****************************************************************\n"
       << "Calculates Temperature from s-wave velocities following approach\n"
       << "by Priestley and McKenzie (2006): \"The thermal structure of the\n"
       << "lithosphere from shear wave velocities\"\n"
       << "*****************************************************************\n"
       << "Source: https://github.com/cmeessen/MantleConversions\n"
       << endl
       << "The pressure for each point is calculated in 1D, regarding\n"
       << "the topographic elevation 'File_z_topo', a crust with a\n"
       << "homogeneous density 'rho_crust' and thickness defined by\n"
       << "'File_t_crust', as well as a homogeneous mantle with density\n"
       << "'rho_mantle'.\n"
       << endl
       << "Requirements\n"
       << "  - all input grids must have the same spacing and X / Y \n"
       << "    dimensions (this is checked prior to calculation)\n"
       << "  - Homogeneous s-wave velocity grid\n"
       << "  - topographic elevation grid\n"
       << "  - crustal thickness grid\n"
       << endl
       << "Information on input data\n"
       << "-------------------------\n"
       << "  1) S-Waves\n"
       << "     a) Homogeneous 3D s-wave velocity grid\n"
       << "        Data fromat: GMS Grid Points\n"
       << "        Columns:\n"
       << "        0 - X  / m\n"
       << "        1 - Y  / m\n"
       << "        2 - Z  / m      values < MSL must be negative\n"
       << "        3 - Vs / km/s\n"
       << "     b) Scattered input data\n"
       << "        Same column format as a) but scattered\n"
       << endl
       << "  2) Topographic elevation grid\n"
       << "        Data format: EarthVision Grid\n"
       << "        Columns:\n"
       << "        0 - X / m\n"
       << "        1 - Y / m\n"
       << "        2 - Z / m\n"
       << endl
       << "  3) Crustal thickness grid\n"
       << "     Data format: EarthVision Grid\n"
       << "        0 - X / m\n"
       << "        1 - Y / m\n"
       << "        2 - t / m\n"
       << endl
       << "Information on pressure calculation\n"
       << "-----------------------------------\n"
       << "If nothing is specified the pressure is calculated using the\n"
       << "AK135 Earth reference model (Kennet et al., 1995). Specifying\n"
       << "\'-ERM model\' allows to calculate pressure with PREM\n"
       << "(Dziewonski and Anderson, 1981). Both models are implemented for\n"
       << "depths up to 660 km. The model \'simple\' uses the average\n"
       << "density specified with \'-ra\'.\n"
       << endl
       << "Debug information\n"
       << "-----------------\n"
       << "Use \'./V2T 0 0 --WRITE_P model\' to print calculated\n"
       << "pressures for the specific model. [model] can be \'AK135\',\n"
       << "\'PREM\' or \'simple\'.\n"
       << endl;
  exit(0);
}

bool V2T::test_data() {
  /// Check minima and maxima of data_Vs, t_crust and z_topo
  if (use_t_crust && (
      (x_min1 != x_min2) || (x_min2 != x_min3) ||
      (x_max1 != x_max2) || (x_max2 != x_max3) ||
      (y_min1 != y_min2) || (y_min2 != y_min3) ||
      (y_max1 != y_max2) || (y_max2 != y_max3) )) {
    cout << PRINT_ERROR "Datasets have different dimensions!\n";
    return false;
    exit(1);
  }
  return true;
}

void V2T::argsError(QString val, bool ok) {
  if (!ok) {
    cout << PRINT_ERROR "Invalid value in " << val.toUtf8().data() << endl;
    exit(1);
  }
}

void V2T::readArgs(int &argc, char *argv[]) {
  /**
  Input convention
  V2T [FileIn] [FileOut] [[args]]
  **/

  int i;
  bool ok, okCrust, okTopo;

  okCrust = false;
  okTopo = false;

  QStringList arg;
  for (i = 0; i < argc; i++)
    arg << argv[i];

  if ((argc > 1) && (arg[1] == "-h" || arg[1] == "-help")) {
    usage();
  } else if ((argc > 1) && (arg[1] == "--help")) {
    usage_extended();
  } else if (argc < 3) {
    cout << endl << endl << PRINT_ERROR "Not enough arguments!\n\n";
    usage();
  } else {
    File_In = arg[1].toUtf8().data();
    File_Out = arg[2].toUtf8().data();

    for (i=3; i < argc; i++) {
      if (arg[i] == "-ERM") {
        ok = SetPMethod(arg[i+1]);
        argsError(arg[i], ok);
        i++;
      } else if (arg[i] == "--WRITE_P") {
        WRITE_P(arg[i+1]);
      } else if (arg[i] == "-outVs") {
        outVs = true;
      } else if (arg[i] == "-rc") {
        rho_crust = arg[i+1].toDouble(&ok);
        argsError(arg[i], ok);
        i++;
      } else if (arg[i] == "-rm") {
        rho_mantle = arg[i+1].toDouble(&ok);
        argsError(arg[i], ok);
        i++;
      } else if (arg[i] == "-ra") {
        rho_avrg = arg[i+1].toDouble(&ok);
        argsError(arg[i], ok);
        i++;
      } else if (arg[i] == "-scaleZ") {
        scaleZ = arg[i+1].toDouble(&ok);
        argsError(arg[i], ok);
        i++;
      } else if (arg[i] == "-scaleVs") {
        scaleVs = arg[i+1].toDouble(&ok);
        argsError(arg[i], ok);
        i++;
      } else if (arg[i] == "-t_crust") {
        File_t_crust = arg[i+1];
        SetPMethod("crust");
        okCrust = true;
      } else if (arg[i] == "-z_topo") {
        File_z_topo = arg[i+1];
        SetPMethod("crust");
        okTopo = true;
      } else if (arg[i] == "-scatter") {
        ArbitraryPoints = true;
      } else if (arg[i] == "-t") {
        threshold = arg[i+1].toDouble(&ok);
        argsError(arg[i], ok);
      } else if (arg[i] == "-v") {
        verbose = true;
      }
    }
  }

  // Some logical checks
  if (ArbitraryPoints && use_t_crust) {
    cout << PRINT_ERROR "Defined crustal thickness/topography using scattered "
            "data. Exit.\n";
    exit(1);
  }
  if (okTopo ^ okCrust) {
    cout << PRINT_ERROR "Not enough arguments:\n";
    if (okTopo) {
      cout << "No crustal thickness file defined!\n";
    } else {
      cout << "No topographic elevation file defined!\n";
    }
    exit(1);
  }
  if ((use_t_crust == false) && (ERM->type() == "Undefined")) {
    ERM->set("AK135");
  }
}

bool V2T::readFile(QString InName, QString InType) {
  // InType "topo", "crust" or "vox"
  double x, y, z, val;
  double vs_min = -1;
  bool okx, oky, okz, okval, okGrid;

  okGrid = false;  // Used to check if grid size was extracted from input voxel

  cout << "Reading file: " << InName.toUtf8().data() << endl;
  QFile file(InName);

  x_min1 = x_min2 = x_min3 = y_min1 = y_min2 = y_min3 = z_min1 = 1.7E308;
  x_max1 = x_max2 = x_max3 = y_max1 = y_max2 = y_max3 = z_max1 = -x_min1;

  int n = 0;
  if (file.open(QIODevice::ReadOnly)) {
    QTextStream stream( &file );

    while (!stream.atEnd()) {
      n++;
      QString t = stream.readLine().simplified();

      /// Topography or crustal thickness
      if (!t.startsWith("#") && (InType == "topo" || InType == "crust")) {
        QStringList vals = t.split(" ");
        if (vals.count() != 3) {
          file.close();
          cout << PRINT_ERROR "In header of " << InName.toUtf8().data()
               << " - grid size.";
          exit(1);
        } else {
          x = vals[0].toDouble(&okx);
          y = vals[1].toDouble(&oky);
          z = vals[2].toDouble(&okz);
          if (okx && oky && okz) {
            Point3D p3D(x, y, z);
            if (InType == "crust") {
              t_crust.append(p3D);
            } else if (InType == "topo") {
              z_topo.append(p3D);
            }
          } else {
            file.close();
            cout << PRINT_ERROR "In value conversion line " << n << endl;
            return false;
          }

          // Determine min/max
          if (InType == "crust") {
            if (x < x_min2) {
              x_min2 = x;
            } else if (x > x_max2) {
              x_max2 = x;
            }
            if (y < y_min2) {
              y_min2 = y;
            } else if (y > y_max2) {
              y_max2 = y;
            }
          } else if (InType =="topo") {
            if (x < x_min3) {
              x_min3 = x;
            } else if (x > x_max3) {
              x_max3 = x;
            }
            if (y < y_min3) {
              y_min3 = y;
            } else if (y > y_max3) {
              y_max3 = y;
            }
          }
        }
      } else if (t.startsWith("# Grid_size:") && InType == "vox") {
        // S-wave velocity grid
        t.remove(0, 12);
        QStringList gridSize = t.split("x", QString::SkipEmptyParts);
        nX = gridSize[0].toInt(&okx);
        nY = gridSize[1].toInt(&oky);
        nZ = gridSize[2].toInt(&okz);
        if (!okx || !oky || !okz) {
          cout << PRINT_ERROR "In header of " << InName.toUtf8().data()
               << " - grid size." << endl;
          exit(1);
        }
        okGrid = true;
      } else if (!t.startsWith("#") && InType == "vox") {
        QStringList vals = t.split(" ");
        if (vals.count() != 4) {
          file.close();
          cout << PRINT_ERROR "In line " << n << ": unkown amount of columns."
               << endl;
          exit(1);
        } else {
          x = vals[0].toDouble(&okx);
          y = vals[1].toDouble(&oky);
          z = scaleZ*vals[2].toDouble(&okz);
          val = scaleVs*vals[3].toDouble(&okval);

          if (okx && oky && okz && okval) {
            Point4D p4D(x, y, z, val);
            data_Vs.append(p4D);
          } else {
            file.close();
            cout << PRINT_ERROR "In value conversion line " << n << endl;
            exit(1);
          }

          // Get minima and maxima
          // X
          if (x < x_min1) {
            x_min1 = x;
          } else if (x > x_max1) {
            x_max1 = x;
          }
          // Y
          if (y < y_min1) {
            y_min1 = y;
          } else if (y > y_max1) {
            y_max1 = y;
          }
          // Z
          if (z < z_min1) {
            z_min1 = z;
          } else if (z > z_max1) {
            z_max1 = z;
          }
          // Get minimum vs to check for km/s or m/s
          if (vs_min == -1)
            vs_min = val;
          if (val < vs_min)
            vs_min = val;
        }
      }
    }
    if (!ArbitraryPoints && !okGrid) {
      cout << PRINT_WARNING "Could not find grid information. Set output to "
             "scattered data.\n";
      ArbitraryPoints = true;
    }
    file.close();
    if (vs_min > 10) {
      cout << PRINT_WARNING "Minimum Vs is " << vs_min << " which is unusually "
              "high. Vs must be in km/s. Use -scaleVs to correct.\n";
    }
    return true;
  }
  cout << PRINT_ERROR "File " << InName.toUtf8().data() << " not found\n";
  exit(1);
}

bool V2T::saveFile(QString OutName) {
  QString T_header;
  QString T_info;
  // Get timestamp
  QDateTime CurrentTime = QDateTime::currentDateTime();
  QString timefmt = "yyyy-MM-dd hh:mm:ss";
  QString timestamp = CurrentTime.toString(timefmt);
  // General information string
  T_info = QString("# Created: %1\n").arg(timestamp);
  T_info += QString("# Input: %1\n").arg(File_In);
  T_info += QString("# Newton threshold: %1\n").arg(threshold, 0, 'f');
  T_info += QString("# z-factor: %1\n").arg(scaleZ, 0, 'f');
  T_info += QString("# Vs-factor: %1\n").arg(scaleVs, 0, 'f');
  T_info += QString("# Pressure calculation method: %1\n").arg(PMethod);
  if (PMethod == "simple") {
    T_info += QString("# Average density: %1 kg/m3\n").arg(rho_avrg, 0, 'f');
  } else if (PMethod == "crust") {
    T_info += QString("# Density crust: %1 kg/m3\n").arg(rho_crust, 0, 'f');
    T_info += QString("# Density mantle: %1 kg/m3").arg(rho_mantle, 0, 'f');
  }
  // GMS compatible file
  if (!ArbitraryPoints) {
    T_header =  QString("# Type: GMS GridPoints\n"
                        "# Version: 2\n"
                        "# Description:\n"
                        "# Format: free\n"
                        "# Field: 1 X / m\n"
                        "# Field: 2 Y / m\n"
                        "# Field: 3 Z / m\n"
                        "# Field: 4 T / degC\n");
    if (outVs) {
      T_header += QString("# Field: 4 VsObs / degC\n"
                          "# Field: 5 VsCalc / degC\n");
    }
    T_header += QString("# Projection: Local Rectangular\n");
    T_header += QString("# Information from grid:\n");
    T_header += QString("# Grid_size: %1 x %2 x %3\n").arg(nX).arg(nY).arg(nZ);
    T_header += QString("# Grid_X_range: %1 to %2\n").arg(x_min1, 0, 'f')
                                                     .arg(x_max1, 0, 'f');
    T_header += QString("# Grid_Y_range: %1 to %2\n").arg(y_min1, 0, 'f')
                                                     .arg(y_max1, 0, 'f');
    T_header += QString("# Grid_Z_range: %1 to %2\n").arg(z_min1, 0, 'f')
                                                     .arg(z_max1, 0, 'f');
    T_header += T_info;
    T_header += QString("# End:");
  } else {
    T_header =  QString("# Point data\n"
                        "# Columns:\n"
                        "# 1 - X\n"
                        "# 2 - Y\n"
                        "# 3 - Z / m\n"
                        "# 4 - T / degC\n");
    if (outVs) {
      T_header += QString("# 5 - VsObs / degC\n"
                          "# 6 - VsCalc / degC\n");
    }
    T_header += T_info;
  }

  cout << "Writing temperature file " << OutName.toUtf8().data() << endl;
  QFile tmp(OutName);

  if (!tmp.open(QIODevice::WriteOnly | QIODevice::Text)) {
    cout << PRINT_ERROR "Could not open file " << OutName.toUtf8().data()
         << endl;
    exit(1);
  }

  QTextStream out(&tmp);
  out.setRealNumberPrecision(2);
  out.setFieldAlignment(QTextStream::AlignRight);
  out.setRealNumberNotation(QTextStream::FixedNotation);
  out << T_header.toUtf8().data();
  for (int i=0; i < data_T.length(); i++) {
    out << data_T[i].x() << "\t"
        << data_T[i].y() << "\t"
        << data_T[i].z() << "\t";
    out.setRealNumberPrecision(1);
    out << data_T[i].v();
    if (outVs) {
      out.setRealNumberPrecision(3);
      out << "\t" << data_Vs[i].v() << "\t"
          << data_Vcalc[i];
    }
    out.setRealNumberPrecision(2);
    out << endl;
  }

  tmp.close();
  return true;
}

bool V2T::SetPMethod(QString method) {
  /**
  Required because AK135 and PREM need to be initialised
  **/
  if (method == "AK135" || method == "PREM") {
    cout << "Setting method to AK135 or PREM\n";
    ERM->set(method);
  } else if (method == "crust") {
    use_t_crust = true;
  } else if (method != "simple") {
    cout << PRINT_ERROR "Undefined method for pressure calculation!\n";
    return false;
  }
  PMethod = method;
  return true;
}

double V2T::ftheta(double VsS, double P, double T) {
  /**
  VsS Corrected velocity / km/s
  P   Pressure           / Pa
  T   Temperature        / degC
  **/
  return c_m*T - VsS + c_c + c_A*exp(-(c_E*1000 + P*c_Va)/c_R/(T + c_K));
}

double V2T::dfdtheta(double P, double T) {
  /**
  Derivative of Temperature function (d Theta / d T)
  Priestley and McKenzie (2006), Eqn 5
  P   Pressure    / Pa
  T   Temperature / degC
  **/
  return c_m + c_A*(c_E*1000 + P*c_Va)/c_R/(pow(T + c_K, 2))*
         exp(-(c_E*1000 + P*c_Va)/c_R/(T + c_K));
}

double V2T::pressure(double x, double y, double z) {
  /**
  Call the appropriate function based on the selected pressure calculation
  **/
  if (use_t_crust) {
    return pressure_crust(x, y, z);
  } else if((PMethod == "AK135") || (PMethod == "PREM")) {
    return ERM->pressure(z);
  } else {
    return pressure_simple(z);
  }
}

double V2T::pressure_crust(double x, double y, double z) {
  /**
  Calculates the pressure [Pa] at point x,y for the given properties
  x             x-coordinate / m
  y             y-coordinate / m
  z             z-coordinate / m
  data_z_topo   Array containing topographic elevation (X, Y, Z / m)
  data_t_crust  Array containing crustal thickness (X, Y, Thickness / m)
  rho_crust     Crustal density
  rho_mantle    Mantle density
  **/

  int i_crust, i_topo, i;
  double P_crust, t_mantle, P_mantle;

  // Get index in data_t_crust and data_z_topo
  i = 0;
  while ((t_crust[i].x() != x) && (t_crust[i].y() != y))
    i++;
  i_crust = i;
  i = 0;
  while ((z_topo[i].x() != x) && (z_topo[i].y() != y))
    i++;
  i_topo=i;

  P_crust = rho_crust*c_g*t_crust[i_crust].z();              // P from crust
  t_mantle = z_topo[i_topo].z() - t_crust[i_crust].z() - z;  // Thickness mantle

  if (t_mantle < 0) {
    cout << "Error: Mantle thickness < 0\nExit\n";
    return false;
  }

  P_mantle = rho_mantle*c_g*t_mantle;

  return P_crust + P_mantle;
}

double V2T::pressure_simple(double z) {
  // Return a simple pressure based on one average density
  return rho_avrg*c_g*fabs(z);
}

bool V2T::newton() {
  /**
  In this function the calculation of the temperature is performed. We want to
  achieve that the calculated temperature equals the observed temperature. In
  other words

  Vsyn(Theta) - Vobs = 0

  This problem can be solved with the Newton-Raphton iteration if we define

  f(Theta) = Vsyn(Theta) - Vobs

  then by iteration we can obtain

  Theta_i+1 = Theta_i - f(Theta_i)/f'(Theta_i)

  where f' = df/dTheta
  **/
  int count_zero, count_fail, n, j, progress;
  double Vs, x, y, z, VsS, theta_init, theta_i1, theta_i2, P, delta_theta,
         numerator, denominator;
  Point4D p4D;

  n = data_Vs.length();

  // Start to iterate every Vs in data_Vs
  cout << endl
       << "********************************\n"
       << "Starting temperature calculation\n"
       << "********************************\n";

  count_zero = 0;   // Counts amount of zero-divisions in Newtonian iterations
  count_fail = 0;   // Counts how often solution could not be found
  j = 0;

  for (int i=0; i < n; i++) {
    // Read data
    Vs = data_Vs[i].v();
    x = data_Vs[i].x();
    y = data_Vs[i].y();
    z = data_Vs[i].z();  // z in m

    // Calculate Vs* [km/s]
    // See Priestley and McKenzie (2006), Eqn 3
    VsS = Vs/(1+c_bV*(fabs(z)/1000.0 - 50.0));
    // Calculate pressure [Pa]
    P = pressure(x, y, z);

    // See Priestley and McKenzie (2006), Eqn 10
    if (VsS < 4.4) {
      theta_init = 1000.0;
      // Calculate initial T estimate theta_i0 and derivative theta_i0_d
      theta_i1 = theta_init
                 - ftheta(VsS, P, theta_init)/dfdtheta(P, theta_init);

      // Start Newton iteration
      delta_theta = threshold + 1;
      j = 0;
      while (delta_theta > threshold) {
        numerator = ftheta(VsS, P, theta_i1);
        denominator = dfdtheta(P, theta_i1);

        if (denominator == 0) {
          // If denominator = 0, add 1 to counter and continue with next value
          count_zero += 1;
          theta_i1 = -1;
          break;
        }

        theta_i2 = theta_i1 - numerator/denominator;
        delta_theta = fabs(theta_i2 - theta_i1);
        theta_i1 = theta_i2;

        j++;

        if (j > 10000) {
          printf("\r" PRINT_WARNING "Too many iterations!\n");
          printf("Coordinates: %f, %f, %f, Vs: %f\n",x,y,z,Vs);
          count_fail += 1;
          theta_i1 = -1;
          break;
        }
      }
      if (verbose){
        cout << "Estimated temperature " << theta_i1 << endl
             << "delta_theta           " << delta_theta << endl
             << "Iteration steps       " << j << endl << endl;
      }
    } else {
      theta_i1 = (VsS-c_c)/c_m;
    }

    if (verbose) {
      cout << endl
           << "Point #" << i << endl
           << "Depth                       z       / m    " << z << endl
           << "Pressure                    P       / Pa   " << P << endl
           << "S-Wave velocity             Vs      / km/s " << Vs << endl
           << "S-Wave velocity, corrected  VsS     / km/s " << VsS << endl
           << "Calculated temperature      theta_i / degC " << theta_i1 << endl
           << endl;
    } else {
      progress = static_cast<int>(
                 round(static_cast<double>(i)/static_cast<double>(n))*100.0);
      if (progress % 5 == 0) {
        printf("\rProgress: %i       ", progress);
      }
    }

    // Write estimated temperature to final table
    p4D.setV(theta_i1);
    p4D.setX(x);
    p4D.setY(y);
    p4D.setZ(z);
    data_T.append(p4D);

    // Calculate synthetic velocity from temperature
    if (outVs) {
      double VsSCalc = c_m*theta_i1 + c_c
                       + c_A*exp(-(c_E*1000 + P*c_Va)/c_R/(theta_i1 + c_K));
      double VsCalc = VsSCalc*(1 + c_bV*(fabs(z)/1000 - 50));
      data_Vcalc.append(VsCalc);
    }
  }
  cout << endl << endl;
  return true;
}

void V2T::WRITE_P(QString method) {
  cout << PRINT_WARNING "Test mode, writing pressures to textfile.\n";
  // Undocumented function to test if pressure calculation method works
  QList <double> TEST_Z;
  QList <double> TEST_P;
  QString OutName = "PRESSURE_"+method+".txt";
  QString T_header;
  double zmin, zmax, dz, zi, i;
  zmin = -300000;
  zmax = 0;
  dz = 1000;
  SetPMethod(method);

  cout << "Calculating P for " << method.toUtf8().data() << endl;
  zi = zmin;
  while (zi <= zmax) {
    TEST_Z.append(zi);
    TEST_P.append(pressure(0, 0, zi));
    zi+=dz;
  }

  cout << "Writing pressures to " << OutName.toUtf8().data() << endl;
  QFile tmp(OutName);

  if (!tmp.open(QIODevice::WriteOnly | QIODevice::Text)) {
    cout << PRINT_ERROR "Could not open file " << OutName.toUtf8().data()
         << endl;
    exit(1);
  }

  T_header =  QString("# P from "+method+"\n");
  T_header += QString("# Columns: Depth, Pressure");

  QTextStream out(&tmp);
  out.setRealNumberPrecision(2);
  out.setFieldAlignment(QTextStream::AlignRight);
  out.setRealNumberNotation(QTextStream::FixedNotation);
  out << T_header.toUtf8().data() << endl;
  for (i=0; i < TEST_P.length(); i++) {
    out << TEST_Z[i];
    out << "\t";
    out << TEST_P[i];
    out << endl;
  }
  tmp.close();
  cout << "Done!" << endl;
  exit(0);
}

//##############################################################################
// Code
//##############################################################################
int main(int argc, char *argv[]) {
  V2T VelTemp;
  if (argc > 0) {
    VelTemp.readArgs(argc, argv);
    VelTemp.Info();
    VelTemp.readFile(VelTemp.FileIn(), "vox");
    if (VelTemp.UseCrust()) {
      VelTemp.readFile(VelTemp.FileTCrust(), "crust");
      VelTemp.readFile(VelTemp.FileZTopo(), "topo");
    }
    VelTemp.test_data();
    VelTemp.newton();
    VelTemp.saveFile(VelTemp.FileOut());
  } else {
    VelTemp.usage();
  }
}
