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

#include "OpenXinputInternal.h"

#if(_WIN32_WINNT >= _WIN32_WINNT_WIN10)
// XInputEnable is deprecated since Windows 10, disable the warning if needed to build Xinput.
#pragma warning(disable : 4995)
// Disable 4996 because of GetVersionExW
#pragma warning(disable : 4996)
#endif

DEFINE_HIDDEN_GUID(CLSID_DeviceBroker, 0xACC56A05, 0xE277, 0x4B1E, 0xA4, 0x3E, 0x7A, 0x73, 0xE3, 0xCD, 0x6E, 0x6C);
DEFINE_HIDDEN_GUID(GUID_8604b268_34a6_4b1a_a59f_cdbd8379fd98, 0x8604b268, 0x34a6, 0x4b1a, 0xa5, 0x9f, 0xcd, 0xbd, 0x83, 0x79, 0xfd, 0x98);

typedef struct _SP_XINPUTINFO_DATA
{ // Tuned SP_DEVINFO_DATA
    DWORD cbSize;
    GUID  ClassGuid;
    DWORD DevInst;    // DEVINST handle
    ULONG_PTR Reserved1;
    ULONG_PTR Reserved2;
    ULONG_PTR Reserved3;
} SP_XINPUTINFO_DATA, * PSP_XINPUTINFO_DATA;

/////////////////////////////////////////////////////
// Declarations
/////////////////////////////////////////////////////
struct DeviceInfo_t {
    DWORD status;
    HANDLE hDevice;
    HANDLE hGuideWait;
    BYTE dwUserIndex;
    LPWSTR lpDevicePath;
    DWORD dwDevicePathSize;
    LPWSTR field_18;
    SIZE_T field_1C;
    WORD wType;
    WORD field_22;
    XINPUT_STATE DeviceState;
    XINPUT_VIBRATION DeviceVibration;
    WORD field_38;
    WORD vendorId;
    WORD productId;
    WORD inputId;
    int dwBusIndex;
    BYTE bTriggers;
    WORD wButtons;
    WORD LeftStickVirtualKey;
    WORD RightStickVirtualKey;
    WORD field_4C;
    WORD field_4E;
};

struct XINPUT_AUDIO_INFORMATION
{
    WORD vendorId;
    WORD productId;
    BYTE inputId;
};

// All theses structs are passed to the XUSB driver, they __NEED__ to be unaligned.
#pragma pack(push, 1)
struct InSetLEDBuffer_t
{
    BYTE deviceIndex;
    BYTE state;
    BYTE unk0;
    BYTE unk1;
    BYTE unk2;
};

struct InVibrationBuffer_t
{
    BYTE deviceIndex;
    BYTE unk0;
    BYTE leftMotorSpeed;
    BYTE rightMotorSpeed;
    BYTE unk1;
};

struct InGetAudioDeviceInformation_t
{
    WORD wType;
    BYTE DeviceIndex;
};

struct OutGetAudioDeviceInformation_t
{
    WORD unk0;
    WORD vendorId;
    WORD productId;
    BYTE inputId;
};

struct InGetLEDBuffer_t
{
    WORD wType;
    BYTE DeviceIndex;
};

struct OutGetLEDBuffer_t
{
    BYTE unk0;
    BYTE unk1;
    BYTE LEDState;
};

struct OutDeviceInfos_t
{
    WORD wType;
    BYTE deviceIndex;
    BYTE unk1;
    BYTE unk2;
    BYTE unk3;
    WORD unk4;
    WORD vendorId;
    WORD productId;
};

struct InPowerOffBuffer_t
{
    WORD wType;
    BYTE DeviceIndex;
};

struct InWaitForGuideButtonBuffer_t
{
    WORD wType;
    BYTE DeviceIndex;
};

struct OutWaitForGuideButtonBuffer_t
{
    BYTE field_0;
    BYTE field_1;
    BYTE status;
    BYTE field_3;
    BYTE field_4;
    DWORD field_5;
    BYTE field_9;
    BYTE field_A;
    WORD field_B;
    BYTE field_D;
    BYTE field_E;
    WORD field_F;
    WORD field_11;
    WORD field_13;
    WORD field_15;
    BYTE field_17;
    BYTE field_18;
    BYTE field_19;
    BYTE field_1A;
    BYTE field_1B;
    BYTE field_1C;
};

struct InGamepadState0100
{
    BYTE DeviceIndex;
};

struct GamepadState0100
{
    BYTE  status;
    BYTE  unk0;
    BYTE  inputId;
    DWORD dwPacketNumber;
    BYTE  unk2;
    WORD  wButtons;
    BYTE  bLeftTrigger;
    BYTE  bRightTrigger;
    SHORT sThumbLX;
    SHORT sThumbLY;
    SHORT sThumbRX;
    SHORT sThumbRY;
};

struct InGamepadState0101
{
    WORD wType;
    BYTE DeviceIndex;
};

struct GamepadState0101
{
    BYTE  unk0;
    BYTE  inputId;
    BYTE  status;
    BYTE  unk2;
    BYTE  unk3;
    DWORD dwPacketNumber;
    BYTE  unk4;
    BYTE  unk5;
    WORD  wButtons;
    BYTE  bLeftTrigger;
    BYTE  bRightTrigger;
    SHORT sThumbLX;
    SHORT sThumbLY;
    SHORT sThumbRX;
    SHORT sThumbRY;
    BYTE  unk6;
    BYTE  unk7;
    BYTE  unk8;
    BYTE  unk9;
    BYTE  unk10;
    BYTE  unk11;
};

struct InGamepadCapabilities0101
{
    WORD wType;
    BYTE DeviceIndex;
};

struct GamepadCapabilities0101
{
    BYTE  unk0;
    BYTE  unk1;
    BYTE  Type;
    BYTE  SubType;
    WORD  wButtons;
    BYTE  bLeftTrigger;
    BYTE  bRightTrigger;
    SHORT sThumbLX;
    SHORT sThumbLY;
    SHORT sThumbRX;
    SHORT sThumbRY;
    BYTE  unk16;
    BYTE  unk17;
    BYTE  unk18;
    BYTE  unk19;
    BYTE  unk20;
    BYTE  unk21;
    BYTE  bLeftMotorSpeed;
    BYTE  bRightMotorSpeed;
};

struct GamepadCapabilities0102
{
    BYTE unk0;
    BYTE unk1;
    BYTE Type;
    BYTE SubType;
    WORD Flags;
    WORD vendorId;
    WORD productId;
    SHORT unk2;
    DWORD unk3;
    WORD wButtons;
    BYTE bLeftTrigger;
    BYTE bRightTrigger;
    SHORT sThumbLX;
    SHORT sThumbLY;
    WORD sThumbRX;
    WORD sThumbRY;
    DWORD unk4;
    BYTE unk5;
    BYTE unk6;
    BYTE wLeftMotorSpeed;
    BYTE wRightMotorSpeed;
};

struct InBaseBusInformation
{
    WORD wType;
    BYTE unk0;
    WORD field_4;
    WORD field_6;
    WORD field_8;
    WORD field_A;
    WORD field_C;
    WORD field_E;
    WORD field_10;
    WORD field_12;
    WORD field_14;
    WORD field_16;
    WORD field_18;
    WORD field_1A;
    WORD field_1C;
    WORD field_1E;
    WORD field_20;
    WORD field_22;
};

struct OutBaseBusInformation
{
    BYTE field_0;
    BYTE field_1;
    BYTE field_2;
    BYTE field_3;
    DWORD field_4;
    DWORD field_8;
    DWORD field_C;
    DWORD field_10;
    BYTE field_14;
    BYTE field_15;
    BYTE field_16;
    WORD vendorId;
    WORD productId;
    WORD inputId;
    BYTE field_1D;
    BYTE field_1E;
    BYTE field_1F;
    BYTE field_20;
    BYTE field_21;
    BYTE field_22;
    BYTE field_23;
    DWORD field_24;
    BYTE field_28[909];
};

struct InGamepadBatteryInformation0102
{
    WORD wType;
    BYTE DeviceIndex;
    BYTE DeviceType;
};

struct GamepadBatteryInformation0102
{
    WORD wType;
    BYTE BatteryType;
    BYTE BatteryLevel;
};

#pragma pack(pop)

struct GetStateApiParam_t
{
    XINPUT_STATE* pState;
};

struct SetStateApiParam_t
{
    XINPUT_VIBRATION* pVibration;
};

struct GetAudioDeviceIdApiParam_t
{
    LPWSTR pRenderDeviceId;
    UINT* pRenderCount;
    LPWSTR pCaptureDeviceId;
    UINT* pCaptureCount;
};

struct GetBatteryInformationApiParam_t
{
    LPBYTE DeviceType;
    XINPUT_BATTERY_INFORMATION* pBatteryInformation;
};

struct GetCapabilitiesApiParam_t
{
    XINPUT_CAPABILITIES_EX* pCapabilities;
};

struct GetKeystrokeApiParam_t
{
    LPDWORD lpReserved;
    XINPUT_KEYSTROKE* pKeystroke;
    LPDWORD lpLastError;
};

struct WaitGuideButtonApiParam_t
{
    LPHANDLE lphEvent;
    XINPUT_LISTEN_STATE* pListenState;
};

struct WaitForGuideButtonHelperApiParam_t
{
    HANDLE hGuideWait;
    HANDLE hGuideEvent;
    BYTE dwUserIndex;
    XINPUT_LISTEN_STATE* pListenState;
};

struct GetBaseBusInformationApiParam_t
{
    XINPUT_BASE_BUS_INFORMATION* pBaseBusInformation;
};

constexpr DWORD SET_USER_LED_ON_CREATE       = (1 << 0);
constexpr DWORD DISABLE_USER_LED_ON_DESTROY  = (1 << 1);
constexpr DWORD DISABLE_VIBRATION_ON_DESTROY = (1 << 2);

/////////////////////////////////////////////////////
// Anonymous namespace
/////////////////////////////////////////////////////
namespace {

static PVOID g_pDetailBuffer;
static DWORD g_dwDetailBufferSize;

static decltype(DeviceIoControl)* g_pfnDeviceIoControl;
static decltype(DevObjCreateDeviceInfoList)* g_pfnCreateDeviceInfoList;
static decltype(DevObjGetClassDevs)* g_pfnGetClassDevs;
static decltype(DevObjEnumDeviceInfo)* g_pfnEnumDeviceInfo;
static decltype(DevObjEnumDeviceInterfaces)* g_pfnEnumDeviceInterfaces;
static decltype(DevObjGetDeviceInterfaceDetail)* g_pfnGetDeviceInterfaceDetail;
static decltype(DevObjGetDeviceProperty)* g_pfnGetDeviceProperty;
static decltype(DevObjDestroyDeviceInfoList)* g_pfnDestroyDeviceInfoList;

static BOOL g_IsInitialized = FALSE;
static BOOL g_IsCommunicationEnabled = FALSE;

static RTL_CRITICAL_SECTION g_csGlobalLock;
static DWORD g_dwSettings = SET_USER_LED_ON_CREATE | DISABLE_USER_LED_ON_DESTROY | DISABLE_VIBRATION_ON_DESTROY;

static DWORD g_dwDeviceListSize = 0;
static DeviceInfo_t** g_pDeviceList;
static DeviceInfo_t** g_pBusDeviceList;

static DWORD g_dwLogVerbosity = 0;

DWORD XInputReturnCodeFromHRESULT(HRESULT hr);

HRESULT GrowList(DWORD newSize);

void CopyGamepadStateToDeviceInfo(DeviceInfo_t* pDevice, GamepadState0100* pGamepadState);
void CopyGamepadStateToDeviceInfo(DeviceInfo_t* pDevice, GamepadState0101* pGamepadState);
void TranslateCapabilities(DeviceInfo_t* pDevice, GamepadCapabilities0101* pGamepadCapabilities, XINPUT_CAPABILITIES_EX* pCapabilities);
void TranslateCapabilities(GamepadCapabilities0102* pGamepadCapabilities, XINPUT_CAPABILITIES_EX* pCapabilitiesEx);

HRESULT SendReceiveIoctl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPOVERLAPPED lpOverlapped);
HRESULT SendIoctl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize);
HRESULT ReceiveIoctl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpOutBuffer, DWORD nOufBufferSize);

HRESULT EnumerateXInputDevices();
HRESULT EnumerateDevicesOnDeviceInterface(HANDLE hDevice, LPCWSTR DevicePath);
HRESULT ProcessEnumeratedDevice(DeviceInfo_t* pDevice);

HRESULT GetAudioInterfaceStringInfo(DeviceInfo_t* pDevice, WORD* VendorId, WORD* ProductId, BYTE* InputId);
HRESULT GetDeviceInterfaceDetail(HANDLE hDev, PDO_DEVICE_INTERFACE_DATA DeviceInterfaceData, PDO_DEVICE_INTERFACE_DETAIL_DATA* ppDeviceInterfaceDetailData);

DWORD CALLBACK WaitForGuideButtonHelper(void* pParam);

/////////////////////////////////////////////////////
// DevQuery namespace
/////////////////////////////////////////////////////
namespace DevQuery {

struct DevQuery_t {
    HANDLE hEvent;
    DEVPROPKEY DeviceInterfaceId;
    LPVOID unk1;
    LPVOID unk2;
    LPWSTR lpDeviceId;
    UINT*  lpuiDeviceIdCount;
};

struct DevQueryCallbackParam_t {
    BOOL unk1;
    LPVOID unk2;
    LPWSTR lpDevicePath;
};

DevQuery_t* Create();
void Destroy(DevQuery_t* pDevQuery);
HRESULT RenderQuery(DevQuery_t* pDeviceQuery, GUID DeviceGuid, int a3, LPWSTR lpDeviceId, UINT* lpuiDeviceIdCount);
HRESULT CaptureQuery(DevQuery_t* pDeviceQuery, GUID DeviceGuid, int a3, LPWSTR lpDeviceId, UINT* lpuiDeviceIdCount);

void CALLBACK DevQueryCallbackProc(HDEVQUERY hQuery, LPVOID pUserParam, PDEV_QUERY_RESULT_ACTION_DATA pCallbackData);

}

/////////////////////////////////////////////////////
// DevQuery namespace
/////////////////////////////////////////////////////
namespace DeviceProperty {

LPVOID* CreateInstance(HANDLE hDev, SP_XINPUTINFO_DATA* unk1, const PROPERTYKEY* lpPropertyKey);
void DestroyInstance(LPVOID* ppInstance);
    
}

}

/////////////////////////////////////////////////////
// Protocol namespace
/////////////////////////////////////////////////////
namespace Protocol {

DEFINE_HIDDEN_GUID(XUSB_INTERFACE_CLASS_GUID, 0xEC87F1E3, 0xC13B, 0x4100, 0xB5, 0xF7, 0x8B, 0x84, 0xD5, 0x42, 0x60, 0xCB);

constexpr DWORD IOCTL_XINPUT_BASE = 0x8000;

static DWORD IOCTL_XINPUT_GET_INFORMATION          = CTL_CODE(IOCTL_XINPUT_BASE, 0x800, METHOD_BUFFERED, FILE_READ_ACCESS);                     // 0x80006000
static DWORD IOCTL_XINPUT_GET_CAPABILITIES         = CTL_CODE(IOCTL_XINPUT_BASE, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS); // 0x8000E004
static DWORD IOCTL_XINPUT_GET_LED_STATE            = CTL_CODE(IOCTL_XINPUT_BASE, 0x802, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS); // 0x8000E008
static DWORD IOCTL_XINPUT_GET_GAMEPAD_STATE        = CTL_CODE(IOCTL_XINPUT_BASE, 0x803, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS); // 0x8000E00C
static DWORD IOCTL_XINPUT_SET_GAMEPAD_STATE        = CTL_CODE(IOCTL_XINPUT_BASE, 0x804, METHOD_BUFFERED, FILE_WRITE_ACCESS);                    // 0x8000A010
static DWORD IOCTL_XINPUT_WAIT_FOR_GUIDE_BUTTON    = CTL_CODE(IOCTL_XINPUT_BASE, 0x805, METHOD_BUFFERED, FILE_WRITE_ACCESS);                    // 0x8000A014
static DWORD IOCTL_XINPUT_GET_BATTERY_INFORMATION  = CTL_CODE(IOCTL_XINPUT_BASE, 0x806, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS); // 0x8000E018
static DWORD IOCTL_XINPUT_POWER_DOWN_DEVICE        = CTL_CODE(IOCTL_XINPUT_BASE, 0x807, METHOD_BUFFERED, FILE_WRITE_ACCESS);                    // 0x8000A01C
static DWORD IOCTL_XINPUT_GET_AUDIO_INFORMATION    = CTL_CODE(IOCTL_XINPUT_BASE, 0x808, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS); // 0x8000E020
static DWORD IOCTL_XINPUT_GET_BASE_BUS_INFORMATION = CTL_CODE(IOCTL_XINPUT_BASE, 0x8FF, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS); // 0x8000E3FC

namespace LEDState {

static BYTE XINPUT_LED_OFF            = 0;
static BYTE XINPUT_LED_BLINK          = 1;
static BYTE XINPUT_LED_1_SWITCH_BLINK = 2;
static BYTE XINPUT_LED_2_SWITCH_BLINK = 3;
static BYTE XINPUT_LED_3_SWITCH_BLINK = 4;
static BYTE XINPUT_LED_4_SWITCH_BLINK = 5;
static BYTE XINPUT_LED_1              = 6;
static BYTE XINPUT_LED_2              = 7;
static BYTE XINPUT_LED_3              = 8;
static BYTE XINPUT_LED_4              = 9;
static BYTE XINPUT_LED_CYCLE          = 10;
static BYTE XINPUT_LED_FAST_BLINK     = 11;
static BYTE XINPUT_LED_SLOW_BLINK     = 12;
static BYTE XINPUT_LED_FLIPFLOP       = 13;
static BYTE XINPUT_LED_ALLBLINK       = 14;

static BYTE XINPUT_PORT_TO_LED_MAP[] =
{
    XINPUT_LED_1,
    XINPUT_LED_2,
    XINPUT_LED_3,
    XINPUT_LED_4,
};
#define MAX_XINPUT_PORT_TO_LED_MAP (sizeof(Protocol::LEDState::XINPUT_PORT_TO_LED_MAP)/sizeof(*Protocol::LEDState::XINPUT_PORT_TO_LED_MAP))

static BYTE XINPUT_LED_TO_PORT_MAP[] =
{
    0xFF,//XINPUT_LED_OFF
    0xFF,//XINPUT_LED_BLINK
    0x00,//XINPUT_LED_1_SWITCH_BLINK
    0x01,//XINPUT_LED_2_SWITCH_BLINK
    0x02,//XINPUT_LED_3_SWITCH_BLINK
    0x03,//XINPUT_LED_4_SWITCH_BLINK
    0x00,//XINPUT_LED_1
    0x01,//XINPUT_LED_2
    0x02,//XINPUT_LED_3
    0x03,//XINPUT_LED_4
    0xFF,//XINPUT_LED_CYCLE
    0xFF,//XINPUT_LED_FAST_BLINK
    0xFF,//XINPUT_LED_SLOW_BLINK
    0xFF,//XINPUT_LED_FLIPFLOP
    0xFF,//XINPUT_LED_ALLBLINK
    0x00,//UNUSED
};

}//namespace LEDState

}//namespace Protocol

