#include "ListSelect.h"



/*
 * ####################################################################
 * 
 *                  ListSelect  constructor destructor and copy
 * 
 * ####################################################################
 */



ListSelect::ListSelect(TFT_eSPI * pTft,String title,coor_t x, coor_t y, coor_t w, coor_t h,stringList_t& entries,listSelect_layout_t layout,coor_t entryFraction){
  DEBUG_PRINT(F("ListSelect::ListSelect(...) this:"));
  DEBUG_PRINTLN((uint32_t)this,HEX);

  font_t fon=LIST_SELECT_DEF_FONT;
  int size=LIST_SELECT_DEF_FONT_SIZE;
 
  m_selectList.copyFrom(entries);
  m_count         = m_selectList.length();
  m_currentEntry  = 0;
  m_active        = true;
  m_circular      = false;
  m_borderStyle   = LIST_SELECT_DEF_BORDER_TYPE;
  m_x             = x;
  m_y             = y;
  m_w             = w;
  m_h             = h;
  m_layoutType    = layout;
  m_entryFraction = entryFraction;

  // define border
  m_borderWidth   = LIST_SELECT_DEF_BORDER_WIDTH;
  m_borderOffsetX = 0;
  m_borderOffsetY = 0; 
  

  // setup title window
  mp_title = new TextWindow(pTft,m_tx,m_ty,m_tw,m_th,fon,size,LIST_SELECT_DEF_COLOR_Title);
  mp_title->setText(title);
  mp_title->setBackgroundColor(LIST_SELECT_DEF_COLOR_BACKGROUND);
  mp_title->setTransparent(false);

  // setup entry window
  mp_entry = new TextWindow(pTft,m_ex,m_ey,m_ew,m_eh,fon,size,LIST_SELECT_DEF_COLOR_ENTRY);  
  mp_entry->setText(m_selectList.get(m_currentEntry));
  mp_entry->setBackgroundColor(LIST_SELECT_DEF_COLOR_BACKGROUND);
  mp_entry->setTransparent(false);

  calcLayout();


  // done
  setVisible(true);
  m_magic = LIST_SELECT_MAGIC;
}



ListSelect::ListSelect(const ListSelect& source){
  DEBUG_PRINT(F("ListSelect::ListSelect(&source) :"));
  DEBUG_PRINTLN((uint32_t)this,HEX);
  copyToThis(source);
}

ListSelect& ListSelect::operator=(const ListSelect& source){
  DEBUG_PRINT(F("ListSelect::operator= this:"));
  DEBUG_PRINT((uint32_t)this,HEX);
  DEBUG_PRINT(F(" source: "));
  DEBUG_PRINTLN((uint32_t)&source,HEX);

  copyToThis(source);
  return *this;
}



ListSelect::~ListSelect(){
  DEBUG_PRINT(F("ListSelect::~ListSelect() this:"));
  DEBUG_PRINTLN((uint32_t)this,HEX);

  m_magic = 0;
  delete mp_title;
  mp_title = 0;
  delete mp_entry;
  mp_entry = 0;

  m_selectList.removeAll();
  m_magic = LIST_SELECT_MAGIC;
}



void ListSelect::copyToThis(const ListSelect& source){
  DEBUG_PRINT(F("ListSelect::copyToThis this:"));
  DEBUG_PRINT((uint32_t)this,HEX);
  DEBUG_PRINT(F(" source: "));
  DEBUG_PRINTLN((uint32_t)&source,HEX);
  if (this == &source){
    return;
  }

  setVisible(false);   // during configuration

  // copy both text windows  (deep copy)
  mp_title   = new TextWindow(*(source.mp_title));
  mp_entry   = new TextWindow(*(source.mp_entry));

  // class content 
  m_selectList.copyFrom(source.m_selectList);
  m_count         = m_selectList.length();
  m_currentEntry  = source.m_currentEntry;
  m_active        = source.m_active;
  m_circular      = source.m_circular;
  m_layoutType    = source.m_layoutType;
  m_x             = source.m_x;
  m_y             = source.m_y;
  m_w             = source.m_w;
  m_h             = source.m_h;
  m_tx            = source.m_tx;
  m_ty            = source.m_ty;
  m_tw            = source.m_tw;
  m_th            = source.m_th;
  m_ex            = source.m_ex;
  m_ey            = source.m_ey;
  m_ew            = source.m_ew;
  m_eh            = source.m_eh;
  m_borderStyle   = source.m_borderStyle;
  m_borderOffsetX = source.m_borderOffsetX;
  m_borderOffsetY = source.m_borderOffsetY;
  m_entryFraction = source.m_entryFraction;

  //not necessary : calcLayout(); 

  // done
  setVisible(source.m_visible);
  m_magic = LIST_SELECT_MAGIC;
 
}


