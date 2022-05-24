#include "mbed.h"
#include "Grove_LCD_RGB_Backlight.h"
#include "fft/FFT.h"

#define N 2048
#define ALTERNANCE_TEMPS_FREC 10

AnalogIn micro(A1);//Mon micro
Grove_LCD_RGB_Backlight screen(I2C_SDA, I2C_SCL);//Mon écran

Ticker ticker;//Un ticker de mbed, il sert à déclencher des interruptions à un 
            //rythme régulier sans avoir à gérer les timers

float echantillons[N]; //un tableau d'échantillons

complex fftTab[N]; //le tableau des valeurs complexes rempli après FFT

int tickNumber = 0; //variable globale qui compte combien d'échantillons ont été réalisés

//L'interruption déclenchée par le timer, j'ai essayé de la faire la plus light
//possible, car elle est appelée à haute fréquence (40KHz)
void timerInterrupt()
{
    echantillons[tickNumber] = micro.read();
    tickNumber++;
    if(tickNumber >=N)
        ticker.detach();//on arrête d'intérompre à 40kHz quand on a rempli
                        //le tableau d'échantillons
}

Timer timer; //timer permettant de mesurer le temps de calcul
char affTempsCnt = 0; //sert à déterminer quand switcher de l'affichage de la 2e frequence max à l'affichage du temps de calcul.

int main()
{
        screen.setRGB(100,100,100);//met les leds de fond
        ticker.attach_us(timerInterrupt, 25);//mise en place de l'interrupt
        
        while(1)
        {
                if(tickNumber >= N)//quand on a les échantillons
                {   
                    timer.reset();
                    timer.start();
                    
                    tickNumber = 0;//réinitialisation
                    
                    FFT(N, echantillons, fftTab);//FFT
                    
                    int indMax = 0, ind2eMax = 0;
                    float amplMax = 0, amplMax2 = 0;
                    long double t;
                    
                    for(int i = 1; i < N/2; i++)//on cherche les 2 armoniques max
                    {
                        t = pow((long double)fftTab[i].Re, 2) + pow((long double)fftTab[i].Im, 2); //module carré des fréquences
                        
                        if(t > amplMax)
                        {
                            //viens de |Fi - Fmax| > 0.5 Fmax => |i*Fe/N - indMax * Fe/N| < O.5 * indMax * Fe/N etc...  
                            //le but est d'éviter de détecter 2 fois la même harmonique, je prend donc une marge de sécurité.
                            if(i/indMax - 1.5 > 0) 
                            {
                                ind2eMax = indMax;//on change le 2e max
                                amplMax2 = amplMax;
                            }
                            
                            indMax = i;//on change le max
                            amplMax = t;
                        }
                        else if(t > amplMax2 && i/indMax - 1.5 > 0)
                        {
                            ind2eMax = i;//on change le 2e max
                            amplMax2 = t;
                        }
                    }

                    //affichage du résultat
                    screen.locate(0,0);
                    char str[30];
                    sprintf(str, "%.1e: %.1e", (float)indMax * 37e3/N, amplMax);
                    screen.print(str);
                    
                    screen.locate(0,1);
                    sprintf(str, "%.1e: %.1e", (float)ind2eMax * 37e3/N, amplMax2);
                    screen.print(str);
                    
                    
                    
                    if(affTempsCnt < ALTERNANCE_TEMPS_FREC)//si on est sur l'affichage du temps de calcul:
                    {
                        sprintf(str, "%f          ", timer.read());//on affiche le temps
                        screen.locate(0,1);
                        screen.print(str);
                    }
                    timer.stop();
                    affTempsCnt = (affTempsCnt + 1)%(2*ALTERNANCE_TEMPS_FREC); //On incrémente le compteur, s'il est au max, on remet à 0
                    
                    
                    //remise en place de l'interrupt
                    ticker.attach_us(timerInterrupt, 25);
                }
        }
}

