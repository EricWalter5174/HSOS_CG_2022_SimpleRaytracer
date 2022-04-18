#include "rgbimage.h"
#include "color.h"
#include "assert.h"
#include <fstream>

RGBImage::RGBImage( unsigned int Width, unsigned int Height)
{
	m_Width = Width;
	m_Height = Height;
	m_Image = new Color[m_Width * m_Height];
}

RGBImage::~RGBImage()
{
	delete m_Image;
}

void RGBImage::setPixelColor( unsigned int x, unsigned int y, const Color& c)
{
	//Access 1D-Array via formula y * Width + x
	if (x < m_Width && y < m_Height) 
	{
		m_Image[y * m_Width + x] = c;
	}
}

const Color& RGBImage::getPixelColor( unsigned int x, unsigned int y) const
{
	if (x < m_Width && y < m_Height) 
	{
		return Color(m_Image[y * m_Width + x]);
	}
}

unsigned int RGBImage::width() const
{
	return m_Width;
}
unsigned int RGBImage::height() const
{
	return m_Height;
}

unsigned char RGBImage::convertColorChannel( float v)
{
    if (v < 0)
        v = 0;
    else if (v > 1)
        v = 1;
	unsigned char c = v * 255;
	return c;
}


bool RGBImage::saveToDisk( const char* Filename)
{

    //Autor: Marcel Henneke
    FILE* fp = fopen(Filename, "wb"); // das b hat gefehlt

    if (fp == NULL) {
        perror("File opening failed");
        return EXIT_FAILURE;
    }

    //Größe des Bildes und der Datei
    int imagesize = 3 * this->m_Width * this->m_Height;
    int filesize = 54 + imagesize;


    //Fileheader
    unsigned char fileHeader[14];
    for (int i = 0; i < 14; i++) {
        fileHeader[i] = 0;
    }
    fileHeader[0] = 'B';
    fileHeader[1] = 'M';

    //Größe der Datei bfSize
    fileHeader[2] = (unsigned char)(filesize);
    fileHeader[3] = (unsigned char)(filesize >> 8);
    fileHeader[4] = (unsigned char)(filesize >> 16);
    fileHeader[5] = (unsigned char)(filesize >> 24);

    //Reserviert von der Software abhänging bfReserved 6-9

    //Headergröße bOffBit 10-13
    fileHeader[10] = 54;

    //Infoheader (Bildheader)
    unsigned char infoHeader[40];
    for (int i = 0; i < 40; i++) {
        infoHeader[i] = 0;
    }
    //Größe des Infoheaders (biSize) 0 - 3
    infoHeader[0] = 40;

    //Bildbreite (biWidth)
    infoHeader[4] = (unsigned char)(this->m_Width);
    infoHeader[5] = (unsigned char)(this->m_Width >> 8);
    infoHeader[6] = (unsigned char)(this->m_Width >> 16);
    infoHeader[7] = (unsigned char)(this->m_Width >> 24);

    //Bildhöhe (biHeight)
    int height = -this->m_Height;
    infoHeader[8] = (unsigned char)(height);
    infoHeader[9] = (unsigned char)(height >> 8);
    infoHeader[10] = (unsigned char)(height >> 16);
    infoHeader[11] = (unsigned char)(-this->m_Height >> 24);

    //biPlanes (wird für BMP nicht verwendet, stamd in älteren Formaten für die Anzahl der Farbebenen) 12-13
    infoHeader[12] = 1;

    //Farbtiefe (biBitCount) 14- 15
    infoHeader[14] = 24;

    //Kompression der Daten (biCompression) 16-19

    //Größe der Bilddaten in Byte (biSizeImage)
    infoHeader[20] = (unsigned char)(imagesize);
    infoHeader[21] = (unsigned char)(imagesize >> 8);
    infoHeader[22] = (unsigned char)(imagesize >> 16);
    infoHeader[23] = (unsigned char)(imagesize >> 24);

    //horizontale Auflösung des Zielgerätes, im BMP meist auf Null gesetzt (biXPelsPerMeter) 24-27

    //vertikale Auflösung des Zielgerätes, im BMP meist auf Null gesetzt (biYPelsPerMeter) 28-31

    //??? (biClrUsed) 32-35

    //??? (biClrImportant) 36-39
    fwrite(fileHeader, 1, 14, fp);
    fwrite(infoHeader, 1, 40, fp);

    for (int y = 0; y < m_Height; y++) {
        for (int x = 0; x < m_Width; x++) {
            Color c = m_Image[x + (y * m_Width)]; //getPixelColor(x, y);
            unsigned char red = convertColorChannel(c.R);
            unsigned char green = convertColorChannel(c.G);
            unsigned char blue = convertColorChannel(c.B);
            fwrite(&blue, 1, 1, fp);
            fwrite(&green, 1, 1, fp);
            fwrite(&red, 1, 1, fp);
        }
    }

    fclose(fp);
    return true;
	
}


