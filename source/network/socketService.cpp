#include "socketService.h"

void SocketService::initSocketService() {
    int ret;
    SOC_buffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);

	if(SOC_buffer == NULL)
		failExit("memalign: failed to allocate\n");
	if ((ret = socInit(SOC_buffer, SOC_BUFFERSIZE)) != 0)
    	failExit("socInit: 0x%08X\n", (unsigned int)ret);
}


void SocketService::failExit(const char *fmt, ...) {
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