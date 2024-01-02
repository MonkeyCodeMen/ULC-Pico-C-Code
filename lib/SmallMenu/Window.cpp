#include "Window.h"


/*
 * ####################################################################
 * 
 *                  Window  static
 * 
 * ####################################################################
 */

list<struct WindowListEntry *>  Window::s_windowList;
uint16_t                Window::s_desktopColor=TFT_BLACK;
TFT_eSPI *              Window::sp_tft=NULL;
bool                    Window::s_updateRequest=false;
bool                    Window::s_updateAll=false;


void Window::updateDisplay(){
  DEBUG_PRINTLN(F("static Window::updateDisplay"));
  WindowListEntry * p;
  int count = Window::s_windowList.length();

  if (Window::s_updateAll == true){
        Window::sp_tft->fillScreen(s_desktopColor);      
  }

  if ((count > 0) && (Window::s_updateRequest == true)){
    bool update = Window::s_updateAll;
    for(int i=0; i < count; i++){
      p = Window::s_windowList.get(i);
      if (p->visible==true){
        if ((p->update == true)||(update==true)){
          // first window for update found
          // update from now on all windows
          update = true;
          // set clipping area
          sp_tft->setViewport(p->posX,p->posY,
                              p->sizeX,p->sizeY);
          p->pWin->draw();
          p->update = false;
        }
      }
    }
    Window::s_updateAll = false;
    Window::s_updateRequest = false;
  }
  sp_tft->resetViewport();
}


void Window::reindexWindowList(){
  DEBUG_PRINTLN(F("Window::reindex  static"));

  int count = Window::s_windowList.length();
  WindowListEntry * p;
  for(int i=0;i < count; i++){
    p = Window::s_windowList.get(i);
    p->index = i;
  }
  
  DEBUG_PRINT(F("Window::reindex  done elments found:"));
  DEBUG_PRINTLN(count); 
}

/*
 * ####################################################################
 * 
 *                  Window  constructor destructor and copy
 * 
 * ####################################################################
 */


void Window::init(){
  DEBUG_PRINT(F("Window::init this:"));
  DEBUG_PRINTLN((uint32_t)this,HEX);
  m_magic         = WINDOW_MAGIC;
  m_entry.magic   = 0;
  m_entry.pWin    = this;
  m_entry.visible = false;
  m_entry.update  = false;
    
  s_windowList.append(&m_entry);
  reindexWindowList();
}

Window::Window(){
  DEBUG_PRINT(F("Window::Window() this:"));
  DEBUG_PRINTLN((uint32_t)this,HEX);
  init();
}

Window::Window(const Window& source){
  DEBUG_PRINT(F("Window::Window(&source) :"));
  DEBUG_PRINTLN((uint32_t)this,HEX);
  init();
  copyToThis(source);
}

Window& Window::operator=(const Window& source){
  DEBUG_PRINT(F("Window::operator= this:"));
  DEBUG_PRINT((uint32_t)this,HEX);
  DEBUG_PRINT(F(" source: "));
  DEBUG_PRINTLN((uint32_t)&source,HEX);

  copyToThis(source);
  return *this;
}



Window::Window(TFT_eSPI * pTft,coor_t x, coor_t y, coor_t w, coor_t h){
  DEBUG_PRINT(F("Window::Window(...) this:"));
  DEBUG_PRINTLN((uint32_t)this,HEX);
  m_magic       = 0;

  if (pTft == NULL){
    DEBUG_PRINT(F("  pointer to TFT is NULL"));
    return;
  }

  if (sp_tft == NULL){
    sp_tft = pTft;
  } else {
    if (sp_tft != pTft){
      DEBUG_PRINT(F("  pointer to TFT shows to other display"));
      return;
    }  
  }

  setVisible(false);    // during update

  m_magic         = WINDOW_MAGIC;
  m_border        = false;
  m_borderOffsetX = 0;
  m_borderOffsetY = 0;
  m_borderWidth   = 1;

  m_entry.posX    = x;
  m_entry.posY    = y;
  m_entry.sizeX   = w;
  m_entry.sizeY   = h;
  m_entry.pWin    = this;
  m_entry.transparent     = false;
  m_entry.backgroundColor = WINDOW_DEF_COLOR_BACKGROUND;

  m_entry.magic   = WINDOW_ENTRY_MAGIC;
  m_entry.index   = s_windowList.length();  // pre set number to avoid complete update on set Visible (new is on top!!!)
  setVisible(true);
  
  setUpdate();
  s_windowList.append(&m_entry);
  reindexWindowList();
}

