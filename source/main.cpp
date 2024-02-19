#include "main.h"
#include <curl/curl.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *string;
    size_t size;
} Response;

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240
#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000

static u32 *SOC_buffer = NULL;
s32 sock = -1, csock = -1;
size_t writeChunk(void *data, size_t size, size_t nmemb, void *userdata);
void failExit(const char *fmt, ...);
void initSocketService();
std::string sendHTTPRequest(std::string test_url, Output* output);

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

size_t writeChunk(void *data, size_t size, size_t nmemb, void *userdata) {
    size_t real_size = size * nmemb;

    Response* response = (Response*) userdata;

    char *ptr = (char*)realloc(response->string, response->size + real_size + 1);
    if (ptr == NULL)
        return -1;
    
    response->string = ptr;
    memcpy(&(response->string[response->size]), data, real_size);
    response->size += real_size;
    response->string[response->size] = 0;
    return real_size;
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

std::string sendHTTPRequest(std::string test_url, Output* output) {
    CURL *curl;
    CURLcode result;
    curl = curl_easy_init();
    std::stringstream debug;

    if (curl == NULL) {
        // output->print("HTTP request failed.");
        return "";
    }

    Response response;
    response.string = (char*)malloc(1);
    response.size = 0;

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_URL, test_url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeChunk);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &response);

    result = curl_easy_perform(curl);

    if (result != CURLE_OK) {
        debug << "Error: " << curl_easy_strerror(result);
        // output->print(debug.str());
        return "";
    }
    
    debug << response.string;

    curl_easy_cleanup(curl);

    free(response.string);

    return debug.str();
}