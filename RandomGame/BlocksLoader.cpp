#include "BlocksLoader.h"

BlocksLoader::BlocksLoader(unsigned int height, unsigned int width) {
	this->blocks_height = height;
	this->blocks_width = width;
	
	g_blockMap = new std::vector<std::vector<std::string>>(blocks_height, std::vector<std::string>(blocks_width));

	if (!readFile("assets/Dungeon1.txt")) {
		std::cout << "Failed to read base block file." << std::endl;
	}

	vec_currentBlocks = new std::vector<std::pair<int, int>>();
}

BlocksLoader::~BlocksLoader() {
	delete[] g_blockMap;
}

bool BlocksLoader::LoadNewFile(std::string filePath) {
	g_blockMap = new std::vector<std::vector<std::string>>(blocks_height, std::vector<std::string>(blocks_width));

	if (!readFile(filePath)) {
		return false;
	}

	return true;
}

std::string BlocksLoader::getRandomValidPosition() {
	int rndI = rand() % (blocks_height - 2);
	int rndJ = rand() % (blocks_width - 2);

	if (g_blockMap->at(rndI).at(rndJ) == "X") {
		return std::to_string(rndI) + "." + std::to_string(rndJ);
	}
	else {
		return getRandomValidPosition();
	}
}

void BlocksLoader::cleanPairs() {
	vec_currentBlocks->clear();
}

