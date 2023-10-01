#include "GraphObjects2D.hpp"

void Line::moveLineToStart(Point& p){
    s32_t dx = _p1.getX() - _p0.getX();
    s32_t dy = _p1.getY() - _p0.getY();

    _p0 = p;
    _p1 = Point(p.getX()+dx,p.getY()+dy,_color);
}

void Line::moveLineToEnd(Point& p){
    s32_t dx = _p0.getX() - _p1.getX();
    s32_t dy = _p0.getY() - _p1.getY();

    _p1 = p;
    _p0 = Point(p.getX()+dx,p.getY()+dy,_color);
}

