#pragma once

#include <TFT_eSPI.h> // Hardware-specific library
#include <GraphObjects2D.hpp>



class Point3D{
        s32_t _x,_y,_z;
        u32_t _color;
    public:
        // constructors
        Point3D()                                       {_x=0; _y=0; _z=0; _color=0;        }
        Point3D(s32_t x,s32_t y, s32_t z,u32_t color)   {_x=x; _y=y; _z=z; _color=color;    }
        
        // getter
        s32_t getX()                        {return _x;}
        s32_t getY()                        {return _y;}
        s32_t getZ()                        {return _z;}
        u32_t getColor()                    {return _color;}

        // setter
        void move(s32_t x,s32_t y,s32_t z)  {_x=x; _y=y; _z=z;}
        
};



class Line3D{
        Point3D _p0,_p1;
        u32_t _color;
    public:
        // constructors
        Line3D()                                                                    {_p0=Point3D(); _p1=Point3D(); _color=0; }
        Line3D(s32_t x0,s32_t y0,s32_t z0,s32_t x1,s32_t y1,s32_t z1,u32_t color)   {_p0=Point3D(x0,y0,z0,color); _p1=Point3D(x1,y1,z1,color); _color=color;}
        Line3D(Point3D& start, Point3D& end,u32_t color)                            {_p0=start; _p1=end; _color=color;}

        // getter
        Point3D getStart()                      {return _p0;}
        Point3D getEnd()                        {return _p1;}
        u32_t getColor()                        {return _color;}

        // setter
        void newStart(s32_t x,s32_t y,s32_t z)  {_p0=Point3D(x,y,z,_color);}
        void newStart(Point3D& p)               {_p0=p;}
        void newEnd(s32_t x,s32_t y,s32_t z)    {_p1=Point3D(x,y,z,_color);}
        void newEnd(Point3D& p)                 {_p1=p;}
        void moveLineToStart(Point3D& p);
        void moveLineToEnd(Point3D& p);
};

