#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h> // Hardware-specific library

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



Cube::Cube(TFT_eSPI * pDisplay){
    _pDisplay = pDisplay;
    _height=_pDisplay->height();
    _width =_pDisplay->width();

    // corners on front
    _points[0] = Point3D( 50, 50, 50,TFT_BLACK);
    _points[1] = Point3D(-50, 50, 50,TFT_BLACK);
    _points[2] = Point3D(-50,-50, 50,TFT_BLACK);
    _points[3] = Point3D( 50,-50, 50,TFT_BLACK);
    // corners on back side
    _points[4] = Point3D( 50, 50,-50,TFT_BLACK);
    _points[5] = Point3D(-50, 50,-50,TFT_BLACK);
    _points[6] = Point3D(-50,-50,-50,TFT_BLACK);
    _points[7] = Point3D( 50,-50,-50,TFT_BLACK);

    // edges front side
    _edges[0] = Line3D(_points[0],_points[1],TFT_RED);
    _edges[1] = Line3D(_points[1],_points[2],TFT_RED);
    _edges[2] = Line3D(_points[2],_points[3],TFT_RED);
    _edges[3] = Line3D(_points[3],_points[0],TFT_RED);
    // edges back side
    _edges[4] = Line3D(_points[4],_points[5],TFT_BLUE);
    _edges[5] = Line3D(_points[5],_points[6],TFT_BLUE);
    _edges[6] = Line3D(_points[6],_points[7],TFT_BLUE);
    _edges[7] = Line3D(_points[7],_points[4],TFT_BLUE);
    // edges between front and back
    _edges[8] = Line3D(_points[0],_points[4],TFT_GREEN);
    _edges[9] = Line3D(_points[1],_points[5],TFT_GREEN);
    _edges[10] = Line3D(_points[2],_points[6],TFT_GREEN);
    _edges[11] = Line3D(_points[3],_points[7],TFT_GREEN);

    _angleX = 0;
    _angleY = 0;
    _distance = 550;
    _view = View3D();
    _view.setSize(_width,_height);
    _view.setNewAngle(_angleX,_angleY);
    _view.setNewDistance(_distance);
    _view.calcTransformationParameter();

    _linesToRender = 12;
    _linesDrawn    = 0;

    Render();
}

void Cube::Render(){
    Point start,end;
    Line  line;
    bool  valid[_linesToRender];
    // project Lines
    for (int i=0; i < _linesToRender; i++ ){
        line=_view.projectLine(_edges[i]);
        _toDrawLines[i] = line;
        start = line.getStart();
        end   = line.getEnd();

        if ((start.getX()+end.getX() < 2*_width) || (start.getY()+end.getY() < 2*_height)){
            valid[i] = true;
        } else {
            valid[i] = false;
        }
    }

    globalSPI0_mutex.lock();
    // remove old lines
    for (int i=0; i < _linesDrawn; i++ ){
        _drawnLines[i].erase(_pDisplay,TFT_BLACK);
    }

    // draw new lines
    _linesDrawn = 0;
    for (int i=0; i < _linesToRender; i++ ){
        if (valid[i] == true){
            line=_toDrawLines[i];
            line.draw(_pDisplay);
            _drawnLines[_linesDrawn] = line;
            _linesDrawn++;
        }
    }
    globalSPI0_mutex.free();

}

void Cube::moveView(){
    static int32_t  deltaZ = -2;
    _angleX++;
    _angleX %= 360;
    _angleY++;
    _angleY %= 360;
    _view.setNewAngle(_angleX,_angleY);

    _distance += deltaZ;
    if (_distance < 200) {deltaZ = +2;};
    if (_distance > 600) {deltaZ = -2;};
    _view.setNewDistance(_distance);
    
    _view.calcTransformationParameter();
}



