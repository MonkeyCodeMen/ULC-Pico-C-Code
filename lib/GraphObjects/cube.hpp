
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

#include <Arduino.h>
#include <Display.hpp> // Hardware-specific library
#include <GraphObjects2D.hpp>
#include <GraphObjects3D.hpp>
#include <View3D.hpp>


/*
    calss to craete a 3D cube mesh 
    setup view and do a view animation 


    + cube()        (lines) and view are created in constructor 
    + render()      draw the cube from currentview
                        - first project from 3D to 2D canvas area
                        - then erase old lines from last render
                        - then draw new lines
    + moveView()    rotates and increase/decraese view point distance = animation by moving view point
*/

class Cube{
        Point3D _points[8];
        Line3D  _edges[12];

        Line    _toDrawLines[12];
        Line    _drawnLines[12];

        int     _linesToRender;
        int     _linesDrawn;

        View3D  _view;

        TFT_eSPI  * _pDisplay;
        uint32_t _height,_width;

        uint32_t   _angleX,_angleY;
        uint32_t   _distance;
        
    public:
        Cube(TFT_eSPI * pDisplay);
        void Render();
        void moveView();
};

