#include "mbed.h"
#include "Grove_LCD_RGB_Backlight.h"

#define N 2048

AnalogIn micro(A1);
Grove_LCD_RGB_Backlight screen(I2C_SDA, I2C_SCL);

Ticker ticker;

float echantillons[N];

void timerInterrupt()
{
	static int tickNumber = 0;
	echantillons[tickNumber]
	
}


int main()
{
		screen.setRGB(100,100,100);//met les leds de fond
		ticker.attach_us(timerInterrupt, 25);
		while(1)
		{
				char str[1000];
				sprintf(str,"%.3f", micro.read());
				screen.locate(0,0);
				screen.print(str);
			  wait(0.05);
		}
}

