#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include "cmd.hpp"
#define TO_RGB565(color) ((color[2] & 0b11111000) << 8) | ((color[1] & 0b11111100) << 3) | (color[0] >> 3)

using namespace cv;
using namespace std;

uint16_t bitCount(int i) {
	i--;
	i |= i >> 1;
	i |= i >> 2;
	i |= i >> 4;
	i |= i >> 8;
	i |= i >> 16;
	i++;
	int n = 0;
	while ((i & 1) == 0) {
		i >>= 1;
		n++;
	}
	return n;
}

string to_hex(uint16_t i) {
	std::stringstream stream;
	stream << "0x"
		<< setfill('0') << setw(2)
		<< hex << i;
	return stream.str();
}

int main(int argc, char** argv) {
	string filename = getCmdOption(argv, argc, "-F");
	Mat image = imread(filename);
	string text = "const uint8_t PROGMEM " + filename.substr(0, filename.length() - 4) + "[] = {\n  ";
	vector<uint16_t> colors;
	vector<bool> bits;
	for (int x = 0; x < image.rows; x++) {
		for (int y = 0; y < image.cols; y++) {
			uint16_t pixel = TO_RGB565(image.ptr(x, y));
			if (!count(colors.begin(), colors.end(), pixel)) {
				colors.push_back(pixel);
			}
		}
	}
	int colorCount = colors.size();
	int bitN = bitCount(colorCount);
	for (int x = 0; x < image.rows; x++) {
		for (int y = 0; y < image.cols; y++) {
			uint16_t pixel = TO_RGB565(image.ptr(x, y));
			int pixelIndex = find(colors.begin(), colors.end(), pixel) - colors.begin();
			for (int i = bitN - 1; i >= 0; i--) {
				bits.push_back(pixelIndex & (1 << i));
			}
		}
	}
	text += to_hex(colorCount) + ", " + to_hex(bitN) + ",\n  ";
	for (uint16_t color : colors) {
		text += to_hex(color >> 8) + ", " + to_hex(color & 0xff) + ", ";
	}
	text += "\n  ";
	for (int i = 0; i < ceil(bits.size() / 8.0); i++) {
		uint8_t thisByte = 0;
		for (int j = 0; j < 8; j++) {
			thisByte <<= 1;
			thisByte |= bits[i * 8 + j];
		}
		text += to_hex(thisByte) + ", ";
	}
	text += "\n};";
	cout << text;
	waitKey(0);
	return 0;
}