Window::~Window(){
  DEBUG_PRINT(F("Window::~Window this:"));
  DEBUG_PRINTLN((uint32_t)this,HEX);

  setVisible(false);

  s_windowList.remove(m_entry.index);
  reindexWindowList();

  m_entry.index   = -1;
  m_entry.pWin    = NULL;
  m_entry.magic   = 0;
  m_entry.visible = false;
  m_entry.update  = false;
  m_magic = 0;
}


void Window::copyToThis(const Window& source){
  DEBUG_PRINT(F("Window::copyToThis this:"));
  DEBUG_PRINT((uint32_t)this,HEX);
  DEBUG_PRINT(F(" source: "));
  DEBUG_PRINTLN((uint32_t)&source,HEX);
  if (this == &source){
    return;
  }
 
  if (this->sp_tft == NULL ){
    this->sp_tft = source.sp_tft;
  }

  // all Windows objects are connected to window list
  // no need to touch window list
  setVisible(false);
  m_entry.magic = 0;
  

  // copy attributes
  m_magic         = source.m_magic;
  m_border        = source.m_border;
  m_borderColor   = source.m_borderColor;
  m_borderOffsetX = source.m_borderOffsetX;
  m_borderOffsetY = source.m_borderOffsetY;
  m_borderWidth   = source.m_borderWidth;

  // copy win entry
  m_entry.posX    = source.m_entry.posX;
  m_entry.posY    = source.m_entry.posY;
  m_entry.sizeX   = source.m_entry.sizeX;
  m_entry.sizeY   = source.m_entry.sizeY;
  m_entry.transparent = source.m_entry.transparent; 
  m_entry.backgroundColor = source.m_entry.backgroundColor;

  m_entry.pWin    = this;
  m_entry.magic   = WINDOW_ENTRY_MAGIC;
  setVisible(source.m_entry.visible);
  setUpdate();
  
  DEBUG_PRINTLN(F("this: ------------------------"));
  this->dump();
  DEBUG_PRINTLN(F("source: ----------------------"));
  ((Window*)(&source))->dump();  
  DEBUG_PRINTLN(F("------------------------------"));
  
}





/*
 * ####################################################################
 * 
 *                  Window  members
 * 
 * ####################################################################
 */
bool Window::isTopWin(){
  if (m_entry.index >= s_windowList.length()-1)  {
    // is on top or will be on top 
    return true;
  } else {
    return false;
  }
}


void Window::setVisible(bool value){
  if (value == m_entry.visible){
    // no change
    // do nothing
  } else {
    // window added or removed
    // update all
    m_entry.visible = value; 
    if (isVisible() && isTopWin() && (value == true)){
      // write on top
      setUpdate();                  
    } else {
      setUpdateAll();
    }
  }
}


void Window::setUpdate() {
  if (isVisible()){
    m_entry.update = true;
    Window::s_updateRequest = true;
  }
} 

void Window::setUpdateAll()  {
  if (isVisible()){
    m_entry.update = true;
    Window::s_updateRequest = true;
    Window::s_updateAll = true;
  }
} 


bool Window::isValid(){
  if ((m_magic != WINDOW_MAGIC)&& 
      (m_magic != WINDOW_TEXT_MAGIC))         {return false; }
  if (sp_tft == NULL)                         {return false; }
  if (m_entry.pWin == NULL)                   {return false; }   
  if (m_entry.index < 0)                      {return false; }
  if (m_entry.index >= s_windowList.length()) {return false; } 
  
  return true;
}


