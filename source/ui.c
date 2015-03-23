#include "global.h"

u32 videoRef = 0;
u32 savedVideoState[10];
u32 allowDirectScreenAccess = 0;

u32 bottomFrameBuffer = 0;
u32 allocFrameBuffer = 0;

extern u32 IoBasePad;

void debounceKey() {
	vu32 t;
	for (t = 0; t < 0x1000000; t++) {
	}
}

void acquireVideo() {
	controlVideo(CONTROLVIDEO_ACQUIREVIDEO, 0, 0, 0);
	bottomFrameBuffer = controlVideo(CONTROLVIDEO_GETFRAMEBUFFER, 0, 0, 0);
}

void releaseVideo() {
	controlVideo(CONTROLVIDEO_RELEASEVIDEO, 0, 0, 0);
}

void updateScreen() {
	controlVideo(CONTROLVIDEO_UPDATESCREEN, 0, 0, 0);
}

s32 showMenu(u8* title, u32 entryCount, u8* captions[]) {
	u32 maxCaptions = 18;
	u32 i;
	s32 select = 0;
	u8 buf[200];
	u32 pos;
	u32 x = 10, y = 10, key = 0;
	u32 drawStart, drawEnd;

	while(1) {
		blank(0, 0, 320, 240);
		pos = x;
		print(title, x, pos, 255, 0, 0);
		pos += 20;
		drawStart = (select / maxCaptions) * maxCaptions;
		drawEnd = drawStart + maxCaptions;
		if (drawEnd > entryCount) {
			drawEnd = entryCount;
		}
		for (i = drawStart; i < drawEnd; i++) {
			strcpy(buf, (i == select) ? " * " : "   ");
			strcat(buf, captions[i]);
			print(buf, x, pos, 0, 0, 0);
			pos += 10;
		}
		updateScreen();
		while((key = waitKey()) == 0);
		if (key == BUTTON_DD) {
			select += 1;
			if (select >= entryCount) {
				select = 0;
			}
		}
		if (key == BUTTON_DU) {
			select -= 1;
			if (select < 0) {
				select = entryCount - 1;
			}
		}
		if (key == BUTTON_A) {
			return select;
		}
		if (key == BUTTON_B) {
			return -1;
		}
	}
}

int showMsg(u8* msg) {
	if (!allowDirectScreenAccess) {
		return;
	}
	acquireVideo();


	while(1) {
		blank(0, 0, 320, 240);
		print(msg, 10, 10, 255, 0, 0);
		print("Press [B] to close.", 10, 220, 0, 0, 255);
		updateScreen();
		u32 key = waitKey();
		if (key == BUTTON_B) {
			break;
		}
	}
	releaseVideo();
	return 0;
}


u32 getKey() {
	return (*(vu32*)(IoBasePad) ^ 0xFFF) & 0xFFF;
}

u32 waitKey() {
	u32 key;
	// wait key to be released
	while (getKey() != 0);
	while (1) {
		key = getKey();
		if (key != 0) {
			break;
		}
	}
	return key;
}

