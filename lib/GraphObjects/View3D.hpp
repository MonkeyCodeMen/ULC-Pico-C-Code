
/*
 * MIT License
 * 
 * Copyright (c) 2024 MonkeyCodeMen@GitHub
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * provided to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#pragma once

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
