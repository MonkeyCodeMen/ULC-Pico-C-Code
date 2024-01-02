# -*- coding: utf-8 -*-
"""
Created on Sat Oct  8 10:52:16 2022

@author: MonkeyCodeMen

a simple text menue with call backs
defined by an init array
like:

   menue = [
       [type,text,call_back,call_back_inc,call_back_dec],
       [MenueObjectTypeType.HEADER,'this is menue x  with status x','','',''],
       [MenueObjectTypeType.CALL_BACK,'do this command','runFunc()','',''],
       ]

Display 128 x 64:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
header and/or status
----------------------
     entry 1 : flag 
     entry 2 : int
<>   entry 3 : list
     entry 4   (call_back)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


entry key:
    up/down/left/right  enter

based on pygame, but pygame is never rendered if not in SIM mode 
the surface is is taken as byte stream and provided to display driver



"""
    
########################################
## text menue  based on luma  

from PIL import Image,ImageDraw,ImageFont
from DisplayDevice  import DisplayDevice
from InputThread    import InputThread
from pygame.locals  import *
        
class TextMenue:
    """
        a class to process and render simple text menues on black/white display
        original written in C for Arduino and now ported Raspi / Python
        
        it shoudl help me (and perhaps you) to generate on very small 2 color displays
        a simple text line based menue like this:

        as an assembler and embedded C programmer I would not call it a low footprint, but  
        for a rasperry Pi it is at least a low/medium footprint 
           
        ~~~~~~~~~~~~~~~~~~
        headline
        ----------------
            item 1 
        #<>#item#2######
            item 3
        ~~~~~~~~~~~~~~~~~~
        
        features as bullet point list:
            * simple setup of entries and action by List of string (example the test at end of this file)
            * target simulation with pygame (on any PC, with full stuffed IDE like spyder ..)
            * handling of entries/text and actions
            * updateable headline for changing status information 
            * menue tree with submenues possible (connected menues, handlin by class)
            * on python many plausbility checks added (and debug print in case of issue found)
            * handling for various entry types included:
                + Sub Menue
                + Text
                + separation line
                + function call
                + Flags
                + INT 
                + LIST 
            * for int,flags and lists handling by "call back" or "direct value manipulation" 
            * true type font rendering of any font of your choice
    """
    
    

    """
        MenueDefEntryEntry:  [Type,"Text","OnEnter","OnLeft","OnRight"]
        MenueDefEntry:       [ MenueDefEntryEntry,MenueDefEntryEntry,... ]
        
        treeEntry: [MenueName,activeIndex,MenueObjectType[]]
        tree :     [  treeEntry,treeEntry,...]
    
    """

    ## init
    
    def __init__(self,name:str,headerAttr:str,displayDevice:DisplayDevice,fontName:str,fontSize:int,entries:list):
        """
        constructor for a Textmenue 
        

        Parameters
        ----------
        name:  Menue name 
            default text for header, header can be overwritten later 
            name stays always as refernce later if used in MenuTree
        
        headerAttr: Attributes for header
            known text attributes: none,invers
        
        device : device type 
            device where menue should be rendered (luma.device see examples)
        
        fontName : string
            name of font used for this text menue
            make sure that the font is avaliable in the path of the raspi 
            or in a "fonts" subdirectory of the current directory
        fontSize : int
            font size
            we have a downwards scrolling menue with header 
            so select a size that allow us three lines + 2 pixel on the rendering device (first argument)

        entries: list with Menue entry defintion
               entries = [
                   [text,textAttr,callBackOnEnter,callBackOnInc,callBackOnDec,CallBackGetValue],
                   [text,textAttr,callBackOnEnter,callBackOnInc,callBackOnDec,CallBackGetValue],
                   [text,textAttr,callBackOnEnter,callBackOnInc,callBackOnDec,CallBackGetValue],
                   [text,textAttr,callBackOnEnter,callBackOnInc,callBackOnDec,CallBackGetValue],
                   [text,textAttr,callBackOnEnter,callBackOnInc,callBackOnDec,CallBackGetValue],
                   [text,textAttr,callBackOnEnter,callBackOnInc,callBackOnDec,CallBackGetValue]
                   ]
            known text attributes: none,invers,line

        Returns
        -------
        None.

        """
        self.__valid             = False        # flag that will be set if init was succesfully
        self.__name              = ''           # name of this object
        self.__debug             = False        # print DEBUG Logs 
        
        
        self.__wrapAround         = False        # scroll to first if last was reached and ...
        self.__font               = None         # pygame font object used for this text menue
        self.__fontHeight         = -1           # height of font
        self.__cursorList         = '<*>'        # cursor for onDec,onEnter,onInc
        self.__cursorInvers       = True         # show line of cursor invers
        self.__ox                 = -1           # offset x for menue start 
        self.__oy                 = -1           # offset y for menue start (keep space for header if needed) 
        self.__lineCount          = -1           # menue lines on one display side

        self.__entryList          = []           # menue entry list
        self.__entryCount         = -1           # count of entries
        self.__entryActive        = -1           # index of active entry
       
        self.__headerExist        = False        # menue has header
        self.__header             = ''           # current header text
        self.__headerInvers       = False        # show header invers
        self.__headerUnderline    = False        # underline header
        
        self.__image               = None         # image (virtual screen)  to draw on  will be created later
        self.__white               = (255,255,255)# draw color
        self.__black               = (0,0,0)      # backgnd color
        
        if name =='':
            self._Warning(" TextMenue.__init__: no name is only allowed for Odysseus ")
            return
        self.__name = name

        try: # device
            self.__device            = displayDevice # use this as display
            self.__width             = displayDevice.getWidth()
            self.__height            = displayDevice.getHeight()
            self.__image             = self.__device.getImage()
        except:
            self._Warning(' device seems not be a DisplayDevice object or could not create Image object')

        try: # header
            headerAttr = headerAttr.lower()
            self.__headerExist  = False    if 'none'      in headerAttr else True
            self.__headerInvers = True     if 'invers'    in headerAttr else False
            self.__headerUnderline = True  if 'underline' in headerAttr else False
            self.__header = name  ## just as init
        except:
            self._Warning(f" TextMenue.__init__: header def not valid {headerAttr}")

        try:  # create font
            ext = fontName[-3:].lower()
            assert(ext in ['ttf','pil'])
            if ext == 'pil':
                self.__font = ImageFont.load(fontName)   # font size ignored 
            else:
                self.__font = ImageFont.truetype(fontName,fontSize)
        except:
            self._Warning(f" TextMenue.__init__: invalid font defintion {fontName}/{fontSize}")
            return

        try: # calc offsets based on font and cursors
            (start_x,start_y,self.__ox,self.__fontHeight) = self.__font.getbbox(self.__cursorList)
            if self.__headerExist == False:
                self.__oy = 0                                
                self.__lineCount = self.__height // self.__fontHeight
            else:
                self.__oy = self.__fontHeight + 2
                self.__lineCount = (self.__height - 2 - self.__fontHeight) // self.__fontHeight
                
            if self.__lineCount < 2:
                self._Warning(" TextMenue.__init__: choose a smaller font for this display ")
                return
        except:
            self._Warning(" TextMenue.__init__: error on calcculating cursor ")
            return

        # still not valid until menues added
        try:
            for desc in entries:
                entry = TextMenueEntry(desc)
                if entry.isValid() == False:
                    self._Warning(f"TextMenue._addEntries: invalid menue entry description: {desc}")
                    return False
                self.__entryList.append(entry)
            self.__entryCount = len(self.__entryList)
            if self.__entryCount < 1:
                self._Warning(f"TextMenue._addEntries: no menue entries found in: {entries}")
                return False
            self.__entryActive = 0
        except:
            self._Warning(f"TextMenue._addEntries: menue entries defintion not valid: {entries}")
            return 

        #seems that all went correct
        self.__valid = True
        
        return 

    def _Debug(self,*args):
        """
        issue a debug message if flag is been set
        :param msg:
        :return:
        """
        if not self.__debug: return
        self._Warning(*args)

    def _Warning(self,*args):
        """
        Issue a warning message - ignores self.debug
        :param str msg:
        :return None:
        """
        print ('##'+self.__name+'::'+self.__class__.__name__ ," ".join(map(str, args)))




    def getName(self):              return self.__name
    def getHeader(self):            return self.__header
    def getActiveEntryIndex(self):  return self.__entryActive
    def getEntryCount(self):        return self.__entryCount
    def isWarpAround(self):         return self.__wrapAround
    def isHeaderInvers(self):       return self.__headerInvers
    def isHeaderUnderline(self):    return self.__headerUnderline
    def hasHeader(self):            return self.__headerExist
    def showHeader(self,switch):    self.__headerExist = switch
    
    def processInput(self,key:int):
        if self.__valid == False: return False
        entry:TextMenueEntry
        if key == K_UP:
            if self.__entryActive == 0:
                if self.__wrapAround == True:
                    self.__entryActive=self.__entryCount-1
                else:
                    return True
            else:
                self.__entryActive=self.__entryActive-1
            self.draw()
            return True
   
        elif key == K_DOWN:
            if self.__entryActive == self.__entryCount-1:
                if self.__wrapAround == True:
                    self.__entryActive = 0
                else:
                    return True
            else:
                self.__entryActive = self.__entryActive +1
            self.draw()
            return True

        elif key == K_LEFT:
            entry = self.__entryList[self.__entryActive]
            if entry.hasOnDec(): 
                entry.onDec()
                self.draw()
            return True

        elif key == K_RIGHT:
            entry = self.__entryList[self.__entryActive]
            if entry.hasOnInc(): 
                entry.onInc()
                self.draw()
            return True

        elif key == K_RETURN:
            entry = self.__entryList[self.__entryActive]
            if entry.hasOnEnter(): 
                entry.onEnter()
                self.draw()
            return True

        return False
        
    def setActiveEntry(self,index):
        if self.__valid == False: return False
        if index < 0 : return False
        if index >= self.__entryCount : return False
        self.__entryActive = index
        return True

    def setWrapAround(self,flag=True):
        if flag != True and flag != False: 
            return False
        self.__wrapAround = flag
        return True
   
    def updateHeader(self,text):
        if self.__valid == False:        return False
        if self.hasHeader() == False:   return False
        self.__header = text
        # if you need a display update do it by youself : menue.draw()
        return True

    
    # render functions
    def draw(self):
        if self.__valid == False: return False
        
        # erase new screen
        self.__image.paste( self.__black, [0,0,self.__image.size[0],self.__image.size[1]])
        
        # draw header
        self._drawHeader()

        # draw visibile objects
        objList      = self.__entryList
        active       = self.__entryActive
        maxLines     = self.__lineCount
        menueEntries = len(objList)
        
        ## select start line to draw
        startEntry = active -  int(maxLines/2)
        if menueEntries <= maxLines:
            startEntry = 0
        elif startEntry < 0:
            startEntry = 0
        elif startEntry + maxLines > menueEntries:
            startEntry = menueEntries - maxLines
        
        for i in range(maxLines):
            if startEntry + i < self.__entryCount:
                oy = self.__oy + i * self.__fontHeight
                self._drawObject(startEntry + i,oy)
                
        self.__device.draw(self.__image)
        return True
    

    def _drawHeader(self):
        if self.__valid == False:        return False
        if self.hasHeader() == False:   return False

        draw = ImageDraw.Draw(self.__image)
        if self.__headerInvers == True:
            bckCol = self.__white
            fntCol = self.__black
            box     = (0,0,self.__width-1,self.__oy-1)
            draw.rectangle(box,fill=bckCol)
        else:
            bckCol = self.__black
            fntCol = self.__white
        
        headerPos = (0,0)
        draw.text(headerPos,self.__header,font=self.__font,fill=fntCol)
        if self.__headerUnderline == True:
            start = (0, self.__fontHeight)
            end   = (self.__width-1,self.__fontHeight)
            draw.line([start,end],fill=fntCol)
        return True


    def _drawObject(self,objIndex,oy):
        if self.__valid == False: return False
        obj:TextMenueEntry


        box     = (0,oy,self.__width-1,oy+self.__fontHeight - 1)
        textPos = (self.__ox,oy)
        obj     = self.__entryList[objIndex]
        draw = ImageDraw.Draw(self.__image)

        if self.__cursorInvers == True and objIndex == self.__entryActive:
            bckCol = self.__white
            fntCol = self.__black
            draw.rectangle(box,fill=bckCol)
        else:
            bckCol = self.__black
            fntCol = self.__white

        ## draw text new
        text = ''
        if obj.hasText() == True:       
            text = text + obj.getText()
        if obj.hasGetValue() == True:   
            text = text + obj.getValue()
        if text != '':  
            draw.text(textPos,text,font=self.__font,fill=fntCol)


        # if active object draw cursor depending on objectTyp
        if objIndex == self.__entryActive:
            curPos = (0,oy)
            cur = ''
            cur += self.__cursorList[0]  if obj.hasOnDec()   else ' '
            cur += self.__cursorList[1]  if obj.hasOnEnter() else ' '
            cur += self.__cursorList[2]  if obj.hasOnInc()   else ' '
            if cur != '': 
                draw.text(curPos,cur,font=self.__font,fill=fntCol)
        return True





