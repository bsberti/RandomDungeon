#ifndef _cMazeMaker_W2023_HG_
#define _cMazeMaker_W2023_HG_

// This code is taken from Jaden Peterson's posting on codereview:
// https://codereview.stackexchange.com/questions/135443/c-maze-generator

// I just put it into a class for you. That's it. He did all the real work!

#include <cctype>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>  
#include <vector>

class cMazeMaker_W2023
{
public:
	cMazeMaker_W2023();
	bool GenerateMaze(unsigned int width, unsigned int height);
	void PrintMaze(void);
	// The "index" is the room or cell index
	// i.e. the index along the width goes from 0 to (width-1)
	// The square is defined by the centre + half the size. 
	// (so 2x this "half size" is the entire width or height of that square region.
	void GetMazeAreaSquare(unsigned int widthIndexCentre, unsigned int heightIndexCentre, unsigned int halfSizeInCells, 
						   std::vector< std::vector< bool > > &mazeRegion);
	// Defined as the centre and radius.
	void GetMazeAreaRound(unsigned int widthIndexCentre, unsigned int heightIndexCentre, unsigned int radiusInCells,
						  std::vector< std::vector< bool > > &mazeRegion);

	// This is the 2D maze. 
	// The 3D dimension of boolean has something to do with how it's generated. 
	// But we only need to look at the 1st element of this bool vector.
	// So: 
	//		this->maze[a][b][0] = true ---> a wall
	// 		this->maze[a][b][0] = false --> not a wall (a hallway or whatever)
	std::vector< std::vector< std::vector< bool > > > maze;


	// This is bascially a duplicate of _PROCESS_MEMORY_COUNTERS
	struct sProcessMemoryCounters
	{
		unsigned int processID = 0;
		unsigned long cb = 0;
		unsigned long long PageFaultCount = 0;
		unsigned long long PeakWorkingSetSize = 0;
		unsigned long long WorkingSetSize = 0;
		unsigned long long QuotaPeakPagedPoolUsage = 0;
		unsigned long long QuotaPagedPoolUsage = 0;
		unsigned long long QuotaPeakNonPagedPoolUsage = 0;
		unsigned long long QuotaNonPagedPoolUsage = 0;
		unsigned long long PagefileUsage = 0;
		unsigned long long PeakPagefileUsage = 0;
	};
	// returns a summary of the memory used by process
	bool getMemoryUse(sProcessMemoryCounters &memoryInfo);
	bool getMemoryUse(std::string &sMemoryInfo);
	int getStartAxis() { return m_start_axis; }
	int getStartSide() { return m_start_side; }

private:
	int m_maze_size[2];

	int m_start_axis;
	int m_start_side;

	static const unsigned int UP = 0;
	static const unsigned int DOWN = 1;
	static const unsigned int LEFT = 2;
	static const unsigned int RIGHT = 3;

	std::vector< std::vector< int > > m_dfs_path;

	bool m_randomMove(bool first_move); 
	bool m_validInteger(char* cstr);
	void m_initializeMaze(void);
	void m_randomPoint(bool part);

};

#endif
