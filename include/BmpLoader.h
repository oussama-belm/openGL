#pragma once
#include <Windows.h>
#include <iostream>
#include <stdio.h>

class BmpLoader
{
public:
    unsigned char* textureData;
    int iWidth, iHeight;
    BmpLoader(const char*);
    ~BmpLoader();
private:
    BITMAPFILEHEADER bfh;
    BITMAPINFOHEADER bih;
};

