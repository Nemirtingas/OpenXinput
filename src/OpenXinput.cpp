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
#endif

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
    WORD XUSBVersion;
    WORD field_22;
    OPENXINPUT_STATE_FULL DeviceState;
    XINPUT_VIBRATION DeviceVibration;
    WORD field_38;
    WORD field_3A;
    WORD field_3C;
    WORD field_3E;
    WORD field_40;
    WORD field_42;
    WORD field_44;
    WORD field_46;
    WORD field_48;
    WORD field_4A;
    WORD field_4C;
    WORD vendorId;
    WORD productId;
    BYTE inputId;
    BYTE bTriggers;
    WORD wButtons;
    WORD LeftStickVirtualKey;
    WORD RightStickVirtualKey;
    WORD field_5A;
    WORD field_5C;
    WORD field_5E;
};
// All theses structs are passed to the XUSB driver, they __NEED__ to be unaligned.
#pragma pack(push, 1)
struct InBaseRequest_t
{
    WORD XUSBVersion;
    BYTE DeviceIndex;
};

#define XUSB_SET_STATE_FLAG_LED         ((BYTE)0x01)
#define XUSB_SET_STATE_FLAG_VIBRATION   ((BYTE)0x02)

struct InSetState_t
{
    BYTE deviceIndex;
    BYTE ledState;
    BYTE leftMotorSpeed;
    BYTE rightMotorSpeed;
    BYTE flags;
};

struct OutGetAudioDeviceInformation_t
{
    WORD XUSBVersion;
    WORD vendorId;
    WORD productId;
    BYTE inputId;
};

struct OutGetLEDBuffer_t
{
    WORD XUSBVersion;
    BYTE LEDState;
};

