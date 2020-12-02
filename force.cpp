// Xavier Bergeron 2020
// https://github.com/xaviergmail/DiscordNotificationDebug/
// This program tricks any Electron instances into thinking that the computer is locked and going to sleep every X seconds
// The purpose of this is to trigger mobile push notifications for Discord regardless of current activity state.

#include <iostream>
#include <windows.h>
#include <vector>

BOOL CALLBACK processWindow(HWND hWindow, LPARAM param)
{
	char szName[1024];
	GetWindowTextA(hWindow, szName, 1024);

	char szClass[1024];
	GetClassNameA(hWindow, szClass, 1024);

	printf("\"%s\" [%s]: ", szName, szClass);
	DWORD sessionID = 0;
	ProcessIdToSessionId(GetCurrentProcessId(), &sessionID);

	printf("Sending sleep event.. ");
	SendMessageA(hWindow, WM_WTSSESSION_CHANGE, WTS_SESSION_LOCK, sessionID);  // "lock" event
	SendMessageA(hWindow, WM_POWERBROADCAST, PBT_APMSUSPEND, NULL);  // "suspend" event
	Sleep(1500);
	printf("Sending resume event\n\n");
	SendMessageA(hWindow, WM_POWERBROADCAST, PBT_APMRESUMEAUTOMATIC, NULL);  // "resume" event
	SendMessageA(hWindow, WM_WTSSESSION_CHANGE, WTS_SESSION_UNLOCK, sessionID);  // "unlock" event

	return TRUE;
}

// Reference:
// https://github.com/electron/electron/blob/a9924e1c32e8445887e3a6b5cdff445d93c2b18f/shell/browser/api/electron_api_power_monitor_win.cc
const char* kPowerMonitorWindowClass = "Electron_PowerMonitorHostWindow";

void findWindows(HWND hWindow = NULL) {
	do {
		hWindow = FindWindowExA(NULL, hWindow, kPowerMonitorWindowClass, NULL);
		processWindow(hWindow, NULL);
	} while (hWindow != NULL);
}

int main(int argc, const char** argv) {
	std::cout
		<< "Starting discord push notification forcer v1\n"
		<< "Written by Xavier Bergeron https://github.com/xaviergmail/DiscordNotificationDebug/\n\n"
		<< std::endl;

	int seconds = 10;

	if (argc < 2) {
		std::cout << "You can also pass the amount in seconds as a command line argument to avoid this prompt\n";
		std::cout << "Enter the amount in seconds between notification pushes (minimum 1): " << std::flush;
		std::cin >> seconds;
	}
	else {
		seconds = atoi(argv[1]);
	}

	seconds = seconds < 1 ? 1 : seconds;
	printf("Starting force notification push every %d seconds! Keep this window open.\n", seconds);
	while (true) {
		findWindows();
		Sleep(seconds * 1000);
	}
}
