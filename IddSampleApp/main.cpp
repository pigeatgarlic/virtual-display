#include <iostream>
#include <vector>

#include <windows.h>
#include <swdevice.h>
#include <conio.h>
#include <wrl.h>

VOID WINAPI
CreationCallback(
    _In_ HSWDEVICE hSwDevice,
    _In_ HRESULT hrCreateResult,
    _In_opt_ PVOID pContext,
    _In_opt_ PCWSTR pszDeviceInstanceId
    )
{
    HANDLE hEvent = *(HANDLE*) pContext;

    SetEvent(hEvent);
    UNREFERENCED_PARAMETER(hSwDevice);
    UNREFERENCED_PARAMETER(hrCreateResult);
    UNREFERENCED_PARAMETER(pszDeviceInstanceId);
}

int __cdecl main(int argc, wchar_t *argv[])
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    HANDLE hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    HSWDEVICE hSwDevice;
    SW_DEVICE_CREATE_INFO createInfo = { 0 };
    PCWSTR description = L"Idd Sample Driver";

    int result;
	int deviceIndex=0;

    // These match the Pnp id's in the inf file so OS will load the driver when the device is created    
    PCWSTR instanceId = L"IddSampleDriver";
    PCWSTR hardwareIds = L"IddSampleDriver\0\0";
    PCWSTR compatibleIds = L"IddSampleDriver\0\0";

    createInfo.cbSize = sizeof(createInfo);
    createInfo.pszzCompatibleIds = compatibleIds;
    createInfo.pszInstanceId = instanceId;
    createInfo.pszzHardwareIds = hardwareIds;
    createInfo.pszDeviceDescription = description;

    createInfo.CapabilityFlags = SWDeviceCapabilitiesRemovable |
                                 SWDeviceCapabilitiesSilentInstall |
                                 SWDeviceCapabilitiesDriverRequired;

    // Create the device
    HRESULT hr = SwDeviceCreate(L"IddSampleDriver",
                                L"HTREE\\ROOT\\0",
                                &createInfo,
                                0,
                                nullptr,
                                CreationCallback,
                                &hEvent,
                                &hSwDevice);
    if (FAILED(hr))
    {
        printf("SwDeviceCreate failed with 0x%lx\n", hr);
        return 1;
    }

    // Wait for callback to signal that the device has been created
    printf("Waiting for device to be created....\n");
    DWORD waitResult = WaitForSingleObject(hEvent, 10*1000);
    if (waitResult != WAIT_OBJECT_0) {
        printf("Wait for device creation failed\n");
        return 1;
    }
    printf("Device created successfully\n");
    Sleep(1000);

    int Width = 0;
    int Height = 0;
    char* width = nullptr;
    char* height = nullptr;
    size_t sz = 0;
    _dupenv_s(&width,  &sz, "DISPLAY_WIDTH");
    _dupenv_s(&height, &sz, "DISPLAY_HEIGHT");
    if(width == nullptr || height == nullptr) {
        printf("Set to default resolution\n");
        Width  = 1920;
        Height = 1080;
    } else {
        Width  = atoi(width);
        Height = atoi(height);
    }

	do
	{
		PDISPLAY_DEVICE displayDevice = new DISPLAY_DEVICE();
		displayDevice->cb = sizeof(DISPLAY_DEVICE);

		result = EnumDisplayDevices(NULL, 
			deviceIndex++, displayDevice, 0);
		if (displayDevice->StateFlags & DISPLAY_DEVICE_ACTIVE)
		{
			PDISPLAY_DEVICE monitor = new DISPLAY_DEVICE();
			monitor->cb = sizeof(DISPLAY_DEVICE);

            if(wcscmp(displayDevice->DeviceString,L"IddSampleDriver Device"))
                continue;

			EnumDisplayDevices(displayDevice->DeviceName, 
				0, monitor, 0);
			
			PDEVMODE dm = new DEVMODE();
			if ( EnumDisplaySettings(displayDevice->DeviceName,
				ENUM_CURRENT_SETTINGS, dm) )
			{

                dm->dmPelsWidth  = Width;
                dm->dmPelsHeight = Height;
                printf("changing resolution of display %ls to %dx%d \n",displayDevice->DeviceString,Width,Height);
				ChangeDisplaySettingsEx(displayDevice->DeviceName, dm,  \
                                     NULL, (CDS_GLOBAL | CDS_UPDATEREGISTRY | CDS_RESET), NULL);
			}
		}
	} while (result);




	do
	{
        result = 0;
        deviceIndex = 0;
		PDISPLAY_DEVICE displayDevice = new DISPLAY_DEVICE();
		displayDevice->cb = sizeof(DISPLAY_DEVICE);

        bool found = false;
        do
        {
            result = EnumDisplayDevices(NULL, 
                deviceIndex++, displayDevice, 0);
            if (displayDevice->StateFlags & DISPLAY_DEVICE_ACTIVE)
            {
                PDISPLAY_DEVICE monitor = new DISPLAY_DEVICE();
                monitor->cb = sizeof(DISPLAY_DEVICE);


                if(!wcscmp(displayDevice->DeviceString,L"IddSampleDriver Device"))
                    found = true;
            }
	    } while (result);
        if (!found)
            break;

        Sleep(1000);
	} while (true);
    printf("virtual display closed by user \n");

    
    // Stop the device, this will cause the sample to be unloaded
    SwDeviceClose(hSwDevice);
    return 0;
}