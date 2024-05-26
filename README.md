# OpenXInput
A re-implementation of the XInput userspace library for Windows that allows for use of more than 4 XInput devices, while maintaining compatibility with standard XInput.

## Purpose
Standard XInput can only handle 4 devices, but the underlying XUSB driver can handle more. By using OpenXInput, you can bypass this limitation and set your own controller limit at compile-time.

## Usage
See the [wiki](../../wiki).

## Disclaimer
This code is based on Xinput binaries and PDBs and might not be suitable for your business as its standing in the reverse engineering area and is very different in many countries.
For a free to use Windows Xinput like library, you can take a look at my [gamepad library](https://github.com/Nemirtingas/gamepad/blob/d4dd203c6bce54521ac73e8d8fd7653ed2056850/src/gamepad.cpp#L354-L714) or switch to the new [Windows Gaming Input (WGI) APIs](https://learn.microsoft.com/en-us/uwp/api/windows.gaming.input):