#include "BMPLoader.h"

BmpLoader::BmpLoader(const char* filename)
{
    FILE* file = 0;
    file = fopen(filename, "rb");
    if (!file)
    {
        std::cout << "file not found" << std::endl; exit(3);
    }
    fread(&bfh, sizeof(BITMAPFILEHEADER), 1, file);
    if (bfh.bfType != 0x4D42)
        std::cout << "this is not valid file" << std::endl;
    fread(&bih, sizeof(BITMAPINFOHEADER), 1, file);
    if (bih.biSizeImage == 0)
        bih.biSizeImage = bih.biHeight * bih.biWidth * 3;
    textureData = new unsigned char[bih.biSizeImage];
    fseek(file, bfh.bfOffBits, SEEK_SET);
    fread(textureData, 1, bih.biSizeImage, file);
    unsigned char temp;
    for (unsigned int i = 0; i < bih.biSizeImage; i += 3) {
        temp = textureData[i];
        textureData[i] = textureData[i + 2];
        textureData[i + 2] = temp;
    }
    iWidth = bih.biWidth;
    iHeight = bih.biHeight;
    fclose(file);
}

BmpLoader::~BmpLoader()
{
    delete[] textureData;
}
