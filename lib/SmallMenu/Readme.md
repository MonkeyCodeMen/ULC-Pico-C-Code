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
