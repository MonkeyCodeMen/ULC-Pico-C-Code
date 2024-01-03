
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

    MenuItem <|-- MenuHeader
    MenuItem <|-- MenuEntry

    MenuHeader <|-- MenuHeaderText
    MenuEntry  <|-- MenuEntryText

    MenuEntryText <|-- MenuEntryBool
    MenuEntryText <|-- MenuEntryInt
    MenuEntryText <|-- MenuEntryList

    MenuHandler --> MenuEntry
    MenuHandler --> TFT_eSPI


    class MenuItem{
        +v_onMenuOpen() bool
        +v_needsUpdate() bool
        +v_getHeight() bool
        +v_setup(pTFT)
    }

    class MenuHeader{
        +v_draw(pTFT,x,y)
    }

    class MenuEntry{
        +v_draw(pTFT,x,y,asActive)
        +v_onEvent(event)
    }

    class MenuHeaderText{
        +draw(pTFT,x,y)
    }

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
