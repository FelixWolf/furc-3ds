#include <stdio.h>
#include <3ds.h>

int main(void)
{

    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    Result rc = romfsInit();
    if (rc)
        printf("romfsInit: %08lX\n", rc);
    else
    {
        printf("romfs Init Successful!\n");
        /*
        FILE* testfile = fopen("romfs:/3ds.fox", "r");
        FileBuffer testbuffer(testfile, MemoryBuffer::Endian::big);
        Fox5 testfox(testbuffer);
        */
    }
    
    while (aptMainLoop()){
        gspWaitForVBlank();
        hidScanInput();

        u32 kDown = hidKeysDown();
        if (kDown & KEY_START)
            break; // break in order to return to hbmenu
    }

    romfsExit();
    gfxExit();
    return 0;
}