/////////////////////////////////////////////////////
// XInputInternal namespace
/////////////////////////////////////////////////////
namespace XInputInternal {

/////////////////////////////////////////////////////
// XInputInternal::DeviceInfo namespace
/////////////////////////////////////////////////////
namespace DeviceInfo {

namespace Enabled {
    HRESULT GetState(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);
    HRESULT SetVibration(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);
}
namespace Disabled {
    HRESULT GetState(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);
    HRESULT SetVibration(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);
}

DeviceInfo_t* Create(HANDLE hDevice, LPCWSTR lpDevicePath);
void Destroy(DeviceInfo_t* pDevice);
void Recycle(DeviceInfo_t* pDevice);

BOOL IsDeviceInactive(DeviceInfo_t* pDevice);
BOOL IsDeviceInactiveOnBus(DeviceInfo_t* pDevice);
void OnEnableSettingChanged(BOOL bEnabled);
bool MinFillFromInterface(HANDLE hDevice, DeviceInfo_t* pDevice);

HRESULT GetKeystroke(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);
HRESULT PowerOffController(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);
HRESULT CancelGuideButtonWait(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);
HRESULT GetAudioDevice(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);
HRESULT GetBatteryInformation(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);
HRESULT GetCapabilities(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);
HRESULT GetBaseBusInformation(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);
HRESULT WaitForGuideButton(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);

static HRESULT(*g_pfnGetStateDispatcher)(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);
static HRESULT(*g_pfnSetVibrationDispatcher)(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);

}
}

/////////////////////////////////////////////////////
// DeviceEnum namespace
/////////////////////////////////////////////////////
namespace DeviceEnum {

HRESULT Initialize();
HRESULT Close();

HRESULT GetDeviceOnPort(DWORD dwUserIndex, DeviceInfo_t** ppDevice, bool rescan);

}

/////////////////////////////////////////////////////
// XInputInternal::DeviceInfo namespace
/////////////////////////////////////////////////////


/////////////////////////////////////////////////////
// DeviceList namespace
/////////////////////////////////////////////////////
namespace DeviceList {

constexpr size_t InitialDeviceListSize = XUSER_MAX_COUNT * 2;
constexpr size_t BusDeviceListSize = 16;

HRESULT Initialize();
HRESULT Close();

HRESULT SetDeviceOnPort(DWORD dwUserIndex, DeviceInfo_t* pDevice);
HRESULT GetDeviceOnPort(DWORD dwUserIndex, DeviceInfo_t** ppDevice);
void RemoveBusDevice(DWORD dwBusIndex);

HRESULT IsDevicePresent(DeviceInfo_t* pDevice);
BYTE IsBusDevicePresent(DeviceInfo_t* pDevice);

BOOL IsDeviceOnPort(DWORD dwUserIndex);

}

/////////////////////////////////////////////////////
// DriverComm namespace
/////////////////////////////////////////////////////
namespace DriverComm {

HRESULT Initialize();
HRESULT Close();
BOOL CheckForDriverHook(DWORD driverHook, LPVOID hookFunction);

HRESULT SendLEDState(DeviceInfo_t* pDevice, BYTE ledState);
HRESULT SendDeviceVibration(DeviceInfo_t* pDevice);
HRESULT GetDeviceInfoFromInterface(HANDLE hDevice, OutDeviceInfos_t* pDeviceInfos);
HRESULT GetLatestDeviceInfo(DeviceInfo_t* pDevice);
HRESULT GetCapabilities(DeviceInfo_t* pDevice, XINPUT_CAPABILITIES_EX* pCapabilitiesEx);
HRESULT GetBaseBusInformation(DeviceInfo_t* pDevice, XINPUT_BASE_BUS_INFORMATION* pBaseBusInformation);
HRESULT GetBatteryInformation(DeviceInfo_t* pDevice, BYTE DeviceType, XINPUT_BATTERY_INFORMATION* pBatteryInformation);
HRESULT GetAudioDeviceInformation(DeviceInfo_t* pDevice, XINPUT_AUDIO_INFORMATION* pAudioInformation);
HRESULT GetLEDState(DeviceInfo_t* pDevice, BYTE* ledState);
HRESULT PowerOffController(DeviceInfo_t* pDevice);
HRESULT WaitForGuideButton(HANDLE hDevice, DWORD dwUserIndex, XINPUT_LISTEN_STATE* pListenState);
HRESULT CancelGuideButtonWait(DeviceInfo_t* pDevice);

namespace SetupDiWrapper {

HANDLE CreateDeviceInfoList();

BOOL GetClassDevs(HANDLE hDev, const GUID* pGuid, LPCWSTR pDeviceID, DWORD Flags);
BOOL EnumDeviceInfo(HANDLE DeviceInfoSet, DWORD MemberIndex, PDO_DEVINFO_DATA DeviceInfoData);
BOOL EnumDeviceInterfaces(HANDLE hDev, PDO_DEVINFO_DATA DeviceInfoData, const GUID* InterfaceClassGuid, DWORD MemberIndex, PDO_DEVICE_INTERFACE_DATA DeviceInterfaceData);
BOOL GetDeviceInterfaceDetail(HANDLE DeviceInfoSet, PDO_DEVICE_INTERFACE_DATA DeviceInterfaceData, PDO_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData, DWORD DeviceInterfaceDetailDataSize, PDWORD RequiredSize);
BOOL DestroyDeviceInfoList(HANDLE hDeviceInfoList);
BOOL GetDeviceProperty(HANDLE DeviceInfoSet, PDO_DEVINFO_DATA DeviceInfoData, CONST DEVPROPKEY* PropertyKey, DEVPROPTYPE* PropertyType, LPVOID PropertyBuffer, DWORD PropertyBufferSize, PDWORD RequiredSize);

}

}

/////////////////////////////////////////////////////
// Utilities namespace
/////////////////////////////////////////////////////
namespace Utilities {

LPVOID MemAlloc(DWORD dwBytes);
void MemFree(LPVOID lpMem);

HANDLE OpenDevice(LPCWSTR DevicePath, DWORD dwFlagsAndAttributes);

HRESULT SafeCopyToUntrustedBuffer(void* pDst, const void* pSrc, DWORD size);
HRESULT SafeCopyFromUntrustedBuffer(void* pDst, const void* pSrc, DWORD size);

BOOL IsSettingSet(DWORD setting);
BOOL CheckForSettings(DWORD setting, LPVOID lpParam);

}

/////////////////////////////////////////////////////
// XInputCore namespace
/////////////////////////////////////////////////////
namespace XInputCore {

#ifndef OPENXINPUT_DISABLE_COM
static IInputHostClient* g_InputHostClient = nullptr;
#endif

static HMODULE g_hXInputUapDll = nullptr;
static INIT_ONCE g_InitOnce;

static decltype(OpenXInputGetState)             * g_pfnXInputGetState_Override              = nullptr;
static decltype(OpenXInputSetState)             * g_pfnXInputSetState_Override              = nullptr;
static decltype(OpenXInputGetCapabilities)      * g_pfnXInputGetCapabilities_Override       = nullptr;
static decltype(OpenXInputEnable)               * g_pfnXInputEnable_Override                = nullptr;
static decltype(OpenXInputGetAudioDeviceIds)    * g_pfnXInputGetAudioDeviceIds_Override     = nullptr;
static decltype(OpenXInputGetBatteryInformation)* g_pfnXInputGetBatteryInformation_Override = nullptr;
static decltype(OpenXInputGetKeystroke)         * g_pfnXInputGetKeystroke_Override          = nullptr;

BOOL Initialize();
BOOL Close(bool ContinueHost);

HRESULT Enter();
HRESULT Leave();
HRESULT ProcessAPIRequest(DWORD dwUserIndex, HRESULT(*pfn_ApiAction)(DeviceInfo_t*, void*, DWORD), void* pApiParam, DWORD reserved, BOOL bBusOnly);

void EnableCommunications(BOOL bEnabled);

}

/////////////////////////////////////////////////////
// QuickDriverEnum namespace
/////////////////////////////////////////////////////
class QuickDriverEnum
{
    HANDLE hDevInfo;
    DWORD  MemberIndex;
    PSP_DEVICE_INTERFACE_DETAIL_DATA_W DeviceInterfaceDetailData;
    DWORD  DeviceInterfaceDetailDataSize;

public:
    QuickDriverEnum()
    {
        hDevInfo = (HDEVINFO)INVALID_HANDLE_VALUE;
        MemberIndex = 0;
        DeviceInterfaceDetailData = nullptr;
        DeviceInterfaceDetailDataSize = 0;
    }

    ~QuickDriverEnum()
    {
        if (hDevInfo != INVALID_HANDLE_VALUE)
        {
            DriverComm::SetupDiWrapper::DestroyDeviceInfoList(hDevInfo);
            hDevInfo = INVALID_HANDLE_VALUE;
            MemberIndex = 0;
        }
        if (DeviceInterfaceDetailData != nullptr)
        {
            Utilities::MemFree(DeviceInterfaceDetailData);
            DeviceInterfaceDetailData = nullptr;
            DeviceInterfaceDetailDataSize = 0;
        }
    }

    bool Restart()
    {
        HANDLE hDev;

        if (hDevInfo != INVALID_HANDLE_VALUE)
        {
            DriverComm::SetupDiWrapper::DestroyDeviceInfoList(hDevInfo);
            hDevInfo = INVALID_HANDLE_VALUE;
        }
        MemberIndex = 0;
        if (DeviceInterfaceDetailData == nullptr)
        {
            DeviceInterfaceDetailDataSize = (sizeof(DWORD) + MAX_PATH * sizeof(WCHAR));
            DeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)Utilities::MemAlloc(DeviceInterfaceDetailDataSize);

            if (DeviceInterfaceDetailData == nullptr)
            {
                DeviceInterfaceDetailDataSize = 0;
                return false;
            }
        }

        hDev = DriverComm::SetupDiWrapper::CreateDeviceInfoList();

        if (hDev == nullptr || hDev == INVALID_HANDLE_VALUE)
            return false;

        hDevInfo = hDev;
        return DriverComm::SetupDiWrapper::GetClassDevs(hDevInfo, &Protocol::XUSB_INTERFACE_CLASS_GUID, nullptr, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    }

    bool GetNext(LPHANDLE phDevice)
    {
        SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
        DWORD RequiredSize;

        if (phDevice == nullptr)
            return false;

        *phDevice = INVALID_HANDLE_VALUE;
        if (hDevInfo == INVALID_HANDLE_VALUE || DeviceInterfaceDetailData == nullptr)
            return false;

        ZeroMemory(&DeviceInterfaceData, sizeof(SP_DEVICE_INTERFACE_DATA));
        DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

        if (DriverComm::SetupDiWrapper::EnumDeviceInterfaces(hDevInfo, nullptr, &Protocol::XUSB_INTERFACE_CLASS_GUID, MemberIndex, (PDO_DEVICE_INTERFACE_DATA)&DeviceInterfaceData) == FALSE)
            return false;

        ++MemberIndex;

        DeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
        RequiredSize = 0;
        if (DriverComm::SetupDiWrapper::GetDeviceInterfaceDetail(hDevInfo, (PDO_DEVICE_INTERFACE_DATA)&DeviceInterfaceData, (PDO_DEVICE_INTERFACE_DETAIL_DATA)DeviceInterfaceDetailData, DeviceInterfaceDetailDataSize, &RequiredSize) == FALSE)
        {
            if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
                return false;

            Utilities::MemFree(DeviceInterfaceDetailData);
            DeviceInterfaceDetailDataSize = RequiredSize;
            DeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)LocalAlloc(LMEM_ZEROINIT, DeviceInterfaceDetailDataSize);
            if (DeviceInterfaceDetailData == nullptr)
            {
                DeviceInterfaceDetailDataSize = 0;
                return false;
            }

            DeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
            if (DriverComm::SetupDiWrapper::GetDeviceInterfaceDetail(hDevInfo, (PDO_DEVICE_INTERFACE_DATA)&DeviceInterfaceData, (PDO_DEVICE_INTERFACE_DETAIL_DATA)DeviceInterfaceDetailData, DeviceInterfaceDetailDataSize, nullptr) == FALSE)
                return false;
        }

        *phDevice = Utilities::OpenDevice(DeviceInterfaceDetailData->DevicePath, FILE_ATTRIBUTE_NORMAL);
        return true;
    }
};

/////////////////////////////////////////////////////
// Controller namespace
/////////////////////////////////////////////////////
DWORD Controller_GetUserKeystroke(DeviceInfo_t* pDevice, BYTE unk1, DWORD reserved, XINPUT_KEYSTROKE* pKeystroke);
DWORD Controller_CalculateKeyFromThumbPos(SHORT base, SHORT X, SHORT Y);

/////////////////////////////////////////////////////
// Global functions
/////////////////////////////////////////////////////
HRESULT InitializeOverrides();
void CleanupOverrides();
void UnInitializeInputHost();

void EventWriteDllLoaded(HRESULT hr);
void EventWriteVidPid(WORD vendorId, WORD productId);

HRESULT RegisterUtcEventProvider();
void UnregisterUtcEventProvider();

/////////////////////////////////////////////////////
// Implementations
/////////////////////////////////////////////////////

