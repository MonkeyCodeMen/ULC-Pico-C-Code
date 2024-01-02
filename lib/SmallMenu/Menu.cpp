#include <Menu.hpp>

bool MenuHandler::begin(MenuEntry ** pEntryList, u8_t listCount, TFT_eSPI * pTFT, Mutex * pDisplaySPImutex){
    if (pTFT == NULL)           STOP(F_CHAR("invalid display driver"));
    if (pEntryList == NULL)     STOP(F_CHAR("invalid menu entry list"));
    if (listCount == 0)         STOP(F_CHAR("empty entry list"));

    for (int i=0;i < listCount;i++){
        if (pEntryList[i] == NULL)     STOP(F_CHAR("invalid entry pointer"));
    }



}