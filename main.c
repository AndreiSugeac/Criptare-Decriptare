#include <stdio.h>
#include <stdlib.h>
#include "cod.h"

int main()
{
    unsigned char *img = "peppers.bmp";
    unsigned char *nume = "cript_pepper.bmp";
    unsigned char *decript = "Peppers_decript.bmp";
    criptare(img,nume);
    decriptare(nume,decript);
    return 0;
}
