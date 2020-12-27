//
// Created by AJ Austinson on 12/26/2020.
//

#include "ui.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include <raygui.h>

static UI _ui = {0};


void UIDoFrame() {
    if(GuiButton((struct Rectangle){ 8, 8, 96, 32 }, "Generate")) {
        if(_ui.ClickGenerateCallback) {
            _ui.ClickGenerateCallback();
        }
    }
}

void UISetClickGenerateCallback(void (*f)()) {
    _ui.ClickGenerateCallback = f;
}