/////////////////////////////////////////////////////
// Anonymous namespace
/////////////////////////////////////////////////////
namespace { 

DWORD XInputReturnCodeFromHRESULT(HRESULT hr)
{
    if (hr >= 0)
    {
        if (hr == 1)
            return ERROR_DEVICE_NOT_CONNECTED;

        return ERROR_SUCCESS;
    }

    if (hr == E_OUTOFMEMORY)
        return ERROR_OUTOFMEMORY;

    if (hr == E_INVALIDARG)
        return ERROR_BAD_ARGUMENTS;

    return hr;
}

HRESULT GrowList(DWORD newSize)
{
    DeviceInfo_t** ppDevice;

    ppDevice = (DeviceInfo_t**)Utilities::MemAlloc(newSize * sizeof(DeviceInfo_t*));
    if (!ppDevice)
        return E_OUTOFMEMORY;

    CopyMemory(ppDevice, g_pDeviceList, g_dwDeviceListSize * sizeof(DeviceInfo_t*));

    Utilities::MemFree(g_pDeviceList);
    g_pDeviceList = ppDevice;
    g_dwDeviceListSize = newSize;
    return S_OK;
}

void CopyGamepadStateToDeviceInfo(DeviceInfo_t* pDevice, GamepadState0100 *pGamepadState)
{
    pDevice->DeviceState.dwPacketNumber = pGamepadState->dwPacketNumber;
    pDevice->DeviceState.Gamepad.bLeftTrigger = pGamepadState->bLeftTrigger;
    pDevice->DeviceState.Gamepad.bRightTrigger = pGamepadState->bRightTrigger;
    pDevice->DeviceState.Gamepad.sThumbLX = pGamepadState->sThumbLX;
    pDevice->DeviceState.Gamepad.sThumbLY = pGamepadState->sThumbLY;
    pDevice->DeviceState.Gamepad.sThumbRX = pGamepadState->sThumbRX;
    pDevice->DeviceState.Gamepad.sThumbRY = pGamepadState->sThumbRY;
    pDevice->DeviceState.Gamepad.wButtons = pGamepadState->wButtons;
    if (pGamepadState->status == 1)
        pDevice->status |= DEVICE_STATUS_ACTIVE;
    else
        pDevice->status &= ~DEVICE_STATUS_ACTIVE;

    pDevice->inputId = pGamepadState->inputId;
}

void CopyGamepadStateToDeviceInfo(DeviceInfo_t* pDevice, GamepadState0101* pGamepadState)
{
    pDevice->DeviceState.dwPacketNumber = pGamepadState->dwPacketNumber;
    pDevice->DeviceState.Gamepad.bLeftTrigger = pGamepadState->bLeftTrigger;
    pDevice->DeviceState.Gamepad.bRightTrigger = pGamepadState->bRightTrigger;
    pDevice->DeviceState.Gamepad.sThumbLX = pGamepadState->sThumbLX;
    pDevice->DeviceState.Gamepad.sThumbLY = pGamepadState->sThumbLY;
    pDevice->DeviceState.Gamepad.sThumbRX = pGamepadState->sThumbRX;
    pDevice->DeviceState.Gamepad.sThumbRY = pGamepadState->sThumbRY;
    pDevice->DeviceState.Gamepad.wButtons = pGamepadState->wButtons;
    pDevice->DeviceState.Gamepad.wButtons &= XINPUT_BUTTON_MASK;

    if (pGamepadState->status == 1)
        pDevice->status |= DEVICE_STATUS_ACTIVE;
    else
        pDevice->status &= ~DEVICE_STATUS_ACTIVE;

    pDevice->inputId = pGamepadState->inputId;
}

void TranslateCapabilities(DeviceInfo_t* pDevice, GamepadCapabilities0101* pGamepadCapabilities, XINPUT_CAPABILITIES_EX* pCapabilities)
{
    pCapabilities->Capabilities.Type = (pGamepadCapabilities->Type != 0 ? XINPUT_DEVTYPE_GAMEPAD : 0);
    pCapabilities->Capabilities.SubType = pGamepadCapabilities->SubType;
    pCapabilities->Capabilities.Flags = XINPUT_CAPS_VOICE_SUPPORTED | XINPUT_CAPS_PMD_SUPPORTED;
    pCapabilities->Capabilities.Gamepad.wButtons = pGamepadCapabilities->wButtons;
    pCapabilities->Capabilities.Gamepad.bLeftTrigger = pGamepadCapabilities->bLeftTrigger;
    pCapabilities->Capabilities.Gamepad.bRightTrigger = pGamepadCapabilities->bRightTrigger;
    pCapabilities->Capabilities.Gamepad.sThumbLX = pGamepadCapabilities->sThumbLX;
    pCapabilities->Capabilities.Gamepad.sThumbLY = pGamepadCapabilities->sThumbLY;
    pCapabilities->Capabilities.Gamepad.sThumbRX = pGamepadCapabilities->sThumbRX;
    pCapabilities->Capabilities.Gamepad.sThumbRY = pGamepadCapabilities->sThumbRY;
    pCapabilities->Capabilities.Vibration.wLeftMotorSpeed = pGamepadCapabilities->bLeftMotorSpeed;
    pCapabilities->Capabilities.Vibration.wRightMotorSpeed = pGamepadCapabilities->bRightMotorSpeed;
    pCapabilities->Capabilities.Gamepad.wButtons &= XINPUT_BUTTON_MASK;
    if (pDevice->vendorId != 0x45E || pDevice->productId != 0x719)
    {
        pCapabilities->ProductId = pDevice->productId;
        pCapabilities->VendorId = pDevice->vendorId;
    }
    else
    {
        pCapabilities->Capabilities.Flags |= XINPUT_CAPS_WIRELESS;
        pCapabilities->ProductId = 0;
        pCapabilities->VendorId = 0;
    }
    pCapabilities->unk2 = 0;
    pCapabilities->unk0 = 0;
}

void TranslateCapabilities(GamepadCapabilities0102* pGamepadCapabilities, XINPUT_CAPABILITIES_EX* pCapabilitiesEx)
{
    pCapabilitiesEx->Capabilities.Type = pGamepadCapabilities->Type;
    pCapabilitiesEx->Capabilities.SubType = pGamepadCapabilities->SubType;
    pCapabilitiesEx->Capabilities.Gamepad.wButtons = pGamepadCapabilities->wButtons;
    pCapabilitiesEx->Capabilities.Gamepad.bLeftTrigger = pGamepadCapabilities->bLeftTrigger;
    pCapabilitiesEx->Capabilities.Gamepad.bRightTrigger = pGamepadCapabilities->bRightTrigger;
    pCapabilitiesEx->Capabilities.Gamepad.sThumbLX = pGamepadCapabilities->sThumbLX;
    pCapabilitiesEx->Capabilities.Gamepad.sThumbLY = pGamepadCapabilities->sThumbLY;
    pCapabilitiesEx->Capabilities.Gamepad.sThumbRX = pGamepadCapabilities->sThumbRX;
    pCapabilitiesEx->Capabilities.Gamepad.sThumbRY = pGamepadCapabilities->sThumbRY;
    pCapabilitiesEx->Capabilities.Vibration.wLeftMotorSpeed = pGamepadCapabilities->wLeftMotorSpeed;
    pCapabilitiesEx->Capabilities.Vibration.wRightMotorSpeed = pGamepadCapabilities->wRightMotorSpeed;
    pCapabilitiesEx->Capabilities.Gamepad.wButtons &= XINPUT_BUTTON_MASK;
    pCapabilitiesEx->Capabilities.Flags = 0;
    if (pGamepadCapabilities->Flags & 0x01)
        pCapabilitiesEx->Capabilities.Flags |= XINPUT_CAPS_FFB_SUPPORTED;

    if (pGamepadCapabilities->Flags & 0x02)
        pCapabilitiesEx->Capabilities.Flags |= XINPUT_CAPS_WIRELESS;

    if (pGamepadCapabilities->Flags & 0x04)
        pCapabilitiesEx->Capabilities.Flags |= XINPUT_CAPS_VOICE_SUPPORTED;

    if (pGamepadCapabilities->Flags & 0x08)
        pCapabilitiesEx->Capabilities.Flags |= XINPUT_CAPS_PMD_SUPPORTED;

    if (pGamepadCapabilities->Flags & 0x10)
        pCapabilitiesEx->Capabilities.Flags |= XINPUT_CAPS_NO_NAVIGATION;

    pCapabilitiesEx->VendorId = pGamepadCapabilities->vendorId;
    pCapabilitiesEx->ProductId = pGamepadCapabilities->productId;
    pCapabilitiesEx->unk0 = pGamepadCapabilities->unk2;
    pCapabilitiesEx->unk2 = pGamepadCapabilities->unk3;
}

HRESULT SendReceiveIoctl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPOVERLAPPED lpOverlapped)
{
    DWORD BytesReturned = 0;
    BOOL bRes = FALSE;

    if (g_pfnDeviceIoControl)
        bRes = g_pfnDeviceIoControl(hDevice, dwIoControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, &BytesReturned, lpOverlapped);
    else
        bRes = DeviceIoControl(hDevice, dwIoControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, &BytesReturned, lpOverlapped);

    if (bRes == TRUE)
        return S_OK;

    if (GetLastError() == ERROR_IO_PENDING && lpOverlapped)
        return E_PENDING;

    return E_FAIL;
}

HRESULT SendIoctl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize)
{
    return SendReceiveIoctl(hDevice, dwIoControlCode, lpInBuffer, nInBufferSize, nullptr, 0, nullptr);
}

HRESULT ReceiveIoctl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpOutBuffer, DWORD nOufBufferSize)
{
    return SendReceiveIoctl(hDevice, dwIoControlCode, nullptr, 0, lpOutBuffer, nOufBufferSize, nullptr);
}

HRESULT EnumerateXInputDevices()
{
    HANDLE hDevice;
    HANDLE hDeviceInfoList;
    DO_DEVICE_INTERFACE_DATA DevinfoData;
    PDO_DEVICE_INTERFACE_DETAIL_DATA DevIfaceDetailData;
    DWORD MemberIndex;

    hDeviceInfoList = DriverComm::SetupDiWrapper::CreateDeviceInfoList();
    if (hDeviceInfoList == nullptr || hDeviceInfoList == INVALID_HANDLE_VALUE)
        return E_FAIL;

    if (DriverComm::SetupDiWrapper::GetClassDevs(hDeviceInfoList, &Protocol::XUSB_INTERFACE_CLASS_GUID, nullptr, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT) == FALSE)
    {
        DriverComm::SetupDiWrapper::DestroyDeviceInfoList(hDeviceInfoList);
        return E_FAIL;
    }

    MemberIndex = 0;
    while (1)
    {
        ZeroMemory(&DevinfoData, sizeof(DO_DEVICE_INTERFACE_DATA));
        DevinfoData.cbSize = sizeof(DO_DEVICE_INTERFACE_DATA);
        if (DriverComm::SetupDiWrapper::EnumDeviceInterfaces(hDeviceInfoList, nullptr, &Protocol::XUSB_INTERFACE_CLASS_GUID, MemberIndex++, &DevinfoData) == FALSE)
            break;

        if (GetDeviceInterfaceDetail(hDeviceInfoList, &DevinfoData, &DevIfaceDetailData) >= 0)
        {
            hDevice = Utilities::OpenDevice(DevIfaceDetailData->DevicePath, FILE_ATTRIBUTE_NORMAL);
            if (hDevice != INVALID_HANDLE_VALUE)
            {
                EnumerateDevicesOnDeviceInterface(hDevice, DevIfaceDetailData->DevicePath);
                CloseHandle(hDevice);
            }
        }
    }

    if (hDeviceInfoList != INVALID_HANDLE_VALUE)
        DriverComm::SetupDiWrapper::DestroyDeviceInfoList(hDeviceInfoList);

    return S_OK;
}

HRESULT EnumerateDevicesOnDeviceInterface(HANDLE hDevice, LPCWSTR DevicePath)
{
    HRESULT hr;
    DeviceInfo_t* pDevice = nullptr;
    OutDeviceInfos_t deviceInfos = {};
    DWORD busIndex;

    hr = DriverComm::GetDeviceInfoFromInterface(hDevice, &deviceInfos);
    if (hr < 0)
        return hr;

    if (deviceInfos.unk2 & 0x80)
        return S_OK;

    pDevice = XInputInternal::DeviceInfo::Create(hDevice, DevicePath);
    if (pDevice == nullptr)
        return E_OUTOFMEMORY;

    pDevice->status |= DEVICE_STATUS_ACTIVE | DEVICE_STATUS_BUS_ACTIVE;
    pDevice->productId = deviceInfos.productId;
    pDevice->vendorId = deviceInfos.vendorId;
    pDevice->wType = deviceInfos.wType;

    busIndex = DeviceList::IsBusDevicePresent(pDevice);
    if (busIndex == 255)
    {
        busIndex = 0;
        while (g_pBusDeviceList[busIndex] != nullptr)
        {
            ++busIndex;
            if (busIndex >= DeviceList::BusDeviceListSize)
            {
                XInputInternal::DeviceInfo::Destroy(pDevice);
                return E_FAIL;
            }
        }
        g_pBusDeviceList[busIndex] = pDevice;
    }
    else
    {
        XInputInternal::DeviceInfo::Destroy(pDevice);
    }

    // It has been inserted in the bus list or destroyed.
    pDevice = nullptr;
    for (int i = 0; i < deviceInfos.deviceIndex; ++i)
    {
        if (pDevice != nullptr)
        {
            XInputInternal::DeviceInfo::Recycle(pDevice);
        }
        else
        {
            pDevice = XInputInternal::DeviceInfo::Create(hDevice, DevicePath);
            if (pDevice == nullptr)
                return E_OUTOFMEMORY;
        }

        pDevice->dwUserIndex = i;
        pDevice->productId = deviceInfos.productId;
        pDevice->vendorId = deviceInfos.vendorId;
        pDevice->wType = deviceInfos.wType;
        pDevice->dwBusIndex = busIndex;
        hr = DriverComm::GetLatestDeviceInfo(pDevice);
        if (hr >= 0)
        {
            hr = ProcessEnumeratedDevice(pDevice);

            if (hr == S_OK)
            {
                pDevice = nullptr;
            }
        }
    }

    if (pDevice)
    {
        XInputInternal::DeviceInfo::Destroy(pDevice);
        pDevice = nullptr;
    }

    return hr;
}

HRESULT ProcessEnumeratedDevice(DeviceInfo_t* pDevice)
{
    DWORD dwUserIndex = 255;
    HRESULT hr;
    BYTE LEDState;

    if (XInputInternal::DeviceInfo::IsDeviceInactive(pDevice))
        return 1;
    
    hr = DeviceList::IsDevicePresent(pDevice);
    if (hr < 0)
        return hr;

    if (hr == 0)
        return 1;

    LEDState = 0;
    if (DriverComm::GetLEDState(pDevice, &LEDState) >= 0 && LEDState < 15)
    {
        dwUserIndex = Protocol::LEDState::XINPUT_LED_TO_PORT_MAP[LEDState];
        if (dwUserIndex != 255)
        {
            hr = DeviceList::IsDeviceOnPort(dwUserIndex);
            if (hr < 0)
                return hr;

            if (hr != TRUE)
            {
                dwUserIndex = 255;
                DriverComm::SendLEDState(pDevice, Protocol::LEDState::XINPUT_LED_FLIPFLOP);
            }
        }
    }

    for (int i = 0; i < XUSER_MAX_COUNT && dwUserIndex == 255; ++i)
    {
        hr = DeviceList::IsDeviceOnPort(i);
        if (hr < 0)
            return hr;
        if (hr == TRUE)
            dwUserIndex = i;
    }

    if (dwUserIndex >= XUSER_MAX_COUNT)
        return 1;

    hr = DeviceList::SetDeviceOnPort(dwUserIndex, pDevice);
    if (hr < 0)
        return hr;

    if (Utilities::IsSettingSet(SET_USER_LED_ON_CREATE))
        DriverComm::SendLEDState(pDevice, Protocol::LEDState::XINPUT_PORT_TO_LED_MAP[dwUserIndex % MAX_XINPUT_PORT_TO_LED_MAP]);

    return S_OK;
}

HRESULT GetAudioInterfaceStringInfo(DeviceInfo_t* pDevice, WORD* VendorId, WORD* ProductId, BYTE* InputId)
{
    HRESULT hr;
    XINPUT_AUDIO_INFORMATION audioInformation;

    *VendorId = pDevice->vendorId;
    *ProductId = pDevice->productId;
    *InputId = static_cast<BYTE>(pDevice->inputId);
    if (pDevice->wType >= 0x102)
    {
        if (DriverComm::GetAudioDeviceInformation(pDevice, &audioInformation) >= 0)
        {
            *VendorId = audioInformation.vendorId;
            *ProductId = audioInformation.productId;
            *InputId = audioInformation.inputId;
            if (!(pDevice->inputId == 255))
                return S_OK;
        }
    }
    else
    {
        hr = DriverComm::GetLatestDeviceInfo(pDevice);
        if (hr < 0)
            return hr;

        if (!XInputInternal::DeviceInfo::IsDeviceInactive(pDevice))
        {
            *InputId = static_cast<BYTE>(pDevice->inputId);
            return S_OK;
        }
    }

    return E_FAIL;
}

HRESULT GetDeviceInterfaceDetail(HANDLE hDev, PDO_DEVICE_INTERFACE_DATA DeviceInterfaceData, PDO_DEVICE_INTERFACE_DETAIL_DATA* ppDeviceInterfaceDetailData)
{
    DWORD uBytes = g_dwDetailBufferSize;
    *ppDeviceInterfaceDetailData = (PDO_DEVICE_INTERFACE_DETAIL_DATA)g_pDetailBuffer;
    if (*ppDeviceInterfaceDetailData)
    {
        memset(*ppDeviceInterfaceDetailData, 0, g_dwDetailBufferSize);
        (*ppDeviceInterfaceDetailData)->cbSize = sizeof(DO_DEVICE_INTERFACE_DETAIL_DATA);
    }

    if (DriverComm::SetupDiWrapper::GetDeviceInterfaceDetail(hDev, DeviceInterfaceData, *ppDeviceInterfaceDetailData, g_dwDetailBufferSize, &uBytes) == TRUE)
        return S_OK;

    *ppDeviceInterfaceDetailData = nullptr;
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        return E_FAIL;

    if (g_pDetailBuffer)
    {
        Utilities::MemFree(g_pDetailBuffer);
        g_pDetailBuffer = 0;
        g_dwDetailBufferSize = 0;
    }

    g_pDetailBuffer = Utilities::MemAlloc(uBytes);
    if (g_pDetailBuffer == nullptr)
        return E_OUTOFMEMORY;

    g_dwDetailBufferSize = uBytes;

    *ppDeviceInterfaceDetailData = (PDO_DEVICE_INTERFACE_DETAIL_DATA)g_pDetailBuffer;
    (*ppDeviceInterfaceDetailData)->cbSize = sizeof(DO_DEVICE_INTERFACE_DETAIL_DATA);
    if (DriverComm::SetupDiWrapper::GetDeviceInterfaceDetail(hDev, DeviceInterfaceData, *ppDeviceInterfaceDetailData, g_dwDetailBufferSize, nullptr) == FALSE)
    {
        *ppDeviceInterfaceDetailData = nullptr;
        return E_FAIL;
    }

    return S_OK;
}

DWORD CALLBACK WaitForGuideButtonHelper(void* pParam)
{
    WaitForGuideButtonHelperApiParam_t* pApiParam = (WaitForGuideButtonHelperApiParam_t*)pParam;

    if (DriverComm::WaitForGuideButton(pApiParam->hGuideWait, pApiParam->dwUserIndex, pApiParam->pListenState) >= 0
        && pApiParam->hGuideEvent)
    {
        SetEvent(pApiParam->hGuideEvent);
    }

    Utilities::MemFree(pApiParam);
    return 0;
}

class QuickModule
{
    HMODULE hModule;

public:
    QuickModule(LPCWSTR lpLibFileName)
    {
        hModule = LoadLibraryW(lpLibFileName);
    }

    ~QuickModule()
    {
        if (hModule != nullptr)
        {
            FreeLibrary(hModule);
            hModule = nullptr;
        }
    }

    bool IsValid()
    {
        return hModule != nullptr;
    }

    void* GetFunctionPointer(LPCSTR lpProcName)
    {
        return GetProcAddress(hModule, lpProcName);
    }
};

