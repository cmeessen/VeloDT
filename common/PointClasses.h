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
#include <QList>
#ifndef POINTCLASSES_H_
#define POINTCLASSES_H_

class Point2D {
private:
  QList <double> Vals;
public:
  Point2D();
  Point2D(double x, double y);
  // Assigning properties
  void setX(double x){Vals[0]=x;}
  void setY(double y){Vals[1]=y;}
  // Retrieving properties
  double x(){return Vals[0];}
  double y(){return Vals[1];}
  // Operators
  double &operator[](int idx);
};

class Point3D {
  private:
    QList <double> Vals;
  public:
    Point3D();
    Point3D(double x, double y, double z);
    // Assigning properties
    void setX(double x){Vals[0]=x;}
    void setY(double y){Vals[1]=y;}
    void setZ(double z){Vals[2]=z;}
    // Retrieving properties
    double x(){return Vals[0];}
    double y(){return Vals[1];}
    double z(){return Vals[2];}
    // Operators
    double &operator[](int idx);
    Point3D &operator=(Point3D p);
};

class Point4D {
  private:
    QList <double> Vals;
  public:
    Point4D();
    Point4D(double x, double y, double z, double v);
    // Assigning properties
    void setX(double x){Vals[0]=x;}
    void setY(double y){Vals[1]=y;}
    void setZ(double z){Vals[2]=z;}
    void setV(double v){Vals[3]=v;}
    // Retrieving properties
    double x(){return Vals[0];}
    double y(){return Vals[1];}
    double z(){return Vals[2];}
    double v(){return Vals[3];}
    double *p(int i){return &Vals[i];}
    // Operators
    double &operator[](int idx);
    Point4D &operator=(Point4D p);
};


class Point5D {
  private:
    QList <double> Vals;
  public:
    Point5D();
    Point5D(double x, double y, double z, double v, double prop);
    // Assigning properties
    void setX(double x){Vals[0]=x;}
    void setY(double y){Vals[1]=y;}
    void setZ(double z){Vals[2]=z;}
    void setV(double v){Vals[3]=v;}
    void setProp(double prop){Vals[4]=prop;}
    // Retrieving properties
    double x(){return Vals[0];}
    double y(){return Vals[1];}
    double z(){return Vals[2];}
    double v(){return Vals[3];}
    double prop(){return Vals[4];}

    double &operator[](int idx);
    Point5D &operator=(Point5D p);
    double *p(int i){return &Vals[i];}
};

#endif // POINTCLASSES_H_
