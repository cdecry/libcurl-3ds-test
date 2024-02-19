#include "gui.h"

#define MAX_BUF 4096

void GUI::addStaticTextElement(char* str) {
    C2D_Text staticText;

    StaticTextElement newStaticTextElement;
    newStaticTextElement.staticText = staticText;
    newStaticTextElement.str = str;

    staticTextEleMap.push_back(newStaticTextElement);
}

void GUI::sceneInit()
{
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	// Create two text buffers: one for static text, and another one for
	// dynamic text - the latter will be cleared at each frame.
	g_staticBuf  = C2D_TextBufNew(4096); // support up to 4096 glyphs in the buffer
	g_dynamicBuf = C2D_TextBufNew(4096);

    for (int i = 0; i < (int)staticTextEleMap.size(); i++) {
        C2D_TextParse(&staticTextEleMap[i].staticText, g_staticBuf, staticTextEleMap[i].str);
        C2D_TextOptimize(&staticTextEleMap[i].staticText);
    }
}

void GUI::sceneRender()
{
    u32 clrClear = C2D_Color32(0xFF, 0xD8, 0xB0, 0x68);

    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
    C2D_TargetClear(top, clrClear);
    C2D_SceneBegin(top);
	C2D_TextBufClear(g_dynamicBuf);

    for (int i = 0; i < (int)this->staticTextEleMap.size(); i++) {
        C2D_DrawText(&staticTextEleMap[i].staticText, 0, staticTextEleMap[i].x, 
                     staticTextEleMap[i].y, staticTextEleMap[i].z, 
                     staticTextEleMap[i].size, staticTextEleMap[i].size);
    }
    
	// Generate and draw dynamic text
	// char buf[160];
	// C2D_Text dynText;
	// snprintf(buf, sizeof(buf), "Current text size: %f (Use  to change)", size);
	// C2D_TextParse(&dynText, g_dynamicBuf, buf);
	// C2D_TextOptimize(&dynText);
	// C2D_DrawText(&dynText, C2D_AlignCenter, 200.0f, 220.0f, 0.5f, 0.5f, 0.5f);

    C3D_FrameEnd(0);
}

void GUI::sceneExit(void)
{
	// Delete the text buffers
	C2D_TextBufDelete(g_dynamicBuf);
	C2D_TextBufDelete(g_staticBuf);

    C2D_Fini();
	C3D_Fini();
}