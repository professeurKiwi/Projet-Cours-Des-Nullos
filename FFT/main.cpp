#include "mbed.h"
#include "Grove_LCD_RGB_Backlight.h"
#include "FFT/FFT.h"

#define N 2048
#define REEL 0
#define IMAG 1

AnalogIn micro(A1);
Grove_LCD_RGB_Backlight screen(I2C_SDA, I2C_SCL);

Ticker ticker;

bool readyToEvaluate = 0;
float echantillons[N];

complex fftTab[N];

void timerInterrupt()
{
	static int tickNumber = 0;
	
	echantillons[tickNumber] = micro.read();
	tickNumber++;
	if(tickNumber>= N)
	{
		tickNumber = 0;
		readyToEvaluate = 1;
		ticker.detach();
	}
	
}


int main()
{
		screen.setRGB(100,100,100);//met les leds de fond
		ticker.attach_us(timerInterrupt, 25);
		while(1)
		{
				if(readyToEvaluate)
				{	
					rad2FFT(N, echantillons, fftTab);
					
					
						
					readyToEvaluate = 0;
					ticker.attach_us(timerInterrupt, 25);
				}
		}
}

