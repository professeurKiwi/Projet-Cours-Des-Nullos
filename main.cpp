#include "mbed.h"
#include "Grove_LCD_RGB_Backlight.h"

#define PCLOCK 25e6;

PwmOut E1(P2_2);
AnalogIn potar(A0);
Grove_LCD_RGB_Backlight screen(I2C_SDA, I2C_SCL);

int lastTick = 0, deltaTick = 0, lastDisplayTick = 0;

void init_GPIO()
{
	LPC_GPIO0->FIODIR0 |= (1 << 7); //M1
	LPC_GPIO0->FIOCLR0 = 1;
}
void init_E1()
{
	E1.period(0.01);
	E1.write(0.5);
}


////question 2////
//extern "C" void Tim0IrqHandler()
//{
//	static float rapportCyclique = 0.5;
//	rapportCyclique += 0.1;
//	if(rapportCyclique > 1)
//		rapportCyclique = 0;
//	E1.write(rapportCyclique);
//	LPC_TIM0->IR = 1;
//}

extern "C" void Tim0IrqHandler() //clear l'écrant pour éviter les bugs
{
	//screen.clear();
	LPC_TIM0->IR = 1;
}

extern "C" void Tim2IrqHandler()
{
	deltaTick = LPC_TIM2->CR0 - lastTick;
	lastTick = LPC_TIM2->CR0;
	screen.setRGB(0,0,255);
	
	LPC_TIM2->IR = 0x10;
}

void init_timer()
{
	//timer 0 pour pulse
	LPC_TIM0->MCR = 3;//MR0, MR1 actif et MR0 sert de reset
	LPC_TIM0->MR0 = 2 * PCLOCK; // période de 2s
	NVIC_EnableIRQ(TIMER0_IRQn);
	NVIC_SetVector(TIMER0_IRQn, uint32_t(Tim0IrqHandler));
	
	//timer 2 pour echo
	//sur P0.4 pour CAP0
	LPC_SC->PCONP |= (1 << 22); // pour alumer les timers 2 et 3
	LPC_PINCON->PINSEL0 |= 0x300;
	LPC_TIM2->CCR = 5; //capture sur front montant + interruption (000101)
	NVIC_EnableIRQ(TIMER2_IRQn);
	NVIC_SetVector(TIMER2_IRQn, uint32_t(Tim2IrqHandler));
	
	
	LPC_TIM0->TCR |= (1 << 0);
	LPC_TIM2->TCR |= (1 << 0);
}
//////

int main()
{
		init_GPIO();
		init_E1();
		init_timer(); 
		
		screen.setRGB(255,0,0);
	
		while(1)
		{

//			char str[1000];
//			sprintf(str, "%3.0f%%                                                                  ", potar.read() * 100);
//			screen.locate(0,0);
//			screen.setRGB(potar.read() * 255,0,(1- potar.read()) * 255); 
//			screen.print(str);
			int tmp = PCLOCK;
			if(lastTick - lastDisplayTick > 0.1 * tmp)
			{
				char str[1000];
				lastDisplayTick = lastTick;
				sprintf(str,"%.3f                                                                  ", (float)2/((float)deltaTick / (float)tmp));
				screen.locate(0,0);
				screen.print(str);
			}
			
			E1.write(potar.read());
		}
}

