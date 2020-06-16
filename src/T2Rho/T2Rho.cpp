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
#include "T2Rho.h"

using std::cout;
using std::endl;

T2Rho::T2Rho() {
  PMethod = "AK135";
  ERM = new EarthReferenceModel(PMethod);

  // Mineral properties Tab. A1 in Goes et al (2000)
  m_rho.append(3222);
  m_rho.append(3198);
  m_rho.append(3280);
  m_rho.append(3578);
  m_rho.append(3565);

  m_drhodX.append(1182);
  m_drhodX.append(804);
  m_drhodX.append(377);
  m_drhodX.append(702);
  m_drhodX.append(758);

  m_K.append(1.29E+11);
  m_K.append(1.11E+11);
  m_K.append(1.05E+11);
  m_K.append(1.98E+11);
  m_K.append(1.73E+11);

  m_dKdT.append(-1.60E+07);
  m_dKdT.append(-1.20E+07);
  m_dKdT.append(-1.30E+07);
  m_dKdT.append(-2.80E+07);
  m_dKdT.append(-2.10E+07);

  m_dKdP.append(4.2);
  m_dKdP.append(6);
  m_dKdP.append(6.2);
  m_dKdP.append(5.7);
  m_dKdP.append(4.9);

  m_dKdPdX.append(0);
  m_dKdPdX.append(0);
  m_dKdPdX.append(-1.9);
  m_dKdPdX.append(0);
  m_dKdPdX.append(0);

  m_dKdX.append(0);
  m_dKdX.append(-10E9);
  m_dKdX.append(13E9);
  m_dKdX.append(12E9);
  m_dKdX.append(7E9);

  m_alpha0.append(2.01E-5);
  m_alpha0.append(3.871E-5);
  m_alpha0.append(3.206E-5);
  m_alpha0.append(6.969E-5);
  m_alpha0.append(9.91E-6);

  // Initiate rock properties
  r_XFe = 0.1;
  r_comp.append(0);
  r_comp.append(0);
  r_comp.append(0);
  r_comp.append(0);
  r_comp.append(0);
  setComp(0);
}

void T2Rho::usage() {
  cout << endl
      << "usage: T2Rho File_In File_Out [options]" << endl
      << endl
      << "T2Rho is designed to directly process output files from V2T in" << endl
      << "order to obtain densities from temperatures assuming a specific" << endl
      << "mantle composition." << endl
      << endl
      << "  Required input parameters:" << endl
      << "  --------------------------" << endl
      << "  File_In          Path and name of grid file containing x y z Vs" << endl
      << "  File_Out         Output file name and path" << endl
      << endl
      << "  Option    Value Default Description" << endl
      << "  ------    ----- ------- -----------" << endl
      << "  -h                      This information" << endl
      << "  -ERM      string  AK135 P calculation method AK135 or PREM" << endl
      << "  -compc    vals          Custom rock composition" << endl
      << "                          -compc Ol Opx Cpx Sp Gnt" << endl
      << "  -compp    val         0 Use predefined rock compositions:" << endl
      << "                          0 - Garnet Lherzolite after (Jordan," << endl
      << "                              1979; Goes et al, 2000)" << endl
      << "                          1 - On-cratonic (Shapiro and Ritzwoller, 2004)" << endl
      << "                          2 - Off-cratonic (Shapiro and Ritzwoller, 2004)" << endl
      << "                          3 - Oceanic (Shapiro and Ritzwoller, 2004)" << endl
      << "  -xfe      val       0.1 Define iron content of the rock in mole fraction" << endl
      << endl;
  exit(0);
}

bool T2Rho::SetPMethod(QString method) {
  if (ERM->set(method)) {
    PMethod = method;
    return true;
  } else {
    return false;
  }
}

