#include "main.h"

// Function to convert user input into a valid URL
std::string formatURL(const std::string& input, size_t mode = 0) {
    std::string url = input;

    // Check if the input starts with "http://" or "https://"
    if (url.substr(0, 7) != "http://" && url.substr(0, 8) != "https://") {
        if (mode == 0) {
            url = "https://www." + url;
        } else if (mode == 1) {
            url = "https://" + url;
        }
    }

    return url;
}

size_t fetchAndRenderContent(const std::string& url, Output& output, GUI& gui) {
    // Fetch the new URL content
    std::string response = sendHTTPRequest(url, &output);

    // Parse the HTML content
    HTMLParser parser(response);
    HTMLElement* root = parser.parse();
    parser.renderTextContent(&output, root);

    // Add dynamic text (todo)
    std::string src = parser.stream.str();
    const char * cstr = src.c_str();
    gui.setDynamicText(cstr);
    output.print(std::to_string(strlen(cstr)));

    return strlen(cstr);
}

int main() {
	gfxInitDefault();

    Output output;
	Engine::Core core(&output);
    SocketService socketService;
    GUI gui;
    Keyboard keyboard;

	consoleSelect(consoleInit(GFX_BOTTOM, NULL));
    socketService.initSocketService();

    output.print("Press A to enter a URL.\n");
    gui.setDynamicText("no website");
    gui.sceneInit();

	while (aptMainLoop()){
		hidScanInput();
		u32 downEvent = hidKeysDown();
		u32 heldEvent = hidKeysHeld();

        std::string inputText;
        SwkbdButton button = SWKBD_BUTTON_NONE;

		if ((downEvent & KEY_START) || (heldEvent & KEY_START)){
            socExit();
			break;
		}
        else if (downEvent & KEY_A) {
            keyboard.init(SWKBD_TYPE_NORMAL, 3);
            keyboard.setHintText("input website url");
            keyboard.setButtons("test", "test2", "go!");
            button = keyboard.getInput(inputText);

            if (button != SWKBD_BUTTON_NONE) {
                std::string validURL = formatURL(inputText);
                output.print("Fetching " + validURL + "...\n");
                if (fetchAndRenderContent(validURL, output, gui) <= 22) {
                    validURL = formatURL(inputText, 1);
                    output.print("Fetching " + validURL + "...\n");
                    fetchAndRenderContent(validURL, output, gui);
                };
            }
        }
        else if (downEvent & KEY_B) {
			output.setReverseFlag(!output.getReverseFlag());
			output.printAll();
		}
        else if ((downEvent & KEY_UP) || (heldEvent & KEY_UP)){
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Adjust the delay as needed
            output.scroll_up();
		}
        else if ((downEvent & KEY_DOWN) || (heldEvent & KEY_DOWN)){
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Adjust the delay as needed
            output.scroll_down();
		}

		gui.sceneRender();
		gspWaitForVBlank();
	}
	
    gui.sceneExit();
	gfxExit();
	return 0;
}