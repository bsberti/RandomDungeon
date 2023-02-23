#include "cMazeMaker_W2023.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <psapi.h>
#include <sstream>


void cMazeMaker_W2023::GetMazeAreaSquare(unsigned int widthIndexCentre, 
                                         unsigned int heightIndexCentre, 
                                         unsigned int halfSizeInCells, 
                                         std::vector< std::vector< bool > > &mazeRegion)
{
    mazeRegion.clear();

    unsigned int mazeSize = halfSizeInCells * 2;

    for ( unsigned int rowIndex = 0; rowIndex < mazeSize; rowIndex++ )
    {    
        // Make an empty row
        mazeRegion.push_back(std::vector<bool>());

        for ( unsigned int columnIndex = 0; columnIndex < mazeSize; columnIndex++ )
        {
            // Add an empty region
            mazeRegion[rowIndex].push_back(false);

        }//for ( unsigned int columnIndex
    }//for ( unsigned int rowIndex

    // Pick the portion you want from the larger maze
    for (unsigned int rowIndex = 0; rowIndex < mazeSize; rowIndex++)
    {
        for (unsigned int columnIndex = 0; columnIndex < mazeSize; columnIndex++)
        {
            int actualRowIndex = (int)rowIndex + (int)widthIndexCentre - (int)halfSizeInCells;
            int actualColumnIndex = (int)columnIndex + (int)heightIndexCentre - (int)halfSizeInCells;

            if ( ( actualRowIndex > 0 ) && ( actualRowIndex < this->maze.size() ) )
            {
                // Row index is OK
                if ( ( actualColumnIndex > 0 ) && (actualColumnIndex < this->maze[actualRowIndex].size() ) )
                {
                    // Column index is OK
                    mazeRegion[rowIndex][columnIndex] = this->maze[actualRowIndex][actualColumnIndex][0];
                }
                else
                {
                    // invalid column index, so leave blank (empty space)
                    mazeRegion[rowIndex][columnIndex] = 0;
                }//if ( ( actualColumnIndex...
            }
            else
            {
                // invalid row index, so leave blank (empty space)
                mazeRegion[rowIndex][columnIndex] = 0;
            }//if ( ( actualRowIndex...


        }//for ( unsigned int columnIndex
    }//for ( unsigned int rowIndex

    return;
}


bool cMazeMaker_W2023::getMemoryUse(std::string& sMemoryInfo)
{

    sProcessMemoryCounters memoryInfo;
    
    if ( ! this->getMemoryUse(memoryInfo) )
    {
        return false;
    }

    std::stringstream ssInfo;

    ssInfo
        << "Process information for PID: " << memoryInfo.processID << "\n"
        << "\tPageFaultCount: " << memoryInfo.PageFaultCount << "\n"
        << "\tPeakWorkingSetSize: " << memoryInfo.PeakWorkingSetSize << "\n"
        << "\tWorkingSetSize: " << memoryInfo.WorkingSetSize << "\n"
        << "\tQuotaPeakPagedPoolUsage: " << memoryInfo.QuotaPeakPagedPoolUsage << "\n"
        << "\tQuotaPagedPoolUsage: " << memoryInfo.QuotaPagedPoolUsage << "\n"
        << "\tQuotaPeakNonPagedPoolUsage: " << memoryInfo.QuotaPeakNonPagedPoolUsage << "\n"
        << "\tQuotaNonPagedPoolUsage: " << memoryInfo.QuotaNonPagedPoolUsage << "\n"
        << "\tPagefileUsage: " << memoryInfo.PagefileUsage << "\n"
        << "\tPeakPagefileUsage: " << memoryInfo.PeakPagefileUsage << "\n";

    sMemoryInfo = ssInfo.str();

    return true;
}



bool cMazeMaker_W2023::getMemoryUse(sProcessMemoryCounters& memoryInfo)
{
	// https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getcurrentprocessid
	// https://learn.microsoft.com/en-us/windows/win32/psapi/collecting-memory-usage-information-for-a-process

	DWORD processID = GetCurrentProcessId();

    HANDLE hProcess;
    PROCESS_MEMORY_COUNTERS pmc;

    // Print information about the memory usage of the process.

    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
                           PROCESS_VM_READ,
                           FALSE, processID);
    if (NULL == hProcess)
    {
        return false;
    }

    std::stringstream ssInfo;
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
    {
        memoryInfo.processID = processID;
        memoryInfo.cb = pmc.cb;
        memoryInfo.PageFaultCount = pmc.PageFaultCount;
        memoryInfo.PeakWorkingSetSize = pmc.PeakWorkingSetSize;
        memoryInfo.WorkingSetSize = pmc.WorkingSetSize;
        memoryInfo.QuotaPeakPagedPoolUsage = pmc.QuotaPeakPagedPoolUsage;
        memoryInfo.QuotaPagedPoolUsage = pmc.QuotaPagedPoolUsage;
        memoryInfo.QuotaPeakNonPagedPoolUsage = pmc.QuotaPeakNonPagedPoolUsage;
        memoryInfo.QuotaNonPagedPoolUsage = pmc.QuotaNonPagedPoolUsage;
        memoryInfo.PagefileUsage = pmc.PagefileUsage;
        memoryInfo.PeakPagefileUsage = pmc.PeakPagefileUsage;
    }


    CloseHandle(hProcess);

    return true;
}
