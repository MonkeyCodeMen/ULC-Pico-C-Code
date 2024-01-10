#pragma once

#include <TFT_eSPI.h> // Hardware-specific library
#include <GraphObjects2D.hpp>
#include <GraphObjects3D.hpp>


class View3D{
        // 2D canvas where 3D objects are projected to
        int16_t _canvasHeight,_canvasWidth;

        // offset from 3D to 2D   (_offX=0 _offY = 0 ==> center of 3D = center of 2d )
        int _offsetX,_offsetY;

        // distance from canvas (obsrver) to 3D center 
        int _obsDistance;
        // viewing angle of observer 
        int _obsAngleX, _obsAngleY;

        // parameters for coordinate tarnsformation
        float _xx, _xy, _xz;
        float _yx, _yy, _yz;
        float _zx, _zy, _zz;
        float _fact;



    public:
        View3D();
        void setSize(uint32_t width,uint32_t height);

        void setNewAngle(uint32_t alpha_x,uint32_t alpha_y);

        void setNewDistance(uint32_t distance)     {_obsDistance=distance;};
        uint32_t getDistance(void)                 {return _obsDistance;};

        Point projectPoint(Point3D& p);
        Line projectLine(Line3D& line);
        void calcTransformationParameter();
};
