// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#ifndef PCH_H
#define PCH_H

#pragma once
#pragma warning(disable : 4136 4244 4305 4312)
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glut32.lib")

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h> 
#include <Commdlg.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/timeb.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "glut.h"

using namespace std;
typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned short ushort;

#endif //PCH_H
