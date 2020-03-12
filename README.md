# ThreateningYeti [![Build status](https://ci.appveyor.com/api/projects/status/l27pfn6pfyf07b0v?svg=true)](https://ci.appveyor.com/project/sup3rvis0r/threateningyeti)

[Download the latest version here](https://github.com/melotic/ThreateningYeti/releases)

ThreateningYeti is a bypass designed for Repondus LockDown Browser. It consists of two projects, the injected DLL (ThreateningYeti) and the DLL Injector (YetiLoader).

ThreateningYeti will hook many WinAPI functions and LockDown Browser's own functions to remove the restrictions of the browser. YetiLoader creates the LockDown Browser process and injects the DLL automatically.

## Features
- Alt-tab during tests
- Disabled covering windows
- Enabled F1-F12 keys, as well as media keys and windows key
- Clipboard is not cleared
- Taskmanager (as well as auto run) is enabled
- LockDown Browser losing focus wont cause it to exit

## Usage

1. Download the latest release from [here](https://github.com/melotic/ThreateningYeti/releases/download/1.1/Release.zip)
2. Ensure you're running the latest version of Lockdown Browser ([How-to](https://support.respondus.com/support/index.php?/Knowledgebase/Article/View/326/0/how-to-update-lockdown-browser))
3. Run YetiLoader.exe

YetiLoader will automatically launch Lockdown Browser.

## Build Instructions

1. Install MinHook as a static package with [vcpkg](https://github.com/microsoft/vcpkg "vcpkg")
2. Compile ThreateningYeti and YetiLoader
3. Run YetiLoader

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.


## License
[GNU GPLv3](https://choosealicense.com/licenses/gpl-3.0/)
