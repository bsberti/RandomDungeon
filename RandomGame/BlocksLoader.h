#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

struct BitmapFileHeader {
	unsigned char headerField[2];
	unsigned char sizeOfBmp[4];
	unsigned char reserved1[2];
	unsigned char reserved2[2];
	unsigned char dataOffset[4];
	unsigned char headerSize[4];
	unsigned char width[4];
	unsigned char height[4];
};

struct Color {
	unsigned char b;
	unsigned char g;
	unsigned char r;
};

class BlocksLoader {
private:
	unsigned int blocks_height;
	unsigned int blocks_width;
public:
	BlocksLoader(unsigned int height, unsigned int width);
	~BlocksLoader();

	std::vector<std::vector<std::string>>* g_blockMap;
	std::vector<std::vector<std::string>>* g_BMPblockMap;
	std::pair<int, int>* startingPosition;
	std::pair<int, int>* endingPosition;

	bool LoadNewFile(std::string filePath);

	bool readJsonFile(std::string filePath);
	bool readFile(std::string filePath);

	void BitmapReading();

	std::string getRandomValidPosition();
	bool checkValidPosition(int i, int j);

	bool checkCurrentBlocks(std::pair<int, int> position);
	std::vector<std::pair<int, int>>* vec_currentBlocks;
	void cleanPairs();
};