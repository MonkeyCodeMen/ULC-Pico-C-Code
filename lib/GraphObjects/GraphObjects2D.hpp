
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
#include <Display.hpp> // Hardware-specific library

class Point{
    private:
        // attributes
        int32_t  _x,_y;
        uint32_t _color;
    public:
        // constructors
        Point()                                     {_x=0; _y=0; _color=0; }
        Point(int32_t  x,int32_t  y, uint32_t color){_x=x; _y=y; _color=color;}
        
        // getter
        int32_t  getX()                        {return _x;}
        int32_t  getY()                        {return _y;}
        uint32_t getColor()                    {return _color;}

        // setter
        void move(int32_t  x,int32_t  y)       {_x=x; _y=y; }
        void newColor(uint32_t color)          {_color=color; }
        
        // draw & erase
        void draw(TFT_eSPI * pDisplay)                  {pDisplay->drawPixel(_x,_y,_color); }
        void erase(TFT_eSPI * pDisplay,uint32_t color)  { pDisplay->drawPixel(_x,_y,color); }

};

class Line{
    private:
        // attributes
        Point _p0,_p1;
        uint32_t _color;
    public:
        // constructors
        Line()                                                                  { _p0=Point(); _p1=Point(); _color=0;     }
        Line(int32_t  x0,int32_t  y0,int32_t  x1,int32_t  y1,uint32_t color)    { _p0=Point(x0,y0,color); _p1=Point(x1,y1,color); _color=color;      }
        Line(Point& start, Point& end,uint32_t color)                           { _p0=start; _p1=end; _color=color; }
        
        // getter
        Point getStart()                    {return _p0;}
        Point getEnd()                      {return _p1;}
        uint32_t getColor()                 {return _color;}

        // setter
        void newColor(uint32_t color)       {_color=color; }
        void newStart(int32_t  x,int32_t  y){_p0=Point(x,y,_color);}
        void newStart(Point& p)             {_p0=p;    }
        void newEnd(int32_t  x,int32_t  y)  {_p1=Point(x,y,_color);  }
        void newEnd(Point& p)               {_p1=p;    }

        void moveLineToStart(Point& p){
                int32_t  dx = _p1.getX() - _p0.getX();
                int32_t  dy = _p1.getY() - _p0.getY();
                _p0 = p;
                _p1 = Point(p.getX()+dx,p.getY()+dy,_color);
            }
        void moveLineToEnd(Point& p){
                int32_t  dx = _p0.getX() - _p1.getX();
                int32_t  dy = _p0.getY() - _p1.getY();
                _p1 = p;
                _p0 = Point(p.getX()+dx,p.getY()+dy,_color);
            }

        // draw & erase
        void draw(TFT_eSPI * pDisplay)                  { pDisplay->drawLine(_p0.getX(),_p0.getY(),_p1.getX(),_p1.getY(),_color);       }
        void erase(TFT_eSPI * pDisplay,uint32_t color)  { pDisplay->drawLine(_p0.getX(),_p0.getY(),_p1.getX(),_p1.getY(),color);        }
};

