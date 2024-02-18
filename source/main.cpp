#include "main.h"
#include <curl/curl.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <thread>

typedef struct {
    char *string;
    size_t size;
} Response;

#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000

static u32 *SOC_buffer = NULL;
s32 sock = -1, csock = -1;
size_t write_chunk(void *data, size_t size, size_t nmemb, void *userdata);
void fail_exit(const char *fmt, ...);
void init_socket_service();
std::string send_http_request(std::string test_url, Output* output);

void scroll_down(Output* output) {
    output->setRowStart(output->getRowStart()-1);
    output->printAll();
}
void scroll_up(Output* output) {
    output->setRowStart(output->getRowStart()+1);
    output->printAll();
}

int main() {

	gfxInitDefault();
	PrintConsole bottomScreen;
	consoleSelect(consoleInit(GFX_BOTTOM, &bottomScreen));
	
	Output output;
	Engine::Core core(&output);
    std::string url = "https://example.com";
    std::stringstream s;

    // output.setRowStart(0);
    output.print("Test.\n\n\n");
    
    init_socket_service();
    
    s << send_http_request(url, &output);
    output.print("Response: " + s.str());

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
        
		gspWaitForVBlank();
	}
	
	gfxExit();
	return 0;
}

size_t write_chunk(void *data, size_t size, size_t nmemb, void *userdata) {
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

void fail_exit(const char *fmt, ...) {
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

void init_socket_service() {
    int ret;
    SOC_buffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);

	if(SOC_buffer == NULL)
		fail_exit("memalign: failed to allocate\n");
	if ((ret = socInit(SOC_buffer, SOC_BUFFERSIZE)) != 0)
    	fail_exit("socInit: 0x%08X\n", (unsigned int)ret);
}

std::string send_http_request(std::string test_url, Output* output) {
    CURL *curl;
    CURLcode result;
    curl = curl_easy_init();
    std::stringstream debug;

    if (curl == NULL) {
        output->print("HTTP request failed.");
        return "";
    }

    Response response;
    response.string = (char*)malloc(1);
    response.size = 0;

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_URL, test_url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &response);

    result = curl_easy_perform(curl);

    if (result != CURLE_OK) {
        debug << "Error: " << curl_easy_strerror(result);
        output->print(debug.str());
        return "";
    }
    
    debug << response.string;

    curl_easy_cleanup(curl);

    free(response.string);

    return debug.str();
}