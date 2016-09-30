#include <iostream>
#include "ImageBMP.h"

using namespace std;

int main() {

	ImageBMP test;
	test.loadBMP("output2");
	test.saveBMP();

	system("PAUSE");
	return 0;
}