/////////////////////////////////////////////////////
// DevQuery namespace
/////////////////////////////////////////////////////
namespace DevQuery {

DevQuery_t* Create()
{
    DevQuery_t* result = nullptr;
    
    result = (DevQuery_t*)Utilities::MemAlloc(sizeof(DevQuery_t));
    if (result == nullptr)
        return nullptr;

    result->hEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (result->hEvent == nullptr)
    {
        Utilities::MemFree(result);
        return nullptr;
    }

    result->DeviceInterfaceId = XINPUT_PKEY_SWD_DeviceInterfaceId;
    result->unk1 = nullptr;
    result->unk2 = nullptr;
    result->lpDeviceId = nullptr;
    result->lpuiDeviceIdCount = nullptr;

    return result;
}

void Destroy(DevQuery_t* pDevQuery)
{
    if (pDevQuery != nullptr)
    {
        if (pDevQuery->hEvent != nullptr)
        {
            CloseHandle(pDevQuery->hEvent);
            pDevQuery->hEvent = nullptr;
        }
        Utilities::MemFree(pDevQuery);
    }
}

HRESULT RenderQuery(DevQuery_t* pDeviceQuery, GUID DeviceGuid, int a3, LPWSTR lpDeviceId, UINT* lpuiDeviceIdCount)
{
    HRESULT result;
    HDEVQUERY hQuery;
    DEV_QUERY_PARAM DeviceQueryParam;

    DeviceQueryParam.unk1 = 0x100000;
    memset(&DeviceQueryParam.unk2, 0, sizeof(DeviceQueryParam.unk2));
    DeviceQueryParam.unk3 = (void*)2;
    DeviceQueryParam.DevPropKeyContainerId = XINPUT_DEVPKEY_Device_ContainerId;
    DeviceQueryParam.unk4 = 0;
    DeviceQueryParam.unk5 = 0;
    DeviceQueryParam.unk6 = 13;
    DeviceQueryParam.unk8 = &DeviceGuid;
    DeviceQueryParam.unk7 = 16;
    DeviceQueryParam.unk9 = (void*)2;
    DeviceQueryParam.DevPropKeyClassGuid = XINPUT_DEVPKEY_DeviceInterface_ClassGuid;
    DeviceQueryParam.unk10 = 0;
    DeviceQueryParam.unk11 = 0;
    DeviceQueryParam.unk12 = 13;
    DeviceQueryParam.unk13 = 16;
    DeviceQueryParam.pQueryGUID = &XINPUT_DEVINTERFACE_AUDIO_RENDER;
    DeviceQueryParam.unk14 = 0x200000;
    memset(&DeviceQueryParam.unk15, 0, sizeof(DeviceQueryParam.unk15));
    ResetEvent(pDeviceQuery->hEvent);
    pDeviceQuery->lpDeviceId = lpDeviceId;
    pDeviceQuery->lpuiDeviceIdCount = lpuiDeviceIdCount;
    result = DevCreateObjectQuery(
        1,
        1,
        1,
        &pDeviceQuery->DeviceInterfaceId,
        4,
        &DeviceQueryParam,
        &DevQueryCallbackProc,
        pDeviceQuery,
        &hQuery);
    if (result >= 0)
        result = WaitForSingleObject(pDeviceQuery->hEvent, 10000) != 0 ? E_FAIL : S_OK;
    if (hQuery)
        DevCloseObjectQuery(hQuery);

    pDeviceQuery->lpDeviceId = nullptr;
    pDeviceQuery->lpuiDeviceIdCount = nullptr;
    return result;
}

HRESULT CaptureQuery(DevQuery_t* pDeviceQuery, GUID DeviceGuid, int a3, LPWSTR lpDeviceId, UINT* lpuiDeviceIdCount)
{
    HRESULT result;
    HDEVQUERY hQuery;
    DEV_QUERY_PARAM DeviceQueryParam;

    DeviceQueryParam.unk1 = 0x100000;
    memset(&DeviceQueryParam.unk2, 0, sizeof(DeviceQueryParam.unk2));
    DeviceQueryParam.unk3 = (void*)2;
    DeviceQueryParam.DevPropKeyContainerId = XINPUT_DEVPKEY_Device_ContainerId;
    DeviceQueryParam.unk4 = 0;
    DeviceQueryParam.unk5 = 0;
    DeviceQueryParam.unk6 = 13;
    DeviceQueryParam.unk8 = &DeviceGuid;
    DeviceQueryParam.unk7 = 16;
    DeviceQueryParam.unk9 = (void*)2;
    DeviceQueryParam.DevPropKeyClassGuid = XINPUT_DEVPKEY_DeviceInterface_ClassGuid;
    DeviceQueryParam.unk10 = 0;
    DeviceQueryParam.unk11 = 0;
    DeviceQueryParam.unk12 = 13;
    DeviceQueryParam.unk13 = 16;
    DeviceQueryParam.pQueryGUID = &XINPUT_DEVINTERFACE_AUDIO_CAPTURE;
    DeviceQueryParam.unk14 = 0x200000;
    memset(&DeviceQueryParam.unk15, 0, sizeof(DeviceQueryParam.unk15));
    ResetEvent(pDeviceQuery->hEvent);
    pDeviceQuery->lpDeviceId = lpDeviceId;
    pDeviceQuery->lpuiDeviceIdCount = lpuiDeviceIdCount;
    result = DevCreateObjectQuery(
        1,
        1,
        1,
        &pDeviceQuery->DeviceInterfaceId,
        4,
        &DeviceQueryParam,
        &DevQueryCallbackProc,
        pDeviceQuery,
        &hQuery);
    if (result >= 0)
        result = WaitForSingleObject(pDeviceQuery->hEvent, 10000) != 0 ? E_FAIL : S_OK;
    if (hQuery)
        DevCloseObjectQuery(hQuery);

    pDeviceQuery->lpDeviceId = nullptr;
    pDeviceQuery->lpuiDeviceIdCount = nullptr;
    return result;
}

void CALLBACK DevQueryCallbackProc(HDEVQUERY hQuery, LPVOID _pUserParam, PDEV_QUERY_RESULT_ACTION_DATA pCallbackData)
{
    size_t Length;
    size_t CopySize;

    DevQuery_t* pUserParam = (DevQuery_t*)_pUserParam;

    if (pCallbackData == nullptr || pUserParam == nullptr)
        return;
    
    if (pCallbackData->bUnk1 == TRUE)
    {
        if (pUserParam->lpuiDeviceIdCount != nullptr)
        {
            LPCWSTR lpDeviceId = (LPCWSTR)pCallbackData->lpvUnk3;
            if (lpDeviceId != nullptr)
            {
                Length = wcslen(lpDeviceId) + 1;
                if (pUserParam->lpDeviceId)
                {
                    CopySize = *pUserParam->lpuiDeviceIdCount;
                    if (Length < *pUserParam->lpuiDeviceIdCount)
                        CopySize = Length;
                    memcpy(pUserParam->lpDeviceId, lpDeviceId, 2 * CopySize);
                }
                *pUserParam->lpuiDeviceIdCount = Length;
                pUserParam->lpuiDeviceIdCount = nullptr;
            }
        }
    }
    else if (pCallbackData->bUnk1 == FALSE)
    {
        SetEvent(pUserParam->hEvent);
    }
}

} // namespace DevQuery

/////////////////////////////////////////////////////
// DevQuery namespace
/////////////////////////////////////////////////////
namespace DeviceProperty {

LPVOID* CreateInstance(HANDLE hDev, SP_XINPUTINFO_DATA* unk1, const DEVPROPKEY* lpPropertyKey)
{
    DEVPROPTYPE PropType = {};
    DWORD RequiredSize;

    LPVOID* ppDeviceInstance = (LPVOID*)Utilities::MemAlloc(sizeof(LPVOID));
    if (ppDeviceInstance == nullptr)
        return nullptr;

    RequiredSize = 0;
    DriverComm::SetupDiWrapper::GetDeviceProperty(hDev, (PDO_DEVINFO_DATA)unk1, lpPropertyKey, &PropType, nullptr, 0, &RequiredSize);

    *ppDeviceInstance = (LPVOID)Utilities::MemAlloc(RequiredSize == 0 ? sizeof(WCHAR) : RequiredSize);
    if (*ppDeviceInstance == nullptr)
    {
        DeviceProperty::DestroyInstance(ppDeviceInstance);
        return nullptr;
    }

    if (DriverComm::SetupDiWrapper::GetDeviceProperty(hDev, (PDO_DEVINFO_DATA)unk1, lpPropertyKey, &PropType, *ppDeviceInstance, RequiredSize, nullptr) == FALSE)
    {
        DeviceProperty::DestroyInstance(ppDeviceInstance);
        return nullptr;
    }

    return ppDeviceInstance;
}

void DestroyInstance(LPVOID* ppInstance)
{
    Utilities::MemFree(*ppInstance);
    *ppInstance = nullptr;
    Utilities::MemFree(ppInstance);
}

} // namespace DeviceProperty

}

/////////////////////////////////////////////////////
// DeviceEnum namespace
/////////////////////////////////////////////////////
namespace DeviceEnum { 

HRESULT Initialize()
{
    g_pDetailBuffer = nullptr;
    g_dwDetailBufferSize = 0;
    return S_OK;
}

HRESULT Close()
{
    if (g_pDetailBuffer)
    {
        Utilities::MemFree(g_pDetailBuffer);
        g_pDetailBuffer = nullptr;
    }
    
    g_dwDetailBufferSize = 0;
    return S_OK;
}

HRESULT GetDeviceOnPort(DWORD dwUserIndex, DeviceInfo_t** ppDevice, bool rescan)
{
    HRESULT hr;

    hr = DeviceList::GetDeviceOnPort(dwUserIndex, ppDevice);
    while (*ppDevice == nullptr && rescan)
    {
        rescan = false;
        if ((hr = EnumerateXInputDevices()) < 0)
            break;

        if ((hr = DeviceList::GetDeviceOnPort(dwUserIndex, ppDevice)) < 0)
            break;
    }

    return hr;
}

}

/////////////////////////////////////////////////////
// XInputInternal::DeviceInfo namespace
/////////////////////////////////////////////////////
namespace XInputInternal { 
namespace DeviceInfo {
namespace Enabled {

HRESULT GetState(DeviceInfo_t* pDevice, void* pParams, DWORD reserved)
{
    HRESULT hr;
    GetStateApiParam_t* pApiParam = (GetStateApiParam_t*)pParams;

    if (IsDeviceInactive(pDevice))
        return E_FAIL;

    if ((hr = DriverComm::GetLatestDeviceInfo(pDevice)) < 0)
        return hr;

    return Utilities::SafeCopyToUntrustedBuffer(pApiParam->pState, &pDevice->DeviceState, sizeof(XINPUT_STATE));
}

HRESULT SetVibration(DeviceInfo_t* pDevice, void* pParams, DWORD reserved)
{
    HRESULT hr;
    SetStateApiParam_t* pApiParam = (SetStateApiParam_t*)pParams;

    if (IsDeviceInactive(pDevice))
        return E_FAIL;

    if (pApiParam->pVibration != nullptr)
    {
        if ((hr = Utilities::SafeCopyFromUntrustedBuffer(&pDevice->DeviceVibration, pApiParam->pVibration, sizeof(XINPUT_VIBRATION))) < 0)
            return hr;
    }
    return DriverComm::SendDeviceVibration(pDevice);
}

}

namespace Disabled {

HRESULT GetState(DeviceInfo_t* pDevice, void* pParams, DWORD reserved)
{
    static XINPUT_STATE DisabledXINPUT_STATE = {};
    GetStateApiParam_t* pApiParam = (GetStateApiParam_t*)pParams;

    if (IsDeviceInactive(pDevice))
        return E_FAIL;

    if (Utilities::SafeCopyToUntrustedBuffer(pApiParam->pState, &DisabledXINPUT_STATE, sizeof(XINPUT_STATE)) >= 0)
        pApiParam->pState->dwPacketNumber = pDevice->DeviceState.dwPacketNumber + 1;

    return S_OK;
}

HRESULT SetVibration(DeviceInfo_t* pDevice, void* pParams, DWORD reserved)
{
    DeviceInfo_t tmpDevice;
    SetStateApiParam_t* pApiParam = (SetStateApiParam_t*)pParams;

    if (IsDeviceInactive(pDevice))
        return E_FAIL;

    if (pApiParam->pVibration != nullptr)
    {
        return Utilities::SafeCopyToUntrustedBuffer(&pDevice->DeviceVibration, pApiParam->pVibration, sizeof(XINPUT_VIBRATION));
    }
    CopyMemory(&tmpDevice, pDevice, sizeof(DeviceInfo_t));
    tmpDevice.DeviceVibration.wLeftMotorSpeed = 0;
    tmpDevice.DeviceVibration.wRightMotorSpeed = 0;
    return DriverComm::SendDeviceVibration(&tmpDevice);
}

}

DeviceInfo_t* Create(HANDLE hDevice, LPCWSTR lpDevicePath)
{
    HANDLE hSourceProcessHandle;
    DeviceInfo_t* pDevice;

    pDevice = (DeviceInfo_t*)Utilities::MemAlloc(sizeof(DeviceInfo_t));
    if (pDevice != nullptr)
    {
        // Shouldn't be needed
        ZeroMemory(pDevice, sizeof(DeviceInfo_t));
        pDevice->hDevice = INVALID_HANDLE_VALUE;
        pDevice->hGuideWait = INVALID_HANDLE_VALUE;
        hSourceProcessHandle = GetCurrentProcess();
        if (DuplicateHandle(hSourceProcessHandle, hDevice, hSourceProcessHandle, &pDevice->hDevice, 0, FALSE, DUPLICATE_SAME_ACCESS))
        {
            pDevice->dwDevicePathSize = wcslen(lpDevicePath) + 1;
            pDevice->lpDevicePath = (LPWSTR)Utilities::MemAlloc((pDevice->dwDevicePathSize + 1) * sizeof(WCHAR));
            if (pDevice->lpDevicePath)
            {
                if (StringCchCopyW(pDevice->lpDevicePath, pDevice->dwDevicePathSize, lpDevicePath) >= 0)
                    return pDevice;
            }
        }
    }

    if (pDevice != nullptr)
    {
        if (pDevice->lpDevicePath)
            Utilities::MemFree(pDevice->lpDevicePath);

        if (pDevice->hDevice != INVALID_HANDLE_VALUE)
            CloseHandle(pDevice->hDevice);

        Utilities::MemFree(pDevice);
    }

    return nullptr;
}

void Destroy(DeviceInfo_t* pDevice)
{
    if (pDevice == nullptr)
        return;

    if (pDevice->hGuideWait != INVALID_HANDLE_VALUE)
        CloseHandle(pDevice->hGuideWait);

    if (pDevice->hDevice != INVALID_HANDLE_VALUE)
        CloseHandle(pDevice->hDevice);

    if (pDevice->lpDevicePath)
        Utilities::MemFree(pDevice->lpDevicePath);

    Utilities::MemFree(pDevice);
}

void Recycle(DeviceInfo_t* pDevice)
{
    if (pDevice == nullptr)
        return;

    HANDLE hDevice = pDevice->hDevice;
    LPWSTR lpDevicePath = pDevice->lpDevicePath;
    memset(pDevice, 0, sizeof(DeviceInfo_t));
    pDevice->hDevice = hDevice;
    pDevice->hGuideWait = INVALID_HANDLE_VALUE;
    pDevice->lpDevicePath = lpDevicePath;
    pDevice->dwBusIndex = 255;
}

BOOL IsDeviceInactive(DeviceInfo_t* pDevice)
{
    return (pDevice->status & DEVICE_STATUS_ACTIVE) == 0;
}

BOOL IsDeviceInactiveOnBus(DeviceInfo_t* pDevice)
{
    return (pDevice->status & DEVICE_STATUS_BUS_ACTIVE) == 0;
}

void OnEnableSettingChanged(BOOL bEnabled)
{
    if (bEnabled)
    {
        g_pfnGetStateDispatcher = Enabled::GetState;
        g_pfnSetVibrationDispatcher = Enabled::SetVibration;
    }
    else
    {
        g_pfnGetStateDispatcher = Disabled::GetState;
        g_pfnSetVibrationDispatcher = Disabled::SetVibration;
    }
}

bool MinFillFromInterface(HANDLE hDevice, DeviceInfo_t* pDevice)
{
    OutDeviceInfos_t outBuff;

    if (hDevice == INVALID_HANDLE_VALUE || pDevice == nullptr)
        return false;

    if (DriverComm::GetDeviceInfoFromInterface(hDevice, &outBuff) < 0 || (outBuff.unk2 & 0x80))
        return false;

    ZeroMemory(pDevice, sizeof(DeviceInfo_t));
    pDevice->hDevice = hDevice;
    pDevice->status = DEVICE_STATUS_ACTIVE;
    pDevice->hGuideWait = INVALID_HANDLE_VALUE;
    pDevice->wType = outBuff.wType;
    pDevice->vendorId = outBuff.vendorId;
    pDevice->productId = outBuff.productId;
    return true;
}

HRESULT GetKeystroke(DeviceInfo_t* pDevice, void* pParams, DWORD reserved)
{
    GetKeystrokeApiParam_t* pApiParam = (GetKeystrokeApiParam_t*)pParams;

    if (IsDeviceInactive(pDevice))
        return E_FAIL;

    *pApiParam->lpLastError = Controller_GetUserKeystroke(pDevice, 0, *pApiParam->lpReserved, pApiParam->pKeystroke);

    return S_OK;
}

HRESULT PowerOffController(DeviceInfo_t* pDevice, void* pParams, DWORD reserved)
{
    if (IsDeviceInactive(pDevice))
        return E_FAIL;
        
    return DriverComm::PowerOffController(pDevice);
}

HRESULT CancelGuideButtonWait(DeviceInfo_t* pDevice, void* pParams, DWORD reserved)
{
    return DriverComm::CancelGuideButtonWait(pDevice);
}

HRESULT GetAudioDevice(DeviceInfo_t* pDevice, void* pParams, DWORD reserved)
{
    HRESULT hr;
    GetAudioDeviceIdApiParam_t* pApiParam = (GetAudioDeviceIdApiParam_t*)pParams;
    WCHAR StrDeviceId[56];
    HANDLE hDev;
    WORD VendorId;
    WORD ProductId;
    BYTE InputId;
    DWORD MemberIndex;
    DevQuery::DevQuery_t* pDevQuery;
    SP_XINPUTINFO_DATA DevInfoData;
    GUID DeviceGuid;
    LPVOID* ppDeviceData;

    if (pApiParam->pRenderDeviceId != nullptr)
    {
        *pApiParam->pRenderDeviceId = L'\0';
    }
    else if (pApiParam->pRenderCount != nullptr)
    {
        *pApiParam->pRenderCount = 0;
    }
    if (pApiParam->pCaptureDeviceId != nullptr)
    {
        *pApiParam->pCaptureDeviceId = L'\0';
    }
    else if (pApiParam->pCaptureCount != nullptr)
    {
        *pApiParam->pCaptureCount = 0;
    }

    if (XInputInternal::DeviceInfo::IsDeviceInactive(pDevice))
        return E_FAIL;

    hr = GetAudioInterfaceStringInfo(pDevice, &VendorId, &ProductId, &InputId);
    if (hr < 0)
        return hr;

    if (InputId == 255)
        return S_OK;

    if (StringCchPrintfW(StrDeviceId, sizeof(StrDeviceId)/sizeof(*StrDeviceId), L"USB\\VID_%04X&PID_%04X&IA_%02X", VendorId, ProductId, InputId) < 0)
        return E_FAIL;

    hDev = DriverComm::SetupDiWrapper::CreateDeviceInfoList();
    if (hDev == nullptr || hDev == INVALID_HANDLE_VALUE)
        return E_FAIL;

    if (DriverComm::SetupDiWrapper::GetClassDevs(hDev, &XINPUT_GUID_DEVCLASS_MEDIA, StrDeviceId, DIGCF_PRESENT) == FALSE)
    {
        DriverComm::SetupDiWrapper::DestroyDeviceInfoList(hDev);
        return E_FAIL;
    }

    MemberIndex = 0;
    while (1)
    {
        DevInfoData.cbSize = sizeof(SP_XINPUTINFO_DATA);
        if (DriverComm::SetupDiWrapper::EnumDeviceInfo(hDev, MemberIndex++, (PDO_DEVINFO_DATA)&DevInfoData) == FALSE)
            break;

        ppDeviceData = DeviceProperty::CreateInstance(hDev, &DevInfoData, &XINPUT_DEVPKEY_Device_InstanceId);
        if (ppDeviceData != nullptr)
        {
            LPCWSTR pDevicePath = (LPCWSTR)*ppDeviceData;
            bool match;
            DWORD PathLength = wcslen(pDevicePath);
            match = (PathLength >= 27 && _wcsnicmp(pDevicePath, StrDeviceId, 27) == 0);
            DeviceProperty::DestroyInstance(ppDeviceData);

            if (match)
            {
                ppDeviceData = DeviceProperty::CreateInstance(hDev, &DevInfoData, &XINPUT_DEVPKEY_Device_ContainerId);
                if (ppDeviceData != nullptr)
                {
                    DeviceGuid = *(GUID*)*ppDeviceData;
                    DeviceProperty::DestroyInstance(ppDeviceData);
                    hr = S_OK;
                }
                else
                {
                    hr = E_FAIL;
                }
                break;
            }
        }
    }

    DriverComm::SetupDiWrapper::DestroyDeviceInfoList(hDev);
    pDevQuery = DevQuery::Create();
    if (pDevQuery == nullptr)
        hr = E_OUTOFMEMORY;

    if (pApiParam->pRenderCount != nullptr && hr >= 0)
    {
        hr = DevQuery::RenderQuery(pDevQuery, DeviceGuid, 0, pApiParam->pRenderDeviceId, pApiParam->pRenderCount);
    }
    if (pApiParam->pCaptureCount != nullptr && hr >= 0)
    {
        hr = DevQuery::CaptureQuery(pDevQuery, DeviceGuid, 0, pApiParam->pCaptureDeviceId, pApiParam->pCaptureCount);
    }

    DevQuery::Destroy(pDevQuery);

    return hr;
}

HRESULT GetBatteryInformation(DeviceInfo_t* pDevice, void* pParams, DWORD reserved)
{
    GetBatteryInformationApiParam_t* pApiParam = (GetBatteryInformationApiParam_t*)pParams;

    if (IsDeviceInactive(pDevice))
        return E_FAIL;

    return DriverComm::GetBatteryInformation(pDevice, *pApiParam->DeviceType, pApiParam->pBatteryInformation);
}

HRESULT GetCapabilities(DeviceInfo_t* pDevice, void* pParams, DWORD reserved)
{
    GetCapabilitiesApiParam_t* pApiParam = (GetCapabilitiesApiParam_t*)pParams;
    
    if (IsDeviceInactive(pDevice))
        return E_FAIL;

    return DriverComm::GetCapabilities(pDevice, pApiParam->pCapabilities);
}

HRESULT GetBaseBusInformation(DeviceInfo_t* pDevice, void* pParams, DWORD reserved)
{
    GetBaseBusInformationApiParam_t* pApiParam = (GetBaseBusInformationApiParam_t*)pParams;

    if (IsDeviceInactive(pDevice) || IsDeviceInactiveOnBus(pDevice))
        return E_FAIL;

    return DriverComm::GetBaseBusInformation(pDevice, pApiParam->pBaseBusInformation);
}

HRESULT WaitForGuideButton(DeviceInfo_t* pDevice, void* pParams, DWORD reserved)
{
    HRESULT hr;
    WaitGuideButtonApiParam_t* pApiParam = (WaitGuideButtonApiParam_t*)pParams;
    WaitForGuideButtonHelperApiParam_t* pThreadParameter;

    if (IsDeviceInactive(pDevice))
    {
        pApiParam->pListenState->Status = ERROR_DEVICE_NOT_CONNECTED;
        return E_FAIL;
    }

    if (pDevice->hGuideWait == INVALID_HANDLE_VALUE && 
        (pDevice->hGuideWait = CreateFileW(pDevice->lpDevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 0)) == INVALID_HANDLE_VALUE)
    {
        return E_FAIL;
    }

    pThreadParameter = (WaitForGuideButtonHelperApiParam_t*)Utilities::MemAlloc(sizeof(WaitForGuideButtonHelperApiParam_t));
    if(pThreadParameter == nullptr)
    {
        pApiParam->pListenState->Status = ERROR_OUTOFMEMORY;
        return E_OUTOFMEMORY;
    }

    pThreadParameter->hGuideWait = pDevice->hGuideWait;
    pThreadParameter->hGuideEvent = *pApiParam->lphEvent;
    pThreadParameter->dwUserIndex = pDevice->dwUserIndex;
    pThreadParameter->pListenState = pApiParam->pListenState;

    if (*pApiParam->lphEvent != nullptr)
    {// Async wait
        if (CreateThread(nullptr, 0, WaitForGuideButtonHelper, pThreadParameter, 0, nullptr) == nullptr)
        {
            Utilities::MemFree(pThreadParameter);
            pApiParam->pListenState->Status = GetLastError();
            hr = E_FAIL;
        }
        else
        {
            hr = S_OK;
        }
    }
    else
    {// Sync wait, free the library lock
        hr = XInputCore::Leave();
        if (hr < 0)
        {
            Utilities::MemFree(pThreadParameter);
        }
        else
        {
            WaitForGuideButtonHelper(pThreadParameter);
            hr = 1;
        }
    }

    return hr;
}

}//namespace DeviceInfo
}//namespace XInputInternal