void Window::dump(){
  DEBUG_PRINT(F("Window Object at"));
  DEBUG_PRINTLN((uint32_t)this,HEX);
  if (isValid()){ DEBUG_PRINTLN(F("  +- is valid: YES"));  } else {  DEBUG_PRINTLN(F("  +- is valid: NO"));  }
  DEBUG_PRINT(F("  +- m_magicNumber: "));
  DEBUG_PRINTLN((uint32_t)m_magic,HEX);
  DEBUG_PRINT(F("  +- object size: "));
  DEBUG_PRINTLN(sizeof(*this));
  DEBUG_PRINT(F("  +- static pointer to TFT: "));
  DEBUG_PRINTLN((uint32_t)sp_tft,HEX);
  DEBUG_PRINT(F("  +- pointer to windowList: "));
  DEBUG_PRINTLN((uint32_t)&s_windowList,HEX);
  DEBUG_PRINT(F("  +- color border     : "));
  DEBUG_PRINTLN(m_borderColor);
  if (m_border == true){ DEBUG_PRINTLN(F("  +- with border: YES"));  } else {  DEBUG_PRINTLN(F("  +- with border: NO"));  }

  DEBUG_PRINTLN(F("  +- WindowListEntry at : "));
  DEBUG_PRINT  (F("    +- m_magicNumber: "));
  DEBUG_PRINTLN((uint32_t)(m_entry.magic),HEX);
  DEBUG_PRINT  (F("    +- index in windows list: "));
  DEBUG_PRINT(m_entry.index); DEBUG_PRINT(F("  of windows count: ")); DEBUG_PRINTLN(s_windowList.length());
  if (m_entry.visible == true){ DEBUG_PRINTLN(F("    +- visible: YES"));  } else {  DEBUG_PRINTLN(F("    +- visible: NO"));  }
  DEBUG_PRINT  (F("    +- pointer to win : "));
  DEBUG_PRINTLN((uint32_t)(m_entry.pWin),HEX);
  DEBUG_PRINT  (F("    +- position x,y: "));
  DEBUG_PRINT(m_entry.posX); DEBUG_PRINT(F(",")); DEBUG_PRINTLN(m_entry.posY);
  DEBUG_PRINT  (F("    +- size x by y: "));
  DEBUG_PRINT(m_entry.sizeX); DEBUG_PRINT(F(" by ")); DEBUG_PRINTLN(m_entry.posY);
  DEBUG_PRINT(F("      +- color background : "));
  DEBUG_PRINTLN(m_entry.backgroundColor);

  DEBUG_PRINTLN(F("###"));
  DEBUG_PRINTLN();
}


void Window::draw(){
  DEBUG_PRINT(F("Window::draw this:"));
  DEBUG_PRINTLN((uint32_t)this,HEX);
  if (!isValid())   {    return;  }
  if (!isVisible()) {    return;  }
  
  if (m_entry.transparent == false){
    sp_tft->fillRect(0,0,
                     m_entry.sizeX,m_entry.sizeY,
                     m_entry.backgroundColor);
  }
  if (m_border == true){
    for (int i = 0; i < m_borderWidth;i++){
      sp_tft->drawRect(m_borderOffsetX+i,m_borderOffsetY+i,
                       m_entry.sizeX-(2*i)-(2*m_borderOffsetX),m_entry.sizeY-(2*i)-(2*m_borderOffsetY),
                       m_borderColor);  
    }
  }
}






color_t Window::getRainbowColor(uint16_t index){
  byte red,green,blue;
  color_t color;
  uint32_t fraction,band,part;

  if (index == 0xFFFF){
    band = 5;
    part = 0xFF;
  } else {
    fraction = 0xFFFF/6;
    band = index / fraction;
    part = (((index % fraction) * 0xFF) / fraction) & 0xFF;
  }

  
  switch (band) {
    case 0:
      red   = 255;
      green = part;
      blue  = 0;
      break;
   
    case 1:
      red   = 255 - part;
      green = 255;
      blue  = 0;
      break;
    
    case 2:
      red   = 0;
      green = 255;
      blue  = part;
      break;
    
    case 3:
      red   = 0;
      green = 255-part;
      blue  = 255;
      break;
    
    case 4:
      red   = part;
      green = 0;
      blue  = 255;
      break;

    case 5:
      red   = 255;
      green = 0;
      blue  = 255-part;
      break;
  }

  //color = RGB565(red,green,blue);
  color = sp_tft->color565(red,green,blue);
  //color = red << 11 | green << 5 | blue;
  return color;
}



