#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <queue>
#include <cmath>

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

struct Node {
	int x, y;					// Node position
	double f, g, h;				// Cost values for the node
	std::vector<Node*> neighbors;    // List of neighboring nodes
	Node* parent;				// Pointer to parent node
	bool is_obstacle;			// Flag to indicate if node is an obstacle
	bool is_visited;			// Flag to indicate if node has been visited
	Node(int x_, int y_) {
		x = x_;
		y = y_;
		f = g = h = 0;
		parent = nullptr;
		is_obstacle = false;
		is_visited = false;
	}
};

struct CompareNodes {
	bool operator()(Node* n1, Node* n2) {
		return n1->f > n2->f;
	}
};

struct Grid {
	int width, height;
	std::vector<Node> nodes;

	Grid(int width, int height) : width(width), height(height), nodes(width* height, Node(0, 0)) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				nodes[y * width + x] = Node(x, y);
			}
		}
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				Node& node = nodes[y * width + x];
				if (y > 0) node.neighbors.push_back(&nodes[(y - 1) * width + x]);
				if (y < height - 1) node.neighbors.push_back(&nodes[(y + 1) * width + x]);
				if (x > 0) node.neighbors.push_back(&nodes[y * width + x - 1]);
				if (x < width - 1) node.neighbors.push_back(&nodes[y * width + x + 1]);
			}
		}
	}
	
	~Grid();

	Node* get_node(int x, int y) {
		return &nodes[y * width + x];
	}
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
	Grid* nodeGrid;

	std::pair<int, int>* startingPosition;
	std::pair<int, int>* endingPosition;
	Node* startNode;
	Node* endNode;

	bool LoadNewFile(std::string filePath);

	bool readJsonFile(std::string filePath);
	bool readFile(std::string filePath);

	void BitmapReading();
	void NodeGridInitialization();

	std::string getRandomValidPosition();
	bool checkValidPosition(int i, int j);

	bool checkCurrentBlocks(std::pair<int, int> position);
	std::vector<std::pair<int, int>>* vec_currentBlocks;
	void cleanPairs();

	std::vector<Node*> AStar();
	std::vector<Node*> AStarEnemy(Node* currentStartNode, Node* endNode);
	void CleanNodePath(std::vector<Node*> nodePath);
};