/////////////////////////////////////////////////////
// DeviceList namespace
/////////////////////////////////////////////////////
namespace DeviceList {

HRESULT Initialize()
{
    g_dwDeviceListSize = 0;
    g_pDeviceList = (DeviceInfo_t**)Utilities::MemAlloc(InitialDeviceListSize * sizeof(DeviceInfo_t*));
    if (g_pDeviceList == nullptr)
        return E_FAIL;
    
    g_dwDeviceListSize = InitialDeviceListSize;
    g_pBusDeviceList = (DeviceInfo_t**)Utilities::MemAlloc(BusDeviceListSize * sizeof(DeviceInfo_t*));
    if (g_pBusDeviceList == nullptr)
    {
        g_dwDeviceListSize = 0;
        Utilities::MemFree(g_pDeviceList);
        g_pDeviceList = nullptr;
        return E_FAIL;
    }

    return S_OK;
}

HRESULT Close()
{
    if (g_dwDeviceListSize == 0 || g_pDeviceList == nullptr || g_pBusDeviceList == nullptr)
        return E_FAIL;

    for (int i = 0; i < BusDeviceListSize; ++i)
    {
        if (g_pBusDeviceList[i] != nullptr)
        {
            XInputInternal::DeviceInfo::Destroy(g_pBusDeviceList[i]);
            g_pBusDeviceList[i] = nullptr;
        }
    }

    for (DWORD i = 0; i < g_dwDeviceListSize; ++i)
    {
        DeviceInfo_t** ppDevice = &g_pDeviceList[i];
        if (*ppDevice == nullptr)
            continue;

        if (Utilities::IsSettingSet(DISABLE_USER_LED_ON_DESTROY) &&
            DriverComm::SendLEDState(*ppDevice, Protocol::LEDState::XINPUT_LED_OFF) < 0 &&
            // && dword_410C24 & 4 &&
            g_dwLogVerbosity >= 2
            )
        {
            // Log
        }
        if (Utilities::IsSettingSet(DISABLE_VIBRATION_ON_DESTROY))
        {
            (*ppDevice)->DeviceVibration = {};
            if (DriverComm::SendDeviceVibration(*ppDevice) < 0 &&
                //dword_410C24 & 4 &&
                g_dwLogVerbosity >= 2
                )
            {
                // Log
            }
        }

        XInputInternal::DeviceInfo::Destroy(*ppDevice);
        *ppDevice = nullptr;
    }

    Utilities::MemFree(g_pDeviceList);
    Utilities::MemFree(g_pBusDeviceList);
    g_pDeviceList = nullptr;
    g_dwDeviceListSize = 0;
    g_pBusDeviceList = nullptr;
    return S_OK;
}

HRESULT SetDeviceOnPort(DWORD dwUserIndex, DeviceInfo_t* pDevice)
{
    HRESULT hr;

    if (dwUserIndex >= g_dwDeviceListSize)
    {
        hr = GrowList(dwUserIndex + 1);
        if (hr < 0)
            return hr;
    }
    if (pDevice && g_pDeviceList[dwUserIndex] )
        return E_FAIL;

    g_pDeviceList[dwUserIndex] = pDevice;
    return S_OK;
}

HRESULT RemoveDeviceFromPort(DWORD dwUserIndex)
{
    if (dwUserIndex >= g_dwDeviceListSize)
        return S_OK;

    if (g_pDeviceList[dwUserIndex] != nullptr)
    {
        XInputInternal::DeviceInfo::Destroy(g_pDeviceList[dwUserIndex]);
        g_pDeviceList[dwUserIndex] = nullptr;
    }

    return S_OK;
}

HRESULT GetDeviceOnPort(DWORD dwUserIndex, DeviceInfo_t** ppDevice)
{
    *ppDevice = (dwUserIndex < g_dwDeviceListSize ? g_pDeviceList[dwUserIndex] : nullptr);
    return S_OK;
}

void RemoveBusDevice(DWORD dwBusIndex)
{
    DeviceInfo_t* pDevice;

    if (dwBusIndex > BusDeviceListSize)
        return;

    if (g_pBusDeviceList[dwBusIndex] == nullptr)
        return;

    if (g_dwDeviceListSize)
    {
        for (DWORD i = 0; i < g_dwDeviceListSize; ++i)
        {
            pDevice = g_pDeviceList[i];
            if (pDevice != nullptr && pDevice->dwBusIndex == dwBusIndex)
            {
                pDevice->status &= ~DEVICE_STATUS_ACTIVE;
                pDevice->dwBusIndex = 255;
            }
        }
    }

    XInputInternal::DeviceInfo::Destroy(g_pBusDeviceList[dwBusIndex]);
    g_pBusDeviceList[dwBusIndex] = nullptr;
}

HRESULT IsDevicePresent(DeviceInfo_t* pDevice)
{
    if (!XInputInternal::DeviceInfo::IsDeviceInactiveOnBus(pDevice))
        return E_NOTIMPL;

    if (g_dwDeviceListSize == 0)
        return 1;

    for (DWORD i = 0; i < g_dwDeviceListSize; ++i)
    {
        if (g_pDeviceList[i] != nullptr &&
            pDevice->dwUserIndex == g_pDeviceList[i]->dwUserIndex &&
            g_pDeviceList[i]->lpDevicePath != nullptr &&
            pDevice->lpDevicePath != nullptr &&
            wcscmp(g_pDeviceList[i]->lpDevicePath, pDevice->lpDevicePath) == 0)
        {
            return 0;
        }
    }

    return 1;
}

BYTE IsBusDevicePresent(DeviceInfo_t* pDevice)
{
    DeviceInfo_t* pBusDevice;

    if(XInputInternal::DeviceInfo::IsDeviceInactiveOnBus(pDevice))
        return 255;

    int i;
    for(i = 0; i < BusDeviceListSize; ++i)
    {
        pBusDevice = g_pBusDeviceList[i];
        if (pBusDevice)
        {
            if (pBusDevice->lpDevicePath != nullptr || pDevice->lpDevicePath != nullptr)
            {
                if (wcscmp(pBusDevice->lpDevicePath, pDevice->lpDevicePath) == 0)
                    break;
            }
        }
    }
    if (i >= BusDeviceListSize)
        return 255;

    return i;
}

BOOL IsDeviceOnPort(DWORD dwUserIndex)
{
    if (dwUserIndex < g_dwDeviceListSize )
        return g_pDeviceList[dwUserIndex] == nullptr;
    
    return TRUE;
}

}