void T2Rho::setComp(QList<double> composition) {
  // Check length
  if (!(composition.length() == 5)) {
    cout << PRINT_ERROR "Number of mineral phases must be equal to 5." << endl;
  }
  // Check sum
  double compsum = 0;
  for (int i=0; i < 5; i++) {
    compsum += composition[i];
  }
  if (!(compsum == 1)) {
    cout << PRINT_ERROR "Sum of the phases is unequal to 1." << endl;
    exit(1);
  } else {
    for (int i=0; i < 5; i++) {
      r_comp[i] = composition[i];
    }
  }
}

void T2Rho::setComp(int c) {
  switch (c) {
    case 0:
      // Default composition after Goes et al (2000)
      r_comp[0] = 0.67;
      r_comp[1] = 0.225;
      r_comp[2] = 0.045;
      r_comp[3] = 0;
      r_comp[4] = 0.06;
      break;
    case 1:
      // On-cratonic Shapiro and Ritzwoller (2004)
      r_comp[0] = 0.83;
      r_comp[1] = 0.15;
      r_comp[2] = 0;
      r_comp[3] = 0;
      r_comp[4] = 0.02;
      break;
    case 2:
      // Off-cratonic Shapiro and Ritzwoller (2004)
      r_comp[0] = 0.68;
      r_comp[1] = 0.18;
      r_comp[2] = 0.11;
      r_comp[3] = 0;
      r_comp[4] = 0.03;
      break;
    case 3:
      // Oceanic Shapiro and Ritzwoller (2004)
      r_comp[0] = 0.75;
      r_comp[1] = 0.21;
      r_comp[2] = 0.035;
      r_comp[3] = 0.005;
      r_comp[4] = 0.0;
      break;
    default:
      cout << PRINT_ERROR "Unknown composition " << c << "\n. Exit.\n";
      exit(1);
  }
}

void T2Rho::argsError(QString val, bool ok) {
  if (!ok) {
    cout << PRINT_ERROR "Invalid value in " << val.toUtf8().data() << endl;
    exit(1);
  }
}

void T2Rho::readArgs(int &argc, char *argv[]) {
  int i;
  bool ok;
  QStringList arg;

  for (i=0; i < argc; i++)
    arg << argv[i];

  if ( (argc > 1) && (arg[1] == "-h" || arg[1] == "-help") ) {
    usage();
  } else if (argc < 3) {
    cout << endl << endl << PRINT_ERROR "Not enough arguments!\n\n";
    usage();
  } else {
    file_in = arg[1].toUtf8().data();
    file_out = arg[2].toUtf8().data();

    for (i=3; i < argc; i++) {
      if (arg[i] == "-ERM") {
        ok = SetPMethod(arg[i+1]);
        argsError(arg[i], ok);
        i++;
      } else if (arg[i] == "-h") {
        usage();
      } else if (arg[i] == "-xfe") {
        r_XFe = arg[i+1].toDouble(&ok);
        argsError(arg[i], ok);
      } else if (arg[i] == "-compc") {
        QList<double> compc;
        for (int j=1; j < 6; j++) {
          compc.append(arg[i+j].toDouble(&ok));
          argsError(arg[i+j], ok);
        }
        setComp(compc);
      } else if (arg[i] == "-compp") {
        int compp = arg[i+1].toInt(&ok);
        argsError(arg[i], ok);
        setComp(compp);
      }
    }
  }
}

void T2Rho::info() {
  cout << "***********************************************************" << endl
       << endl
       << "             Temperature to Density Conversion" << endl
       << endl
       << "***********************************************************" << endl
       << endl
       << "Settings" << endl
       << "--------" << endl
       << "Rock composition (ol,opx,cpx,gnt,sp):" << endl;
  for (int i=0; i < 5; i++)
    cout << r_comp[i] << " ";
  cout << endl
       << "Iron content XFe: " << r_XFe << endl
       << "Earth reference model: " << ERM->type().toUtf8().data() << endl
       << endl;
}

