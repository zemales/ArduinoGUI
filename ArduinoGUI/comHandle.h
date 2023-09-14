#pragma once
#include <string>
#include <Windows.h>
#include "pch.h"

class comHandle 
{
	HWND hWnd;
	HANDLE hCom;
	DCB dcbSerialParams = { 0 };
public:
	comHandle(HWND hWnd, std::string str);
	bool awakeArduino();
	void readCOM(std::string path);
};