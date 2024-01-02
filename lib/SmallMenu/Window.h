#ifndef WINDOW_H
#define WINDOW_H

#include "Arduino.h"
#include <TFT_eSPI.h>
#include "linkList.h"

/*
 * Window 
 * this class iss the basse class to use windows on base of the TFT_espi lib 
 * a window is a rectangullar area on your tft, that provide clipping  
 * 
 */



//#define WINDOW_DEBUG
#define WINDOW_TEST

#ifdef WINDOW_DEBUG
  #define DEBUG_PRINT(...)    Serial.print(__VA_ARGS__)
  #define DEBUG_PRINTLN(...)  Serial.println(__VA_ARGS__)
#else
  #define DEBUG_PRINT(...)
  #define DEBUG_PRINTLN(...)
#endif


#ifdef WINDOW_TEST
  #define TEST_PRINT(...)    Serial.print(__VA_ARGS__)
  #define TEST_PRINTLN(...)  Serial.println(__VA_ARGS__)

#else
  #define TEST_PRINT(...)
  #define TEST_PRINTLN(...)
#endif

#define WINDOW_ENTRY_MAGIC  0x1A2B3C00
#define WINDOW_MAGIC        0x1A2B3C01
#define WINDOW_TEXT_MAGIC   0x1A2B3C02


#define WINDOW_DEF_COLOR_BACKGROUND   TFT_BLACK
#define WINDOW_DEF_COLOR_TEXT         TFT_WHITE
#define WINDOW_DEF_COLOR_BORDER       TFT_ORANGE
#define WINDOW_DEF_FONT               1


// to enable strict type check define own types
typedef uint16_t    color_t;
typedef int32_t     coor_t;
typedef uint32_t    magic_t;
typedef uint8_t     font_t;

class Window;
class TextWindow;


struct WindowListEntry{
    Window *        pWin;
    coor_t          posX,posY;
    coor_t          sizeX,sizeY;
    color_t         backgroundColor;
    bool            visible;
    bool            update;
    bool            transparent;
    int             index;
    magic_t         magic;         
};



class Window{
  private:
  protected:
    static list<struct WindowListEntry *>   s_windowList;
    static color_t               s_desktopColor;
    static TFT_eSPI *            sp_tft;
    static bool                  s_updateRequest;
    static bool                  s_updateAll;

    struct WindowListEntry       m_entry;
    magic_t                      m_magic;
    color_t                      m_borderColor;
    coor_t                       m_borderOffsetX,m_borderOffsetY;
    coor_t                       m_borderWidth;
    bool                         m_border;
    
    void init();
    void copyToThis(const Window& source);
 
    virtual void draw();
    //virtual void calcLayout()   {};
    static void reindexWindowList();
    
  public:
    Window();         
    Window(const Window& source);
    Window(TFT_eSPI * pTft,coor_t x, coor_t y, coor_t w, coor_t h);  // not virtual 
    virtual ~Window();
    virtual Window& operator =(const Window& source);
    virtual void dump();
    virtual bool isValid();
    
    int  getIndex()                           { return m_entry.index; }
    bool isVisible()                          { return m_entry.visible; }
    bool isTopWin();
    void setVisible(bool value);
    
    virtual void setPos(coor_t x,coor_t y)            {m_entry.posX = x; m_entry.posY = y;        setUpdateAll();}
    virtual void setSize(coor_t x,coor_t y)           {m_entry.sizeX = x; m_entry.sizeY = y;      setUpdateAll();}
    virtual void setBackgroundColor(color_t c)        {m_entry.backgroundColor = c;               setUpdate();}
    virtual void setBorderColor(color_t c)            {m_borderColor = c;                         setUpdate();}
    virtual void setBorder(bool value)                {m_border = value;                          setUpdate();}
    virtual void setTransparent(bool value)           {m_entry.transparent = value;               setUpdate();}
    virtual void setBorderOffset(coor_t x,coor_t y)   {m_borderOffsetX = x; m_borderOffsetY = y;  setUpdate();}
    virtual void setBorderWidth(coor_t w)             {if (w>=1){ m_borderWidth = w;}             setUpdate();}


    void setUpdate();
    void setUpdateAll();
    static void updateDisplay();              // draw all changes since last update, redraw windows above any changed window
    
    static void setColorDesktop(color_t color) {Window::s_desktopColor = color;           Window::s_updateAll = true;};
    static color_t getRainbowColor(uint16_t index);


#ifdef WINDOW_TEST
    static bool test(TFT_eSPI * pTft);
#else
    static bool test(TFT_eSPI * pTft)    {return true;}
#endif

};


class TextWindow : public Window{
  private:
  protected:
    String             m_content;
    coor_t             m_textOffsetX,m_textOffsetY;
    color_t            m_textColor;
    font_t             m_textFont;
    int                m_textSize;
    void draw();
    void calcLayout();

  public:
    TextWindow();         
    TextWindow(const TextWindow& source);
    TextWindow(TFT_eSPI * pTft,coor_t x, coor_t y, coor_t w, coor_t h, font_t fon=WINDOW_DEF_FONT, int size=1,color_t col = WINDOW_DEF_COLOR_TEXT );
    ~TextWindow();
    TextWindow& operator =(const TextWindow& source);

    void setText(String newText)                      {m_content = newText;                       setUpdate();}
    void setTextColor(color_t c)                      {m_textColor = c;                           setUpdate();}
    void setTextFont(font_t f)                        {m_textFont = f;                            calcLayout();}
    void setTextOffset(coor_t x,coor_t y)             {m_textOffsetX = x; m_textOffsetY = y;      setUpdate();}
    void setTextSize(int i)                           {m_textSize = i;                            calcLayout();}

    virtual void setSize(coor_t x,coor_t y)           {m_entry.sizeX = x; m_entry.sizeY = y;      calcLayout(); setUpdateAll();}
    virtual void setBorderOffset(coor_t x,coor_t y)   {m_borderOffsetX = x; m_borderOffsetY = y;  calcLayout();}
    virtual void setBorderWidth(coor_t w)             {if (w>=1){ m_borderWidth = w;}             calcLayout();}


};


#endif
