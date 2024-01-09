#pragma once

#include <TFT_eSPI.h> // Hardware-specific library
#include <GraphObjects2D.hpp>



class Point3D{
        int32_t  _x,_y,_z;
        uint32_t _color;
    public:
        // constructors
        Point3D()                                       {_x=0; _y=0; _z=0; _color=0;        }
        Point3D(int32_t  x,int32_t  y, int32_t  z,uint32_t color)   {_x=x; _y=y; _z=z; _color=color;    }
        
        // getter
        int32_t  getX()                        {return _x;}
        int32_t  getY()                        {return _y;}
        int32_t  getZ()                        {return _z;}
        uint32_t getColor()                    {return _color;}

        // setter
        void move(int32_t  x,int32_t  y,int32_t  z)  {_x=x; _y=y; _z=z;}
        
};



class Line3D{
        Point3D _p0,_p1;
        uint32_t _color;
    public:
        // constructors
        Line3D()                                                                    {_p0=Point3D(); _p1=Point3D(); _color=0; }
        Line3D(int32_t  x0,int32_t  y0,int32_t  z0,int32_t  x1,int32_t  y1,int32_t  z1,uint32_t color)   {_p0=Point3D(x0,y0,z0,color); _p1=Point3D(x1,y1,z1,color); _color=color;}
        Line3D(Point3D& start, Point3D& end,uint32_t color)                            {_p0=start; _p1=end; _color=color;}

        // getter
        Point3D getStart()                      {return _p0;}
        Point3D getEnd()                        {return _p1;}
        uint32_t getColor()                        {return _color;}

        // setter
        void newStart(int32_t  x,int32_t  y,int32_t  z)  {_p0=Point3D(x,y,z,_color);}
        void newStart(Point3D& p)               {_p0=p;}
        void newEnd(int32_t  x,int32_t  y,int32_t  z)    {_p1=Point3D(x,y,z,_color);}
        void newEnd(Point3D& p)                 {_p1=p;}
        void moveLineToStart(Point3D& p);
        void moveLineToEnd(Point3D& p);
};

void Line3D::moveLineToStart(Point3D& p){
    int32_t  dx = _p1.getX() - _p0.getX();
    int32_t  dy = _p1.getY() - _p0.getY();
    int32_t  dz = _p1.getZ() - _p0.getZ();

    _p0 = p;
    _p1 = Point3D(p.getX()+dx,p.getY()+dy,p.getZ()+dz,_color);
}

void Line3D::moveLineToEnd(Point3D& p){
    int32_t  dx = _p0.getX() - _p1.getX();
    int32_t  dy = _p0.getY() - _p1.getY();
    int32_t  dz = _p0.getZ() - _p1.getZ();

    _p1 = p;
    _p0 = Point3D(p.getX()+dx,p.getY()+dy,p.getZ()+dz,_color);
}