########################################
### Menue objects 

class TextMenueEntry:
    def __init__(self,desc:list):
        """
        [text,textAttr,callBackOnEnter,callBackOnInc,callBackOnDec,CallBackGetValue],

        Parameters
        ----------
        desc : descriptor as list
            [text,textAttr,callBackOnEnter,callBackOnInc,callBackOnDec,CallBackGetValue]

        Returns
        -------
        None.

        """
        self.__debug    = False
        self._valid     = False
        self.__name     = 'TexteMenueEntry'
        
        self._text       = ''
        self._textInvers = ''
        
        self._onEnter    = None
        self._onInc      = None
        self._onDec      = None
        self._getValue   = None

        for i in [0,1]:
            if isinstance(desc[i], str) == False:
                    self._Warning(f"TextMenueEntry.__init: descriptor not valid : {desc} ")
                    self._Warning(f" parameter {i} is not a string")
                    return 



        for i in [2,3,4,5]:
            if desc[i] != None:
                if callable(desc[i]) == False:
                    self._Warning(f"TextMenueEntry.__init: descriptor not valid : {desc} ")
                    self._Warning(f" parameter {i} is not None and not callable")
                    return 
        
        
        try:
            if len(desc) != 6:  raise Exception('descriptor not valid', str(desc))
            textAttr:str
            textAttr = desc[1].lower()
            self._text       = desc[0]
            self.__name     += self._text
            self._textExist  = False if 'none'   in textAttr else True
            self._textInvers = True  if 'invers' in textAttr else False
            self._onEnter    = desc[2]   
            self._onInc      = desc[3]   
            self._onDec      = desc[4]
            self._getValue   = desc[5]
            
        except:
            self._Warning(f"TextMenueEntry.__init: descriptor not valid : {desc} ")
            self._Warning("TextMenueEntry.__init: should look like [text,textAttr,callBackOnEnter,callBackOnInc,callBackOnDec,CallBackGetValue] ")
            return 
        
        self._valid = True
        return

    def _Debug(self,*args):
        """
        issue a debug message if flag is been set
        :param msg:
        :return:
        """
        if not self.__debug: return
        self._Warning(*args)

    def _Warning(self,*args):
        """
        Issue a warning message - ignores self.debug
        :param str msg:
        :return None:
        """
        print (self.__name+'::' ," ".join(map(str, args)), '##'+self.__class__.__name__)

        
    def isValid(self):      return self._valid
    def hasOnEnter(self):   return False if self._onEnter==None  else True
    def hasOnInc(self):     return False if self._onInc==None    else True
    def hasOnDec(self):     return False if self._onDec==None    else True
    def hasGetValue(self):  return False if self._getValue==None else True
    def hasText(self):      return False if self._text==''       else True
    def isInvers(self):     return self._textInvers
    def getText(self):      return self._text if self.hasText() == True else ''
    
    def onEnter(self):      
        try:
            if self.hasOnEnter() == True:
                self._onEnter()
                return True
        except:
            pass
        return False
    
    def onInc(self):        
        try:
            if self.hasOnInc() == True:
                self._onInc()
                return True
        except:
            pass
        return False
    
    def onDec(self):       
        try:
            if self.hasOnDec() == True:
                self._onDec()
                return True
        except:
            pass
        return False
    
    def getValue(self):   
        try:
            if self.hasGetValue() == True:
                value = self._getValue()
                return str(value)
        except:
            pass
        return ''
    
    
    