/////////////////////////////////////////////////////
// DriverComm namespace
/////////////////////////////////////////////////////
namespace DriverComm {

HRESULT Initialize()
{
    g_pfnDeviceIoControl = nullptr;
    g_pfnGetClassDevs = nullptr;
    g_pfnEnumDeviceInfo = nullptr;
    g_pfnEnumDeviceInterfaces = nullptr;
    g_pfnGetDeviceInterfaceDetail = nullptr;
    g_pfnGetDeviceProperty = nullptr;
    g_pfnDestroyDeviceInfoList = nullptr;
    return S_OK;
}

HRESULT Close()
{
    return Initialize();
}

BOOL CheckForDriverHook(DWORD driverHook, LPVOID hookFunction)
{
    switch (driverHook)
    {
        case 0xBAAD0000:
            //if(dword_410C24 & 8 && g_dwLogVerbosity >= 4)
                // Log
            break;

        case 0xBAAD0001:
            //if(dword_410C24 & 8 && g_dwLogVerbosity >= 4)
                // Log
            g_pfnDeviceIoControl = (decltype(g_pfnDeviceIoControl))hookFunction;
            break;

        case 0xBAAD0002:
            //if(dword_410C24 & 8 && g_dwLogVerbosity >= 4)
                // Log
            g_pfnDeviceIoControl = nullptr;
            break;

        case 0xBAAD0003:
            //if(dword_410C24 & 8 && g_dwLogVerbosity >= 4)
                // Log
            g_pfnGetClassDevs = (decltype(g_pfnGetClassDevs))hookFunction;
            break;

        case 0xBAAD0004:
            //if(dword_410C24 & 8 && g_dwLogVerbosity >= 4)
                // Log
            g_pfnGetClassDevs = nullptr;
            break;

        case 0xBAAD0005:
            //if(dword_410C24 & 8 && g_dwLogVerbosity >= 4)
                // Log
            g_pfnEnumDeviceInterfaces = (decltype(g_pfnEnumDeviceInterfaces))hookFunction;
            break;

        case 0xBAAD0006:
            //if(dword_410C24 & 8 && g_dwLogVerbosity >= 4)
                // Log
            g_pfnEnumDeviceInterfaces = nullptr;
            break;

        case 0xBAAD0007:
            //if(dword_410C24 & 8 && g_dwLogVerbosity >= 4)
                // Log
            g_pfnGetDeviceInterfaceDetail = (decltype(g_pfnGetDeviceInterfaceDetail))hookFunction;
            break;

        case 0xBAAD0008:
            //if(dword_410C24 & 8 && g_dwLogVerbosity >= 4)
                // Log
            g_pfnGetDeviceInterfaceDetail = nullptr;
            break;

        case 0xBAAD0009:
            //if(dword_410C24 & 8 && g_dwLogVerbosity >= 4)
                // Log
            g_pfnDestroyDeviceInfoList = (decltype(g_pfnDestroyDeviceInfoList))hookFunction;
            break;

        case 0xBAAD000A:
            //if(dword_410C24 & 8 && g_dwLogVerbosity >= 4)
                // Log
            g_pfnDestroyDeviceInfoList = nullptr;
            break;

        case 0xBAAD000B:
            //if(dword_410C24 & 8 && g_dwLogVerbosity >= 4)
                // Log
            g_pfnCreateDeviceInfoList = (decltype(g_pfnCreateDeviceInfoList))hookFunction;;
            break;

        case 0xBAAD000C:
            //if(dword_410C24 & 8 && g_dwLogVerbosity >= 4)
                // Log
            g_pfnCreateDeviceInfoList = nullptr;
            break;

        case 0xBAAD000D:
            //if(dword_410C24 & 8 && g_dwLogVerbosity >= 4)
                // Log
            g_pfnEnumDeviceInfo = (decltype(g_pfnEnumDeviceInfo))hookFunction;;
            break;

        case 0xBAAD000E:
            //if(dword_410C24 & 8 && g_dwLogVerbosity >= 4)
                // Log
            g_pfnEnumDeviceInfo = nullptr;
            break;

        case 0xBAAD000F:
            //if(dword_410C24 & 8 && g_dwLogVerbosity >= 4)
                // Log
            g_pfnGetDeviceProperty = (decltype(g_pfnGetDeviceProperty))hookFunction;;
            break;

        case 0xBAAD0010:
            //if(dword_410C24 & 8 && g_dwLogVerbosity >= 4)
                // Log
            g_pfnGetDeviceProperty = nullptr;
            break;

    }

    return TRUE;
}

HRESULT SendLEDState(DeviceInfo_t* pDevice, BYTE ledState)
{
    InSetLEDBuffer_t inBuffer = {
        pDevice->dwUserIndex,
        ledState,
        0,
        0,
        1
    };

    return SendIoctl(pDevice->hDevice, Protocol::IOCTL_XINPUT_SET_GAMEPAD_STATE, &inBuffer, sizeof(InSetLEDBuffer_t));
}

HRESULT SendDeviceVibration(DeviceInfo_t* pDevice)
{
    InVibrationBuffer_t inBuffer = {
        pDevice->dwUserIndex,
        0,
        (BYTE)(pDevice->DeviceVibration.wLeftMotorSpeed / 256),
        (BYTE)(pDevice->DeviceVibration.wRightMotorSpeed / 256),
        2
    };

    return SendIoctl(pDevice->hDevice, Protocol::IOCTL_XINPUT_SET_GAMEPAD_STATE, &inBuffer, sizeof(InVibrationBuffer_t));
}

HRESULT GetDeviceInfoFromInterface(HANDLE hDevice, OutDeviceInfos_t* pDeviceInfos)
{
    HRESULT hr;

    hr = ReceiveIoctl(hDevice, Protocol::IOCTL_XINPUT_GET_INFORMATION, pDeviceInfos, sizeof(OutDeviceInfos_t));
    if (hr >= 0)
        hr = S_OK;

    return hr;
}

HRESULT GetLatestDeviceInfo(DeviceInfo_t* pDevice)
{
    HRESULT hr;

    union {
        InGamepadState0100 in0100;
        InGamepadState0101 in0101;
    } inBuffer = {};

    DWORD inSize;

    union {
        GamepadState0100 out0100;
        GamepadState0101 out0101;
    } outBuffer = {};

    DWORD outSize;

    if (pDevice->wType == 0x0100)
    {
        inBuffer.in0100.DeviceIndex = pDevice->dwUserIndex;
        inSize = sizeof(inBuffer.in0100);

        outSize = sizeof(outBuffer.out0100);
    }
    else
    {
        inBuffer.in0101.wType = 0x0101;
        inBuffer.in0101.DeviceIndex = pDevice->dwUserIndex;
        inSize = sizeof(inBuffer.in0101);

        outSize = sizeof(outBuffer.out0101);
    }

    hr = SendReceiveIoctl(pDevice->hDevice, Protocol::IOCTL_XINPUT_GET_GAMEPAD_STATE, &inBuffer, inSize, &outBuffer, outSize, nullptr);
    if (hr < 0)
        return hr;

    if (pDevice->wType == 0x0100)
    {
        CopyGamepadStateToDeviceInfo(pDevice, &outBuffer.out0100);
    }
    else
    {
        CopyGamepadStateToDeviceInfo(pDevice, &outBuffer.out0101);
    }

    return S_OK;
}

HRESULT GetCapabilities(DeviceInfo_t* pDevice, XINPUT_CAPABILITIES_EX* pCapabilitiesEx)
{
    static XINPUT_CAPABILITIES s_GamepadCapabilities = {
        XINPUT_DEVTYPE_GAMEPAD            , // Type
        XINPUT_DEVSUBTYPE_GAMEPAD         , // SubType
        XINPUT_CAPS_VOICE_SUPPORTED       , // Flags
        {// Gamepad
          XINPUT_BUTTON_MASK_WITHOUT_GUIDE, // Gamepad.wButtons
          1                               , // Gamepad.bLeftTrigger
          1                               , // Gamepad.bRightTrigger
          1                               , // Gamepad.sThumbLX
          1                               , // Gamepad.sThumbLY
          1                               , // Gamepad.sThumbRX
          1                               , // Gamepad.sThumbRY
        },
        {// Vibration
          1                               , // Vibration.wLeftMotorSpeed
          1                               , // Vibration.wRightMotorSpeed
        }
    };
    HRESULT hr;

    if (pDevice->wType == 0x0100)
    {
        hr = Utilities::SafeCopyToUntrustedBuffer(&pCapabilitiesEx->Capabilities, &s_GamepadCapabilities, sizeof(XINPUT_CAPABILITIES));
        pCapabilitiesEx->VendorId = pDevice->vendorId;
        pCapabilitiesEx->ProductId = pDevice->productId;
        pCapabilitiesEx->unk2 = 0;
        pCapabilitiesEx->unk0 = 0;
    }
    else if (pDevice->wType == 0x0101)
    {
        InGamepadCapabilities0101 InBuffer;
        GamepadCapabilities0101 OutBuffer;

        InBuffer.wType = 0x0101;
        InBuffer.DeviceIndex = pDevice->dwUserIndex;
        ZeroMemory(&OutBuffer, sizeof(GamepadCapabilities0101));

        hr = SendReceiveIoctl(pDevice->hDevice, Protocol::IOCTL_XINPUT_GET_CAPABILITIES, &InBuffer, sizeof(InGamepadCapabilities0101), &OutBuffer, sizeof(GamepadCapabilities0101), nullptr);
        if (hr >= 0)
            TranslateCapabilities(pDevice, &OutBuffer, pCapabilitiesEx);
    }
    else
    {
        InGamepadCapabilities0101 InBuffer;
        GamepadCapabilities0102 OutBuffer = {};

        InBuffer.wType = 0x102;
        InBuffer.DeviceIndex = pDevice->dwUserIndex;
        hr = SendReceiveIoctl(pDevice->hDevice, Protocol::IOCTL_XINPUT_GET_CAPABILITIES, &InBuffer, sizeof(InGamepadCapabilities0101), &OutBuffer, sizeof(GamepadCapabilities0102), 0);
        if (hr >= 0)
            TranslateCapabilities(&OutBuffer, pCapabilitiesEx);
    }
    

    return hr;
}

HRESULT GetBaseBusInformation(DeviceInfo_t* pDevice, XINPUT_BASE_BUS_INFORMATION* pBaseBusInformation)
{
    HRESULT hr;
    InBaseBusInformation InBuffer;
    OutBaseBusInformation OutBuffer;

    ZeroMemory(pBaseBusInformation, sizeof(XINPUT_BASE_BUS_INFORMATION));
    pBaseBusInformation->vendorId = pDevice->vendorId;
    pBaseBusInformation->productId = pDevice->productId;

    if (pDevice->wType < 0x0103)
        return S_OK;

    ZeroMemory(&InBuffer, sizeof(InBaseBusInformation));
    ZeroMemory(&OutBuffer, sizeof(OutBaseBusInformation));

    hr = SendReceiveIoctl(pDevice->hDevice, Protocol::IOCTL_XINPUT_GET_BASE_BUS_INFORMATION, &InBuffer, sizeof(InBaseBusInformation), &OutBuffer, sizeof(OutBaseBusInformation), nullptr);
    if (hr < 0)
        return hr;

    if (OutBuffer.field_2 != 0)
        return E_NOTIMPL;

    pBaseBusInformation->vendorId = OutBuffer.vendorId;
    pBaseBusInformation->productId = OutBuffer.productId;
    pBaseBusInformation->inputId = OutBuffer.inputId;

    if (OutBuffer.field_1D != 0)
    {
        pBaseBusInformation->field_8 = OutBuffer.field_21 | ((OutBuffer.field_20 | ((OutBuffer.field_1F | (OutBuffer.field_1E << 8)) << 8)) << 8);
        pBaseBusInformation->field_C = OutBuffer.field_22;
        pBaseBusInformation->field_D = OutBuffer.field_23;
        pBaseBusInformation->field_E = static_cast<BYTE>(OutBuffer.field_24);
    }

    return hr;
}

HRESULT GetBatteryInformation(DeviceInfo_t* pDevice, BYTE DeviceType, XINPUT_BATTERY_INFORMATION* pBatteryInformation)
{
    static const GUID null_guid = {};

    HRESULT hr;
    InGamepadBatteryInformation0102 InBuffer;
    GamepadBatteryInformation0102 OutBuffer;
    XINPUT_BATTERY_INFORMATION BatteryInformation;
    GetAudioDeviceIdApiParam_t apiParam;
    UINT RenderCount;
    int v7;

    if (pDevice->wType >= 0x102u)
    {
        InBuffer.wType = 0x102;
        InBuffer.DeviceIndex = pDevice->dwUserIndex;
        InBuffer.DeviceType = DeviceType;
        
        ZeroMemory(&OutBuffer, sizeof(GamepadBatteryInformation0102));

        hr = SendReceiveIoctl(pDevice->hDevice, Protocol::IOCTL_XINPUT_GET_BATTERY_INFORMATION, &InBuffer, sizeof(InGamepadBatteryInformation0102), &OutBuffer, sizeof(GamepadBatteryInformation0102), nullptr);
        if (hr >= 0)
        {
            hr = Utilities::SafeCopyToUntrustedBuffer(pBatteryInformation, &OutBuffer.BatteryType, sizeof(XINPUT_BATTERY_INFORMATION));
        }
    }
    else
    {
        BatteryInformation.BatteryType = BATTERY_TYPE_WIRED;
        BatteryInformation.BatteryLevel = BATTERY_LEVEL_FULL;

        if (DeviceType == XINPUT_DEVTYPE_GAMEPAD)
        {
            v7 = 0;
            apiParam.pRenderDeviceId = nullptr;
            apiParam.pRenderCount = &RenderCount;
            apiParam.pCaptureDeviceId = nullptr;
            apiParam.pCaptureCount = nullptr;

            RenderCount = 0;
            if (XInputInternal::DeviceInfo::GetAudioDevice(pDevice, &apiParam, 4) < 0 || RenderCount == 0)
            {
                BatteryInformation.BatteryType = BATTERY_TYPE_DISCONNECTED;
                BatteryInformation.BatteryLevel = BATTERY_LEVEL_EMPTY;
            }
        }
        hr = Utilities::SafeCopyToUntrustedBuffer(pBatteryInformation, &BatteryInformation, sizeof(XINPUT_BATTERY_INFORMATION));
    }
    return hr;
}

HRESULT GetAudioDeviceInformation(DeviceInfo_t* pDevice, XINPUT_AUDIO_INFORMATION* pAudioInformation)
{
    HRESULT hr;
    InGetAudioDeviceInformation_t InBuffer;
    OutGetAudioDeviceInformation_t OutBuffer;

    if (pDevice->wType < 0x0102)
        return E_FAIL;

    InBuffer.wType = 0x102;
    InBuffer.DeviceIndex = pDevice->dwUserIndex;
    hr = SendReceiveIoctl(pDevice->hDevice, Protocol::IOCTL_XINPUT_GET_AUDIO_INFORMATION, &InBuffer, sizeof(InGetAudioDeviceInformation_t), &OutBuffer, sizeof(OutGetAudioDeviceInformation_t), nullptr);
    if (hr >= 0)
    {
        pAudioInformation->vendorId = OutBuffer.vendorId;
        pAudioInformation->productId = OutBuffer.productId;
        pAudioInformation->inputId = OutBuffer.inputId;
    }
    return hr;
}

HRESULT GetLEDState(DeviceInfo_t* pDevice, BYTE* ledState)
{
    InGetLEDBuffer_t inBuffer;
    OutGetLEDBuffer_t outBuffer;
    HRESULT hr;

    *ledState = Protocol::LEDState::XINPUT_LED_OFF;
    if (pDevice->wType < 0x0101)
        return S_OK;

    inBuffer.wType = 0x0101;
    inBuffer.DeviceIndex = pDevice->dwUserIndex;
    hr = SendReceiveIoctl(pDevice->hDevice, Protocol::IOCTL_XINPUT_GET_LED_STATE, &inBuffer, sizeof(InGetLEDBuffer_t), &outBuffer, sizeof(OutGetLEDBuffer_t), nullptr);
    if (hr >= 0)
    {
        *ledState = outBuffer.LEDState;
    }

    return hr;
}

HRESULT PowerOffController(DeviceInfo_t* pDevice)
{
    InPowerOffBuffer_t inBuff;

    if (pDevice->wType < 0x0102)
        return E_FAIL;

    inBuff.wType = 0x0102;
    inBuff.DeviceIndex = pDevice->dwUserIndex;
    return SendIoctl(pDevice->hDevice, Protocol::IOCTL_XINPUT_POWER_DOWN_DEVICE, &inBuff, sizeof(InPowerOffBuffer_t));
}

HRESULT WaitForGuideButton(HANDLE hDevice, DWORD dwUserIndex, XINPUT_LISTEN_STATE* pListenState)
{
    HRESULT hr;
    HANDLE hEvent;
    OVERLAPPED overlapped;
    DWORD receivedBytes;
    InWaitForGuideButtonBuffer_t inBuffer;
    OutWaitForGuideButtonBuffer_t outBuffer;

    hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (hEvent == nullptr)
        return E_OUTOFMEMORY;

    overlapped.Internal = 0;
    overlapped.InternalHigh = 0;
    overlapped.Offset = 0;
    overlapped.OffsetHigh = 0;
    overlapped.hEvent = hEvent;

    inBuffer.wType = 0x0102;
    inBuffer.DeviceIndex = (BYTE)dwUserIndex;

    memset(&outBuffer, 0, sizeof(OutWaitForGuideButtonBuffer_t));

    hr = SendReceiveIoctl(hDevice, Protocol::IOCTL_XINPUT_WAIT_FOR_GUIDE_BUTTON, &inBuffer, sizeof(InWaitForGuideButtonBuffer_t), &outBuffer, sizeof(OutWaitForGuideButtonBuffer_t), &overlapped);
    if (hr < 0 && hr != E_PENDING)
    {
        CloseHandle(hEvent);
        pListenState->Status = GetLastError();
        return hr;
    }

    receivedBytes = 0;
    pListenState->Status = ERROR_IO_PENDING;
    if (GetOverlappedResult(hDevice, &overlapped, &receivedBytes, TRUE) != FALSE)
    {
        if (receivedBytes == sizeof(OutWaitForGuideButtonBuffer_t))
        {
            if (outBuffer.status == 1)
            {
                pListenState->Status = 0;
                pListenState->unk1 = outBuffer.field_5;
                pListenState->unk2 = outBuffer.field_B;
                pListenState->unk3 = outBuffer.field_D;
                pListenState->unk4 = outBuffer.field_E;
                pListenState->unk5 = outBuffer.field_F;
                pListenState->unk6 = outBuffer.field_11;
                pListenState->unk7 = outBuffer.field_13;
                pListenState->unk8 = outBuffer.field_15;
            }
            else
            {
                pListenState->Status = ERROR_DEVICE_NOT_CONNECTED;
            }
        }
        else
        {
            pListenState->Status = ERROR_CANCELLED;
        }
    }
    else if (GetLastError() == ERROR_OPERATION_ABORTED)
    {
        pListenState->Status = ERROR_CANCELLED;
    }
    else
    {
        pListenState->Status = GetLastError();
    }

    CloseHandle(hEvent);
    return S_OK;
}

HRESULT CancelGuideButtonWait(DeviceInfo_t* pDevice)
{
    if (pDevice->hGuideWait != INVALID_HANDLE_VALUE)
    {
        CloseHandle(pDevice->hGuideWait);
        pDevice->hGuideWait = INVALID_HANDLE_VALUE;
    }

    return S_OK;
}

namespace SetupDiWrapper {

HANDLE CreateDeviceInfoList()
{
    if (g_pfnCreateDeviceInfoList != nullptr)
        return g_pfnCreateDeviceInfoList(nullptr, nullptr, nullptr, nullptr, nullptr);

    return DevObjCreateDeviceInfoList(nullptr, nullptr, nullptr, nullptr, nullptr);
}

BOOL GetClassDevs(HANDLE hDev, const GUID* pGuid, LPCWSTR pDeviceID, DWORD Flags)
{
    if (g_pfnGetClassDevs)
        return g_pfnGetClassDevs(hDev, pGuid, pDeviceID, Flags, nullptr, 0);

    return DevObjGetClassDevs(hDev, pGuid, pDeviceID, Flags, nullptr, 0);
}

BOOL EnumDeviceInfo(HANDLE DeviceInfoSet, DWORD MemberIndex, PDO_DEVINFO_DATA DeviceInfoData)
{
    if (g_pfnEnumDeviceInfo)
        return g_pfnEnumDeviceInfo(DeviceInfoSet, MemberIndex, DeviceInfoData);

    return DevObjEnumDeviceInfo(DeviceInfoSet, MemberIndex, DeviceInfoData);
}

BOOL EnumDeviceInterfaces(HANDLE hDev, PDO_DEVINFO_DATA DeviceInfoData, const GUID* InterfaceClassGuid, DWORD MemberIndex, PDO_DEVICE_INTERFACE_DATA DeviceInterfaceData)
{
    if (g_pfnEnumDeviceInterfaces)
        return g_pfnEnumDeviceInterfaces(hDev, nullptr, &Protocol::XUSB_INTERFACE_CLASS_GUID, MemberIndex, DeviceInterfaceData);

    return DevObjEnumDeviceInterfaces(hDev, nullptr, &Protocol::XUSB_INTERFACE_CLASS_GUID, MemberIndex, DeviceInterfaceData);
}

BOOL GetDeviceInterfaceDetail(HANDLE hDev, PDO_DEVICE_INTERFACE_DATA DeviceInterfaceData, PDO_DEVICE_INTERFACE_DETAIL_DATA DeviceInterfaceDetailData, DWORD DeviceInterfaceDetailDataSize, PDWORD RequiredSize)
{
    if (g_pfnGetDeviceInterfaceDetail)
        return g_pfnGetDeviceInterfaceDetail(hDev, DeviceInterfaceData, DeviceInterfaceDetailData, DeviceInterfaceDetailDataSize, RequiredSize, nullptr);

    return DevObjGetDeviceInterfaceDetail(hDev, DeviceInterfaceData, DeviceInterfaceDetailData, DeviceInterfaceDetailDataSize, RequiredSize, nullptr);
}

BOOL DestroyDeviceInfoList(HANDLE hDeviceInfoList)
{
    if (g_pfnDestroyDeviceInfoList )
        return g_pfnDestroyDeviceInfoList(hDeviceInfoList);
    
    return DevObjDestroyDeviceInfoList(hDeviceInfoList);
}

BOOL GetDeviceProperty(HANDLE DeviceInfoSet, PDO_DEVINFO_DATA DeviceInfoData, CONST DEVPROPKEY* PropertyKey, DEVPROPTYPE* PropertyType, LPVOID PropertyBuffer, DWORD PropertyBufferSize, PDWORD RequiredSize)
{
    if (g_pfnGetDeviceProperty)
        return g_pfnGetDeviceProperty(DeviceInfoSet, DeviceInfoData, PropertyKey, PropertyType, PropertyBuffer, PropertyBufferSize, RequiredSize, 0);

    return DevObjGetDeviceProperty(DeviceInfoSet, DeviceInfoData, PropertyKey, PropertyType, PropertyBuffer, PropertyBufferSize, RequiredSize, 0);
}

}//namespace SetupDiWrapper

}

/////////////////////////////////////////////////////
// Utilities namespace
/////////////////////////////////////////////////////
namespace Utilities {

LPVOID MemAlloc(DWORD dwBytes)
{
    return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytes);
}

void MemFree(LPVOID lpMem)
{
    if (lpMem != nullptr)
    {
        HeapFree(GetProcessHeap(), 0, lpMem);
    }
}

HANDLE OpenDevice(LPCWSTR DevicePath, DWORD dwFlagsAndAttributes)
{
    HANDLE hDevice;

#ifndef OPENXINPUT_DISABLE_COM
    IXInputDevice* pComXInput;
    if (CoCreateInstance(CLSID_DeviceBroker, nullptr, CLSCTX_INPROC_SERVER, GUID_8604b268_34a6_4b1a_a59f_cdbd8379fd98, (LPVOID*)&pComXInput) >= 0)
    {
        pComXInput->CreateFileW(DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, dwFlagsAndAttributes, &hDevice);
        pComXInput->Release();
    }
    else
#endif
    {
        hDevice = CreateFileW(DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, dwFlagsAndAttributes, nullptr);
    }

    return hDevice;
}

