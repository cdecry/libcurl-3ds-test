#include "main.h"
#include <curl/curl.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <thread>
#include <citro2d.h>

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

void scroll_down(Output* output) {
    output->setRowStart(output->getRowStart()-1);
    output->printAll();
}
void scroll_up(Output* output) {
    output->setRowStart(output->getRowStart()+1);
    output->printAll();
}

u32 clrRed   = C2D_Color32(0xFF, 0x00, 0x00, 0xFF);
u32 clrBlue  = C2D_Color32(0x00, 0x00, 0xFF, 0xFF);
// const char teststring[] =
// 	"Hello World - now with citro2d!\n"
// 	"The quick brown fox jumps over the lazy dog.\n"
// 	"\n"
// 	"En français: Vous ne devez pas éteindre votre console.\n"
// 	"日本語文章を見せるのも出来ますよ。\n"
// 	"Un poco de texto en español nunca queda mal.\n"
// 	"Πού είναι η τουαλέτα;\n"
// 	"Я очень рад, ведь я, наконец, возвращаюсь домой\n";
    
typedef struct {
    C2D_Text staticText;
    char* str;
} StaticTextElement;

std::vector<StaticTextElement> staticTextEleMap;
C2D_TextBuf g_staticBuf, g_dynamicBuf;

// add to text buffer;
void addStaticTextElement(char* str) {
    C2D_Text staticText;

    StaticTextElement newStaticTextElement;
    newStaticTextElement.staticText = staticText;
    newStaticTextElement.str = str;

    staticTextEleMap.push_back(newStaticTextElement);
}

static void sceneInit(char* textToDisplay)
{
	// Create two text buffers: one for static text, and another one for
	// dynamic text - the latter will be cleared at each frame.
	g_staticBuf  = C2D_TextBufNew(4096); // support up to 4096 glyphs in the buffer
	g_dynamicBuf = C2D_TextBufNew(4096);

    for (int i = 0; i < (int)staticTextEleMap.size(); i++) {
        C2D_TextParse(&staticTextEleMap[i].staticText, g_staticBuf, staticTextEleMap[i].str);
        C2D_TextOptimize(&staticTextEleMap[i].staticText);
    }
}

static void sceneRender(float size)
{
	// Clear the dynamic text buffer
	C2D_TextBufClear(g_dynamicBuf);

	// Draw static text strings

    for (int i = 0; i < (int)staticTextEleMap.size(); i++) {
        C2D_DrawText(&staticTextEleMap[i].staticText, 0, 8.0f, 8.0f, 0.5f, size, size);
    }

    // Draw boxes;
    C2D_DrawRectSolid(50, 0, 0, 50, 50, clrRed);
    // C2D_DrawRectangle(0, 0, 0, 50, 50, clrRed, clrRed, clrRed, clrRed);

	// Generate and draw dynamic text
	char buf[160];
	C2D_Text dynText;
	snprintf(buf, sizeof(buf), "Current text size: %f (Use  to change)", size);
	C2D_TextParse(&dynText, g_dynamicBuf, buf);
	C2D_TextOptimize(&dynText);
	C2D_DrawText(&dynText, C2D_AlignCenter, 200.0f, 220.0f, 0.5f, 0.5f, 0.5f);
}

static void sceneExit(void)
{
	// Delete the text buffers
	C2D_TextBufDelete(g_dynamicBuf);
	C2D_TextBufDelete(g_staticBuf);
}

int main() {
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

    C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

    Output output;
	Engine::Core core(&output);

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

    u32 clrClear = C2D_Color32(0xFF, 0xD8, 0xB0, 0x68);
    
    addStaticTextElement(cstr);
    sceneInit(cstr);
    float size = 0.5f;

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
            scroll_up(&output);
		}
        else if ((downEvent & KEY_DOWN) || (heldEvent & KEY_DOWN)){;
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Adjust the delay as needed
            scroll_down(&output);
		}
        
        if (heldEvent & KEY_L)
			size -= 1.0f/128;
		else if (heldEvent & KEY_R)
			size += 1.0f/128;
		else if (heldEvent & KEY_X)
			size = 0.5f;
		else if (heldEvent & KEY_Y)
			size = 1.0f;

		if (size < 0.25f)
			size = 0.25f;
		else if (size > 2.0f)
			size = 2.0f;

        // Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		C2D_TargetClear(top, clrClear);
		C2D_SceneBegin(top);
        
		sceneRender(size);

		C3D_FrameEnd(0);

		gspWaitForVBlank();
	}
	
    sceneExit();

    C2D_Fini();
	C3D_Fini();
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