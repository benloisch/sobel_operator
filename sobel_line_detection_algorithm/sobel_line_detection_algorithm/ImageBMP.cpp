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
	cout << "Loading BMP file..." << endl;
	//open bmp and read width, height, and get RGB data
	FILE *file;
	string openFile = inFileName;
	if (fopen_s(&file, openFile.append(".bmp").c_str(), "r") == 0) {

		//read first two characters and determine that file is in fact a bmp image
		char first = fgetc(file);
		char second = fgetc(file);
		if (first != EOF && second != EOF) {
			if ((first != 'B' && first != 'M') || (second != 'B' && second != 'M')) {
				cout << "Input file is not a BMP image!" << endl;
				return;
			}
		}

		//read up until the width and height of pixels and get those values
		for (int i = 0; i < 16; i++)
			fgetc(file);

		//now store the next two integer values which represent the width and height of image in pixels
		unsigned int a = (unsigned int)fgetc(file);
		unsigned int b = (unsigned int)fgetc(file);
		unsigned int c = (unsigned int)fgetc(file);
		unsigned int d = (unsigned int)fgetc(file);

		int n = 1;
		if (*(char *)&n == 1) { //if computer system is little endian then extract integer this way...
			d <<= 24;
			c <<= 16;
			b <<= 8;
			this->width = a | b | c | d;
		}
		else { //if big endian, extract width differently
			a <<= 24;
			b <<= 16;
			c <<= 8;
			this->width = a | b | c | d;
		}

		//now store height of bmp image
		a = (unsigned int)fgetc(file);
		b = (unsigned int)fgetc(file);
		c = (unsigned int)fgetc(file);
		d = (unsigned int)fgetc(file);

		n = 1;
		if (*(char *)&n == 1) { //if computer system is little endian then extract integer this way...
			d <<= 24;
			c <<= 16;
			b <<= 8;
			this->height = a | b | c | d;
		}
		else { //if big endian, extract width differently
			a <<= 24;
			b <<= 16;
			c <<= 8;
			this->height = a | b | c | d;
		}

		//now copy over bmp pixel RGB values into buffer
		this->createBMP(this->width, this->height, inFileName.append("_output")); //first allocate buffer data

		//read the rest of the header file until start of RGB data
		for (int i = 0; i < 28; i++)
			fgetc(file);

		//calculate padding needed for image so we can extract RGB properly
		//each row must be a multiple of 4 bytes
		int pad = 0;
		if ((width * 3) % 4 != 0) {
			pad = 4 - ((width * 3) % 4); //remaining bytes to be added
		}

		//read rgb data and put into buffer
		for (int h = height - 1; h >= 0; h--) {
			for (int w = 0; w < width; w++) {
				unsigned char b = fgetc(file);
				unsigned char g = fgetc(file);
				unsigned char r = fgetc(file);
				setPixelColor(w, h, r, g, b);

				if (w == 435) {
					int stop = 0;
				}
			}

			//eat up padding bytes
			for (int i = 0; i < pad; i++) {
				fgetc(file);
			}
		}

		int stop = 0;
	}
	else {
		cout << "Could not open image file!" << endl;
	}
}

void ImageBMP::saveBMP() {
	ofstream file;

	cout << "Writing BMP file..." << endl;

	WORD bfType = 'BM';

	//check for endianness of the computer and store 'BM' accordingly 
	int n = 1;
	if (*(char *)&n == 1) //if computer system is little endian
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