/*
 * ####################################################################
 * 
 *                  ListSelect  draw / update
 * 
 * ####################################################################
 */

void ListSelect::setVisible(bool value){
  DEBUG_PRINT(F("ListSelect::setVisible this:"));
  DEBUG_PRINTLN((uint32_t)this,HEX);
  if (!isValid()){    return;  }
  m_visible = value;
  mp_title->setVisible(m_visible);
  mp_entry->setVisible(m_visible);
}




void ListSelect::calcLayout(){
  m_tx = m_x;
  m_ty = m_y;
  m_tw = m_w;
  m_th = m_h;

  switch(m_borderStyle){
    case BORDER_NO: 
      mp_title->setBorderWidth(0);
      mp_title->setBorderOffset(0,0);
      mp_title->setBorder(false);
      
      mp_entry->setBorderWidth(0);
      mp_entry->setBorderOffset(0,0);
      mp_entry->setBorder(false);
      break;

    case BORDER_ALL:
      mp_title->setBorderWidth(m_borderWidth);
      mp_title->setBorderOffset(m_borderOffsetX,m_borderOffsetY);
      mp_title->setBorder(true);
      
      mp_entry->setBorderWidth(0);
      mp_entry->setBorderOffset(0,0);
      mp_entry->setBorder(false);
      break;

    case BORDER_ENTRY:
      mp_title->setBorderWidth(0);
      mp_title->setBorderOffset(0,0);
      mp_title->setBorder(false);

      mp_entry->setBorderWidth(m_borderWidth);
      mp_entry->setBorderOffset(m_borderOffsetX,m_borderOffsetY);
      mp_entry->setBorder(true);
      break;
  }


    
  if (m_layoutType == LAYOUT_VERTICAL){
    // toDo
    
    if (m_entryFraction == -1){
      m_entryFraction = (m_h+1) / 2;
    }
    m_ex = m_x;
    m_ey = m_y+(m_h/2);
    m_ew = m_w;
    m_eh = m_entryFraction;
  } else {
    if (m_entryFraction == -1){
      m_entryFraction = (m_w+1) / 2;
    }
    m_ex = m_x + m_w - m_entryFraction;
    m_ey = m_y;
    m_ew = m_entryFraction;
    m_eh = m_h;
    if (m_borderStyle == BORDER_ALL){
      // if border over all reduce size of entry window 
      m_ey = m_ey + m_borderWidth + m_borderOffsetY;
      m_eh = m_eh - (2*m_borderWidth) - (2*m_borderOffsetY);
      m_ew = m_ew - m_borderWidth - m_borderOffsetX;
    }
  }

  mp_title->setSize(m_tw,m_th);
  mp_title->setPos(m_tx,m_ty);
  
  mp_entry->setSize(m_ew,m_eh);
  mp_entry->setPos(m_ex,m_ey);
}

void ListSelect::setPos(coor_t x,coor_t y){
  m_x = x;
  m_y = y;
  calcLayout();
}

void ListSelect::setSize(coor_t w,coor_t h){
  m_w = w;
  m_h = h;
  calcLayout();
}

/*
 * ####################################################################
 * 
 *                  ListSelect  data handling
 * 
 * ####################################################################
 */

