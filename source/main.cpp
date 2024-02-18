#include "main.h"

int main() {
	gfxInitDefault();
	PrintConsole bottomScreen;
	consoleSelect(consoleInit(GFX_BOTTOM, &bottomScreen));
	
	Output output;
	Engine::Core core(&output);

	while (aptMainLoop()){
		hidScanInput();
		u32 downEvent = hidKeysDown();
		u32 heldEvent = hidKeysHeld();
		if ((downEvent & KEY_START) || (heldEvent & KEY_START)){
			break;
		}

		gspWaitForVBlank();
	}
	
	gfxExit();
	return 0;
}
