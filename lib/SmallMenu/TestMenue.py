from datetime import datetime
from TextMenue import TextMenue
from TextMenue import TextMenueIntValue
from TextMenue import TextMenueListValue
from TextMenue import TextMenueBoolValue
from pygame.locals import *

from DisplayDevice  import DisplayDevice
from InputThread    import InputThread

class TestMenue:
    def __init__(self,displayType:DisplayDevice,inputManager:InputThread):
        self.displayType = displayType
        self.input = inputManager

        self.listEntry = TextMenueListValue( ['item1','item2','item3','item4','item5','item6','item7','item8','item9'],0,False)
        self.intEntry  = TextMenueIntValue(10,-3,15,False)
        self.boolEntry = TextMenueBoolValue(False,False,['Off','On'])

        testMenue = [
            ['list: '    ,'',    self.listEntry.reset,self.listEntry.onInc,self.listEntry.onDec,self.listEntry.show         ],
            ['int:  '    ,'',    None,self.intEntry.onInc,self.intEntry.onDec,self.intEntry.show                      ],
            ['bool: '    ,'',    self.boolEntry.toggle,self.boolEntry.onInc,self.boolEntry.onDec,self.boolEntry.show  ],
            ['do print'  ,'',    self.doPrint,None,None,None                                    ],
            ['stop'      ,'',    self.terminate,None,None,None          ]
        ]
        self.menue     = TextMenue('testMenue:','invers,underline',displayType, "fonts/pixelmix.ttf", 8,testMenue)

    def getHeader(self):
        return datetime.now().strftime("testMen: %H:%M:%S")

    def run(self):
        self.runStatus = True   # will be reset by terminate
        lastHeader = ''
        header = ''
        while self.runStatus == True:
            self.menue.processInput( self.input.keyPressed())

            header = self.getHeader()
            if header != lastHeader:
                lastHeader = header
                self.menue.updateHeader(header) 
                self.menue.draw()
        # somebody has called terminate or closed the sim Window

    def doPrint(self):
        print("Hello world!")

    def terminate(self):
        self.runStatus = False