/*
 * ####################################################################
 * 
 *                  TextWindow
 * 
 * ####################################################################
 */

TextWindow::TextWindow(){
  DEBUG_PRINT(F("TextWindow::TextWindow() this:"));
  DEBUG_PRINTLN((uint32_t)this,HEX);
  Window::init();

}

TextWindow::TextWindow(const TextWindow& source){
  DEBUG_PRINT(F("TextWindow::TextWindow(&source) :"));
  DEBUG_PRINTLN((uint32_t)this,HEX);
  Window::init();
  *this = source;
}


TextWindow::TextWindow(TFT_eSPI * pTft,coor_t x, coor_t y, coor_t w, coor_t h, font_t fon, int size,color_t col  )
           :Window(pTft,x,y,w,h){
  DEBUG_PRINT(F("TextWindow::TextWindow(...) this:"));
  DEBUG_PRINTLN((uint32_t)this,HEX);
  m_magic       = WINDOW_TEXT_MAGIC;
  m_content     = "";
  m_textColor   = col;
  m_textFont    = fon;
  m_textSize    = size;
  calcLayout();
}

TextWindow& TextWindow::operator=(const TextWindow& source){
  if (this == &source){
    return *this;
  }
  Window::copyToThis((const Window&)source);
  m_magic       = source.m_magic;
  m_content     = source.m_content;
  m_textColor   = source.m_textColor;
  m_textOffsetX = source.m_textOffsetX;
  m_textOffsetY = source.m_textOffsetY;
  m_textFont    = source.m_textFont;
  m_textSize    = source.m_textSize;

  return *this;
}


TextWindow::~TextWindow(){
  DEBUG_PRINT(F("TextWindow::~TextWindow this:"));
  DEBUG_PRINTLN((uint32_t)this,HEX);
  m_content = "";
}

void TextWindow::calcLayout(){
  sp_tft->setTextSize(m_textSize);
  coor_t delta = m_entry.sizeY - sp_tft->fontHeight(m_textFont);
  if (delta  > 0){
    m_textOffsetY = (delta)/2;
    m_textOffsetX = max(m_textOffsetY/4,m_borderOffsetX+m_borderWidth+1);
  } else {
    m_textOffsetY = m_borderOffsetY+m_borderWidth+1;
    m_textOffsetX = m_borderOffsetX+m_borderWidth+1;
  }
  setUpdate();
}


void TextWindow::draw(){
  DEBUG_PRINT(F("TextWindow::draw this:"));
  DEBUG_PRINTLN((uint32_t)this,HEX);
  if (!isValid())   {    return;  }
  if (!isVisible()) {    return;  }
  
  Window::draw(); // border & background
  sp_tft->setTextColor(m_textColor);
  sp_tft->setTextSize(m_textSize);
  sp_tft->drawString(m_content,m_textOffsetX,m_textOffsetY,m_textFont);
}




/*
 * ####################################################################
 * 
 *                  Modul Test
 * 
 * ####################################################################
 */


#ifdef WINDOW_TEST