###############################################
##  value entry classes

class TextMenueIntValue:
    def __init__(self,startValue:int=0,min:int=0,max:int=10,wrapAround:bool=False,onChangeHook:callable=None):
        self.val = startValue
        self.resetValue = startValue
        self.min = min
        self.max = max
        self.wrapAround = wrapAround

        self.hook = onChangeHook
        if onChangeHook != None:
            if callable(onChangeHook) == False:
                self.hook = None

    def reset(self):
        self.val = self.resetValue
        if self.hook != None:   
            self.hook()

    def onInc(self):
        self.val += 1
        if self.val > self.max:
            if self.wrapAround == True:
                self.val = self.min 
            else:
                self.val = self.max
        if self.hook != None:   
            self.hook()

    def onDec(self):
        self.val -= 1
        if self.val < self.min:
            if self.wrapAround == True:
                self.val = self.max 
            else:
                self.val = self.min
        if self.hook != None:   
            self.hook()
    
    def show(self):
        return str(self.val)

    def value(self):
        return self.val

class TextMenueListValue:
    def __init__(self,listEntries:list,startIndex:int=0,wrapAround:bool=False,onChangeHook:callable=None):
        self.ind = startIndex
        self.resetIndex = startIndex
        self.wrapAround = wrapAround
        try:
            for i in listEntries:
                if isinstance(i,str) == False:
                    raise ValueError(' list element is not string !!')
            self.list = listEntries
        except:
            self.ind = 0
            self.resetIndex = 0
            self.list = ['def1','def2','def3']
        self.len = len(self.list)

        self.hook = onChangeHook
        if onChangeHook != None:
            if callable(onChangeHook) == False:
                self.hook = None


    def reset(self):
        self.ind = self.resetIndex
        if self.hook != None:   
            self.hook()
    
    def setNewList(self,newList:list,startIndex:int=0):
        self.list = newList
        self.len = len(self.list)        
        self.resetIndex = startIndex
        self.reset()

    def onInc(self):
        self.ind += 1
        if self.ind >= self.len:
            if self.wrapAround == True:
                self.ind = 0 
            else:
                self.ind = self.len-1
        if self.hook != None:   
            self.hook()

    def onDec(self):
        self.ind -= 1
        if self.ind < 0:
            if self.wrapAround == True:
                self.ind = self.len-1 
            else:
                self.ind = 0
        if self.hook != None:   
            self.hook()
    
    def show(self):
        return self.list[self.ind]


    def index(self):
        return self.ind



class TextMenueBoolValue:
    def __init__(self,startValue:bool=False,wrapAround:bool=True,text:list=['off','on'],onChangeHook:callable=None):
        self.val = startValue
        self.resetVal = startValue
        self.wrapAround = wrapAround
        try:
            for i in text:
                if isinstance(i,str) == False:
                    raise ValueError(' text element is not string !!')
            self.text = text
        except:
            self.text = ['off','on']

        self.hook = onChangeHook
        if onChangeHook != None:
            if callable(onChangeHook) == False:
                self.hook = None

    def reset(self):
        self.val = self.resetVal
        if self.hook != None:   
            self.hook()

    def toggle(self):
        self.val = not self.val
        if self.hook != None:   
            self.hook()

    def onInc(self):
        if self.wrapAround == False:
            self.val = True
        else:
            self.toggle()
        if self.hook != None:   
            self.hook()

    def onDec(self):
        if self.wrapAround == False:
            self.val = False
        else:
            self.toggle()
        if self.hook != None:   
            self.hook()
    
    def show(self):
        if self.val == False:
            return self.text[0]
        else:
            return self.text[1]

    def value(self):
        return self.val



    


    

