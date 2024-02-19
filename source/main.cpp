#include "main.h"
#include <stdarg.h>

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240
#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000

static u32 *SOC_buffer = NULL;
s32 sock = -1, csock = -1;

void failExit(const char *fmt, ...);
void initSocketService();

int main() {
	gfxInitDefault();

    Output output;
	Engine::Core core(&output);
    GUI gui;

	consoleSelect(consoleInit(GFX_BOTTOM, NULL));
    initSocketService();
    
    std::string url = "https://example.com";
    std::stringstream s;

    output.print("Testing GUI.\n");

    // s << sendHTTPRequest(url, &output);
    // output.print("Response: " + s.str());
    std::string response = sendHTTPRequest(url, &output);
    char *cstr = new char[response.length() + 1];
    strcpy(cstr, response.c_str());
    
    gui.addStaticTextElement(cstr);
    gui.sceneInit();

	while (aptMainLoop()){
		hidScanInput();
		u32 downEvent = hidKeysDown();
		u32 heldEvent = hidKeysHeld();
		if ((downEvent & KEY_START) || (heldEvent & KEY_START)){
            socExit();
			break;
		}
        else if (downEvent & KEY_B){;
			output.setReverseFlag(!output.getReverseFlag());
			output.printAll();
		}
        else if ((downEvent & KEY_UP) || (heldEvent & KEY_UP)){;
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Adjust the delay as needed
            output.scroll_up();
		}
        else if ((downEvent & KEY_DOWN) || (heldEvent & KEY_DOWN)){;
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Adjust the delay as needed
            output.scroll_down();
		}
        
        // if (heldEvent & KEY_L)
		// 	gui.staticTextEleMap[0].size -= 1.0f/128;
		// else if (heldEvent & KEY_R)
		// 	gui.staticTextEleMap[0].size += 1.0f/128;
		// else if (heldEvent & KEY_X)
		// 	gui.staticTextEleMap[0].size = 0.5f;
		// else if (heldEvent & KEY_Y)
		// 	gui.staticTextEleMap[0].size = 1.0f;

		// if (gui.staticTextEleMap[0].size < 0.25f)
		// 	gui.staticTextEleMap[0].size = 0.25f;
		// else if (gui.staticTextEleMap[0].size > 2.0f)
		// 	gui.staticTextEleMap[0].size = 2.0f;

		gui.sceneRender();
		gspWaitForVBlank();
	}
	
    gui.sceneExit();
	gfxExit();
	return 0;
}

void failExit(const char *fmt, ...) {
	if(sock>0) close(sock);
	if(csock>0) close(csock);

	va_list ap;

	printf(CONSOLE_RED);
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	printf(CONSOLE_RESET);
	printf("\nPress B to exit\n");

	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();

		u32 kDown = hidKeysDown();
		if (kDown & KEY_B) exit(0);
	}
}

void initSocketService() {
    int ret;
    SOC_buffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);

	if(SOC_buffer == NULL)
		failExit("memalign: failed to allocate\n");
	if ((ret = socInit(SOC_buffer, SOC_BUFFERSIZE)) != 0)
    	failExit("socInit: 0x%08X\n", (unsigned int)ret);
}