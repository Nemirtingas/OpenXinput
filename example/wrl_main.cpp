#include <roapi.h>
#include <wrl.h>
#include <windows.gaming.input.h>
#include <cassert>

namespace WRL = Microsoft::WRL;
namespace AWF = ABI::Windows::Foundation;
namespace AWFC = ABI::Windows::Foundation::Collections;
namespace WGI = ABI::Windows::Gaming::Input;

using namespace Microsoft::WRL::Wrappers;

#pragma comment(lib, "runtimeobject.lib")

#include <list>
#include <mutex>

struct Gamepad_t
{
    WRL::ComPtr<WGI::IGamepad> gamepad;
    WGI::GamepadReading oldReading;
    WGI::GamepadReading Reading;
};

std::list<Gamepad_t> gamepads;

std::mutex gamepads_mutex;

HRESULT OnGamepadAdded(IInspectable*, WGI::IGamepad* pGamepad)
{
    std::lock_guard<std::mutex> lk(gamepads_mutex);
    for (auto it = gamepads.begin(); it != gamepads.end(); ++it)
    {
        if (it->gamepad.Get() == pGamepad)
            return S_OK;
    }

    gamepads.emplace_back(Gamepad_t{ pGamepad, {}, {} });

    return S_OK;
}

HRESULT OnGamepadRemoved(IInspectable*, WGI::IGamepad* pGamepad)
{
    std::lock_guard<std::mutex> lk(gamepads_mutex);
    for (auto it = gamepads.begin(); it != gamepads.end(); ++it)
    {
        if (it->gamepad.Get() == pGamepad)
        {
            gamepads.erase(it);
            break;
        }
    }

    return S_OK;
}

auto g_oGamepadAddedHandler = WRL::Callback<AWF::IEventHandler<WGI::Gamepad*>>(OnGamepadAdded);
auto g_oGamepadRemovedHandler = WRL::Callback<AWF::IEventHandler<WGI::Gamepad*>>(OnGamepadRemoved);

EventRegistrationToken	oOnControllerAddedToken;
EventRegistrationToken	oOnControllerRemovedToken;

void test_gaming_input()
{
    auto hr = RoInitialize(RO_INIT_MULTITHREADED);
    assert(SUCCEEDED(hr));

    WRL::ComPtr<WGI::IGamepadStatics> gamepadStatics;
    hr = RoGetActivationFactory(HStringReference(L"Windows.Gaming.Input.Gamepad").Get(), __uuidof(WGI::IGamepadStatics), &gamepadStatics);
    assert(SUCCEEDED(hr));

    oOnControllerAddedToken = { 0 };
    oOnControllerRemovedToken = { 0 };

    gamepadStatics->add_GamepadAdded(g_oGamepadAddedHandler.Get(), &oOnControllerAddedToken);
    gamepadStatics->add_GamepadRemoved(g_oGamepadRemovedHandler.Get(), &oOnControllerRemovedToken);

    while (1)
    {
        {
            std::lock_guard<std::mutex> lk(gamepads_mutex);
            for (auto& gamepad : gamepads)
            {

                hr = gamepad.gamepad->GetCurrentReading(&gamepad.Reading);
                if (memcmp(&gamepad.oldReading, &gamepad.Reading, sizeof(WGI::GamepadReading)) != 0)
                {
                    gamepad.oldReading = gamepad.Reading;
                }
            }
        }
    }

    gamepadStatics->remove_GamepadAdded(oOnControllerAddedToken);
    gamepadStatics->remove_GamepadRemoved(oOnControllerRemovedToken);

}

int main(int argc, char* argv[])
{
    test_gaming_input();
    return 0;
}