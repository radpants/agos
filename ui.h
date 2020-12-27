//
// Created by AJ Austinson on 12/26/2020.
//

#ifndef AGOS_UI_H
#define AGOS_UI_H

typedef struct {
    void (*ClickGenerateCallback)(void);
} UI;

void UIDoFrame();
void UISetClickGenerateCallback(void (*f)());

#endif //AGOS_UI_H
