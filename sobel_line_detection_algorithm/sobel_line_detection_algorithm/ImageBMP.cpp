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
	if (fopen_s(&file, openFile.append(".bmp").c_str(), "rb") == 0) {

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
			for (unsigned int w = 0; w < width; w++) {
				unsigned char b = fgetc(file);
				unsigned char g = fgetc(file);
				unsigned char r = fgetc(file);
				setPixelColor(w, h, r, g, b);
			}

			//eat up padding bytes
			for (int i = 0; i < pad; i++) {
				fgetc(file);
			}
		}
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

void ImageBMP::convertToGrayScale() {
	for (unsigned int i = 0; i < height; i++) {
		for (unsigned int j = 0; j < width; j++) {
			RGB pixel = this->getPixelColor(j, i);
			//convert to grayscale by weighing each color
			//GIMP / Photoshop uses a method like this
			unsigned char gray = (unsigned char)((pixel.r * 0.3) + (pixel.g * 0.59) + (pixel.b * 0.11));
			this->setPixelColor(j, i, gray, gray, gray);
		}
	}
}

void ImageBMP::performSobelEdgeDetection(unsigned int brightness) {
	//create a temporary BMP so we can operate on temp, and store values in original and save it
	ImageBMP temp;
	temp.createBMP(width, height, fileName);
	for (unsigned int y = 0; y < height; y++) {
		for (unsigned int x = 0; x < width; x++) {
			RGB copy = this->getPixelColor(x, y);
			temp.setPixelColor(x, y, (unsigned char)copy.r, (unsigned char)copy.g, (unsigned char)copy.b);
		}
	}

	for (unsigned int h = 0; h < this->height; h++) {
		for (unsigned int w = 0; w < this->width; w++) {
			if (h == 0 || w == 0 || h == height - 1 || w == width - 1) {
				this->setPixelColor(w, h, 0, 0, 0);
				continue;
			}

			int pix11 = (int)temp.getPixelColor(w - 1, h - 1).r;
			int pix12 = (int)temp.getPixelColor(w, h - 1).r;
			int pix13 = (int)temp.getPixelColor(w + 1, h - 1).r;
			int pix21 = (int)temp.getPixelColor(w - 1, h).r;
			int pix22 = (int)temp.getPixelColor(w, h).r;
			int pix23 = (int)temp.getPixelColor(w + 1, h).r;
			int pix31 = (int)temp.getPixelColor(w - 1, h + 1).r;
			int pix32 = (int)temp.getPixelColor(w, h + 1).r;
			int pix33 = (int)temp.getPixelColor(w + 1, h + 1).r;

			double Gx = (pix11*-1) + (pix12 * 0) + (pix13 * 1) + (pix21*-2) + (pix22 * 0) + (pix23 * 2) + (pix31*-1) + (pix32 * 0) + (pix33 * 1);
			double Gy = (pix11*-1) + (pix12 * -2) + (pix13 * -1) + (pix21*0) + (pix22 * 0) + (pix23 * 0) + (pix31*1) + (pix32 * 2) + (pix33 * 1);

			//clamp Gx and Gy
			Gx = abs(Gx) / 4.0;
			Gy = abs(Gy) / 4.0;

			double dgradient = sqrt(Gx*Gx + Gy*Gy);
			//increase brightness a bit and clamp to values from 0 to 255
			dgradient *= brightness;
			unsigned char gradient = dgradient > 255 ? 255 : (unsigned char)dgradient;
			//gradient = gradient < 0 ? 0 : gradient;

			this->setPixelColor(w, h, gradient, gradient, gradient);
		}
	}
}