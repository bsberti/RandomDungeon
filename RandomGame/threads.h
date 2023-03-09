#include <tchar.h>
#include <windows.h>
#include <iostream>
#include <process.h>
#include <synchapi.h>

#ifdef _UNICODE
#define mycout wcout
#else
#define mycout cout
#endif

// Where did we get this....?

using namespace std;

typedef unsigned (__stdcall *PTHREAD_START) (void *);

#define chBEGINTHREADEX(psa, cbStack, pfnStartAddr, \
   pvParam, fdwCreate, pdwThreadID)                 \
      ((HANDLE) _beginthreadex(                     \
         (void *) (psa),                            \
         (unsigned) (cbStack),                      \
         (PTHREAD_START) (pfnStartAddr),            \
         (void *) (pvParam),                        \
         (unsigned) (fdwCreate),                    \
         (unsigned *) (pdwThreadID))) 