bool BlocksLoader::checkValidPosition(int i, int j) {

	if (i >= blocks_height) return false;
	if (j >= blocks_width) return false;

	if (g_blockMap->at(i).at(j) == "X") {
		if (!checkCurrentBlocks(std::pair<int, int>(i, j))) {
			vec_currentBlocks->push_back(std::pair<int, int>(i, j));
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

bool BlocksLoader::checkCurrentBlocks(std::pair<int, int> position) {
	if (std::count(vec_currentBlocks->begin(), vec_currentBlocks->end(), position)) {
		return true;
	}
	else {
		return false;
	}
}

bool BlocksLoader::readJsonFile(std::string filePath) {
	return false;
}

// Here's a simple way to load the comma delimited files:
bool BlocksLoader::readFile(std::string filePath) {
	
	// Open the file
	std::ifstream namesFile2(filePath);
	if (!namesFile2.is_open()) {
		return false;
	}

	std::string theLine2;
	std::string currentString;
	for (int i = 0; i < g_blockMap->size(); i++)
	{
		if (std::getline(namesFile2, theLine2)) {
			std::stringstream ssLine(theLine2);
			std::string token;
			for (int j = 0; j < g_blockMap->at(i).size(); j++) {
				if (std::getline(ssLine, token, ' ')) {
					g_blockMap->at(i).at(j) = token;
				}
			}
		}
	}

	return true;
}

bool isSimilarTo(float x, float y) {
	float tolerance = 0.9; // adjust this value for desired tolerance
	return abs(x - y) <= tolerance;
}

void BlocksLoader::BitmapReading()
{
	std::ifstream bitmapStream;
	bitmapStream.open("assets/Dungeon1.bmp");
	if (!bitmapStream.is_open()) {
		bitmapStream.close();
		return;
	}

	bitmapStream.seekg(0);

	BitmapFileHeader fileHeader;
	
	int size = sizeof(fileHeader);
	bitmapStream.read((char*)&fileHeader.headerField, size);
	bitmapStream.seekg(fileHeader.dataOffset[0]);

	this->blocks_height = fileHeader.height[0];
	this->blocks_width = fileHeader.width[0];

	nodeGrid = new Grid(fileHeader.width[0], fileHeader.height[0]);

	int mapSize = fileHeader.width[0] * fileHeader.height[0];
	g_BMPblockMap = new std::vector<std::vector<std::string>>(fileHeader.height[0], std::vector<std::string>(fileHeader.width[0]));

	Color color;
	int i = fileHeader.height[0] - 1;
	int j = 0;
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	for (int x = 0; x < mapSize; x++) {
		bitmapStream.read((char*)&color, 3);

		//printf("%d %d %d\n", color.r, color.g, color.b);
		r = color.r / 255.0f;
		g = color.g / 255.0f;
		b = color.b / 255.0f;

		if (r != 0.0f && r != 1.0f)
			int breakpoint = 1;

		if (r == 0.0f && g == 0.0f && b == 0.0f) {
			g_BMPblockMap->at(i).at(j) = ".";
			nodeGrid->get_node(i, j)->is_obstacle = true;
		}
		else if (r == 1.0f && g == 1.0f && b == 1.0f) {
			g_BMPblockMap->at(i).at(j) = "X";
		}
		else if (isSimilarTo(r, 1.0f) && isSimilarTo(g, 0.0f) && isSimilarTo(b, 0.0f)) {
			// RED
			g_BMPblockMap->at(i).at(j) = "X";
			endingPosition = new std::pair<int, int>(i, j);
			endNode = nodeGrid->get_node(i, j);
		}
		else if (isSimilarTo(r, 0.0f) && isSimilarTo(g, 1.0f) && isSimilarTo(b, 0.0f)) {
			// GREEN
			g_BMPblockMap->at(i).at(j) = "X";
			startingPosition = new std::pair<int, int>(i, j);
			startNode = nodeGrid->get_node(i, j);
		}
		else {
			g_BMPblockMap->at(i).at(j) = ".";
			nodeGrid->get_node(i, j)->is_obstacle = true;
		}

		j++;
		if (j >= fileHeader.width[0]) {
			j = 0;

			i--;
			if (i < 0) {
				i = fileHeader.height[0] - 1;
			}
		}

	}

	bitmapStream.close();
}

void BlocksLoader::NodeGridInitialization() {

	nodeGrid = new Grid(this->blocks_width, this->blocks_height);

	for (int i = 0; i < blocks_height; i++) {
		for (int j = 0; j < blocks_width; j++) {
			if (g_blockMap->at(i).at(j) == ".") {
				nodeGrid->get_node(i, j)->is_obstacle = true;
			}
		}
	}
}

std::vector<Node*> BlocksLoader::AStar() {
	// Initialize start node
	startNode->g = 0;
	startNode->h = sqrt(pow(endNode->x - startNode->x, 2) + pow(endNode->y - startNode->y, 2));
	startNode->f = startNode->g + startNode->h;
	startNode->is_visited = true;
	std::priority_queue<Node*, std::vector<Node*>, CompareNodes> open_list;
	open_list.push(startNode);

	// Search for end node
	while (!open_list.empty()) {
		Node* current = open_list.top();
		open_list.pop();
		if (current == endNode) {
			// Path found, return path
			std::vector<Node*> path;
			while (current) {
				path.push_back(current);
				current = current->parent;
			}
			reverse(path.begin(), path.end());
			return path;
		}
		// Explore neighbors of current node
		for (auto neighbor : current->neighbors) {
			if (neighbor->is_obstacle || neighbor->is_visited) {
				continue;
			}
			double g = current->g + sqrt(pow(neighbor->x - current->x, 2) + pow(neighbor->y - current->y, 2));
			double h = sqrt(pow(endNode->x - neighbor->x, 2) + pow(endNode->y - neighbor->y, 2));
			double f = g + h;
			if (neighbor->f == 0 || f < neighbor->f) {
				neighbor->g = g;
				neighbor->h = h;
				neighbor->f = f;
				neighbor->parent = current;
				neighbor->is_visited = true;
				open_list.push(neighbor);
			}
		}
	}
	// No path found
	return {};
}

void BlocksLoader::CleanNodePath(std::vector<Node*> nodePath) {
	for (auto node : nodePath) {
		node->is_visited = false;

		for (auto neighbor : node->neighbors) {
			neighbor->g = 0;
			neighbor->f = 0;
			neighbor->h = 0;
		}
	}
}

std::vector<Node*> BlocksLoader::AStarEnemy(Node* currentStartNode)
{
	// Initialize start node
	currentStartNode->g = 0;
	currentStartNode->h = sqrt(pow(endNode->x - currentStartNode->x, 2) + pow(endNode->y - currentStartNode->y, 2));
	currentStartNode->f = currentStartNode->g + currentStartNode->h;
	currentStartNode->is_visited = true;
	std::priority_queue<Node*, std::vector<Node*>, CompareNodes> open_list;
	open_list.push(currentStartNode);

	// Search for end node
	while (!open_list.empty()) {
		Node* current = open_list.top();
		open_list.pop();
		if (current == endNode) {
			// Path found, return path
			std::vector<Node*> path;
			while (current) {
				path.push_back(current);
				current = current->parent;
			}
			reverse(path.begin(), path.end());
			CleanNodePath(path);
			return path;
		}
		// Explore neighbors of current node
		for (auto neighbor : current->neighbors) {
			if (neighbor->is_obstacle || neighbor->is_visited) {
				continue;
			}
			double g = current->g + sqrt(pow(neighbor->x - current->x, 2) + pow(neighbor->y - current->y, 2));
			double h = sqrt(pow(endNode->x - neighbor->x, 2) + pow(endNode->y - neighbor->y, 2));
			double f = g + h;
			if (neighbor->f == 0 || f < neighbor->f) {
				neighbor->g = g;
				neighbor->h = h;
				neighbor->f = f;
				neighbor->parent = current;
				neighbor->is_visited = true;
				open_list.push(neighbor);
			}
		}
	}
	// No path found
	return {};
}
