
menue class ermittelt erstes und letztes sichtbares element
es setzt elemente active und nicht active
verwaltet header (nicht scrollbar immer sichtbar )

bei loop prüft es auf update und updates vom ersten bis zum letzten sichtbaren element
es nimmt die eingaben entgegen und leitet sue zum activen element weiter


die menue elemente können sich selber darstellen, löschen den bereich vorher
sie werden aufegerufen mit draw pTft ox,oy
sie beinhalten die komplette logik für das element
clipping obliegt den elementen




:::mermaid
classDiagram
    note "text Menu class diagram"
    MenuEntry <|-- MenuListEntry
    MenuEntry <|-- MenuIntEntry
    MenuEntry <|-- MenuBoolEntry
    SmallMenu --> MenuEntry
    SmallMenu --> TFT_eSPI


    class MenuEntry{
        +v_onEvent(MENU_Event_Type)
        +v_getText() String
        +v_setNewText(String)
        +v_hasChanged() bool
        +v_needUpdate() bool
        +v_draw(pTFT,ox,oy,bool active,(dx,dy))
        +v_getHeight() int


    }

    class MenuIntEntry{
        -int _min
        -int _max
        -int _currentValue

        +getValue() int

        #v_onEvent()
        #v_getText() String
    }

    class MenuBoolEntry{
        -String _offText
        -String _onText

        +setOffText()
        +setOnText()
        +getValue() bool

        #v_onEvent()
        #v_getText() String
    }

    class MenuListEntry{
        -String[] _entries
        -bool     _wrapAround
        -int      _currentIndex

        +getCurrentIndex() int
        +getCurrentEntry() String
        
        #v_onEvent()
        #v_getText() String
    }
:::
