#include "main.h"

int main() {
	gfxInitDefault();

    Output output;
	Engine::Core core(&output);
    SocketService socketService;
    GUI gui;

	consoleSelect(consoleInit(GFX_BOTTOM, NULL));
    socketService.initSocketService();
    
    std::string url = "https://example.com";
    std::stringstream s;

    output.print("Testing GUI.\n");

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