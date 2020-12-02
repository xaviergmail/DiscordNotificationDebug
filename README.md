# DiscordNotificationDebug
Are you frustrated by discord's `Push Notification Inactive Timeout` feature seemingly not working as expected?  **It might not be Discord's fault after all!**

In my case, the culprit was the `JC-W01UWH` Wii gamepad USB adapter commonly used by Guitar Hero / Clone Hero players. After troubleshooting with other users who were experiencing the same issue, I also found that other gamepad controllers as well as some no-name OEM gaming mice and suspicious unidentified processes were also to blame.


## Usage
1. Head on over to the [Releases tab](https://github.com/xaviergmail/DiscordNotificationDebug/releases) and download the program (or compile it yourself)
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
etc
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

## Background
I initially ran into this issue with the Discord Windows application.
I made the assumption that it used Electron's [powerMonitor API](https://www.electronjs.org/docs/api/power-monitor) or the browser equivalent [window.requestIdleCallback](https://developer.mozilla.org/en-US/docs/Web/API/Window/requestIdleCallback) and started investigating.
After much digging around, I found that Electron creates an invisible off-screen window to listen to sleep and screen lock events and falls back on chromium to detect an inactivity timeout. On Windows platforms, Chromium's [idle handler](https://chromium.googlesource.com/chromium/src/+/refs/tags/89.0.4343.1/ui/base/idle/idle_win.cc) relies on Win32's [GetLastInputInfo](https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getlastinputinfo). After making this discovery and realizing that GetLastInputInfo was never returning the same tick count, I wrote this program to help me identify the source of the phantom inputs.

## Related bug reports that went unnoticed
https://support.discord.com/hc/en-us/community/posts/360051256154-Push-Notification-Inactive-Timeout-no-longer-working
https://support.discord.com/hc/en-us/community/posts/360049313814-MOBILE-PUSH-NOTIFICATIONS-REMOVED-Bring-them-back
https://support.discord.com/hc/en-us/community/posts/360052612473-Will-not-receive-push-notifications-until-desktop-client-is-closed

## Other info
While this solution requires some trial and error by the user's part to find the root cause, it is by far the simplest to implement. After some quick research, I was unable to find any way to reliably determine the source of the input events and it is unknown what kind of events raise the tick count returned by GetLastInputInfo so such an approach may not even be 100% accurate.

## Known faulty devices
Found something that causes phantom inputs? Add it to the [list](https://github.com/xaviergmail/DiscordNotificationDebug/wiki/Known-problematic-devices)!
