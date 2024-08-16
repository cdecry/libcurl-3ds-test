#include "main.h"

/* Split string to lines for teeny 3ds screen.
   This is a function used for debugging the raw HTML returned. */
char* split_text_to_lines(const std::string& text, int max_chars_per_line) {
    std::istringstream stream(text);
    std::string word;
    std::string line;
    std::string combinedText;

    while (stream >> word) {
        if (line.length() + word.length() + 1 <= max_chars_per_line) {
            // Word fits within the line
            if (!line.empty()) {
                line += " ";
            }
            line += word;
        } else {
            // Word doesn't fit; add the current line to combinedText
            if (!line.empty()) {
                combinedText += line + "\n";
                line.clear();
            }
            
            // Check if the word itself is longer than max_chars_per_line
            while (word.length() > max_chars_per_line) {
                combinedText += word.substr(0, max_chars_per_line) + "\n";
                word = word.substr(max_chars_per_line);
            }
            
            // Start the next line with the remainder of the word
            line = word;
        }
    }

    // Add the last line if there's any remaining text
    if (!line.empty()) {
        combinedText += line;
    }

    // Allocate memory for the char* and copy the combinedText into it
    char* cstr = new char[combinedText.length() + 1];
    strcpy(cstr, combinedText.c_str());

    return cstr;
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
    
    std::string url = "https://gamefaqs.gamespot.com/boards/641334-zero-escape-virtues-last-reward/68077218";
    std::stringstream s;

    output.print("Test parsing HTML.\n");

    std::string response = sendHTTPRequest(url, &output);

    HTMLParser parser(response);
    HTMLElement* root = parser.parse();
    // parser.renderHTMLTree(&output, root);
    parser.renderTextContent(&output, root);

    // raw html
    // output.print(parser.stream.str());

    char *cstr = new char[parser.stream.str().length() + 1];
    strcpy(cstr, parser.stream.str().c_str());
    cstr = split_text_to_lines(cstr, 40);

    // render
    gui.addStaticTextElement(cstr);
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
                output.print("You entered: " + inputText + "\n");
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