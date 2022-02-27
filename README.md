# DiscordNotificationDebug
Are you frustrated by discord's `Push Notification Inactive Timeout` feature seemingly not working as expected?  **It might not be Discord's fault after all!**

In my case, the culprit was the `JC-W01UWH` Wii gamepad USB adapter commonly used by Guitar Hero / Clone Hero players. After troubleshooting with other users who were experiencing the same issue, I also found that other gamepad controllers as well as some no-name OEM gaming mice and suspicious unidentified processes were also to blame.

## Usage
1. Head on over to the [Releases tab](https://github.com/xaviergmail/DiscordNotificationDebug/releases) and download the **`InputDebugger.exe`** program (or compile main.cpp yourself)
2. Run the program and read the on-screen instructions
3. Let go of your mouse and keyboard and watch the output

Example of **correct** output:
```
/ System is not idle!
- System is not idle!
\ System has been idle for 2 seconds.
| System has been idle for 3 seconds.
/ System has been idle for 4 seconds.
- System has been idle for 5 seconds.
\ System has been idle for 6 seconds.
| System has been idle for 7 seconds.
```

Example of **INCORRECT** output:
```
/ System is not idle!
- System is not idle!
\ System is not idle!
| System is not idle!
/ System is not idle!
- System is not idle!
\ System is not idle!
| System is not idle!
```

It is also theoretically possible that a device may be sending periodical phantom inputs rather than constantly producing them.
For example, a device could be generating a single input event every 30 seconds.

Whatever the interval is (if it is lower than the configured idle timeout), discord will never send you mobile push notifications.

Example of **INCORRECT** output (periodical phantom inputs):
```
| System has been idle for 61 seconds.
/ System has been idle for 62 seconds.
- System has been idle for 64 seconds.
\ System has been idle for 65 seconds.
| System has been idle for 66 seconds.
/ System is not idle!
- System is not idle!
\ System has been idle for 2 seconds.
| System has been idle for 3 seconds.
/ System has been idle for 4 seconds.
```

## Discord Notification Forcer
If you are unable (or unwilling) to fix phantom inputs, or you just want to force discord to send you notifications even if you're currently active at your computer, head on over to the [Releases tab](https://github.com/xaviergmail/DiscordNotificationDebug/releases) and download the **`NotificationForcer.exe`** (or compile force.cpp yourself).

You can either run the program directly which will ask you at what interval you wish to receive mobile notifications. Alternatively, you can supply the time in seconds as a command-line argument if you wish to make this program run in the background as a service or as part of your autorun.

I highly recommend using a value above 30 seconds, as this will toggle your discord status from online to 'away' each time it forces the notifications.

Please note that this targets _ALL_ Electron-based applications as their implementation use system-wide singleton for idle checking. Meaning that other Electron applications relying on the `powerMonitor` api will also get the `suspend` and `lock` signals. Thankfully, Spotify, Tidal and Messenger seem to be unaffected by this from my testing. Please let me know if you find a program that this conflicts with.

Once again, I highly recommend fixing the phantom inputs before relying to this.

## Background
I initially ran into this issue with the Discord Windows application.

I made the assumption that it used Electron's [powerMonitor API](https://www.electronjs.org/docs/api/power-monitor) or the browser equivalent [window.requestIdleCallback](https://developer.mozilla.org/en-US/docs/Web/API/Window/requestIdleCallback) and started investigating.

After much digging around, I found that Electron creates an invisible off-screen window to listen to sleep and screen lock events and falls back on chromium to detect an inactivity timeout. On Windows platforms, Chromium's [idle handler](https://chromium.googlesource.com/chromium/src/+/refs/tags/89.0.4343.1/ui/base/idle/idle_win.cc) relies on Win32's [GetLastInputInfo](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getlastinputinfo). After making this discovery and realizing that GetLastInputInfo was never returning the same tick count, I wrote this program to help me identify the source of the phantom inputs. 

## Mac Users
Disconnect all non-vital peripherals and enable your screensaver.

## Linux Users
Disconnect all non-vital peripherals.

Chromium currently only supports Xorg / Ozone.

## Related bug reports that went unnoticed
https://support.discord.com/hc/en-us/community/posts/360051256154-Push-Notification-Inactive-Timeout-no-longer-working

https://support.discord.com/hc/en-us/community/posts/360049313814-MOBILE-PUSH-NOTIFICATIONS-REMOVED-Bring-them-back

https://support.discord.com/hc/en-us/community/posts/360052612473-Will-not-receive-push-notifications-until-desktop-client-is-closed


## Other info
While this solution requires some trial and error by the user's part to find the root cause, it is by far the simplest to implement. After some quick research, I was unable to find any way to reliably determine the source of the input events and it is unknown what kind of events raise the tick count returned by GetLastInputInfo so such an approach may not even be 100% accurate.

## Known faulty devices
Found something that causes phantom inputs? Add it to the [list](https://github.com/xaviergmail/DiscordNotificationDebug/wiki/Known-problematic-devices)!
