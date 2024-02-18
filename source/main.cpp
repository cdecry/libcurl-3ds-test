#include "main.h"
#include <curl/curl.h>
#include <malloc.h>
#include <stdarg.h>

#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000

static u32 *SOC_buffer = NULL;
s32 sock = -1, csock = -1;

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

void sendHTTPRequest(std::string test_url, Output output) {
    CURL *curl;
    CURLcode result;
    curl = curl_easy_init();
    std::stringstream debug;

    if (curl == NULL) {
        output.print("HTTP request failed.");
        return;
    }

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_URL, test_url.c_str());

    result = curl_easy_perform(curl);

    if (result != CURLE_OK) {
        debug << "Error: " << curl_easy_strerror(result);
        output.print(debug.str());
        return;
    }
    
    curl_easy_cleanup(curl);
}

int main() {
	gfxInitDefault();
	PrintConsole bottomScreen;
	consoleSelect(consoleInit(GFX_BOTTOM, &bottomScreen));
	
	Output output;
	Engine::Core core(&output);
    
    initSocketService();
    sendHTTPRequest("https://example.com/", output);

	while (aptMainLoop()){
		hidScanInput();
		u32 downEvent = hidKeysDown();
		u32 heldEvent = hidKeysHeld();
		if ((downEvent & KEY_START) || (heldEvent & KEY_START)){
            socExit();
			break;
		}
        
		gspWaitForVBlank();
	}
	
	gfxExit();
	return 0;
}