void ListSelect::updateTicks(int delta){
  DEBUG_PRINT(F("ListSelect::updateTicks(...) this:"));
  DEBUG_PRINT((uint32_t)this,HEX);
  DEBUG_PRINT(F("  delta ticks"));
  DEBUG_PRINT(delta);
  DEBUG_PRINT(F("  m_currentEntry"));
  DEBUG_PRINT(m_currentEntry);


  if ((m_active == true) && (delta != 0)){
    if (delta > 0){
      if (delta+m_currentEntry >= m_count){
       if (m_circular == true){
         // closed loop selection list
         m_currentEntry = (m_currentEntry+delta)% m_count;
       } else {
         // selection list with head and tail
         m_currentEntry = m_count-1;
       }
      } else {
        m_currentEntry += delta;
      }
    } else if (delta < 0){
      if (delta+m_currentEntry < 0){
       if (m_circular == true){
         // closed loop selection list
         m_currentEntry = (m_currentEntry+delta)% m_count;
         if (m_currentEntry < 0){
          m_currentEntry+=m_count;
         }
       } else {
         // selection list with head and tail
         m_currentEntry = 0;
       }
      } else {
        m_currentEntry += delta;
      }
    }
    mp_entry->setText(getSelectedString());
    //mp_title->setUpdate();  update only entry
  }
  DEBUG_PRINT(F("  new entry"));
  DEBUG_PRINTLN(m_currentEntry);
}

String ListSelect::getSelectedString(){
  DEBUG_PRINT(F("ListSelect::getSelectedString() this:"));
  DEBUG_PRINT((uint32_t)this,HEX);
  DEBUG_PRINT(F("  m_currentEntry"));
  DEBUG_PRINTLN(m_currentEntry);

  return m_selectList.get(m_currentEntry);
}

int ListSelect::getSelectedIndex(){
  DEBUG_PRINT(F("ListSelect::getSelectedIndex() this:"));
  DEBUG_PRINT((uint32_t)this,HEX);
  DEBUG_PRINT(F("  m_currentEntry"));
  DEBUG_PRINTLN(m_currentEntry);

  return m_currentEntry;
}

void ListSelect::setSelected(int index){
  DEBUG_PRINT(F("ListSelect::setSelected(index) this:"));
  DEBUG_PRINT((uint32_t)this,HEX);

  if((index >= 0) && (index < m_count)){
    m_currentEntry = index;
  }
  
  DEBUG_PRINT(F("  m_currentEntry"));
  DEBUG_PRINTLN(m_currentEntry);
}

void ListSelect::setSelected(String str){
  DEBUG_PRINT(F("ListSelect::setSelected(str) this:"));
  DEBUG_PRINT((uint32_t)this,HEX);

  for (int i=0;i < m_count; i++){
    if (str == m_selectList.get(i)){
      m_currentEntry = i; 
    }
  }
  
  DEBUG_PRINT(F("  m_currentEntry"));
  DEBUG_PRINTLN(m_currentEntry);
  
}

bool ListSelect::isValid(){
  if (m_magic != LIST_SELECT_MAGIC)           {return false; }
  if (mp_title == NULL)                       {return false; }
  if (mp_entry == NULL)                       {return false; }
  if (m_count <= 0)                           {return false; }
  if ((m_currentEntry<0) || (m_currentEntry >= m_count)) {return false; }
  return true;
}

/*
 * ####################################################################
 * 
 *                  Modul Test
 * 
 * ####################################################################
 */


#ifdef LIST_SELECT_TEST