HRESULT SafeCopyToUntrustedBuffer(void* pDst, const void* pSrc, DWORD size)
{
    CopyMemory(pDst, pSrc, size);
    return S_OK;
}

HRESULT SafeCopyFromUntrustedBuffer(void* pDst, const void* pSrc, DWORD size)
{
    CopyMemory(pDst, pSrc, size);
    return S_OK;
}

BOOL IsSettingSet(DWORD setting)
{
    return (setting & g_dwSettings) == setting;
}

BOOL CheckForSettings(DWORD setting, LPVOID lpParam)
{
    switch (setting)
    {
        case 0xBEEF0000:
            break;

        case 0xBEEF0001:
            if (lpParam != nullptr)
                g_dwSettings |= SET_USER_LED_ON_CREATE;
            else
                g_dwSettings &= ~SET_USER_LED_ON_CREATE;
            break;

        case 0xBEEF0002:
            if (lpParam != nullptr)
                g_dwSettings |= DISABLE_USER_LED_ON_DESTROY;
            else
                g_dwSettings &= ~DISABLE_USER_LED_ON_DESTROY;
            break;

        case 0xBEEF0003:
            if (lpParam != nullptr)
                g_dwSettings |= DISABLE_VIBRATION_ON_DESTROY;
            else
                g_dwSettings &= ~DISABLE_VIBRATION_ON_DESTROY;
            break;
    }

    return TRUE;
}

}

/////////////////////////////////////////////////////
// XInputCore namespace
/////////////////////////////////////////////////////
namespace XInputCore {

BOOL Initialize()
{
    InitializeCriticalSection(&g_csGlobalLock);

    g_dwSettings = SET_USER_LED_ON_CREATE | DISABLE_USER_LED_ON_DESTROY | DISABLE_VIBRATION_ON_DESTROY;

    if (DeviceList::Initialize() < 0)
        return FALSE;

    g_pDetailBuffer = 0;
    g_dwDetailBufferSize = 0;
    g_pfnDeviceIoControl = nullptr;
    g_pfnGetClassDevs = nullptr;
    g_pfnEnumDeviceInfo = nullptr;
    g_pfnEnumDeviceInterfaces = nullptr;
    g_pfnGetDeviceInterfaceDetail = nullptr;
    g_pfnGetDeviceProperty = nullptr;
    g_pfnDestroyDeviceInfoList = nullptr;

    if (InitializeOverrides() < 0)
    {
        CleanupOverrides();
        return FALSE;
    }

    QueueUserWorkItem([](LPVOID lpUser) -> DWORD
    {
        InitOnceExecuteOnce(&g_InitOnce, [](PINIT_ONCE InitOnce, PVOID Parameter, PVOID* Context) -> BOOL
        {
#ifndef OPENXINPUT_DISABLE_COM
            //DWORD v0; // ebx
            //int v1; // edi
            //struct IInputHostClient* v2; // ecx
            //int v4; // [esp+8h] [ebp-8h]
            //struct IInputHostClient* v5; // [esp+Ch] [ebp-4h]
            //
            //v0 = 0;
            //v4 = 0;
            //v5 = 0;
            //v1 = CreateInputHostForProcess(&v4);
            //if (v1 < 0)
            //    goto LABEL_14;
            //v1 = CreateGenericInputHost(&XInputCore::g_XInputInputClient, &v5);
            //if (v1 < 0
            //    || v5->unk2(0x10000),
            //        v1 = (*(int(__thiscall**)(int, struct IInputHostClient*))(*(_DWORD*)v4 + 12))(v4, v5),
            //        v1 < 0))
            //{
            //    v2 = v5;
            //}
            //else
            //{
            //    v2 = 0;
            //    XInputCore::g_InputHostClient = v5;
            //    v5 = 0;
            //}
            //if (v2)
            //{
            //    v2->Release();
            //    v5 = 0;
            //}
            //if (v1 < 0)
            //{
            //LABEL_14:
            //    v0 = (unsigned __int16)v1;
            //    if ((v1 & 0x1FFF0000) != 0x70000)
            //        v0 = v1;
            //}
            //SetLastError(v0);
            //return v1 >= 0;
#endif
            return TRUE;
        }, nullptr, nullptr);
        return GetLastError();
    }, nullptr, 0);

    EnableCommunications(TRUE);
    g_IsInitialized = TRUE;

    return TRUE;
}

BOOL Close(bool ContinueHost)
{
    g_IsInitialized = FALSE;
    EnableCommunications(FALSE);

    if (!ContinueHost)
        UnInitializeInputHost();

    DeviceList::Close();
    DeviceEnum::Close();
    DriverComm::Close();

    UnregisterUtcEventProvider();
    CleanupOverrides();

    DeleteCriticalSection(&g_csGlobalLock);
    return TRUE;
}

HRESULT Enter()
{
    if (!g_IsInitialized)
        return E_FAIL;

    EnterCriticalSection(&g_csGlobalLock);
    return S_OK;
}

HRESULT Leave()
{
    if (!g_IsInitialized)
        return E_FAIL;

    LeaveCriticalSection(&g_csGlobalLock);
    return S_OK;
}

HRESULT ProcessAPIRequest(DWORD dwUserIndex, HRESULT(*pfn_ApiAction)(DeviceInfo_t*, void*, DWORD), void* pApiParam, DWORD reserved, BOOL bBusOnly)
{
    HRESULT hr;
    DeviceInfo_t* pDevice = nullptr;
    BOOL retry = FALSE;

    for (DWORD i = 0; i < reserved; ++i);

    hr = Enter();
    if (hr < 0)
    {
        return hr;
    }

    do
    {
        if (bBusOnly == TRUE)
        {
            pDevice = g_pBusDeviceList[dwUserIndex];
        }
        else
        {
            hr = DeviceEnum::GetDeviceOnPort(dwUserIndex, &pDevice, g_IsCommunicationEnabled);
            if (hr < 0)
                break;
        }

        if (pDevice == nullptr)
        {
            hr = 1; // NOT_CONNECTED
            break;
        }

        hr = pfn_ApiAction(pDevice, pApiParam, reserved);
        if (hr == 1)
            return S_OK;

        if (hr < 0)
        {// Error in internal API
            pDevice->status &= ~DEVICE_STATUS_ACTIVE;
        }

        if (!XInputInternal::DeviceInfo::IsDeviceInactive(pDevice))
            break;

        pDevice = nullptr;
        DeviceList::RemoveBusDevice(dwUserIndex);
        if (bBusOnly == FALSE)
        {
            hr = DeviceList::RemoveDeviceFromPort(dwUserIndex);
        }

        if (hr < 0)
        {
            break;
        }

        if (retry == TRUE)
        {
            hr = 1; // NOT_CONNECTED
            break;
        }
    
        // We removed the old device, retry in case a new device has been plugged-in
        retry = TRUE;
    }
    while (retry == TRUE);

    Leave();
    return hr;
}

void EnableCommunications(BOOL bEnabled)
{
    DeviceInfo_t* pDevice;

    if (g_IsCommunicationEnabled == bEnabled)
        return;

    g_IsCommunicationEnabled = bEnabled;
    
    XInputInternal::DeviceInfo::OnEnableSettingChanged(bEnabled);

    for (DWORD dwUserIndex = 0; dwUserIndex < XUSER_MAX_COUNT; ++dwUserIndex)
    {
        pDevice = nullptr;
        if (DeviceEnum::GetDeviceOnPort(dwUserIndex, &pDevice, 0) >= 0 && pDevice != nullptr)
        {
            SetStateApiParam_t apiParam;
            apiParam.pVibration = nullptr;
            XInputInternal::DeviceInfo::g_pfnSetVibrationDispatcher(pDevice, &apiParam, 1);
        }
    }
}

}

/////////////////////////////////////////////////////
// Controller namespace
/////////////////////////////////////////////////////
DWORD Controller_GetUserKeystroke(DeviceInfo_t* pDevice, BYTE bUserIndex, DWORD reserved, XINPUT_KEYSTROKE* pKeystroke)
{
    static DWORD s_KeyCode;
    static DWORD s_dwKeyPressStart;
    static BOOL  s_bKeyDown;

    XINPUT_STATE gamepadState;
    GetStateApiParam_t apiParam;
    DWORD virtualKey;
    int key;
    DWORD pressedTriggers;
    DWORD dwTickCount;

    apiParam.pState = &gamepadState;
    if (XInputInternal::DeviceInfo::g_pfnGetStateDispatcher(pDevice, &apiParam, 1) < 0)
        return ERROR_EMPTY;

    gamepadState.Gamepad.wButtons &= XINPUT_BUTTON_MASK_WITHOUT_GUIDE;

    pKeystroke->UserIndex = bUserIndex;
    pKeystroke->Unicode = 0;

    key = 0;

    virtualKey = Controller_CalculateKeyFromThumbPos(VK_PAD_LTHUMB_UP, gamepadState.Gamepad.sThumbLX, gamepadState.Gamepad.sThumbLY);
    pKeystroke->VirtualKey = (WORD)virtualKey;
    if (pDevice->LeftStickVirtualKey != virtualKey)
    {
        if (pDevice->LeftStickVirtualKey != 0)
        {
            pKeystroke->VirtualKey = pDevice->LeftStickVirtualKey;
            pKeystroke->Flags = XINPUT_KEYSTROKE_KEYUP;
            s_KeyCode = 0;
            pDevice->LeftStickVirtualKey = 0;
            return ERROR_SUCCESS;
        }
        key = virtualKey;
        pDevice->LeftStickVirtualKey = (WORD)virtualKey;
    }

    virtualKey = Controller_CalculateKeyFromThumbPos(VK_PAD_RTHUMB_UP, gamepadState.Gamepad.sThumbRX, gamepadState.Gamepad.sThumbRY);

    if (virtualKey)
        pKeystroke->VirtualKey = (WORD)virtualKey;
    if (pDevice->RightStickVirtualKey != virtualKey)
    {
        if (pDevice->RightStickVirtualKey != 0)
        {
            pKeystroke->VirtualKey = pDevice->RightStickVirtualKey;
            pKeystroke->Flags = XINPUT_KEYSTROKE_KEYUP;
            s_KeyCode = 0;
            pDevice->RightStickVirtualKey = 0;
            return ERROR_SUCCESS;
        }
        key = virtualKey;
        pDevice->RightStickVirtualKey = (WORD)virtualKey;
    }

    pressedTriggers = 0;
    if (gamepadState.Gamepad.bLeftTrigger > 30u)
        pressedTriggers = 1;
    if (gamepadState.Gamepad.bRightTrigger > 30u)
        pressedTriggers = 2;

    virtualKey = VK_PAD_LTRIGGER;
    for (int i = 0; i < 2; ++i)
    {
        BYTE keyBit = 1 << i;
        if (pDevice->bTriggers & keyBit)
        {
            if (!(pressedTriggers & keyBit))
            {
                pDevice->bTriggers &= ~keyBit;
                s_KeyCode = 0;
                pKeystroke->VirtualKey = (WORD)virtualKey;
                pKeystroke->Flags = XINPUT_KEYSTROKE_KEYUP;
                return ERROR_SUCCESS;
            }
            pKeystroke->VirtualKey = (WORD)virtualKey;
        }
        else if (pressedTriggers & keyBit)
        {
            pDevice->bTriggers |= keyBit;
            key = virtualKey;
        }
        ++virtualKey;
    }

    for (int i = 0; i < (sizeof(XINPUT_GAMEPAD::wButtons)*8); ++i)
    {
        WORD keyBit = 1 << i;
        switch (i)
        {
            case 0 : virtualKey = VK_PAD_DPAD_UP; break;
            case 8 : virtualKey = VK_PAD_LSHOULDER; break;
            case 9 : virtualKey = VK_PAD_RSHOULDER; break;
            case 12: virtualKey = VK_PAD_A; break;
        }

        if(pDevice->wButtons & keyBit)
        {
            if (!(gamepadState.Gamepad.wButtons & keyBit))
            {
                pDevice->wButtons &= ~keyBit;
                pKeystroke->VirtualKey = (WORD)virtualKey;
                pKeystroke->Flags = XINPUT_KEYSTROKE_KEYUP;
                s_KeyCode = 0;
                return ERROR_SUCCESS;
            }
            pKeystroke->VirtualKey = (WORD)virtualKey;
        }
        else if (gamepadState.Gamepad.wButtons & keyBit)
        {
            pDevice->wButtons |= keyBit;
            key = virtualKey;
        }
        ++virtualKey;
    }

    dwTickCount = GetTickCount();
    if (key)
    {
        pKeystroke->VirtualKey = key;
        pKeystroke->Flags = XINPUT_KEYSTROKE_KEYDOWN;
        s_KeyCode = key;
        s_dwKeyPressStart = dwTickCount;
        s_bKeyDown = TRUE;
    }
    else if (pKeystroke->VirtualKey == s_KeyCode)
    {
        DWORD repeatTimeMs = (s_bKeyDown == TRUE ? 400 : 100);

        if ((dwTickCount - s_dwKeyPressStart) < repeatTimeMs)
        {
            pKeystroke->VirtualKey = 0;
        }
        else
        {
            pKeystroke->Flags = XINPUT_KEYSTROKE_KEYDOWN | XINPUT_KEYSTROKE_REPEAT;
            s_dwKeyPressStart = dwTickCount;
            s_bKeyDown = FALSE;
        }
    }
    else
    {
        pKeystroke->VirtualKey = 0;
    }
    if (!pKeystroke->VirtualKey)
        return ERROR_EMPTY;

    return ERROR_SUCCESS;
}

DWORD Controller_CalculateKeyFromThumbPos(SHORT base, SHORT X, SHORT Y)
{
    if (Y > 20000)
    {
        if (X < -20000)
            return base + 4;

        if (X > 20000)
            return base + 5;

        return base + 0;
    }

    if (Y < -20000)
    {
        
        if (X > 20000)
            return base + 6;

        if (X < -20000)
            return base + 7;

        return base + 1;
    }

    if (X > 20000)
        return base + 2;

    if (X < -20000)
        return base + 3;

    return 0;
}

