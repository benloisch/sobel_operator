#include "ImageBMP.h"

#include <iostream>

ImageBMP::ImageBMP() {
	width = 0;
	height = 0;
	fileName = "none";
	red = NULL;
	green = NULL;
	blue = NULL;
}

ImageBMP::~ImageBMP() {
	for (unsigned int i = 0; i < width; i++) {
		if (red != NULL) 
			delete[] red[i];
		
		if (green != NULL)
				delete[] green[i];

		if (blue != NULL)
			delete[] blue[i];
	}

	delete[] red;
	delete[] green;
	delete[] blue;
}

void ImageBMP::cleanup() {
	for (unsigned int i = 0; i < width; i++) {
		if (red != NULL)
			delete[] red[i];

		if (green != NULL)
			delete[] green[i];

		if (blue != NULL)
			delete[] blue[i];
	}

	delete[] red;
	delete[] green;
	delete[] blue;
}

void ImageBMP::createBMP(unsigned int width, unsigned int height, string fileName) {
	this->width = width;
	this->height = height;
	this->fileName = fileName;

	red = NULL;
	green = NULL;
	blue = NULL;
	try  {
		red = new unsigned char*[width];
		for (unsigned int r = 0; r < width; r++) { //set newly created array to NULL so if a catch happens, we can delete it
			red[r] = NULL;
		}

		for (unsigned int i = 0; i < width; i++) {
			red[i] = new unsigned char[height];
			for (unsigned int j = 0; j < height; j++) {
				red[i][j] = 0;
			}
		}
		
		green = new unsigned char*[width];
		for (unsigned int g = 0; g < width; g++) {
			green[g] = NULL;
		}
		for (unsigned int i = 0; i < width; i++) {
			green[i] = new unsigned char[height];
			for (unsigned int j = 0; j < height; j++) {
				green[i][j] = 0;
			}
		}

		blue = new unsigned char*[width];
		for (unsigned int b = 0; b < width; b++) {
			blue[b] = NULL;
		}
		for (unsigned int i = 0; i < width; i++) {
			blue[i] = new unsigned char[height];
			for (unsigned int j = 0; j < height; j++) {
				blue[i][j] = 0;
			}
		}
	}
	catch (...) {
		throw exception("Error in ImageBMP.createBMP()");
		cleanup();
	}
}

void ImageBMP::loadBMP(string inFileName) {
	//open bmp and read width, height, and get RGB data
	FILE *file;
	if (fopen_s(&file, inFileName.append(".bmp").c_str(), "r") == 0) {

		//read first two characters and determine that file is in fact a bmp image
		char first = fgetc(file);
		char second = fgetc(file);
		if (first != EOF && second != EOF) {
			if ((strcmp(first, 'B') && second != 'M') || (first != 'M' && second != 'B')) {
				cout << "Input file is not a BMP image!" << endl;
				return;
			}
		}

	}
	else {
		cout << "Could not open image file!" << endl;
	}

	int stop = 0;

	/*
	for (int i = 0; i < 54; i++)
		fgetc(file);

	while (1) {
		cout << fgetc(file) << endl;
	}
	char c1 = fgetc(file);
	char c2 = fgetc(file);
	char c3 = fgetc(file);
	char c4 = fgetc(file);

	unsigned int ai = (int)c1;
	unsigned int bi = (int)c2;
	unsigned int ci = (int)c3;
	unsigned int di = (int)c4;
	
	di <<= 24;
	ci <<= 16;
	bi <<= 8;

	unsigned int size = ai | bi | ci | di;

	cout << size << endl;
	int stop = 0;
	*/
}

void ImageBMP::saveBMP() {
	ofstream file;

	cout << "Writing BMP file..." << endl;

	WORD bfType = 'BM';

	//check for endianness of the computer and store 'BM' accordingly 
	int n = 1;
	if (*(char *)&n == 1)
		bfType = 'MB';

	//each row must be a multiple of 4 bytes
	int pad = 0;
	if ((width * 3) % 4 != 0) {
		pad = 4 - ((width * 3) % 4); //remaining bytes to be added
	}

	BITMAPFILEHEADER fh;
	fh.bfType = bfType;
	fh.bfSize = ((width * 3) + pad) * height + 54;
	fh.bfReserved1 = 0;
	fh.bfReserved2 = 0;
	fh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	BITMAPINFOHEADER ih;
	ih.biSize = 40;
	ih.biWidth = width;
	ih.biHeight = height;
	ih.biPlanes = 1;
	ih.biBitCount = 24;
	ih.biCompression = 0;
	ih.biSizeImage = ((width * 3) + pad) * height;
	ih.biXPelsPerMeter = 0;
	ih.biYPelsPerMeter = 0;
	ih.biClrUsed = 0;
	ih.biClrImportant = 0;

	BITMAPFILEHEADER *pfh = &fh;
	BITMAPINFOHEADER *pih = &ih;
		
	file.open(fileName.append(".bmp"), ios::binary);//Do not you ios:out *error*
		
	if (file.is_open()) {

		file.write((char*)pfh, sizeof(fh));
		file.write((char*)pih, sizeof(ih));


		//write data of double pointer characters arrays (red, green, and blue) to bmp file
		try {
			for (int j = height - 1; j >= 0; j--) {
				for (unsigned int i = 0; i < width; i++) {
					file << blue[i][j];
					file << green[i][j];
					file << red[i][j];
				}

				//add padding bytes
				for (int i = 0; i < pad; i++) {
					file << (char)0;
				}

			}
		}
		catch (...) {
			throw exception("Error in ImageBMP.save()");
			cleanup();
		}

		file.close();

		cout << "File saved." << endl;
	}
	else {
		cout << "Could not open file for writing" << endl;
	}
	
}

void ImageBMP::setInitialBMPColor(unsigned char r, unsigned char g, unsigned char b) {
	for (unsigned int i = 0; i < height; i++) {
		for (unsigned int j = 0; j < width; j++)
			this->setPixelColor(j, i, r, g, b);
	}
}
