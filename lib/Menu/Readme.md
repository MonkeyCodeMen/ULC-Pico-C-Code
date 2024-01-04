
menue class ermittelt erstes und letztes sichtbares element
es setzt elemente active und nicht active
verwaltet header (nicht scrollbar immer sichtbar )

bei loop prüft es auf update und updates vom ersten bis zum letzten sichtbaren element
es nimmt die eingaben entgegen und leitet sue zum activen element weiter


die menue elemente können sich selber darstellen, löschen den bereich vorher
sie werden aufegerufen mit draw pTft ox,oy
sie beinhalten die komplette logik für das element
clipping obliegt den elementen


base class diagram to show differrent abstraction levels
============


base is MenuItem

ther are two types of menu items: header & entries
until now only the text base elements are implemented

MenuEntryText and MenuHeaderText take care about rendering the objects to a screen (TFT_eSpI)

MenuEntry{TYPE} takes care about the logical handling / selecting data, but uitilize the parent calss MenuEntryText to render

in the diagram only the important methods are reflected. A lot of methods (like changing fonts, colors etc.) are removed by intention to keep the foucs on th emain concept 

:::mermaid
classDiagram 
    MenuHandler --> TFT_eSPI
    MenuHandler --> MenuEntry
    MenuHandler --> MenuHeader

    MenuItem <|-- MenuHeader
    MenuItem <|-- MenuEntry

    MenuHeader <|-- MenuHeaderText
    MenuEntry  <|-- MenuEntryText
    note for MenuHeader "until now this class does not add any functionality"

    MenuEntryText <|-- MenuEntryBool
    MenuEntryText <|-- MenuEntryInt
    MenuEntryText <|-- MenuEntryList



    class MenuItem{
        +v_onMenuOpen() bool
        +v_needsUpdate() bool
        +v_getHeight() int
        +v_setup(pTFT) bool
        +v_draw(pTFT,x,y)
    }
  


  
  
    class MenuEntry{
        +v_onEvent(event)
    }

    class MenuHeaderText{
        +draw(pTFT,x,y)
        +setup(pTFT) bool
    }
    note for MenuHeaderText "_setup adapt the concrete object 
    to the attributes of the screen 
    (like fon height..)"

    class MenuEntryText{
        +draw(pTFT,x,y,asActive)
    }
    note for MenuEntryText "implements concrete draw method"

    class MenuEntryBool{
        +onEvent(event)
    }
    note for MenuEntryBool "implements concrete event logic"

    class MenuEntryList{
        +onEvent(event)
    }
    class MenuEntryInt{
        +onEvent(event)
    }


:::
