# OpenXInput
An open-source re-implementation of the XInput driver for Windows that allows for use of more than 4 XInput devices, while maintaining compatibility with standard XInput.

## Purpose
Standard XInput can only handle 4 devices, but the underlying XUSB driver can handle more. By using OpenXInput, you can bypass this limitation and set your own controller limit at compile-time.

## Usage
See the [wiki](../../wiki).