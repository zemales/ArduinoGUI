#pragma once
#include "comHandle.h"

comHandle::comHandle(HWND hWnd, std::string comPortName)
{
	this->hWnd = hWnd;
	hCom = CreateFileA(comPortName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	if (hCom == INVALID_HANDLE_VALUE) {
		int error = GetLastError();
		MessageBox(hWnd, L"Неправильно введен COM", L"Ошибка", MB_OK);
	}

	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hCom, &dcbSerialParams)) {
		CloseHandle(hCom);
	}

	dcbSerialParams.BaudRate = CBR_9600;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;

	if (!SetCommState(hCom, &dcbSerialParams)) {
		CloseHandle(hCom);
	}

	Sleep(2000);
}

void comHandle::readCOM(std::string path)
{
	HANDLE file = CreateFileA(path.c_str(),
		GENERIC_WRITE, 
		0, 
		NULL, 
		CREATE_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL
	);

	if (file == INVALID_HANDLE_VALUE) {
		MessageBox(hWnd, L"readCOM", L"Ошибка", MB_OK);
	}

	DWORD size;
	char receivedChar = NULL;

	while (ReadFile(hCom, &receivedChar, 1, &size, 0)) {
		if (receivedChar != 'q') {
			WriteFile(file, &receivedChar, 1, &size, 0);
		}
		else { break; }
	}

	CloseHandle(file);
}

bool comHandle::awakeArduino()
{
	std::string command = "awake!";
	DWORD bytesWritten;
	if (!WriteFile(hCom, command.c_str(), command.size(), &bytesWritten, nullptr)) {
		MessageBox(hWnd, L"Не получилось послать ардуино сообщение", L"Ошибка", MB_OK);
		return false;
	}
	return true;
}