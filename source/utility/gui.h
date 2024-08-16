#ifndef GUI_H
#define GUI_H
#define MAX_BUF 24576

#include "common.h"
#include <citro2d.h>

typedef struct {
    C2D_Text staticText;
    char* str;
    float x=2.0f;
    float y=2.0f;
    float z=0.5f;
    float size=0.3f;
} StaticTextElement;

class GUI {
    private:
        C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
        char dynamicText[MAX_BUF];
    public:
        std::vector<StaticTextElement> staticTextEleMap;
        C2D_TextBuf g_staticBuf, g_dynamicBuf;
        void addStaticTextElement(char* str);
        void clearStaticTextElements();
        void sceneInit(void);
        void sceneRender(void);
        void sceneExit(void);
        void setDynamicText(const char* src);
        void appendToDynamicText(const char* src);
};
#endif
