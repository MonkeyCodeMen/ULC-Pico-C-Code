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

