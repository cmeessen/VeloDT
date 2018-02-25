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
#include "PointClasses.h"
/*******************************************************************************
Point2D
*******************************************************************************/
Point2D::Point2D() {
  Vals.append(0);
  Vals.append(0);
}

Point2D::Point2D(double x, double y) {
  Vals.append(x);
  Vals.append(y);
}

double &Point2D::operator[](int idx) {
  return Vals[idx];
}

/*******************************************************************************
Point3D
*******************************************************************************/
Point3D::Point3D() {
  Vals.append(0.0);
  Vals.append(0.0);
  Vals.append(0.0);
}

Point3D::Point3D(double x, double y, double z) {
  Vals.append(x);
  Vals.append(y);
  Vals.append(z);
}

Point3D &Point3D::operator=(const Point3D p) {
  Vals[0]=p.Vals[0];
  Vals[1]=p.Vals[1];
  Vals[2]=p.Vals[2];
  return *this;
}

double &Point3D::operator [](int idx) {
  return Vals[idx];
}

/*******************************************************************************
Point4D
*******************************************************************************/
Point4D::Point4D() {
  Vals.append(0.0);   // at least 4 values, representing x,y,z,v
  Vals.append(0.0);
  Vals.append(0.0);
  Vals.append(0.0);
};

Point4D::Point4D(double x, double y, double z, double v) {
  Vals.append(x);   // at least 4 values, representing x,y,z,v
  Vals.append(y);
  Vals.append(z);
  Vals.append(v);
};

double &Point4D::operator [](int idx) {
  return Vals[idx];
}

Point4D &Point4D::operator=(const Point4D p) {
  Vals[0]=p.Vals[0];
  Vals[1]=p.Vals[1];
  Vals[2]=p.Vals[2];
  Vals[3]=p.Vals[3];
  return *this;
}

/*******************************************************************************
Point5D
*******************************************************************************/
Point5D::Point5D() {
  Vals.append(0.0);   // at least 5 values, representing x,y,z,v,rho
  Vals.append(0.0);
  Vals.append(0.0);
  Vals.append(0.0);
  Vals.append(0.0);
}

Point5D::Point5D(double x, double y, double z, double v, double prop) {
  Vals.append(x);   // at least 5 values, representing x,y,z,v, and another property (e.g. rho/T)
  Vals.append(y);
  Vals.append(z);
  Vals.append(v);
  Vals.append(prop);
}

double &Point5D::operator [](int idx) {
  return Vals[idx];
}

Point5D &Point5D::operator=(const Point5D p) {
  Vals[0]=p.Vals[0];
  Vals[1]=p.Vals[1];
  Vals[2]=p.Vals[2];
  Vals[3]=p.Vals[3];
  return *this;
}
