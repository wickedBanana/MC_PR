#ifndef RGBLED_H
#define RGBLED_H

/*Klasse für RGB-LED*/
class rgbLed
{
public:
    rgbLed();
    rgbLed(int red, int green, int blue);
    int red, green, blue;
};

#endif // RGBLED_H