/////////////////////////////////////////////////////
// Global functions
/////////////////////////////////////////////////////
HRESULT InitializeOverrides()
{
    int TokenInformation;
    DWORD ReturnLength;
    HRESULT hr;

    if (QuirkIsEnabled(0x1F0000) ||
        GetTokenInformation((HANDLE)-4, (TOKEN_INFORMATION_CLASS)(TokenAuditPolicy|TokenSessionId|TokenUser), &TokenInformation, sizeof(TokenInformation), &ReturnLength) == FALSE ||
        TokenInformation == 0)
    {
        return S_OK;
    }

    XInputCore::g_hXInputUapDll = LoadLibraryExW(L"XInputUap.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (XInputCore::g_hXInputUapDll == nullptr)
        goto on_error;

#define LoadOverride(X)\
    XInputCore::g_pfn##X##_Override = (decltype(XInputCore::g_pfn##X##_Override))GetProcAddress(XInputCore::g_hXInputUapDll, #X);\
    if(XInputCore::g_pfn##X##_Override == nullptr)\
        goto on_error;

    LoadOverride(XInputGetState);
    LoadOverride(XInputSetState);
    LoadOverride(XInputGetCapabilities);
    LoadOverride(XInputEnable);
    LoadOverride(XInputGetAudioDeviceIds);
    LoadOverride(XInputGetBatteryInformation);
    LoadOverride(XInputGetKeystroke);
        
#undef LoadOverride

    return S_OK;

on_error:
    hr = GetLastError();
    if (hr > 0)
        hr = (hr&0xFFFF) | 0x80070000;

    return hr;
}

void CleanupOverrides()
{
    if (XInputCore::g_hXInputUapDll != nullptr)
    {
        XInputCore::g_pfnXInputGetState_Override = nullptr;
        XInputCore::g_pfnXInputSetState_Override = nullptr;
        XInputCore::g_pfnXInputGetCapabilities_Override = nullptr;
        XInputCore::g_pfnXInputEnable_Override = nullptr;
        XInputCore::g_pfnXInputGetAudioDeviceIds_Override = nullptr;
        XInputCore::g_pfnXInputGetBatteryInformation_Override = nullptr;
        XInputCore::g_pfnXInputGetKeystroke_Override = nullptr;
        FreeLibrary(XInputCore::g_hXInputUapDll);
        XInputCore::g_hXInputUapDll = nullptr;
    }
}

void UnInitializeInputHost()
{
    //int v1;

#ifndef OPENXINPUT_DISABLE_COM
    if (XInputCore::g_InputHostClient == nullptr)
        return;

    //v1 = 0;
    //if (CreateInputHostForProcess(&v1) >= 0
    //    && (*(int(__thiscall**)(int, struct IInputHostClient*))(*(_DWORD*)v1 + 16))(v1, XInputCore::g_InputHostClient) >= 0)
    //{
    //    XInputCore::g_InputHostClient->Release();
    //    XInputCore::g_InputHostClient = 0;
    //}
#endif
}

void EventWriteDllLoaded(HRESULT hr)
{
    //void* v1; // esi
    //int v2; // ecx
    //void* v3; // [esp+4h] [ebp-3Ch]
    //char v4; // [esp+8h] [ebp-38h]
    //int* v5; // [esp+28h] [ebp-18h]
    //int v6; // [esp+2Ch] [ebp-14h]
    //int v7; // [esp+30h] [ebp-10h]
    //int v8; // [esp+34h] [ebp-Ch]
    //
    //v1 = this;
    //if ((unsigned int)dword_10008010 > 5 && _TlgKeywordOn(0x400000000000i64))
    //{
    //    v6 = 0;
    //    v8 = 0;
    //    v3 = v1;
    //    v5 = (int*)&v3;
    //    v7 = 4;
    //    _TlgWrite(v2, v2, 3, &v4);
    //}
}

void EventWriteVidPid(WORD vendorId, WORD productId)
{
    //int* v2;
    //unsigned __int8 v3;
    //int* v4;
    //unsigned __int16 v5;
    //unsigned __int16 v6;
    //char v7;
    //unsigned __int16* v8;
    //int v9;
    //int v10;
    //int v11;
    //unsigned __int16* v12;
    //int v13;]
    //int v14;]
    //int v15;
    //
    //v5 = productId;
    //v2 = (int*)(vendorId | (productId << 16));
    //v6 = vendorId;
    //v3 = 0;
    //while (1)
    //{
    //    v4 = g_VidPids[v3];
    //    if (!v4)
    //        break;
    //    if (v4 == v2)
    //        return;
    //    if (++v3 >= 0x10u)
    //        goto LABEL_7;
    //}
    //g_VidPids[v3] = v2;
//LABEL_7:
    //if (v3 != 16 && (unsigned int)dword_10008010 > 5 && _TlgKeywordOn(0x400000000000i64))
    //{
    //    v9 = 0;
    //    v8 = &v6;
    //    v10 = 2;
    //    v11 = 0;
    //    v12 = &v5;
    //    v13 = 0;
    //    v14 = 2;
    //    v15 = 0;
    //    _TlgWrite(2, 2, 4, &v7);
    //}
}

HRESULT RegisterUtcEventProvider()
{
    //HRESULT hr;
    //unsigned int v1;
    //int v3;
    //int v4;
    //int v5;
    //int v6;
    //
    //memset(&g_VidPids, 0, 0x40u);
    //v3 = *(_DWORD*)&(*off_10008014)[-16];
    //v4 = *(_DWORD*)&(*off_10008014)[-12];
    //v5 = *(_DWORD*)&(*off_10008014)[-8];
    //v6 = *(_DWORD*)&(*off_10008014)[-4];
    //dword_10008030 = 0;
    //dword_10008034 = 0;
    //hr = EventRegister(&v3, _TlgEnableCallback, &dword_10008010, &dword_10008028);
    //if (hr == S_OK)
    //{
    //    EventSetInformation(dword_10008028, dword_1000802C, 2, off_10008014, *(_WORD*)off_10008014);
    //}
    //else if(hr > 0)
    //{
    //    hr = (hr&0xFFFF) | 0x80070000;
    //}
    //return v1;
    return S_OK;
}

void UnregisterUtcEventProvider()
{
    //EventUnregister(dword_10008028, dword_1000802C);
    //dword_10008028 = 0;
    //dword_1000802C = 0;
    //dword_10008010 = 0;
}

/////////////////////////////////////////////////////
// Exported functions
/////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

#ifdef OPENXINPUT_BUILD_SHARED

BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, LPVOID lpvReserved)
{
    BOOL res = TRUE;
    HRESULT hr;

    switch (fdwReason)
    {
        case DLL_PROCESS_DETACH:
            res = XInputCore::Close(lpvReserved != nullptr);
            UnregisterUtcEventProvider();
            break;

        case DLL_PROCESS_ATTACH:
            hr = RegisterUtcEventProvider();
            res = XInputCore::Initialize();
            if (hr >= 0)
            {
                EventWriteDllLoaded(res == TRUE ? 0 : E_FAIL);
            }
            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;

        default:
            if (DriverComm::CheckForDriverHook(fdwReason, lpvReserved) == TRUE)
            {
                Utilities::CheckForSettings(fdwReason, lpvReserved);
            }
    }

    return res;
}

#else

int OpenXinputInitLibrary()
{
    return (XInputCore::Initialize() == TRUE ? ERROR_SUCCESS : ERROR_NOT_ENOUGH_MEMORY);
}

void OpenXinputReleaseLibrary()
{
    XInputCore::Close(false);
}

#endif

DWORD WINAPI OpenXInputGetState(_In_ DWORD dwUserIndex, _Out_ XINPUT_STATE* pState)
{
    DWORD result;

    if (dwUserIndex >= XUSER_MAX_COUNT || pState == nullptr)
        return ERROR_BAD_ARGUMENTS;

    if (XInputCore::g_pfnXInputGetState_Override)
    {
        result = XInputCore::g_pfnXInputGetState_Override(dwUserIndex, pState);
    }
    else
    {
        result = OpenXInputGetStateEx(dwUserIndex, pState);

        if (result == ERROR_SUCCESS)
        {
            DeviceInfo_t* pDevice;

            if (DeviceEnum::GetDeviceOnPort(dwUserIndex, &pDevice, 0) >= 0 && pDevice != nullptr)
                EventWriteVidPid(pDevice->vendorId, pDevice->productId);

            pState->Gamepad.wButtons &= XINPUT_BUTTON_MASK_WITHOUT_GUIDE;
        }
    }

    return result;
}

DWORD WINAPI OpenXInputSetState(_In_ DWORD dwUserIndex, _In_ XINPUT_VIBRATION* pVibration)
{
    SetStateApiParam_t apiParam;
    HRESULT hr;
    DWORD result;

    if (dwUserIndex >= XUSER_MAX_COUNT || pVibration == nullptr)
        return ERROR_BAD_ARGUMENTS;

    if (XInputCore::g_pfnXInputSetState_Override)
    {
        result = XInputCore::g_pfnXInputSetState_Override(dwUserIndex, pVibration);
    }
    else
    {
        apiParam.pVibration = pVibration;

        hr = XInputCore::ProcessAPIRequest(dwUserIndex, XInputInternal::DeviceInfo::g_pfnSetVibrationDispatcher, &apiParam, 1, FALSE);
        result = XInputReturnCodeFromHRESULT(hr);
    }
    return result;
}

DWORD WINAPI OpenXInputGetCapabilities(_In_ DWORD dwUserIndex, _In_ DWORD dwFlags, _Out_ XINPUT_CAPABILITIES* pCapabilities)
{
    XINPUT_CAPABILITIES_EX capabilitiesEx;
    DWORD result;
    if (dwUserIndex >= XUSER_MAX_COUNT || pCapabilities == nullptr)
        return ERROR_BAD_ARGUMENTS;

    if (XInputCore::g_pfnXInputGetCapabilities_Override)
    {
        result = XInputCore::g_pfnXInputGetCapabilities_Override(dwUserIndex, dwFlags, pCapabilities);
    }
    else
    {
        result = OpenXInputGetCapabilitiesEx(1, dwUserIndex, dwFlags, &capabilitiesEx);
        if (result == ERROR_SUCCESS)
            CopyMemory(pCapabilities, &capabilitiesEx.Capabilities, sizeof(XINPUT_CAPABILITIES));
    }
    return result;
}

void WINAPI OpenXInputEnable(_In_ BOOL enable)
{
    XInputCore::EnableCommunications(enable != FALSE);
}

DWORD WINAPI OpenXInputGetAudioDeviceIds(_In_ DWORD dwUserIndex, _Out_writes_opt_(*pRenderCount) LPWSTR pRenderDeviceId, _Inout_opt_ UINT* pRenderCount, _Out_writes_opt_(*pCaptureCount) LPWSTR pCaptureDeviceId, _Inout_opt_ UINT* pCaptureCount)
{
    DWORD result;
    HRESULT hr;
    OSVERSIONINFOW VersionInformation;
    bool doApiCall;
    GetAudioDeviceIdApiParam_t apiParam;

    if(dwUserIndex >= XUSER_MAX_COUNT ||
        (pRenderCount == nullptr && pCaptureCount == nullptr) || 
        (pRenderDeviceId != nullptr && pRenderCount == nullptr) ||
        (pCaptureDeviceId != nullptr && pCaptureCount == nullptr))
    {
        return ERROR_BAD_ARGUMENTS;
    }

    if (XInputCore::g_pfnXInputGetAudioDeviceIds_Override)
    {
        return XInputCore::g_pfnXInputGetAudioDeviceIds_Override(dwUserIndex, pRenderDeviceId, pRenderCount, pCaptureDeviceId, pCaptureCount);
    }

    doApiCall = true;
    memset(&VersionInformation, 0, sizeof(OSVERSIONINFOW));
    VersionInformation.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
    if (!GetVersionExW(&VersionInformation) || VersionInformation.dwMajorVersion >= 6)
    {
        HANDLE hDevice;
        QuickDriverEnum quickEnum;
        DeviceInfo_t device;

        apiParam.pCaptureCount = pCaptureCount;
        apiParam.pCaptureDeviceId = pCaptureDeviceId;
        apiParam.pRenderCount = pRenderCount;
        apiParam.pRenderDeviceId = pRenderDeviceId;

        bool run = quickEnum.Restart();
        while (run)
        {
            run = quickEnum.GetNext(&hDevice);
            if (hDevice != INVALID_HANDLE_VALUE)
            {
                if (XInputInternal::DeviceInfo::MinFillFromInterface(hDevice, &device) == 1 && device.wType >= 0x0102)
                {
                    device.dwUserIndex = static_cast<BYTE>(dwUserIndex);
                    if (XInputInternal::DeviceInfo::GetAudioDevice(&device, &apiParam, 4) >= 0)
                    {
                        doApiCall = false;
                        run = false;
                        result = ERROR_SUCCESS;
                    }
                }
                CloseHandle(hDevice);
            }
        }
        if (doApiCall)
        {
            hr = XInputCore::ProcessAPIRequest(
                dwUserIndex,
                XInputInternal::DeviceInfo::GetAudioDevice,
                &apiParam,
                4u,
                FALSE);
            result = XInputReturnCodeFromHRESULT(hr);
        }
    }
    else
    {
        result = ERROR_NOT_SUPPORTED;
    }

    return result;
}

DWORD WINAPI OpenXInputGetBatteryInformation(_In_ DWORD dwUserIndex, _In_ BYTE devType, _Out_ XINPUT_BATTERY_INFORMATION* pBatteryInformation)
{
    DWORD result;
    HRESULT hr;
    bool doApiCall;
    GetBatteryInformationApiParam_t apiParam;

    if (dwUserIndex >= XUSER_MAX_COUNT || pBatteryInformation == nullptr)
        return ERROR_BAD_ARGUMENTS;

    if (XInputCore::g_pfnXInputGetBatteryInformation_Override)
    {
        result = XInputCore::g_pfnXInputGetBatteryInformation_Override(dwUserIndex, devType, pBatteryInformation);
    }
    else
    {
        pBatteryInformation->BatteryLevel = 0;
        pBatteryInformation->BatteryType = 0;

        result = ERROR_SUCCESS;
        doApiCall = true;
        if (devType == XINPUT_DEVTYPE_GAMEPAD)
        {
            HANDLE hDevice;
            DeviceInfo_t device;
            result = ERROR_DEVICE_NOT_CONNECTED;
            QuickDriverEnum quickEnum;
            bool run = quickEnum.Restart();
            while (run)
            {
                hDevice = INVALID_HANDLE_VALUE;
                run = quickEnum.GetNext(&hDevice);
                if (hDevice != INVALID_HANDLE_VALUE)
                {
                    if (XInputInternal::DeviceInfo::MinFillFromInterface(hDevice, &device) && device.wType >= 0x0102)
                    {
                        device.dwUserIndex = (BYTE)dwUserIndex;
                        if (DriverComm::GetBatteryInformation(&device, XINPUT_DEVTYPE_GAMEPAD, pBatteryInformation) >= 0)
                        {
                            doApiCall = 0;
                            run = 0;
                            result = ERROR_SUCCESS;
                        }
                    }
                    CloseHandle(hDevice);
                }
            }
        }
        if (doApiCall)
        {
            apiParam.DeviceType = &devType;
            apiParam.pBatteryInformation = pBatteryInformation;
            hr = XInputCore::ProcessAPIRequest(dwUserIndex, XInputInternal::DeviceInfo::GetBatteryInformation, &apiParam, 2, FALSE);
            result = XInputReturnCodeFromHRESULT(hr);
        }
    }

    return result;
}

DWORD WINAPI OpenXInputGetKeystroke(_In_ DWORD dwUserIndex, _Reserved_ DWORD dwReserved, _Out_ PXINPUT_KEYSTROKE pKeystroke)
{
    GetKeystrokeApiParam_t apiParam;
    DWORD result;
    HRESULT hr;
    if ((dwUserIndex >= XUSER_MAX_COUNT && dwUserIndex != XUSER_INDEX_ANY) || pKeystroke == nullptr)
        return ERROR_BAD_ARGUMENTS;

    if (XInputCore::g_pfnXInputGetKeystroke_Override)
    {
        result = XInputCore::g_pfnXInputGetKeystroke_Override(dwUserIndex, dwReserved, pKeystroke);
    }
    else
    {
        apiParam.lpReserved = &dwReserved;
        apiParam.pKeystroke = pKeystroke;
        apiParam.lpLastError = &result;
        if (dwUserIndex < XUSER_MAX_COUNT)
        {
            result = ERROR_SUCCESS;
            hr = XInputCore::ProcessAPIRequest(dwUserIndex, XInputInternal::DeviceInfo::GetKeystroke, &apiParam, 3, FALSE);
            if (hr >= 0)
                pKeystroke->UserIndex = (BYTE)dwUserIndex;
            else
                result = XInputReturnCodeFromHRESULT(hr);
        }
        else
        {
            result = ERROR_EMPTY;
            for (BYTE i = 0; i < XUSER_MAX_COUNT; ++i)
            {
                hr = XInputCore::ProcessAPIRequest(i, XInputInternal::DeviceInfo::GetKeystroke, &apiParam, 3, FALSE);
                if (result == ERROR_SUCCESS)
                {
                    pKeystroke->UserIndex = i;
                    break;
                }
            }
            if (result != ERROR_SUCCESS)
                result = ERROR_EMPTY;
        }
    }

    return result;
}

DWORD WINAPI OpenXInputGetStateEx(_In_ DWORD dwUserIndex, _Out_ XINPUT_STATE* pState)
{
    DWORD result;
    HRESULT hr;
    GetStateApiParam_t apiParam;

    if (dwUserIndex >= XUSER_MAX_COUNT || pState == nullptr)
        return ERROR_BAD_ARGUMENTS;

    if (XInputCore::g_pfnXInputGetState_Override)
    {
        result = XInputCore::g_pfnXInputGetState_Override(dwUserIndex, pState);
    }
    else
    {
        apiParam.pState = pState;

        hr = XInputCore::ProcessAPIRequest(dwUserIndex, XInputInternal::DeviceInfo::g_pfnGetStateDispatcher, &apiParam, 1, FALSE);
        result = XInputReturnCodeFromHRESULT(hr);
    }
    return result;
}

DWORD WINAPI OpenXInputWaitForGuideButton(_In_ DWORD dwUserIndex, _In_ HANDLE hEvent, _Out_ XINPUT_LISTEN_STATE* pListenState)
{
    HRESULT hr;
    WaitGuideButtonApiParam_t apiParam;

    if (dwUserIndex >= XUSER_MAX_COUNT || pListenState == nullptr)
        return ERROR_BAD_ARGUMENTS;

    if (hEvent == INVALID_HANDLE_VALUE)
        hEvent = 0;

    apiParam.lphEvent = &hEvent;
    apiParam.pListenState = pListenState;

    hr = XInputCore::ProcessAPIRequest(dwUserIndex, XInputInternal::DeviceInfo::WaitForGuideButton, &apiParam, 2, FALSE);
    return XInputReturnCodeFromHRESULT(hr);
}

DWORD WINAPI OpenXInputCancelGuideButtonWait(_In_ DWORD dwUserIndex)
{
    HRESULT hr;

    if (dwUserIndex >= XUSER_MAX_COUNT)
        return ERROR_BAD_ARGUMENTS;

    hr = XInputCore::ProcessAPIRequest(dwUserIndex, XInputInternal::DeviceInfo::CancelGuideButtonWait, nullptr, 0, FALSE);
    return XInputReturnCodeFromHRESULT(hr);
}

DWORD WINAPI OpenXInputPowerOffController(_In_ DWORD dwUserIndex)
{
    HRESULT hr;

    if (dwUserIndex >= XUSER_MAX_COUNT)
        return ERROR_BAD_ARGUMENTS;

    hr = XInputCore::ProcessAPIRequest(dwUserIndex, XInputInternal::DeviceInfo::PowerOffController, nullptr, 0, FALSE);
    return XInputReturnCodeFromHRESULT(hr);

}

DWORD WINAPI OpenXInputGetBaseBusInformation(_In_ DWORD dwBusIndex, _Out_ XINPUT_BASE_BUS_INFORMATION* pBaseBusInformation)
{
    DWORD result;
    HRESULT hr;
    GetBaseBusInformationApiParam_t apiParam;

    if (dwBusIndex < DeviceList::BusDeviceListSize || pBaseBusInformation == nullptr)
        return ERROR_BAD_ARGUMENTS;

    apiParam.pBaseBusInformation = pBaseBusInformation;

    result = XInputCore::ProcessAPIRequest(
        dwBusIndex,
        XInputInternal::DeviceInfo::GetBaseBusInformation,
        &apiParam,
        1u,
        TRUE);
    hr = XInputReturnCodeFromHRESULT(result);

    return hr;
}

DWORD WINAPI OpenXInputGetCapabilitiesEx(_In_ DWORD dwReserved, _In_ DWORD dwUserIndex, _In_ DWORD dwFlags, _Out_ XINPUT_CAPABILITIES_EX* pCapabilitiesEx)
{
    GetCapabilitiesApiParam_t apiParam;
    DWORD result;
    HRESULT hr;
    if (dwUserIndex >= XUSER_MAX_COUNT || (dwFlags != 0 && dwFlags != XINPUT_CAPS_FFB_SUPPORTED) || pCapabilitiesEx == nullptr)
        return ERROR_BAD_ARGUMENTS;

    apiParam.pCapabilities = pCapabilitiesEx;

    if (dwReserved != 1)
        return ERROR_INVALID_DATA;

    hr = XInputCore::ProcessAPIRequest(dwUserIndex, XInputInternal::DeviceInfo::GetCapabilities, &apiParam, 1, FALSE);
    result = XInputReturnCodeFromHRESULT(hr);

    if (result == ERROR_SUCCESS)
        pCapabilitiesEx->Capabilities.Gamepad.wButtons &= XINPUT_BUTTON_MASK;

    return result;
}

DWORD WINAPI OpenXInputGetMaxControllerCount()
{
    return XUSER_MAX_COUNT;
}

#ifdef __cplusplus
}
#endif