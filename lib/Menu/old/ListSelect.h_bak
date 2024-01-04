#ifndef LIST_SELECT_H
#define LIST_SELECT_H

#include "Arduino.h"
#include "Window.h"
#include "linkList.h"



//#define LIST_SELECT_DEBUG
#define LIST_SELECT_TEST

#ifdef LIST_SELECT_DEBUG
  #define DEBUG_PRINT(...)    Serial.print(__VA_ARGS__)
  #define DEBUG_PRINTLN(...)  Serial.println(__VA_ARGS__)
#else
  #define DEBUG_PRINT(...)
  #define DEBUG_PRINTLN(...)
#endif


#ifdef LIST_SELECT_TEST
  #define TEST_PRINT(...)    Serial.print(__VA_ARGS__)
  #define TEST_PRINTLN(...)  Serial.println(__VA_ARGS__)

#else
  #define TEST_PRINT(...)
  #define TEST_PRINTLN(...)
#endif

#define LIST_SELECT_MAGIC        0x1A2B3D01

enum listSelect_border_t {BORDER_NO,BORDER_ALL,BORDER_ENTRY};
enum listSelect_layout_t {LAYOUT_HORIZONTAL,LAYOUT_VERTICAL};

#define LIST_SELECT_DEF_COLOR_BACKGROUND   TFT_BLACK
#define LIST_SELECT_DEF_COLOR_Title        TFT_WHITE
#define LIST_SELECT_DEF_COLOR_ENTRY        TFT_YELLOW
#define LIST_SELECT_DEF_COLOR_BORDER       TFT_GREEN

#define LIST_SELECT_DEF_FONT               1
#define LIST_SELECT_DEF_FONT_SIZE          1
#define LIST_SELECT_DEF_BORDER_TYPE        BORDER_ALL
#define LIST_SELECT_DEF_BORDER_WIDTH       2


typedef list<String> stringList_t;


class ListSelect{
  private:
  protected:
    magic_t               m_magic;
    TextWindow            * mp_title, * mp_entry;
    stringList_t          m_selectList;
    int                   m_count,m_currentEntry;
    bool                  m_circular,m_active,m_visible;
    listSelect_border_t   m_borderStyle;
    listSelect_layout_t   m_layoutType;
    coor_t                m_x,m_y,m_w,m_h;        // this coor
    coor_t                m_tx,m_ty,m_tw,m_th;    // Title coor
    coor_t                m_ex,m_ey,m_ew,m_eh;    // entry coor
    coor_t                m_entryFraction,m_borderWidth,m_borderOffsetX,m_borderOffsetY;
    

    void copyToThis(const ListSelect& source);
    void calcLayout();
    
  public:
    // class base functionality
    ListSelect(TFT_eSPI * pTft,String Title,coor_t x, coor_t y, coor_t w, coor_t h,stringList_t& entries,listSelect_layout_t layout=LAYOUT_HORIZONTAL,coor_t entryFraction=-1);  
    ~ListSelect();
    ListSelect(const ListSelect& source);
    ListSelect& operator =(const ListSelect& source);

    void dump();
    bool isValid();

    // draw all
    void setVisible(bool value);
    void setPos(coor_t x,coor_t y); 
    void setSize(coor_t x,coor_t y);
    void setBackgroundColor(color_t c)      {mp_title->setBackgroundColor(c); mp_entry->setBackgroundColor(c);}    

    // draw border
    void setBorderColor(color_t c)          {mp_title->setBorderColor(c);               mp_entry->setBorderColor(c);}    
    void setBorderOffset(coor_t x,coor_t y) {m_borderOffsetX = x; m_borderOffsetY = y;  calcLayout();}
    void setBorderWidth(coor_t w)           {m_borderWidth = w;                         calcLayout();}
    void setBorderStyle(listSelect_border_t value) {m_borderStyle = value;              calcLayout();} 

    // draw title text
    void setTitleText(String str)           {mp_title->setText(str);}
    void setTitleTextFont(font_t f)         {mp_title->setTextFont(f);}
    void setTitleTextColor(color_t c)       {mp_title->setTextColor(c);}    
    void setTitleTextSize(int i)            {mp_title->setTextSize(i);}
    void setTitleTextOffset(coor_t x,coor_t y)  {mp_title->setTextOffset(x,y);}

    // draw entry text
    void setEntryTextFont(font_t f)         {mp_entry->setTextFont(f);      mp_title->setUpdate();}
    void setEntryTextColor(color_t c)       {mp_entry->setTextColor(c);     mp_title->setUpdate();} 
    void setEntryTextSize(int i)            {mp_entry->setTextSize(i);      mp_title->setUpdate();}
    void setEntryTextOffset(coor_t x,coor_t y)  {mp_entry->setTextOffset(x,y);  mp_title->setUpdate();}
 
    // business logic 
    void setAtcive(bool value)              {m_active = value;}
    void setCircular(bool value)            {m_circular=value;}   
    void updateTicks(int delta);

    String getSelectedString();
    int getSelectedIndex();
    
    void setSelected(int index);
    void setSelected(String str);


#ifdef LIST_SELECT_TEST
    static bool test(TFT_eSPI * pTft);
#else
    static bool test(TFT_eSPI * pTft)    {return true;}
#endif

};




#endif
