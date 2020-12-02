// Xavier Bergeron 2020 - https://github.com/xaviergmail/discordnotificationdebug
// This program helps detect devices that cause false input events on Windows, breaking idle detection scripts that rely on GetLastInputInfo
// I initially ran into this issue with the Discord Windows application. I assumed it used Electron's powerMonitor API (https://www.electronjs.org/docs/api/power-monitor)
// After much digging around, I finally isolated the root of the issue to the underlying implementation in Chromium:
// https://chromium.googlesource.com/chromium/src/+/refs/tags/89.0.4343.1/ui/base/idle/idle_win.cc
//
// In my case, the culprit was the JC-W01UWH Wii gamepad USB adapter commonly used by Guitar Hero / Clone Hero players.


#include <iostream>
#include <windows.h>

// Taken directly from Chromium's implementation
DWORD CalculateIdleTimeInternal() {
	LASTINPUTINFO last_input_info = { 0 };
	last_input_info.cbSize = sizeof(LASTINPUTINFO);
	DWORD current_idle_time = 0;
	if (::GetLastInputInfo(&last_input_info)) {
		DWORD now = ::GetTickCount();
		if (now < last_input_info.dwTime) {
			// GetTickCount() wraps around every 49.7 days -- assume it wrapped just
			// once.
			const DWORD kMaxDWORD = static_cast<DWORD>(-1);
			DWORD time_before_wrap = kMaxDWORD - last_input_info.dwTime;
			DWORD time_after_wrap = now;
			// The sum is always smaller than kMaxDWORD.
			current_idle_time = time_before_wrap + time_after_wrap;
		}
		else {
			current_idle_time = now - last_input_info.dwTime;
		}
		// Convert from ms to seconds.
		current_idle_time /= 1000;
	}
	return current_idle_time;
}

void pause() {
	std::cout << "Press Enter continue..." << std::flush;
	getchar();
	std::cout << "\n\n\n" << std::flush;
}

int main()
{
	std::cout
		<< "DiscordNotificationDebug v1 written by Xavier Bergeron\n"
		<< "https://github.com/xaviergmail/DiscordNotificationDebug/\n\n\n"
		<< "If you are here, you are likely experiencing issues with discord\n"
		<< "notifications not being sent to your mobile device regardless of\n"
		<< "the \"Push Notification Inactive Timeout\" setting. Chances are\n"
		<< "that you either have a program, driver or peripheral firing\n"
		<< "phantom input events that get picked up by Windows as real user\n"
		<< "input. This program will help you isolate the cause of the problem.\n\n";
	pause();

	std::cout
		<< "You should only be getting the \"System was idle for X seconds\" message\n"
		<< "when using your keyboard or moving your mouse. If it happens without you\n"
		<< "touching anything, there is something problematic causing phantom inputs.\n\n"
		<< "Pay close attention to the console output. You want the counter to continuously\n"
		<< "count upwards and it should ONLY reset if you touch your mouse or keyboard.\n"
		<< "If it never counts up or resets at random times without you touching anything,\n"
		<< "you may be victim of phantom inputs. If that's the case, start by unplugging\n"
		<< "peripherals such as mice and keyboards followed by all other USB devices.\n"
		<< "If that doesn't work, start closing all programs from the system tray\n"
		<< "then move on to killing processes from task manager and stopping services.\n"
		<< "If all else fails, update your drivers and/or try isolating them.\n\n";
	pause();

	int i = 0;
	const char* loading = "|/-\\";
	while (true) {
		DWORD delta = CalculateIdleTimeInternal();
		char witness = loading[i++ % 4];
		if (delta <= 1) {
			printf("%c System is not idle!\n", witness);
		}
		else {
			printf("%c System has been idle for %d seconds.\n", witness, delta);
		}
		Sleep(1000);
	}
}
