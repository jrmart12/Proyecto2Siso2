#pragma once

#include "resource.h"
#include "Drive.h"
#include "atlstr.h"
#include <vector>
#include <iostream>
#include <commctrl.h>
#define MAIN_WINDOW_CLASS		"Main Window"
#define LEFT_WINDOW_CLASS		"Left Window"
#define RIGHT_WINDOW_CLASS		"Right Window"
#define LEFT_WINDOW_WIDTH		200
#define	SPLITTER_BAR_WIDTH		0
#define	LEFT_MINIMUM_SPACE		150
#define	RIGHT_MINIMUM_SPACE		500

//Height of the static text is equal to TOP_POS
#define	STATIC_TEXT_HEIGHT		TOP_POS

#define MAX_LOADSTRING 100
using namespace std;

#pragma comment(lib, "ComCtl32.lib")
#include <shellapi.h>