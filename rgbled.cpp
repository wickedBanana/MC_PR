#include "rgbled.h"
/*Klasse zum darstellen des Zustands der RGB-LED*/
rgbLed::rgbLed()
{
    green = 0;
    blue = 0;
    red = 0;
}

rgbLed::rgbLed(int red, int green, int blue):
    red(red),green(green), blue(blue){
}
