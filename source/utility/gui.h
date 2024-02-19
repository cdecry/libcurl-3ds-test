#ifndef GUI_H
#define GUI_H

#include "common.h"
#include <citro2d.h>

typedef struct {
    C2D_Text staticText;
    char* str;
    float x=8.0f;
    float y=8.0f;
    float z=0.5f;
    float size=0.5f;
} StaticTextElement;

class GUI {
    private:
        C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    public:
        std::vector<StaticTextElement> staticTextEleMap;
        C2D_TextBuf g_staticBuf, g_dynamicBuf;
        void addStaticTextElement(char* str);
        void sceneInit(void);
        void sceneRender(void);
        void sceneExit(void);
};
#endif
