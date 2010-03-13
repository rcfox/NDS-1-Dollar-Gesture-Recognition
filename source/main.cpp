#include <nds.h>
#include "dollar.h"
#include "draw.h"
//#include <fat.h>
//#include <fstream>

using namespace dollar;

void ClearScreen(void)
{
     int i;

     for(i = 0; i < 256 * 192; i++)
           VRAM_A[i] = RGB15(0,0,0);
}

/*
void WriteTemplate(TemplatePtr t)
{
    std::ofstream file(t->Name.c_str());

    file << "points.clear();\n";

    for (size_t i = 0; i < t->Points.size(); ++i)
    {
        file << "points.push_back(PointPtr(new Point(" << t->Points[i]->X << ", " << t->Points[i]->Y << ")));\n";
    }

    file << "templates.push_back(TemplatePtr(new Template(" << t->Name << ", points)));\n";

    file.close();
}*/

int main()
{
    touchPosition touch;

	defaultExceptionHandler();

	lcdMainOnBottom();
	irqInit();
	irqEnable(IRQ_VBLANK);

	videoSetMode(MODE_FB0);
	vramSetBankA(VRAM_A_LCD);

	consoleDemoInit();

    ClearScreen();

	/*if (!fatInitDefault())
	{
		printf("Unable to initialize FAT.");
	}*/

	touchPosition firstTouch = { 0, 0 };
	touchPosition lastTouch = { 0, 0 };

	int colours[3] = { 31, 31, 31 };

	TemplateVector templates;
    PointVector points;

    { // Templates
        #include "..\data\A"
        #include "..\data\B"
        #include "..\data\C"
        #include "..\data\D"
        #include "..\data\E"
        #include "..\data\F"
        #include "..\data\G"
        #include "..\data\H"
        #include "..\data\I"
        #include "..\data\J"
        #include "..\data\K"
        #include "..\data\L"
        #include "..\data\M"
        #include "..\data\N"
        #include "..\data\O"
        #include "..\data\P"
        #include "..\data\Q"
        #include "..\data\R"
        #include "..\data\S"
        #include "..\data\T"
        #include "..\data\U"
        #include "..\data\V"
        #include "..\data\W"
        #include "..\data\X"
        #include "..\data\Y"
        #include "..\data\Z"
    }

    ResultPtr r;

	//we like infinite loops in console dev!
	while(1)
	{
		swiWaitForVBlank();

		scanKeys();
		touch=touchReadXY();

		int kd = keysDown();
		if (kd & KEY_A)
		{
			printf(" ");
		}

		/*if (kd & KEY_B)
		{
			templates.push_back(TemplatePtr(new Template(std::string(1, letter++),points)));
		}*/

		if (kd & KEY_X)
		{
			printf("\n");
		}

		if (kd & KEY_Y)
		{
			printf("\x1B[1D \x1B[1D");
		}

		/*if (kd & KEY_R)
		{
			for(size_t i = 0; i < templates.size(); ++i)
			{
			    printf("Writing template: %s\n",templates[i]->Name.c_str());
			    WriteTemplate(templates[i]);
			}
		}*/

		if(kd & KEY_TOUCH)
		{
		    firstTouch = touch;
            draw::plot(touch.px,touch.py,RGB15(colours[0],colours[1],colours[2]));
            points.clear();
            points.push_back(PointPtr(new Point(touch.px, touch.py)));
		}

		if(keysHeld() & KEY_TOUCH)
		{
            lastTouch = touch;
            draw::line(firstTouch.px, firstTouch.py, lastTouch.px, lastTouch.py, RGB15(colours[0],colours[1],colours[2]));
            firstTouch = touch;
            points.push_back(PointPtr(new Point(touch.px, touch.py)));
		}

		if(keysUp() & KEY_TOUCH)
		{
		    if (points.size() > 5)
		    {
                if (templates.size() > 0)
                {
                    r = Recognize(points,templates);
                    if (r->Score > 0.5)
                    {
                        printf("%s",r->Name.c_str());
                    }
                    else
                    {
                        printf("\x1B[1D \x1B[1D");
                    }
                }
		    }
		    else
		    {
		        printf(" ");
		    }
            ClearScreen();
		}
	}

	return 0;
}