void T2Rho::readFile() {
  /**
  * Basically everything happens in this function. This function reads the
  * input file and directly computes the density. The result is written into
  * the list data_out.
  **/
  double x, y, z, T, P;
  bool okx, oky, okz, okT;
  double T0 = 293.5;  // Reference temperature
  double P0 = 0;      // Reference pressure
  QRegExp separator("(\\s)");

  cout << "Reading file: " << file_in.toUtf8().data() << endl;
  QFile file(file_in);

  if (file.open(QIODevice::ReadOnly)) {
    QTextStream stream(&file);

    while (!stream.atEnd()) {
      QString t = stream.readLine().simplified();

      if (!t.startsWith("#")) {
        QStringList vals = t.split(separator);
        if (vals.count() != 4) {
          file.close();
          cout << PRINT_ERROR "In header of " << file_in.toUtf8().data()
               << ". Counting " << vals.count() << " entries instead of 4."
               << endl;
          exit(1);
        } else {
        x = vals[0].toDouble(&okx);
        y = vals[1].toDouble(&oky);
        z = vals[2].toDouble(&okz);
        T = vals[3].toDouble(&okT);
        // Directly compute density
        P = ERM->pressure(z+1);
        // Compute phase properties and density
        double rho_avrg = 0;
        for (int i=0; i < 5; i++) {
          double dT = T - T0;
          double dP = P - P0;
          double m_K_PX = m_dKdP[i] + r_XFe*m_dKdPdX[i];
          double K_T = m_K[i] + dT*m_dKdT[i] + dP*m_K_PX;
          double rho_PTX = m_rho[i]*(1 - m_alpha0[i]*dT + dP/K_T)
          + m_drhodX[i]*r_XFe;
          rho_avrg += r_comp[i]*rho_PTX;
        }
        // Add result to output data list
        data_out.append(Point5D(x, y, z, T, rho_avrg));
        }
      }
    }
  }
}

void T2Rho::writeFile() {
  QString header;
  // Get timestamp
  QDateTime CurrentTime = QDateTime::currentDateTime();
  QString timefmt = "yyyy-MM-dd hh:mm:ss";
  QString timestamp = CurrentTime.toString(timefmt);
  // General information string
  header = QString("# Created: %1\n").arg(timestamp);
  header += QString("# Input: %1\n").arg(file_in);
  header += QString("# Pressure calculation method: %1\n").arg(PMethod);
  header += QString("# Mantle composition:\n");
  header += QString("# Ol - %1\n").arg(r_comp[0], 5, 'f', 2);
  header += QString("# Opx - %1\n").arg(r_comp[1], 5, 'f', 2);
  header += QString("# Cpx - %1\n").arg(r_comp[2], 5, 'f', 2);
  header += QString("# Sp - %1\n").arg(r_comp[3], 5, 'f', 2);
  header += QString("# Gnt - %1\n").arg(r_comp[4], 5, 'f', 2);
  header += QString("# Iron content XFe: %1\n").arg(r_XFe, 3, 'f', 2);
  header += QString("# Point data\n"
                    "# Columns:\n"
                    "# 1 - X\n"
                    "# 2 - Y\n"
                    "# 3 - Z / m\n"
                    "# 4 - T / degC\n"
                    "# 5 - Rho / kg/m3\n");

  cout << "Writing output file " << file_out.toUtf8().data() << endl;

  QFile tmp(file_out);
  if (!tmp.open(QIODevice::WriteOnly | QIODevice::Text)) {
    cout << PRINT_ERROR "Could not open file " << file_out.toUtf8().data()
         << endl;
    exit(1);
  }

  QTextStream out(&tmp);
  out.setRealNumberPrecision(2);
  out.setFieldAlignment(QTextStream::AlignRight);
  out.setRealNumberNotation(QTextStream::FixedNotation);
  out << header.toUtf8().data();
  for (int i=0; i < data_out.length(); i++) {
    out << data_out[i][0] << "\t"
        << data_out[i][1] << "\t"
        << data_out[i][2] << "\t"
        << data_out[i][3] << "\t"
        << data_out[i][4] << "\n";
  }
  tmp.close();
}

int main(int argc, char *argv[]) {
  T2Rho converter;
  if (argc > 0) {
    converter.readArgs(argc, argv);
    converter.info();
    converter.readFile();
    converter.writeFile();
  } else {
    converter.usage();
  }
}