bool Window::test(TFT_eSPI * pTft){
  TEST_PRINTLN(F("######################################################"));
  TEST_PRINTLN(F("--- module test TEST_Window -- start --------------"));
  bool error = false;
/////////////////// test 1
  TEST_PRINTLN(F("--------------------------------------------------------------------"));
  TEST_PRINTLN(F("--- module test TEST_Window -- test 1:  (windows) List -------------"));
  TEST_PRINT(F("~~~: Heap memory free: ")); TEST_PRINTLN(ESP.getFreeHeap(),DEC);
  
  Window test;
  if (test.isValid()){
    TEST_PRINTLN(F("OK :  although an empty windows is valid "));
  } else {
    error = true;
    TEST_PRINTLN(F("NOK: base constructor craeted invalid object"));
  }

  test = Window(pTft,10,10,100,20);
  if (test.isValid()){
    TEST_PRINTLN(F("OK :  a valid object of Window class"));
  } else {
    error = true;
    TEST_PRINTLN(F("NOK: not a valid object of Window class"));
  }
  test.setBackgroundColor(TFT_BLUE);
  test.setBorderColor(TFT_RED);
  test.setBorder(true);
  test.setVisible(true);
  Window::updateDisplay();
  delay(1000);


  Window test2 =  Window(pTft,20,20,100,20);
  if (test2.isValid()){
    TEST_PRINTLN(F("OK :  a valid object of Window class"));
  } else {
    error = true;
    TEST_PRINTLN(F("NOK: not a valid object of Window class"));
  }
  test2.setBackgroundColor(TFT_YELLOW);
  test2.setBorderColor(TFT_GREEN);
  test2.setBorder(true);
  test2.setVisible(true);
  Window::updateDisplay();
  delay(1000);

  test.setPos(100,40);
  Window::updateDisplay();
  delay(1000);

  TextWindow test3 =  TextWindow(pTft,20,20,100,20);
  test3.setText(" Hello world! ");
  test3.setBorder(true);
  Window::updateDisplay();
  delay(1000);

  test.setVisible(false);
  test2.setVisible(false);
  test3.setVisible(false);

  TEST_PRINT(F("~~~: Heap memory free: ")); TEST_PRINTLN(ESP.getFreeHeap(),DEC);

  TEST_PRINTLN(F("--- module test TEST_Window -- test 1: finished --------------------"));
  TEST_PRINTLN(F("--------------------------------------------------------------------"));
  
///////// test 2
  TEST_PRINTLN(F("--------------------------------------------------------------------"));
  TEST_PRINTLN(F("--- module test TEST_Window -- test 2: multi windows   --------------"));

  int maxWin = 255;
  color_t color;
  Window * pWins[maxWin];
  uint16_t colorStep=0xFFFF / maxWin;
  for(int i=0;i < maxWin;i++){
    pWins[i]=new Window(pTft,i,i,100,100);
    color = Window::getRainbowColor(i*colorStep);
    pWins[i]->setBackgroundColor(color);
    Window::updateDisplay();
    TEST_PRINT(F("OK :  windows created until now: ")); TEST_PRINT(i); TEST_PRINT(F(" bckg color :")); TEST_PRINTLN(color,HEX);
    TEST_PRINT(F("~~~: Heap memory free: ")); TEST_PRINTLN(ESP.getFreeHeap(),DEC);
    yield();
  }

  for(int i=0;i < maxWin;i++){
    delete pWins[i];
  }
  TEST_PRINTLN(F("OK :  all windows destroyed again"));
  TEST_PRINT(F("~~~: Heap memory free: ")); TEST_PRINTLN(ESP.getFreeHeap(),DEC);


  
  TEST_PRINTLN(F("--- module test TEST_Window -- test 2: finished ----------------"));
  TEST_PRINTLN(F("--------------------------------------------------------------------"));

///////// test 3
//  TEST_PRINTLN(F("--------------------------------------------------------------------"));
//  TEST_PRINTLN(F("--- module test TEST_Window -- test 3: ------------ ------------"));
//  TEST_PRINTLN(F("--- module test TEST_Window -- test 3: finished ----------------"));
//  TEST_PRINTLN(F("--------------------------------------------------------------------"));


///////// test 4
//  TEST_PRINTLN(F("--------------------------------------------------------------------"));
//  TEST_PRINTLN(F("--- module test TEST_Window -- test 4: -------------------------"));
//  TEST_PRINTLN(F("--- module test TEST_Window -- test 4: finished ----------------"));
//  TEST_PRINTLN(F("--------------------------------------------------------------------"));

  if (error == false) {
    TEST_PRINTLN(F("############## success  ################################"));  
    return true;
  } else {
    TEST_PRINTLN(F("############## error ################################"));
  }
  return false;
}
#endif