bool ListSelect::test(TFT_eSPI * pTft){
  TEST_PRINTLN(F("######################################################"));
  TEST_PRINTLN(F("--- module test ListSelect -- start --------------"));
  bool error = false;
/////////////////// test 1
  TEST_PRINTLN(F("######################################################"));
  TEST_PRINTLN(F("--- module test ListSelect -- constructor/dest / copy --------------"));
  TEST_PRINT(F("~~~: Heap memory free: ")); TEST_PRINTLN(ESP.getFreeHeap(),DEC);
  TEST_PRINTLN(F("---:  constructor "));

  String res;
  stringList_t sel;
  sel.append(String("item 0"));
  sel.append(String("item 1"));
  sel.append(String("item 2"));
  sel.append(String("item 3"));
  sel.append(String("item 4"));
  sel.append(String("item 5"));
  TEST_PRINT(F("~~~: Heap memory free: ")); TEST_PRINTLN(ESP.getFreeHeap(),DEC);
  TEST_PRINTLN(F("---:  constructor ListSelect (stringList before)"));

  ListSelect test(pTft,String("select from"),10,10,300,80,sel,LAYOUT_HORIZONTAL,180);
  test.setTitleTextSize(2);
  test.setEntryTextSize(2);
  test.setTitleTextFont(2);
  test.setEntryTextFont(2);
  
  if (test.isValid()==true){
    TEST_PRINTLN(F("OK :  constructor"));
  } else {
    error = true;
    TEST_PRINTLN(F("NOK:  constructor"));
  }

  ListSelect test2(test);
  TEST_PRINT(F("~~~: Heap memory free: ")); TEST_PRINTLN(ESP.getFreeHeap(),DEC);
  TEST_PRINTLN(F("---:  copy constructor ListSelect "));


  if (test2.isValid()==true){
    TEST_PRINTLN(F("OK :  copy constructor"));
  } else {
    error = true;
    TEST_PRINTLN(F("NOK:  copy constructor"));
  }
  test.setTitleTextSize(2);
  test.setTitleText("select range");
  test.setBorderStyle(BORDER_ALL);
  test2.setTitleTextSize(1);
  test2.setTitleText("select loop");
  test2.setCircular(true);
  test2.setPos(30,160);
  test2.setBorderStyle(BORDER_ENTRY);
  test2.setBackgroundColor(TFT_BLUE);
  test2.setBorderColor(TFT_MAGENTA);
  test2.setBorderWidth(5);

  Window::updateDisplay();

  TEST_PRINT(F("~~~: Heap memory free: ")); TEST_PRINTLN(ESP.getFreeHeap(),DEC);
  TEST_PRINTLN(F("######################################################"));
  TEST_PRINTLN(F("--- module test ListSelect -- inc /dec --------------"));
  int pos;

  TEST_PRINTLN(F("---:  one up "));
  for(int i=0;i < 6;i++){
    pos = test.getSelectedIndex();
    res = test.getSelectedString();
    if (pos==i){
      TEST_PRINTLN(F("OK :  one tick up index "));
    } else {
      error = true;
      TEST_PRINT(F("NOK:  one tick up index  soll:"));
      TEST_PRINT(i);
      TEST_PRINT(F(" ist: "));
      TEST_PRINTLN(pos);
    }
    if (res == sel.get(i)){
      TEST_PRINTLN(F("OK :  one tick up string "));
    } else {
      error = true;
      TEST_PRINT(F("NOK:  one tick up string  soll:"));
      TEST_PRINT(sel.get(i).c_str());
      TEST_PRINT(F(" ist: "));
      TEST_PRINTLN(res.c_str());
    }
    test.updateTicks(1);      
    Window::updateDisplay();
    delay(1000);
  }

  
  TEST_PRINTLN(F("---:  more up "));
  for(int i=0;i < 60;i+=13){
    pos = test.getSelectedIndex();
    if (pos==5){
      TEST_PRINTLN(F("OK :  more up"));
    } else {
      error = true;
      TEST_PRINT(F("NOK:  more up  soll:"));
      TEST_PRINT(5);
      TEST_PRINT(F(" ist: "));
      TEST_PRINTLN(pos);
    }
    test.updateTicks(1);      
  }
  Window::updateDisplay();
  delay(1000);
  
  TEST_PRINTLN(F("---:  one down "));
  for(int i=5;i >= 0;i--){
    pos = test.getSelectedIndex();
    if (pos==i){
      TEST_PRINTLN(F("OK :  one tick down"));
    } else {
      error = true;
      TEST_PRINT(F("NOK:  one tick up  down:"));
      TEST_PRINT(i);
      TEST_PRINT(F(" ist: "));
      TEST_PRINTLN(pos);
    }
    test.updateTicks(-1);      
    Window::updateDisplay();
    delay(1000);
  }

  TEST_PRINTLN(F("---:  more down "));
  for(int i=0;i < 60;i+=13){
    pos = test.getSelectedIndex();
    if (pos==0){
      TEST_PRINTLN(F("OK :  more down"));
    } else {
      error = true;
      TEST_PRINT(F("NOK:  more down  soll:"));
      TEST_PRINT(0);
      TEST_PRINT(F(" ist: "));
      TEST_PRINTLN(pos);
    }
    test.updateTicks((-1)*i);      
  }
  Window::updateDisplay();
  delay(1000);


  TEST_PRINTLN(F("---:  set selected by index "));
  test.setSelected(1);
  res = test.getSelectedString();
  if (res == sel.get(1)){
    TEST_PRINTLN(F("OK :  set selected by index"));
  } else {
      error = true;
      TEST_PRINT(F("NOK:  et selected by index soll:"));
      TEST_PRINT(sel.get(1).c_str());
      TEST_PRINT(F(" ist: "));
      TEST_PRINTLN(res.c_str());
  }
  Window::updateDisplay();
  delay(1000);

  TEST_PRINTLN(F("---:  set selected by string "));
  test.setSelected(sel.get(5));
  pos = test.getSelectedIndex();
  if (pos==5){
    TEST_PRINTLN(F("OK :  set selected by string "));
  } else {
    error = true;
    TEST_PRINT(F("NOK:  set selected by string  soll:"));
    TEST_PRINT(5);
    TEST_PRINT(F(" ist: "));
    TEST_PRINTLN(pos);
  }
  Window::updateDisplay();
  delay(1000);


  TEST_PRINTLN(F("---:  loop one tick up "));
  int soll = 0;
  test2.setSelected(soll);
  for(int i=0;i < 20;i++){
    pos = test2.getSelectedIndex();
    res = test2.getSelectedString();
    if (pos==soll){
      TEST_PRINTLN(F("OK :  loop one tick up index "));
    } else {
      error = true;
      TEST_PRINT(F("NOK:  loop one tick up index  soll:"));
      TEST_PRINT(soll);
      TEST_PRINT(F(" ist: "));
      TEST_PRINTLN(pos);
    }
    if (res == sel.get(soll)){
      TEST_PRINTLN(F("OK :  loop one tick up string "));
    } else {
      error = true;
      TEST_PRINT(F("NOK:  loop one tick up string  soll:"));
      TEST_PRINT(sel.get(soll).c_str());
      TEST_PRINT(F(" ist: "));
      TEST_PRINTLN(res.c_str());
    }
    test2.updateTicks(+1); 
    soll++;
    if (soll >= sel.length()){
      soll=0;     
    }
    Window::updateDisplay();
    delay(1000);
  }


  TEST_PRINTLN(F("---:  loop one tick down "));
  soll =0;
  test2.setSelected(soll);
  for(int i=0;i < 20;i++){
    pos = test2.getSelectedIndex();
    res = test2.getSelectedString();
    if (pos==soll){
      TEST_PRINTLN(F("OK :  loop one tick down index "));
    } else {
      error = true;
      TEST_PRINT(F("NOK:  loop one tick down index  soll:"));
      TEST_PRINT(soll);
      TEST_PRINT(F(" ist: "));
      TEST_PRINTLN(pos);
    }
    if (res == sel.get(soll)){
      TEST_PRINTLN(F("OK :  loop one tick down string "));
    } else {
      error = true;
      TEST_PRINT(F("NOK:  loop one tick down string  soll:"));
      TEST_PRINT(sel.get(soll).c_str());
      TEST_PRINT(F(" ist: "));
      TEST_PRINTLN(res.c_str());
    }
    test2.updateTicks(-1);  
    soll--;
    if (soll < 0){
      soll = sel.length()-1;    
    }
    Window::updateDisplay();
    delay(1000);
  }

  TEST_PRINTLN(F("---:  loop hugh ticks up "));
  soll = 3;
  test2.setSelected(soll);
  test2.updateTicks(1000*sel.length());
  pos = test2.getSelectedIndex();
  if (pos==soll){
    TEST_PRINTLN(F("OK :  loop hugh ticks up"));
  } else {
    error = true;
    TEST_PRINT(F("NOK:  loop hugh ticks up  soll:"));
    TEST_PRINT(soll);
    TEST_PRINT(F(" ist: "));
    TEST_PRINTLN(pos);
  }
  Window::updateDisplay();

  TEST_PRINTLN(F("---:  loop hugh ticks down "));
  soll = 2;
  test2.setSelected(soll);
  test2.updateTicks((-1000)*sel.length());
  pos = test2.getSelectedIndex();
  if (pos==soll){
    TEST_PRINTLN(F("OK :  loop hugh ticks down"));
  } else {
    error = true;
    TEST_PRINT(F("NOK:  loop hugh ticks down  soll:"));
    TEST_PRINT(soll);
    TEST_PRINT(F(" ist: "));
    TEST_PRINTLN(pos);
  }
  Window::updateDisplay();

  TEST_PRINTLN(F("--- module test ListSelect -- test 1: finished --------------------"));
  TEST_PRINTLN(F("--------------------------------------------------------------------"));
  

  if (error == false) {
    TEST_PRINTLN(F("############## success  ################################"));  
    return true;
  } else {
    TEST_PRINTLN(F("############## error ################################"));
  }
  return false;
}
#endif
