#include "GraphObjects3D.hpp"

void Line3D::moveLineToStart(Point3D& p){
    s32_t dx = _p1.getX() - _p0.getX();
    s32_t dy = _p1.getY() - _p0.getY();
    s32_t dz = _p1.getZ() - _p0.getZ();

    _p0 = p;
    _p1 = Point3D(p.getX()+dx,p.getY()+dy,p.getZ()+dz,_color);
}

void Line3D::moveLineToEnd(Point3D& p){
    s32_t dx = _p0.getX() - _p1.getX();
    s32_t dy = _p0.getY() - _p1.getY();
    s32_t dz = _p0.getZ() - _p1.getZ();

    _p1 = p;
    _p0 = Point3D(p.getX()+dx,p.getY()+dy,p.getZ()+dz,_color);
}
