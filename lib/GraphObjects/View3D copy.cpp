#include "View3D.hpp"

View3D::View3D(){
    // select some possible defults
    _fact=180 / 3.14159259; // conversion from degrees to radians.
    setSize(100,100);
    setNewAngle(0,0);
    setNewDistance(550);
    calcTransformationParameter();
}

void View3D::setSize(u32_t width,u32_t height){
    _canvasWidth=width;
    _canvasHeight=height;
    
    _offsetX=_canvasWidth/2; // Position the centre of the 3d conversion space into the centre of the TFT screen.
    _offsetY=_canvasHeight/2;
}

void View3D::setNewAngle(u32_t alpha_x,u32_t alpha_y){
    alpha_x=alpha_x % 360;
    alpha_y=alpha_y % 360;
    _obsAngleX=alpha_x;
    _obsAngleY=alpha_y;
}
Point View3D::projectPoint(Point3D& p){
    s32_t xv, yv, zv;
    float zvt;
    s32_t rx,ry;

    s32_t x=p.getX();
    s32_t y=p.getY();
    s32_t z=p.getZ();

    xv=(x * _xx) + (y * _xy) + (z * _xz);
    yv=(x * _yx) + (y * _yy) + (z * _yz);
    zv=(x * _zx) + (y * _zy) + (z * _zz);

    zvt=zv - _obsDistance;

    rx=256 * (xv / zvt) + _offsetX;
    ry=256 * (yv / zvt) + _offsetY;
    
    return Point(rx,ry,p.getColor());
}

Line View3D::projectLine(Line3D& line){

    Point3D p1=line.getStart();
    Point  pp1=projectPoint(p1); 

    Point3D p2=line.getEnd();
    Point  pp2=projectPoint(p2); 

    return Line(pp1,pp2,line.getColor());
}

void View3D::calcTransformationParameter(){
    float radX, radY, radZ;
    float s1, s2, s3, c1, c2, c3;

    radX=_obsAngleX / _fact; // convert degrees to radians.
    radY=_obsAngleY / _fact;

    // Zan is assumed to be zero

    s1=sin(radY);
    s2=sin(radX);

    c1=cos(radY);
    c2=cos(radX);

    _xx=c1;
    _xy=0;
    _xz=-s1;

    _yx=(s1 * s2);
    _yy=c2;
    _yz=(c1 * s2);

    _zx=(s1 * c2);
    _zy=-s2;
    _zz=(c1 * c2);            
}