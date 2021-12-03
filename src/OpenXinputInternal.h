/* Copyright (C) Nemirtingas
 * This file is part of the OpenXinput project.
 *
 * OpenXinput project is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXinput project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXinput project.  If not, see <https://www.gnu.org/licenses/>
 */

#pragma once

#ifdef OPENXINPUT_STATIC_GUIDS
    // Build guids, don't need to link against other dll
    #define INITGUID
#else
    #include <mmdeviceapi.h>
    #include <functiondiscoverykeys.h>
#endif

#define VC_EXTRALEAN
#define NOMINMAX

#include "OpenXinput.h"
#include <strsafe.h>
#include <SetupAPI.h>

///////////////////////////////////////
// api-ms-win-core-quirks-l1-1-0
extern "C" {
BOOL WINAPI QuirkIsEnabled(ULONG quirk);
}

///////////////////////////////////////
// api-ms-win-devices-query-l1-1-0
#include <devpropdef.h>

typedef HANDLE HDEVQUERY;

typedef struct _DEV_QUERY_RESULT_ACTION_DATA
{
    BOOL bUnk1;
    PVOID lpvUnk2;
    PVOID lpvUnk3;
} DEV_QUERY_RESULT_ACTION_DATA, * PDEV_QUERY_RESULT_ACTION_DATA;

typedef struct _DEV_QUERY_UNK_STRUCT1
{
    char unk1[32];
    void* unk2;
    void* unk3;
} DEV_QUERY_UNK_STRUCT1, * PDEV_QUERY_UNK_STRUCT1;

typedef struct _DEV_QUERY_PARAM
{
    DWORD unk1;
    DEV_QUERY_UNK_STRUCT1 unk2;
    void* unk3;
    DEVPROPKEY DevPropKeyContainerId;
    DWORD unk4;
    DWORD unk5;
    DWORD unk6;
    DWORD unk7;
    void* unk8;
    void* unk9;
    DEVPROPKEY DevPropKeyClassGuid;
    DWORD unk10;
    DWORD unk11;
    DWORD unk12;
    DWORD unk13;
    const GUID* pQueryGUID;
    DWORD unk14;
    DEV_QUERY_UNK_STRUCT1 unk15;
} DEV_QUERY_PARAM, *PDEV_QUERY_PARAM;

typedef void(CALLBACK* DO_DEVICE_QUERY_CALLBACK)(DWORD a, PVOID pUserParam, PVOID pCallbackData);

extern "C" {
HRESULT WINAPI DevCreateObjectQuery(DWORD a, DWORD b, DWORD c, DEVPROPKEY* DevPropKey, DWORD e, PDEV_QUERY_PARAM pDeviceQueryParam, void(CALLBACK* ObjectQueryCallback)(HDEVQUERY, PVOID, PDEV_QUERY_RESULT_ACTION_DATA), PVOID pUserParam, HDEVQUERY* lpHDevQuery);
void WINAPI DevCloseObjectQuery(HDEVQUERY hQuery);
}

///////////////////////////////////////
///////////////////////////////////////

#include <DEVOBJ.h>

#ifndef OPENXINPUT_DISABLE_COM
DECLARE_INTERFACE_(IXInputDevice, IUnknown)
{
    // IUnknown methods
    STDMETHOD(QueryInterface)   (THIS_ _In_ REFIID, _Outptr_ LPVOID*) PURE;
    STDMETHOD_(ULONG, AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG, Release)   (THIS) PURE;

    // IXInputDevice methods
    STDMETHOD_(ULONG, CreateFileW)(THIS_ LPCWSTR DevicePath, DWORD dwDesiredAccess, DWORD dwSharedMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, LPHANDLE lpHandle) PURE;
};

DECLARE_INTERFACE_(IInputHostClient, IUnknown)
{
    // IUnknown methods
    STDMETHOD(QueryInterface)   (THIS_ _In_ REFIID, _Outptr_ LPVOID*) PURE;
    STDMETHOD_(ULONG, AddRef)    (THIS) PURE;
    STDMETHOD_(ULONG, Release)   (THIS) PURE;

    // IInputHostClient methods
    STDMETHOD_(ULONG, unk0)(THIS) PURE;
    STDMETHOD_(ULONG, unk1)(THIS) PURE;
    STDMETHOD_(ULONG, unk2)(THIS_ DWORD) PURE;
};
#endif

#define DEVICE_STATUS_ACTIVE 1
#define DEVICE_STATUS_BUS_ACTIVE 2

