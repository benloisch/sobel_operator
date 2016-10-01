#include <iostream>
#include "ImageBMP.h"

using namespace std;

int main() {

	cout << "Type in name of image file to perform Sobel edge detection algorithm on..." << endl;
	string filenameInput = "";
	getline(cin, filenameInput);

	cout << "Type in an integer for brightness on scale of 1 to 10..." << endl;
	unsigned int brightness = 0;
	cin >> brightness;

	ImageBMP sobelImage;
	sobelImage.loadBMP(filenameInput);
	sobelImage.convertToGrayScale();
	sobelImage.performSobelEdgeDetection(brightness);
	sobelImage.saveBMP();

	system("PAUSE");
	return 0;
}