struct OutDeviceInfos_t
{
    WORD XUSBVersion;
    BYTE deviceIndex;
    BYTE unk1;
    WORD unk2;
    WORD unk3;
    WORD vendorId;
    WORD productId;
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

struct GamepadState0101
{
    WORD  XUSBVersion;
    BYTE  status;
    BYTE  unk2;
    BYTE  inputId;
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
    BYTE  bExtraButtons;
};

struct GamepadCapabilities0101
{
    WORD  XUSBVersion;
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

struct InGamepadBatteryInformation0102
{
    WORD XUSBVersion;
    BYTE DeviceIndex;
    BYTE DeviceType;
};

struct GamepadBatteryInformation0102
{
    WORD XUSBVersion;
    BYTE BatteryType;
    BYTE BatteryLevel;
};

#pragma pack(pop)

struct GetStateApiParam_t
{
    OPENXINPUT_STATE_FULL* pState;
};

struct SetStateApiParam_t
{
    XINPUT_VIBRATION* pVibration;
};

struct DSoundCallbackParam_t
{
    PCWSTR DevicePath;
    GUID* HeadphoneGuid;
    GUID* MicrophoneGuid;
};

struct GetAudioDeviceGuidsApiParam_t
{
    GUID* pHeadphoneGuid;
    GUID* pMicrophoneGuid;
};

struct GetBatteryInformationApiParam_t
{
    LPBYTE DeviceType;
    XINPUT_BATTERY_INFORMATION* pBatteryInformation;
};

struct GetCapabilitiesApiParam_t
{
    XINPUT_CAPABILITIES* pCapabilities;
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

struct GetDeviceUSBIdsApiParam_t
{
    WORD* pVendorId;
    WORD* pProductId;
    WORD* pInputId;
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
static decltype(SetupDiGetClassDevsW)* g_pfnGetClassDevs;
static decltype(SetupDiEnumDeviceInterfaces)* g_pfnEnumDeviceInterfaces;
static decltype(SetupDiGetDeviceInterfaceDetailW)* g_pfnGetDeviceInterfaceDetail;
static decltype(SetupDiDestroyDeviceInfoList)* g_pfnDestroyDeviceInfoList;

static BOOL g_IsInitialized = FALSE;
static BOOL g_IsCommunicationEnabled = FALSE;

static RTL_CRITICAL_SECTION g_csGlobalLock;
static DWORD g_dwSettings = SET_USER_LED_ON_CREATE | DISABLE_USER_LED_ON_DESTROY | DISABLE_VIBRATION_ON_DESTROY;
static DWORD g_dwDeviceListSize = 0;
static DeviceInfo_t** g_pDeviceList;

static DWORD g_dwLogVerbosity = 0;

DWORD XInputReturnCodeFromHRESULT(HRESULT hr);

HRESULT GrowList(DWORD newSize);

void CopyGamepadStateToDeviceInfo(DeviceInfo_t* pDevice, GamepadState0100* pGamepadState);
void CopyGamepadStateToDeviceInfo(DeviceInfo_t* pDevice, GamepadState0101* pGamepadState);
void TranslateCapabilities(GamepadCapabilities0101* pGamepadCapabilities, XINPUT_CAPABILITIES* pCapabilities);

HRESULT SendReceiveIoctl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, LPOVERLAPPED lpOverlapped);
HRESULT SendIoctl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize);
HRESULT ReceiveIoctl(HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpOutBuffer, DWORD nOufBufferSize);

HRESULT EnumerateXInputDevices();
HRESULT EnumerateDevicesOnDeviceInterface(HANDLE hDevice, LPCWSTR lpDevicePath);
HRESULT ProcessEnumeratedDevice(DeviceInfo_t* pDevice);

HRESULT GetDeviceList(HDEVINFO* phDevInfo);
HRESULT GetDeviceInterfaceData(HDEVINFO DeviceInfoSet, DWORD MemberIndex, PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData);
HRESULT GetDeviceInterfaceDetail(HDEVINFO hDeviceInfo, PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData, PSP_DEVICE_INTERFACE_DETAIL_DATA_W* ppDeviceInterfaceDetailData);

DWORD CALLBACK WaitForGuideButtonHelper(void* pParam);

BOOL CALLBACK DirectSoundEnumerateCallback(PDSPROPERTY_DIRECTSOUNDDEVICE_DESCRIPTION_W_DATA lpData, LPVOID lpUser);

}

/////////////////////////////////////////////////////
// Protocol namespace
/////////////////////////////////////////////////////
namespace Protocol {

DEFINE_HIDDEN_GUID(XUSB_INTERFACE_CLASS_GUID, 0xEC87F1E3, 0xC13B, 0x4100, 0xB5, 0xF7, 0x8B, 0x84, 0xD5, 0x42, 0x60, 0xCB);

constexpr DWORD IOCTL_XINPUT_BASE = 0x8000;

//                                                                16bits         12bits       3bits            3bits
static DWORD IOCTL_XINPUT_GET_INFORMATION          = CTL_CODE(IOCTL_XINPUT_BASE, 0x800, METHOD_BUFFERED, FILE_READ_ACCESS);                     // 0x80006000
static DWORD IOCTL_XINPUT_GET_CAPABILITIES         = CTL_CODE(IOCTL_XINPUT_BASE, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS); // 0x8000E004
static DWORD IOCTL_XINPUT_GET_LED_STATE            = CTL_CODE(IOCTL_XINPUT_BASE, 0x802, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS); // 0x8000E008
static DWORD IOCTL_XINPUT_GET_GAMEPAD_STATE        = CTL_CODE(IOCTL_XINPUT_BASE, 0x803, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS); // 0x8000E00C
static DWORD IOCTL_XINPUT_SET_GAMEPAD_STATE        = CTL_CODE(IOCTL_XINPUT_BASE, 0x804, METHOD_BUFFERED, FILE_WRITE_ACCESS);                    // 0x8000A010
static DWORD IOCTL_XINPUT_WAIT_FOR_GUIDE_BUTTON    = CTL_CODE(IOCTL_XINPUT_BASE, 0x805, METHOD_BUFFERED, FILE_WRITE_ACCESS);                    // 0x8000A014
static DWORD IOCTL_XINPUT_GET_BATTERY_INFORMATION  = CTL_CODE(IOCTL_XINPUT_BASE, 0x806, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS); // 0x8000E018
static DWORD IOCTL_XINPUT_POWER_DOWN_DEVICE        = CTL_CODE(IOCTL_XINPUT_BASE, 0x807, METHOD_BUFFERED, FILE_WRITE_ACCESS);                    // 0x8000A01C
static DWORD IOCTL_XINPUT_GET_AUDIO_INFORMATION    = CTL_CODE(IOCTL_XINPUT_BASE, 0x808, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS); // 0x8000E020

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
// DeviceEnum namespace
/////////////////////////////////////////////////////
namespace DeviceEnum {

HRESULT Initialize();
HRESULT Close();

HRESULT GetDeviceOnPort(DWORD dwUserIndex, DeviceInfo_t** ppDevice, bool rescan);

}

/////////////////////////////////////////////////////
// DeviceInfo namespace
/////////////////////////////////////////////////////
namespace DeviceInfo {

struct XINPUT_AUDIO_INFORMATION
{
    WORD vendorId;
    WORD productId;
    BYTE inputId;
};

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

HRESULT GetKeystroke(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);
HRESULT PowerOffController(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);
HRESULT CancelGuideButtonWait(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);
HRESULT GetAudioDeviceGuids(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);
HRESULT GetBatteryInformation(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);
HRESULT GetCapabilities(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);
HRESULT WaitForGuideButton(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);

HRESULT GetDeviceUSBIds(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);

void OnEnableSettingChanged(BOOL bEnabled);


static HRESULT(*g_pfnGetStateDispatcher)(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);
static HRESULT(*g_pfnSetVibrationDispatcher)(DeviceInfo_t* pDevice, void* pParams, DWORD reserved);
static HRESULT(*g_pfnGetKeystrokeDispatcher)(DeviceInfo_t* pDevice, void* pParams, DWORD reserved) = GetKeystroke;
static HRESULT(*g_pfnPowerOffController)(DeviceInfo_t* pDevice, void* pParams, DWORD reserved) = PowerOffController;
static HRESULT(*g_pfnCancelGuideButtonWait)(DeviceInfo_t* pDevice, void* pParams, DWORD reserved) = CancelGuideButtonWait;
static HRESULT(*g_pfnGetAudioDeviceGuidsDispatcher)(DeviceInfo_t* pDevice, void* pParams, DWORD reserved) = GetAudioDeviceGuids;
static HRESULT(*g_pfnGetBatteryInformationDispatcher)(DeviceInfo_t* pDevice, void* pParams, DWORD reserved) = GetBatteryInformation;
static HRESULT(*g_pfnGetCapabilitiesDispatcher)(DeviceInfo_t* pDevice, void* pParams, DWORD reserved) = GetCapabilities;
static HRESULT(*g_pfnWaitForGuideButton)(DeviceInfo_t* pDevice, void* pParams, DWORD reserved) = WaitForGuideButton;

}

/////////////////////////////////////////////////////
// DeviceList namespace
/////////////////////////////////////////////////////
namespace DeviceList {

constexpr size_t InitialDeviceListSize = XUSER_MAX_COUNT * 2;

HRESULT Initialize();
HRESULT Close();

HRESULT SetDeviceOnPort(DWORD dwUserIndex, DeviceInfo_t* pDevice);
HRESULT GetDeviceOnPort(DWORD dwUserIndex, DeviceInfo_t** ppDevice);
HRESULT IsDevicePresent(DeviceInfo_t* pDevice);
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
HRESULT GetCapabilities(DeviceInfo_t* pDevice, XINPUT_CAPABILITIES* pCapabilities);
HRESULT GetBatteryInformation(DeviceInfo_t* pDevice, BYTE DeviceType, XINPUT_BATTERY_INFORMATION* pBatteryInformation);
HRESULT GetAudioDeviceInformation(DeviceInfo_t* pDevice, DeviceInfo::XINPUT_AUDIO_INFORMATION* pAudioInformation);
HRESULT GetLEDState(DeviceInfo_t* pDevice, BYTE* ledState);
HRESULT PowerOffController(DeviceInfo_t* pDevice);
HRESULT WaitForGuideButton(HANDLE hDevice, DWORD dwUserIndex, XINPUT_LISTEN_STATE* pListenState);
HRESULT CancelGuideButtonWait(DeviceInfo_t* pDevice);

namespace SetupDiWrapper {

HDEVINFO GetClassDevs(const GUID* ClassGuid, PCWSTR Enumerator, HWND hwndParent, DWORD Flags);
BOOL EnumDeviceInterfaces(HDEVINFO DeviceInfoSet, PSP_DEVINFO_DATA DeviceInfoData, const GUID* InterfaceClassGuid, DWORD MemberIndex, PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData);
BOOL GetDeviceInterfaceDetail(HDEVINFO DeviceInfoSet, PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData, PSP_DEVICE_INTERFACE_DETAIL_DATA_W DeviceInterfaceDetailData, DWORD DeviceInterfaceDetailDataSize, PDWORD RequiredSize, PSP_DEVINFO_DATA DeviceInfoData);
BOOL DestroyDeviceInfoList(HDEVINFO DeviceInfoSet);

}

}

/////////////////////////////////////////////////////
// Utilities namespace
/////////////////////////////////////////////////////
namespace Utilities {

HRESULT Initialize();
HRESULT Close();

LPVOID MemAlloc(DWORD size);
void MemFree(LPVOID lpMem);

HRESULT SafeCopyToUntrustedBuffer(void* pDst, const void* pSrc, DWORD size);
HRESULT SafeCopyFromUntrustedBuffer(void* pDst, const void* pSrc, DWORD size);

BOOL IsSettingSet(DWORD setting);
BOOL CheckForSettings(DWORD setting, LPVOID lpParam);

}

/////////////////////////////////////////////////////
// XInputCore namespace
/////////////////////////////////////////////////////
namespace XInputCore {

BOOL Initialize();
BOOL Close();

HRESULT Enter();
HRESULT Leave();
HRESULT ProcessAPIRequest(DWORD dwUserIndex, HRESULT(*pfn_ApiAction)(DeviceInfo_t*, void*, DWORD), void* pApiParam, DWORD reserved);

void EnableCommunications(BOOL bEnabled);

}

/////////////////////////////////////////////////////
// QuickDriverEnum namespace
/////////////////////////////////////////////////////
class QuickDriverEnum
{
    HDEVINFO hDevInfo;
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
        if (hDevInfo != (HDEVINFO)INVALID_HANDLE_VALUE)
        {
            DriverComm::SetupDiWrapper::DestroyDeviceInfoList(hDevInfo);
            hDevInfo = (HDEVINFO)INVALID_HANDLE_VALUE;
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
        if (hDevInfo != (HDEVINFO)INVALID_HANDLE_VALUE)
        {
            DriverComm::SetupDiWrapper::DestroyDeviceInfoList(hDevInfo);
            hDevInfo = (HDEVINFO)INVALID_HANDLE_VALUE;
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

        hDevInfo = DriverComm::SetupDiWrapper::GetClassDevs(&Protocol::XUSB_INTERFACE_CLASS_GUID, nullptr, nullptr, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
        return hDevInfo != (HDEVINFO)INVALID_HANDLE_VALUE;
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

        if (DriverComm::SetupDiWrapper::EnumDeviceInterfaces(hDevInfo, nullptr, &Protocol::XUSB_INTERFACE_CLASS_GUID, MemberIndex, &DeviceInterfaceData) == FALSE)
            return false;

        ++MemberIndex;

        DeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
        RequiredSize = 0;
        if (DriverComm::SetupDiWrapper::GetDeviceInterfaceDetail(hDevInfo, &DeviceInterfaceData, DeviceInterfaceDetailData, DeviceInterfaceDetailDataSize, &RequiredSize, nullptr) == TRUE)
        {

            *phDevice = CreateFileW(DeviceInterfaceDetailData->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
            return true;
        }

        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
            return false;

        Utilities::MemFree(DeviceInterfaceDetailData);
        DeviceInterfaceDetailDataSize = RequiredSize;
        DeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)Utilities::MemAlloc(DeviceInterfaceDetailDataSize);
        if (DeviceInterfaceDetailData == nullptr)
        {
            DeviceInterfaceDetailDataSize = 0;
            return false;
        }

        DeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
        if (DriverComm::SetupDiWrapper::GetDeviceInterfaceDetail(hDevInfo, &DeviceInterfaceData, DeviceInterfaceDetailData, DeviceInterfaceDetailDataSize, nullptr, nullptr) == FALSE)
            return false;

        *phDevice = CreateFileW(DeviceInterfaceDetailData->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        return true;
    }

    bool MinFillFromInterface(HANDLE hDevice, DeviceInfo_t *pDevice)
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
        pDevice->XUSBVersion = outBuff.XUSBVersion;
        pDevice->vendorId = outBuff.vendorId;
        pDevice->productId = outBuff.productId;
        return true;
    }
};

/////////////////////////////////////////////////////
// Controller namespace
/////////////////////////////////////////////////////
DWORD Controller_GetUserKeystroke(DeviceInfo_t* pDevice, BYTE unk1, DWORD reserved, XINPUT_KEYSTROKE* pKeystroke);
DWORD Controller_CalculateKeyFromThumbPos(SHORT base, SHORT X, SHORT Y);

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
    pDevice->DeviceState.XinputState.dwPacketNumber = pGamepadState->dwPacketNumber;
    pDevice->DeviceState.XinputState.Gamepad.bLeftTrigger = pGamepadState->bLeftTrigger;
    pDevice->DeviceState.XinputState.Gamepad.bRightTrigger = pGamepadState->bRightTrigger;
    pDevice->DeviceState.XinputState.Gamepad.sThumbLX = pGamepadState->sThumbLX;
    pDevice->DeviceState.XinputState.Gamepad.sThumbLY = pGamepadState->sThumbLY;
    pDevice->DeviceState.XinputState.Gamepad.sThumbRX = pGamepadState->sThumbRX;
    pDevice->DeviceState.XinputState.Gamepad.sThumbRY = pGamepadState->sThumbRY;
    pDevice->DeviceState.XinputState.Gamepad.wButtons = pGamepadState->wButtons;

    pDevice->DeviceState.GamepadExtras.dwExtraButtons = 0;

    if (pGamepadState->status == 1)
        pDevice->status |= DEVICE_STATUS_ACTIVE;
    else
        pDevice->status &= ~DEVICE_STATUS_ACTIVE;

    pDevice->inputId = pGamepadState->inputId;
}

void CopyGamepadStateToDeviceInfo(DeviceInfo_t* pDevice, GamepadState0101* pGamepadState)
{
    pDevice->DeviceState.XinputState.dwPacketNumber = pGamepadState->dwPacketNumber;
    pDevice->DeviceState.XinputState.Gamepad.bLeftTrigger = pGamepadState->bLeftTrigger;
    pDevice->DeviceState.XinputState.Gamepad.bRightTrigger = pGamepadState->bRightTrigger;
    pDevice->DeviceState.XinputState.Gamepad.sThumbLX = pGamepadState->sThumbLX;
    pDevice->DeviceState.XinputState.Gamepad.sThumbLY = pGamepadState->sThumbLY;
    pDevice->DeviceState.XinputState.Gamepad.sThumbRX = pGamepadState->sThumbRX;
    pDevice->DeviceState.XinputState.Gamepad.sThumbRY = pGamepadState->sThumbRY;
    pDevice->DeviceState.XinputState.Gamepad.wButtons = pGamepadState->wButtons;
    pDevice->DeviceState.XinputState.Gamepad.wButtons &= XINPUT_BUTTON_MASK;

    pDevice->DeviceState.GamepadExtras.dwExtraButtons = 0;
    if (pGamepadState->bExtraButtons & 0x01)
        pDevice->DeviceState.GamepadExtras.dwExtraButtons |= OPENXINPUT_GAMEPAD_EXTRAS_SHARE;

    if (pGamepadState->status == 1)
        pDevice->status |= DEVICE_STATUS_ACTIVE;
    else
        pDevice->status &= ~DEVICE_STATUS_ACTIVE;

    pDevice->inputId = pGamepadState->inputId;
}

void TranslateCapabilities(GamepadCapabilities0101* pGamepadCapabilities, XINPUT_CAPABILITIES* pCapabilities)
{
    pCapabilities->Type = pGamepadCapabilities->Type;
    pCapabilities->SubType = pGamepadCapabilities->SubType;
    pCapabilities->Flags = XINPUT_CAPS_VOICE_SUPPORTED;
    pCapabilities->Gamepad.wButtons = pGamepadCapabilities->wButtons;
    pCapabilities->Gamepad.bLeftTrigger = pGamepadCapabilities->bLeftTrigger;
    pCapabilities->Gamepad.bRightTrigger = pGamepadCapabilities->bRightTrigger;
    pCapabilities->Gamepad.sThumbLX = pGamepadCapabilities->sThumbLX;
    pCapabilities->Gamepad.sThumbLY = pGamepadCapabilities->sThumbLY;
    pCapabilities->Gamepad.sThumbRX = pGamepadCapabilities->sThumbRX;
    pCapabilities->Gamepad.sThumbRY = pGamepadCapabilities->sThumbRY;
    pCapabilities->Vibration.wLeftMotorSpeed = pGamepadCapabilities->bLeftMotorSpeed;
    pCapabilities->Vibration.wRightMotorSpeed = pGamepadCapabilities->bRightMotorSpeed;
    pCapabilities->Gamepad.wButtons &= XINPUT_BUTTON_MASK;
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
    HRESULT hr;
    HDEVINFO hDeviceInfoSet;
    DWORD dwMemberIndex = 0;
    SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA_W pDeviceInterfaceDetailData;
    HANDLE hDevice;

    if ((hr = GetDeviceList(&hDeviceInfoSet)) < 0)
        return hr;
    
    while (GetDeviceInterfaceData(hDeviceInfoSet, dwMemberIndex++, &DeviceInterfaceData) >= 0)
    {
        if ((hr = GetDeviceInterfaceDetail(hDeviceInfoSet, &DeviceInterfaceData, &pDeviceInterfaceDetailData)) < 0)
            continue;

        hDevice = CreateFileW(pDeviceInterfaceDetailData->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if (hDevice != INVALID_HANDLE_VALUE)
        {
            hr = EnumerateDevicesOnDeviceInterface(hDevice, pDeviceInterfaceDetailData->DevicePath);
            if (hr < 0)
                hr = S_OK;

            CloseHandle(hDevice);
            hDevice = INVALID_HANDLE_VALUE;
        }
    }

    if (hDeviceInfoSet != (HDEVINFO)INVALID_HANDLE_VALUE)
    {
        DriverComm::SetupDiWrapper::DestroyDeviceInfoList(hDeviceInfoSet);
        hDeviceInfoSet = (HDEVINFO)INVALID_HANDLE_VALUE;
    }

    return hr;
}

HRESULT EnumerateDevicesOnDeviceInterface(HANDLE hDevice, LPCWSTR lpDevicePath)
{
    HRESULT hr;
    DeviceInfo_t* pDevice = nullptr;
    OutDeviceInfos_t deviceInfos = {};

    hr = DriverComm::GetDeviceInfoFromInterface(hDevice, &deviceInfos);
    if (hr < 0 || (deviceInfos.unk3 & 0x80))
        return hr;

    for (int i = 0; i < deviceInfos.deviceIndex; ++i)
    {
        if (pDevice != nullptr)
        {
            DeviceInfo::Recycle(pDevice);
        }
        else
        {
            pDevice = DeviceInfo::Create(hDevice, lpDevicePath);
            if (pDevice == nullptr)
                return E_OUTOFMEMORY;
        }

        pDevice->dwUserIndex = i;
        pDevice->productId = deviceInfos.productId;
        pDevice->vendorId = deviceInfos.vendorId;
        pDevice->XUSBVersion = deviceInfos.XUSBVersion;
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
        DeviceInfo::Destroy(pDevice);
        pDevice = nullptr;
    }

    return hr;
}

HRESULT ProcessEnumeratedDevice(DeviceInfo_t* pDevice)
{
    DWORD dwUserIndex = 255;
    HRESULT hr;
    BYTE LEDState;

    if (DeviceInfo::IsDeviceInactive(pDevice) == TRUE)
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

HRESULT GetDeviceList(HDEVINFO* phDevInfo)
{
    HRESULT hr;

    *phDevInfo = DriverComm::SetupDiWrapper::GetClassDevs(&Protocol::XUSB_INTERFACE_CLASS_GUID, nullptr, nullptr, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if (*phDevInfo == (HDEVINFO)INVALID_HANDLE_VALUE)
        hr = E_FAIL;
    else
        hr = S_OK;

    return hr;
}

HRESULT GetDeviceInterfaceData(HDEVINFO DeviceInfoSet, DWORD MemberIndex, PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData)
{
    HRESULT hr;

    ZeroMemory(DeviceInterfaceData, sizeof(SP_DEVICE_INTERFACE_DATA));
    DeviceInterfaceData->cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    if (DriverComm::SetupDiWrapper::EnumDeviceInterfaces(DeviceInfoSet, nullptr, &Protocol::XUSB_INTERFACE_CLASS_GUID, MemberIndex, DeviceInterfaceData) == TRUE)
        hr = S_OK;
    else
        hr = E_FAIL;

    return hr;
}

HRESULT GetDeviceInterfaceDetail(HDEVINFO hDeviceInfo, PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData, PSP_DEVICE_INTERFACE_DETAIL_DATA_W* ppDeviceInterfaceDetailData)
{
    DWORD uBytes = g_dwDetailBufferSize;
    *ppDeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)g_pDetailBuffer;
    if (*ppDeviceInterfaceDetailData)
    {
        ZeroMemory(*ppDeviceInterfaceDetailData, g_dwDetailBufferSize);
        (*ppDeviceInterfaceDetailData)->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
    }
    if (DriverComm::SetupDiWrapper::GetDeviceInterfaceDetail(hDeviceInfo, DeviceInterfaceData, *ppDeviceInterfaceDetailData, g_dwDetailBufferSize, &uBytes, nullptr) == TRUE)
    {
        return S_OK;
    }

    *ppDeviceInterfaceDetailData = nullptr;
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
    {
        return E_FAIL;
    }

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
    *ppDeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)g_pDetailBuffer;

    (*ppDeviceInterfaceDetailData)->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
    if (DriverComm::SetupDiWrapper::GetDeviceInterfaceDetail(hDeviceInfo, DeviceInterfaceData, *ppDeviceInterfaceDetailData, g_dwDetailBufferSize, nullptr, nullptr) == FALSE)
    {
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

BOOL CALLBACK DirectSoundEnumerateCallback(PDSPROPERTY_DIRECTSOUNDDEVICE_DESCRIPTION_W_DATA lpData, LPVOID lpUser)
{
    DSoundCallbackParam_t* pParam = (DSoundCallbackParam_t*)lpUser;
    size_t devicePathLen;

    if (lpUser == nullptr)
        return FALSE; // Stop enumeration.

    if (lpData->Type != DIRECTSOUNDDEVICE_TYPE_WDM)
        return TRUE; // Skip this device and continue enumeration.

    devicePathLen = wcslen(pParam->DevicePath);
    if (wcslen(lpData->Interface) < devicePathLen)
        return TRUE; // Skip this device and continue enumeration.
    
    if (CopyMemory(lpData->Interface, pParam->DevicePath, devicePathLen * sizeof(WCHAR)) != 0)
        return TRUE; // Skip this device and continue enumeration.

    if (lpData->DataFlow == DIRECTSOUNDDEVICE_DATAFLOW_RENDER)
    {
        *pParam->HeadphoneGuid = lpData->DeviceId;
    }
    else if (lpData->DataFlow == DIRECTSOUNDDEVICE_DATAFLOW_CAPTURE)
    {
        *pParam->MicrophoneGuid = lpData->DeviceId;
    }

    return TRUE; // Continue enumeration.
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
        ((HLOCAL)g_pDetailBuffer);
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
// DeviceInfo namespace
/////////////////////////////////////////////////////
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

    return Utilities::SafeCopyToUntrustedBuffer(pApiParam->pState, &pDevice->DeviceState, sizeof(*pApiParam->pState));
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
        pApiParam->pState->XinputState.dwPacketNumber = pDevice->DeviceState.XinputState.dwPacketNumber + 1;

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
    if (pDevice)
    {
        // Shouldn't be needed
        ZeroMemory(pDevice, sizeof(DeviceInfo_t));
        pDevice->hDevice = INVALID_HANDLE_VALUE;
        pDevice->hGuideWait = INVALID_HANDLE_VALUE;
        hSourceProcessHandle = GetCurrentProcess();
        if (DuplicateHandle(hSourceProcessHandle, hDevice, hSourceProcessHandle, &pDevice->hDevice, 0, FALSE, DUPLICATE_SAME_ACCESS))
        {
            pDevice->dwDevicePathSize = wcslen(lpDevicePath) + 1;
            pDevice->lpDevicePath = (LPWSTR)Utilities::MemAlloc(pDevice->dwDevicePathSize * sizeof(WCHAR));
            if (pDevice->lpDevicePath)
            {
                if (StringCchCopyW(pDevice->lpDevicePath, pDevice->dwDevicePathSize, lpDevicePath) >= 0)
                    return pDevice;
            }
        }
    }

    if (pDevice)
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
    ZeroMemory(pDevice, sizeof(DeviceInfo_t));
    pDevice->hDevice = hDevice;
    pDevice->hGuideWait = INVALID_HANDLE_VALUE;
    pDevice->lpDevicePath = lpDevicePath;
}

BOOL IsDeviceInactive(DeviceInfo_t* pDevice)
{
    return (pDevice->status & DEVICE_STATUS_ACTIVE) == 0;
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

HRESULT GetAudioDeviceGuids(DeviceInfo_t* pDevice, void* pParams, DWORD reserved)
{
    GetAudioDeviceGuidsApiParam_t* pApiParam = (GetAudioDeviceGuidsApiParam_t*)pParams;
    HRESULT hr;
    DWORD vendorId;
    DWORD productId;
    DWORD inputId;

    IClassFactory* pClassFactory;
    IKsPropertySet* pIKsPropertySet;

    WCHAR devicePath[64];

    DSoundCallbackParam_t CallbackData;
    DSPROPERTY_DIRECTSOUNDDEVICE_ENUMERATE_W_DATA EnumerateData;

    XINPUT_AUDIO_INFORMATION AudioInformation;

    ZeroMemory(pApiParam->pHeadphoneGuid, sizeof(GUID));
    ZeroMemory(pApiParam->pMicrophoneGuid, sizeof(GUID));

    if (IsDeviceInactive(pDevice))
        return E_FAIL;

    vendorId = pDevice->vendorId;
    productId = pDevice->productId;
    inputId = pDevice->inputId;
    if (pDevice->XUSBVersion >= XUSB_VERSION_1_2)
    {
        if (DriverComm::GetAudioDeviceInformation(pDevice, &AudioInformation) < 0)
            return E_FAIL;

        vendorId = AudioInformation.vendorId;
        productId = AudioInformation.productId;
        inputId = AudioInformation.inputId;
        
        if (pDevice->inputId == 0xFF)
            return E_FAIL;
    }
    else
    {
        if((hr = DriverComm::GetLatestDeviceInfo(pDevice)) < 0)
            return hr;

        if (IsDeviceInactive(pDevice))
            return E_FAIL;

        inputId = pDevice->inputId;
        if (pDevice->inputId == 0xFF)
            return S_OK;
    }

    QuickModule quickModule(L"dsound.dll");
    if (!quickModule.IsValid())
        return E_FAIL;

    DllGetClassObject_t* pfn_DllGetClassObject = (DllGetClassObject_t*)quickModule.GetFunctionPointer("DllGetClassObject");
    if (pfn_DllGetClassObject == nullptr)
        return E_FAIL;

    if (StringCchPrintfW(devicePath, sizeof(devicePath)/sizeof(*devicePath), L"\\\\?\\USB#VID_%04X&PID_%04X&IA_%02X", vendorId, productId, inputId) < 0)
        return E_FAIL;

    if ((hr = pfn_DllGetClassObject(&XINPUT_CLSID_DirectSoundPrivate, &XINPUT_IID_IClassFactory, (LPVOID*)&pClassFactory)) < 0)
        return hr;

    pIKsPropertySet = nullptr;
    hr = pClassFactory->CreateInstance(pClassFactory, XINPUT_IID_IKsPropertySet, (LPVOID*)&pIKsPropertySet);
    pClassFactory->Release();
    pClassFactory = nullptr;

    if (hr < 0)
        return hr;

    CallbackData.DevicePath = devicePath;
    CallbackData.HeadphoneGuid = pApiParam->pHeadphoneGuid;
    CallbackData.MicrophoneGuid = pApiParam->pMicrophoneGuid;

    EnumerateData.Callback = &DirectSoundEnumerateCallback;
    EnumerateData.Context = &CallbackData;
    hr = pIKsPropertySet->Get(XINPUT_DSPROPSETID_DirectSoundDevice, 8, nullptr, 0, (LPVOID)&EnumerateData, sizeof(DSPROPERTY_DIRECTSOUNDDEVICE_ENUMERATE_W_DATA), nullptr);

    pIKsPropertySet->Release();
    pIKsPropertySet = nullptr;

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

HRESULT GetDeviceUSBIds(DeviceInfo_t* pDevice, void* pParams, DWORD reserved)
{
    GetDeviceUSBIdsApiParam_t* pApiParam = (GetDeviceUSBIdsApiParam_t*)pParams;

    if (IsDeviceInactive(pDevice))
        return E_FAIL;

    *pApiParam->pVendorId = pDevice->vendorId;
    *pApiParam->pProductId = pDevice->productId;
    *pApiParam->pInputId = pDevice->inputId;

    return S_OK;
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

}

/////////////////////////////////////////////////////
// DeviceList namespace
/////////////////////////////////////////////////////
namespace DeviceList {

HRESULT Initialize()
{
    g_dwDeviceListSize = 0;
    g_pDeviceList = (DeviceInfo_t**)Utilities::MemAlloc(InitialDeviceListSize * sizeof(DeviceInfo_t*));
    if (!g_pDeviceList)
        return E_FAIL;
    
    g_dwDeviceListSize = InitialDeviceListSize;
    return S_OK;
}

HRESULT Close()
{
    if (g_dwDeviceListSize == 0 || g_pDeviceList == nullptr)
        return E_FAIL;

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

        DeviceInfo::Destroy(*ppDevice);
        *ppDevice = nullptr;
    }

    Utilities::MemFree(g_pDeviceList);
    g_pDeviceList = nullptr;
    g_dwDeviceListSize = 0;
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
        DeviceInfo::Destroy(g_pDeviceList[dwUserIndex]);
        g_pDeviceList[dwUserIndex] = nullptr;
    }

    return S_OK;
}

HRESULT GetDeviceOnPort(DWORD dwUserIndex, DeviceInfo_t** ppDevice)
{
    *ppDevice = (dwUserIndex < g_dwDeviceListSize ? g_pDeviceList[dwUserIndex] : nullptr);
    return S_OK;
}

HRESULT IsDevicePresent(DeviceInfo_t* pDevice)
{
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
    g_pfnEnumDeviceInterfaces = nullptr;
    g_pfnGetDeviceInterfaceDetail = nullptr;
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

    }

    return TRUE;
}

HRESULT SendLEDState(DeviceInfo_t* pDevice, BYTE ledState)
{
    InSetState_t inBuffer = {
        pDevice->dwUserIndex,
        ledState,
        0,
        0,
        XUSB_SET_STATE_FLAG_LED
    };

    return SendIoctl(pDevice->hDevice, Protocol::IOCTL_XINPUT_SET_GAMEPAD_STATE, &inBuffer, sizeof(InSetState_t));
}

HRESULT SendDeviceVibration(DeviceInfo_t* pDevice)
{
    InSetState_t inBuffer = {
        pDevice->dwUserIndex,
        0,
        (BYTE)(pDevice->DeviceVibration.wLeftMotorSpeed / 256),
        (BYTE)(pDevice->DeviceVibration.wRightMotorSpeed / 256),
        XUSB_SET_STATE_FLAG_VIBRATION
    };

    return SendIoctl(pDevice->hDevice, Protocol::IOCTL_XINPUT_SET_GAMEPAD_STATE, &inBuffer, sizeof(InSetState_t));
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
        InBaseRequest_t in0101;
    } inBuffer = {};

    DWORD inSize;

    union {
        GamepadState0100 out0100;
        GamepadState0101 out0101;
    } outBuffer = {};

    DWORD outSize;

    if (pDevice->XUSBVersion == XUSB_VERSION_1_0)
    {
        inBuffer.in0100.DeviceIndex = pDevice->dwUserIndex;
        inSize = sizeof(inBuffer.in0100);

        outSize = sizeof(outBuffer.out0100);
    }
    else
    {
        inBuffer.in0101.XUSBVersion = XUSB_VERSION_1_1;
        inBuffer.in0101.DeviceIndex = pDevice->dwUserIndex;
        inSize = sizeof(inBuffer.in0101);

        outSize = sizeof(outBuffer.out0101);
    }

    hr = SendReceiveIoctl(pDevice->hDevice, Protocol::IOCTL_XINPUT_GET_GAMEPAD_STATE, &inBuffer, inSize, &outBuffer, outSize, nullptr);
    if (hr < 0)
        return hr;

    if (pDevice->XUSBVersion == XUSB_VERSION_1_0)
    {
        CopyGamepadStateToDeviceInfo(pDevice, &outBuffer.out0100);
    }
    else
    {
        CopyGamepadStateToDeviceInfo(pDevice, &outBuffer.out0101);
    }

    return S_OK;
}

HRESULT GetCapabilities(DeviceInfo_t* pDevice, XINPUT_CAPABILITIES* pCapabilities)
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
    InBaseRequest_t InBuffer;
    GamepadCapabilities0101 OutBuffer;

    if (pDevice->XUSBVersion == XUSB_VERSION_1_0)
        return Utilities::SafeCopyToUntrustedBuffer(pCapabilities, &s_GamepadCapabilities, sizeof(XINPUT_CAPABILITIES));

    InBuffer.XUSBVersion = XUSB_VERSION_1_1;
    InBuffer.DeviceIndex = pDevice->dwUserIndex;
    ZeroMemory(&OutBuffer, sizeof(GamepadCapabilities0101));

    hr = SendReceiveIoctl(pDevice->hDevice, Protocol::IOCTL_XINPUT_GET_CAPABILITIES, &InBuffer, sizeof(InBaseRequest_t), &OutBuffer, sizeof(GamepadCapabilities0101), nullptr);
    if (hr >= 0)
        TranslateCapabilities(&OutBuffer, pCapabilities);

    return hr;
}

HRESULT GetBatteryInformation(DeviceInfo_t* pDevice, BYTE DeviceType, XINPUT_BATTERY_INFORMATION* pBatteryInformation)
{
    static const GUID null_guid = {};

    HRESULT hr;
    InGamepadBatteryInformation0102 InBuffer;
    GamepadBatteryInformation0102 OutBuffer;
    XINPUT_BATTERY_INFORMATION BatteryInformation;
    GetAudioDeviceGuidsApiParam_t apiParam;
    GUID HeadphoneGuid;
    GUID MicrophoneGuid;

    if (pDevice->XUSBVersion >= XUSB_VERSION_1_2)
    {
        InBuffer.XUSBVersion = XUSB_VERSION_1_2;
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
            apiParam.pHeadphoneGuid = &HeadphoneGuid;
            apiParam.pMicrophoneGuid = &MicrophoneGuid;
            if (DeviceInfo::g_pfnGetAudioDeviceGuidsDispatcher(pDevice, &apiParam, 2) < 0 || IsEqualGUID(HeadphoneGuid, null_guid) != 0)
            {
                BatteryInformation.BatteryType = BATTERY_TYPE_DISCONNECTED;
                BatteryInformation.BatteryLevel = BATTERY_LEVEL_EMPTY;
            }
        }
        hr = Utilities::SafeCopyToUntrustedBuffer(pBatteryInformation, &BatteryInformation, sizeof(XINPUT_BATTERY_INFORMATION));
    }
    return hr;
}

HRESULT GetAudioDeviceInformation(DeviceInfo_t* pDevice, DeviceInfo::XINPUT_AUDIO_INFORMATION* pAudioInformation)
{
    HRESULT hr;
    InBaseRequest_t InBuffer;
    OutGetAudioDeviceInformation_t OutBuffer;

    if (pDevice->XUSBVersion < XUSB_VERSION_1_2)
        return E_FAIL;

    InBuffer.XUSBVersion = XUSB_VERSION_1_2;
    InBuffer.DeviceIndex = pDevice->dwUserIndex;
    hr = SendReceiveIoctl(pDevice->hDevice, Protocol::IOCTL_XINPUT_GET_AUDIO_INFORMATION, &InBuffer, sizeof(InBaseRequest_t), &OutBuffer, sizeof(OutGetAudioDeviceInformation_t), nullptr);
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
    InBaseRequest_t inBuffer;
    OutGetLEDBuffer_t outBuffer;
    HRESULT hr;

    *ledState = Protocol::LEDState::XINPUT_LED_OFF;
    if (pDevice->XUSBVersion < XUSB_VERSION_1_1)
        return S_OK;

    inBuffer.XUSBVersion = XUSB_VERSION_1_1;
    inBuffer.DeviceIndex = pDevice->dwUserIndex;
    hr = SendReceiveIoctl(pDevice->hDevice, Protocol::IOCTL_XINPUT_GET_LED_STATE, &inBuffer, sizeof(InBaseRequest_t), &outBuffer, sizeof(OutGetLEDBuffer_t), nullptr);
    if (hr >= 0)
    {
        *ledState = outBuffer.LEDState;
    }

    return hr;
}

HRESULT PowerOffController(DeviceInfo_t* pDevice)
{
    InBaseRequest_t inBuff;

    if (pDevice->XUSBVersion < XUSB_VERSION_1_2)
        return E_FAIL;

    inBuff.XUSBVersion = XUSB_VERSION_1_2;
    inBuff.DeviceIndex = pDevice->dwUserIndex;
    return SendIoctl(pDevice->hDevice, Protocol::IOCTL_XINPUT_POWER_DOWN_DEVICE, &inBuff, sizeof(InBaseRequest_t));
}

HRESULT WaitForGuideButton(HANDLE hDevice, DWORD dwUserIndex, XINPUT_LISTEN_STATE* pListenState)
{
    HRESULT hr;
    HANDLE hEvent;
    OVERLAPPED overlapped;
    DWORD receivedBytes;
    InBaseRequest_t inBuffer;
    GamepadState0101 outBuffer;

    hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (hEvent == nullptr)
        return E_OUTOFMEMORY;

    overlapped.Internal = 0;
    overlapped.InternalHigh = 0;
    overlapped.Offset = 0;
    overlapped.OffsetHigh = 0;
    overlapped.hEvent = hEvent;

    inBuffer.XUSBVersion = XUSB_VERSION_1_2;
    inBuffer.DeviceIndex = (BYTE)dwUserIndex;

    ZeroMemory(&outBuffer, sizeof(GamepadState0101));

    hr = SendReceiveIoctl(hDevice, Protocol::IOCTL_XINPUT_WAIT_FOR_GUIDE_BUTTON, &inBuffer, sizeof(InBaseRequest_t), &outBuffer, sizeof(GamepadState0101), &overlapped);
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
        if (receivedBytes == sizeof(GamepadState0101))
        {
            if (outBuffer.status == 1)
            {
                pListenState->Status = 0;
                pListenState->State.dwPacketNumber   = outBuffer.dwPacketNumber;
                pListenState->State.Gamepad.wButtons = outBuffer.wButtons;
                pListenState->State.Gamepad.bLeftTrigger  = outBuffer.bLeftTrigger;
                pListenState->State.Gamepad.bRightTrigger = outBuffer.bRightTrigger;
                pListenState->State.Gamepad.sThumbLX = outBuffer.sThumbLX;
                pListenState->State.Gamepad.sThumbLY = outBuffer.sThumbLY;
                pListenState->State.Gamepad.sThumbRX = outBuffer.sThumbRX;
                pListenState->State.Gamepad.sThumbRY = outBuffer.sThumbRY;
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

HDEVINFO GetClassDevs(const GUID* ClassGuid, PCWSTR Enumerator, HWND hwndParent, DWORD Flags)
{
    if (g_pfnGetClassDevs)
        return g_pfnGetClassDevs(ClassGuid, Enumerator, hwndParent, Flags);

    return SetupDiGetClassDevsW(ClassGuid, Enumerator, hwndParent, Flags);
}

BOOL EnumDeviceInterfaces(HDEVINFO DeviceInfoSet, PSP_DEVINFO_DATA DeviceInfoData, const GUID* InterfaceClassGuid, DWORD MemberIndex, PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData)
{
    if (g_pfnEnumDeviceInterfaces)
        return g_pfnEnumDeviceInterfaces(DeviceInfoSet, DeviceInfoData, InterfaceClassGuid, MemberIndex, DeviceInterfaceData);

    return SetupDiEnumDeviceInterfaces(DeviceInfoSet, DeviceInfoData, InterfaceClassGuid, MemberIndex, DeviceInterfaceData);
}

BOOL GetDeviceInterfaceDetail(HDEVINFO DeviceInfoSet, PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData, PSP_DEVICE_INTERFACE_DETAIL_DATA_W DeviceInterfaceDetailData, DWORD DeviceInterfaceDetailDataSize, PDWORD RequiredSize, PSP_DEVINFO_DATA DeviceInfoData)
{
    if (g_pfnGetDeviceInterfaceDetail)
        return g_pfnGetDeviceInterfaceDetail(DeviceInfoSet, DeviceInterfaceData, DeviceInterfaceDetailData, DeviceInterfaceDetailDataSize, RequiredSize, DeviceInfoData);

    return SetupDiGetDeviceInterfaceDetailW(DeviceInfoSet, DeviceInterfaceData, DeviceInterfaceDetailData, DeviceInterfaceDetailDataSize, RequiredSize, DeviceInfoData);;
}

BOOL DestroyDeviceInfoList(HDEVINFO DeviceInfoSet)
{
    if (g_pfnDestroyDeviceInfoList )
        return g_pfnDestroyDeviceInfoList(DeviceInfoSet);
    
    return SetupDiDestroyDeviceInfoList(DeviceInfoSet);
}

}//namespace SetupDiWrapper

}

/////////////////////////////////////////////////////
// Utilities namespace
/////////////////////////////////////////////////////
namespace Utilities {

HRESULT Initialize()
{
    g_dwSettings = SET_USER_LED_ON_CREATE | DISABLE_USER_LED_ON_DESTROY | DISABLE_VIBRATION_ON_DESTROY;
    return S_OK;
}

HRESULT Close()
{
    return S_OK;
}

LPVOID MemAlloc(DWORD size)
{
    return LocalAlloc(LMEM_ZEROINIT, size);
}

void MemFree(LPVOID lpMem)
{
    if (lpMem != nullptr)
    {
        LocalFree(lpMem);
    }
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

    if (Utilities::Initialize() < 0)
        return FALSE;

    if (DeviceList::Initialize() < 0)
    {
        Utilities::Close();
        return FALSE;
    }

    if (DeviceEnum::Initialize() < 0)
    {
        DeviceList::Close();
        Utilities::Close();
        return FALSE;
    }

    if (DriverComm::Initialize() < 0)
    {
        DeviceEnum::Close();
        DeviceList::Close();
        Utilities::Close();
        return FALSE;
    }

    EnableCommunications(TRUE);
    g_IsInitialized = TRUE;

    return TRUE;
}

BOOL Close()
{
    g_IsInitialized = FALSE;
    EnableCommunications(FALSE);

    Utilities::Close();
    DeviceList::Close();
    DeviceEnum::Close();
    DriverComm::Close();

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

HRESULT ProcessAPIRequest(DWORD dwUserIndex, HRESULT(*pfn_ApiAction)(DeviceInfo_t*, void*, DWORD), void* pApiParam, DWORD reserved)
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
        hr = DeviceEnum::GetDeviceOnPort(dwUserIndex, &pDevice, g_IsCommunicationEnabled);
        if (hr < 0)
            break;

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

        if (!DeviceInfo::IsDeviceInactive(pDevice))
            break;

        pDevice = nullptr;
        hr = DeviceList::RemoveDeviceFromPort(dwUserIndex);

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
    DeviceInfo::OnEnableSettingChanged(bEnabled);

    for (DWORD dwUserIndex = 0; dwUserIndex < XUSER_MAX_COUNT; ++dwUserIndex)
    {
        pDevice = nullptr;
        if (DeviceEnum::GetDeviceOnPort(dwUserIndex, &pDevice, false) >= 0 && pDevice != nullptr)
        {
            SetStateApiParam_t apiParam;
            apiParam.pVibration = nullptr;
            DeviceInfo::g_pfnSetVibrationDispatcher(pDevice, &apiParam, 1);
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

    OPENXINPUT_STATE_FULL gamepadState;
    GetStateApiParam_t apiParam;
    DWORD virtualKey;
    int key;
    DWORD pressedTriggers;
    DWORD dwTickCount;

    apiParam.pState = &gamepadState;
    if (DeviceInfo::g_pfnGetStateDispatcher(pDevice, &apiParam, 1) < 0)
        return ERROR_EMPTY;

    gamepadState.XinputState.Gamepad.wButtons &= XINPUT_BUTTON_MASK_WITHOUT_GUIDE;

    pKeystroke->UserIndex = bUserIndex;
    pKeystroke->Unicode = 0;

    key = 0;

    virtualKey = Controller_CalculateKeyFromThumbPos(VK_PAD_LTHUMB_UP, gamepadState.XinputState.Gamepad.sThumbLX, gamepadState.XinputState.Gamepad.sThumbLY);
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

    virtualKey = Controller_CalculateKeyFromThumbPos(VK_PAD_RTHUMB_UP, gamepadState.XinputState.Gamepad.sThumbRX, gamepadState.XinputState.Gamepad.sThumbRY);

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
    if (gamepadState.XinputState.Gamepad.bLeftTrigger > 30u)
        pressedTriggers = 1;
    if (gamepadState.XinputState.Gamepad.bRightTrigger > 30u)
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
            if (!(gamepadState.XinputState.Gamepad.wButtons & keyBit))
            {
                pDevice->wButtons &= ~keyBit;
                pKeystroke->VirtualKey = (WORD)virtualKey;
                pKeystroke->Flags = XINPUT_KEYSTROKE_KEYUP;
                s_KeyCode = 0;
                return ERROR_SUCCESS;
            }
            pKeystroke->VirtualKey = (WORD)virtualKey;
        }
        else if (gamepadState.XinputState.Gamepad.wButtons & keyBit)
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
            res = XInputCore::Close();
            break;

        case DLL_PROCESS_ATTACH:
            res = XInputCore::Initialize();
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
    XInputCore::Close();
}

#endif

DWORD WINAPI OpenXInputGetState(_In_ DWORD dwUserIndex, _Out_ XINPUT_STATE* pState)
{
    DWORD result;
    result = OpenXInputGetStateEx(dwUserIndex, pState);

    if (result == ERROR_SUCCESS)
        pState->Gamepad.wButtons &= XINPUT_BUTTON_MASK_WITHOUT_GUIDE;

    return result;
}

DWORD WINAPI OpenXInputSetState(_In_ DWORD dwUserIndex, _In_ XINPUT_VIBRATION* pVibration)
{
    SetStateApiParam_t apiParam;
    HRESULT hr;

    if (dwUserIndex >= XUSER_MAX_COUNT || pVibration == nullptr)
        return ERROR_BAD_ARGUMENTS;

    apiParam.pVibration = pVibration;

    hr = XInputCore::ProcessAPIRequest(dwUserIndex, DeviceInfo::g_pfnSetVibrationDispatcher, &apiParam, 1);
    return XInputReturnCodeFromHRESULT(hr);
}

DWORD WINAPI OpenXInputGetCapabilities(_In_ DWORD dwUserIndex, _In_ DWORD dwFlags, _Out_ XINPUT_CAPABILITIES* pCapabilities)
{
    GetCapabilitiesApiParam_t apiParam;
    DWORD result;
    HRESULT hr;
    if (dwUserIndex >= XUSER_MAX_COUNT || (dwFlags != 0 && dwFlags != XINPUT_CAPS_FFB_SUPPORTED) || pCapabilities == nullptr)
        return ERROR_BAD_ARGUMENTS;

    apiParam.pCapabilities = pCapabilities;

    hr = XInputCore::ProcessAPIRequest(dwUserIndex, DeviceInfo::g_pfnGetCapabilitiesDispatcher, &apiParam, 1);
    result = XInputReturnCodeFromHRESULT(hr);

    if (result == ERROR_SUCCESS)
        pCapabilities->Gamepad.wButtons &= XINPUT_BUTTON_MASK_WITHOUT_GUIDE;

    return result;
}

void WINAPI OpenXInputEnable(_In_ BOOL enable)
{
    XInputCore::EnableCommunications(enable != FALSE);
}

DWORD WINAPI OpenXInputGetDSoundAudioDeviceGuids(_In_ DWORD dwUserIndex, _Out_ GUID* pDSoundRenderGuid, _Out_ GUID* pDSoundCaptureGuid)
{
    HRESULT hr;
    DWORD result;
    bool doApiCall;
    bool run;
    HANDLE hDevice;
    DeviceInfo_t device;
    GetAudioDeviceGuidsApiParam_t apiParam;

    if (dwUserIndex >= XUSER_MAX_COUNT || pDSoundRenderGuid == nullptr || pDSoundCaptureGuid == nullptr)
        return ERROR_BAD_ARGUMENTS;

    apiParam.pHeadphoneGuid = pDSoundRenderGuid;
    apiParam.pMicrophoneGuid = pDSoundCaptureGuid;

    doApiCall = true;
    result = ERROR_DEVICE_NOT_CONNECTED;
    QuickDriverEnum quickEnum;
    run = quickEnum.Restart();
    while (run)
    {
        hDevice = INVALID_HANDLE_VALUE;
        run = quickEnum.GetNext(&hDevice);
        if (hDevice != INVALID_HANDLE_VALUE)
        {
            if (quickEnum.MinFillFromInterface(hDevice, &device) && device.XUSBVersion == XUSB_VERSION_1_2)
            {
                device.dwUserIndex = (BYTE)dwUserIndex;
                if (DeviceInfo::g_pfnGetAudioDeviceGuidsDispatcher(&device, &apiParam, 2) >= 0)
                {
                    doApiCall = false;
                    run = false;
                    result = ERROR_SUCCESS;
                }
            }
            CloseHandle(hDevice);
        }
    }
    if (doApiCall == 1)
    {
        hr = XInputCore::ProcessAPIRequest(dwUserIndex, DeviceInfo::g_pfnGetAudioDeviceGuidsDispatcher, &apiParam, 2u);
        result = XInputReturnCodeFromHRESULT(hr);
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
                if (quickEnum.MinFillFromInterface(hDevice, &device) && device.XUSBVersion >= XUSB_VERSION_1_2)
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
        hr = XInputCore::ProcessAPIRequest(dwUserIndex, DeviceInfo::g_pfnGetBatteryInformationDispatcher, &apiParam, 2);
        result = XInputReturnCodeFromHRESULT(hr);
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

    apiParam.lpReserved = &dwReserved;
    apiParam.pKeystroke = pKeystroke;
    apiParam.lpLastError = &result;
    if (dwUserIndex < XUSER_MAX_COUNT)
    {
        result = ERROR_SUCCESS;
        hr = XInputCore::ProcessAPIRequest(dwUserIndex, DeviceInfo::g_pfnGetKeystrokeDispatcher, &apiParam, 3);
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
            hr = XInputCore::ProcessAPIRequest(i, DeviceInfo::g_pfnGetKeystrokeDispatcher, &apiParam, 3);
            if (result == ERROR_SUCCESS)
            {
                pKeystroke->UserIndex = i;
                break;
            }
        }
        if (result != ERROR_SUCCESS)
            result = ERROR_EMPTY;
    }

    return result;
}

DWORD WINAPI OpenXInputGetStateEx(_In_ DWORD dwUserIndex, _Out_ XINPUT_STATE* pState)
{
    DWORD result;
    OPENXINPUT_STATE_FULL full_state;

    if (dwUserIndex >= XUSER_MAX_COUNT || pState == nullptr)
        return ERROR_BAD_ARGUMENTS;

    result = OpenXInputGetStateFull(dwUserIndex, &full_state);

    if (result == ERROR_SUCCESS)
    {
        memcpy(pState, &full_state.XinputState, sizeof(XINPUT_STATE));
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

    hr = XInputCore::ProcessAPIRequest(dwUserIndex, DeviceInfo::g_pfnWaitForGuideButton, &apiParam, 2);
    return XInputReturnCodeFromHRESULT(hr);
}

DWORD WINAPI OpenXInputCancelGuideButtonWait(_In_ DWORD dwUserIndex)
{
    HRESULT hr;

    if (dwUserIndex >= XUSER_MAX_COUNT)
        return ERROR_BAD_ARGUMENTS;

    hr = XInputCore::ProcessAPIRequest(dwUserIndex, DeviceInfo::g_pfnCancelGuideButtonWait, nullptr, 0);
    return XInputReturnCodeFromHRESULT(hr);
}

DWORD WINAPI OpenXInputPowerOffController(_In_ DWORD dwUserIndex)
{
    HRESULT hr;

    if (dwUserIndex >= XUSER_MAX_COUNT)
        return ERROR_BAD_ARGUMENTS;

    hr = XInputCore::ProcessAPIRequest(dwUserIndex, DeviceInfo::g_pfnPowerOffController, nullptr, 0);
    return XInputReturnCodeFromHRESULT(hr);

}

DWORD WINAPI OpenXInputGetMaxControllerCount()
{
    return XUSER_MAX_COUNT;
}

DWORD WINAPI OpenXInputGetDeviceUSBIds(DWORD dwUserIndex, WORD* pVendorId, WORD* pProductId, WORD* pInputId)
{
    DWORD result;
    HRESULT hr;
    GetDeviceUSBIdsApiParam_t apiParam;

    if (dwUserIndex >= XUSER_MAX_COUNT || pVendorId == nullptr || pProductId == nullptr || pInputId == nullptr)
        return ERROR_BAD_ARGUMENTS;

    apiParam.pVendorId = pVendorId;
    apiParam.pProductId = pProductId;
    apiParam.pInputId = pInputId;

    hr = XInputCore::ProcessAPIRequest(dwUserIndex, DeviceInfo::GetDeviceUSBIds, &apiParam, 1);
    result = XInputReturnCodeFromHRESULT(hr);

    return result;
}

DWORD WINAPI OpenXInputGetStateFull(DWORD dwUserIndex, OPENXINPUT_STATE_FULL* pState)
{
    DWORD result;
    HRESULT hr;
    GetStateApiParam_t apiParam;

    if (dwUserIndex >= XUSER_MAX_COUNT || pState == nullptr)
        return ERROR_BAD_ARGUMENTS;

    apiParam.pState = pState;

    hr = XInputCore::ProcessAPIRequest(dwUserIndex, DeviceInfo::g_pfnGetStateDispatcher, &apiParam, 1);
    result = XInputReturnCodeFromHRESULT(hr);

    return result;
}


#ifdef __cplusplus
}
#endif