#define DEFINE_HIDDEN_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        static const GUID name = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }

#define DEFINE_HIDDEN_DEVPROPKEY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8, pid) \
        static const DEVPROPKEY name = { { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }, pid }

#define DEFINE_HIDDEN_API_PKEY(name, managed_name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8, pid) \
        DEFINE_HIDDEN_DEVPROPKEY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8, pid)

DEFINE_HIDDEN_DEVPROPKEY(static_PKEY_SWD_DeviceInterfaceId, 0x9C119480, 0x4954, 0xDDC2, 0xD2, 0x5B, 0x50, 0xA1, 0xAD, 0x54, 0xD4, 0x40, 0x00000001);
#define XINPUT_PKEY_SWD_DeviceInterfaceId static_PKEY_SWD_DeviceInterfaceId

DEFINE_HIDDEN_API_PKEY(static_DEVPKEY_Device_InstanceId, DeviceInstanceId, 0x78c34fc8, 0x104a, 0x4aca, 0x9e, 0xa4, 0x52, 0x4d, 0x52, 0x99, 0x6e, 0x57, 0x00000100);
DEFINE_HIDDEN_DEVPROPKEY(static_DEVPKEY_Device_ContainerId       , 0x8c7ed206, 0x3f8a, 0x4827, 0xb3, 0xab, 0xae, 0x9e, 0x1f, 0xae, 0xfc, 0x6c, 2); // DEVPROP_TYPE_GUID
DEFINE_HIDDEN_DEVPROPKEY(static_DEVPKEY_DeviceInterface_ClassGuid, 0x026e516e, 0xb814, 0x414b, 0x83, 0xcd, 0x85, 0x6d, 0x6f, 0xef, 0x48, 0x22, 4); // DEVPROP_TYPE_GUID

#define XINPUT_DEVPKEY_Device_InstanceId         static_DEVPKEY_Device_InstanceId
#define XINPUT_DEVPKEY_Device_ContainerId        static_DEVPKEY_Device_ContainerId
#define XINPUT_DEVPKEY_DeviceInterface_ClassGuid static_DEVPKEY_DeviceInterface_ClassGuid

#ifdef OPENXINPUT_STATIC_GUIDS
    DEFINE_HIDDEN_GUID(static_XINPUT_IID_IClassFactory     , 0x00000001, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46);
    DEFINE_HIDDEN_GUID(static_XINPUT_IID_IKsPropertySet    , 0x31efac30, 0x515c, 0x11d0, 0xa9, 0xaa, 0x00, 0xaa, 0x00, 0x61, 0xbe, 0x93);

    DEFINE_HIDDEN_GUID(static_GUID_DEVCLASS_MEDIA          , 0x4d36e96c, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18);
    DEFINE_HIDDEN_GUID(static_DEVINTERFACE_AUDIO_RENDER    , 0xe6327cad, 0xdcec, 0x4949, 0xae, 0x8a, 0x99, 0x1e, 0x97, 0x6a, 0x79, 0xd2);
    DEFINE_HIDDEN_GUID(static_DEVINTERFACE_AUDIO_CAPTURE   , 0x2eef81be, 0x33fa, 0x4800, 0x96, 0x70, 0x1c, 0xd4, 0x74, 0x97, 0x2c, 0x3f);

    #define XINPUT_IID_IClassFactory                 static_XINPUT_IID_IClassFactory
    #define XINPUT_IID_IKsPropertySet                static_XINPUT_IID_IKsPropertySet

    #define XINPUT_GUID_DEVCLASS_MEDIA               static_GUID_DEVCLASS_MEDIA
    #define XINPUT_DEVINTERFACE_AUDIO_RENDER         static_DEVINTERFACE_AUDIO_RENDER
    #define XINPUT_DEVINTERFACE_AUDIO_CAPTURE        static_DEVINTERFACE_AUDIO_CAPTURE
#else
    #define XINPUT_IID_IClassFactory                 IID_IClassFactory
    #define XINPUT_IID_IKsPropertySet                IID_IKsPropertySet

    #define XINPUT_GUID_DEVCLASS_MEDIA               GUID_DEVCLASS_MEDIA
    #define XINPUT_DEVINTERFACE_AUDIO_RENDER         DEVINTERFACE_AUDIO_RENDER
    #define XINPUT_DEVINTERFACE_AUDIO_CAPTURE        DEVINTERFACE_AUDIO_CAPTURE
#endif