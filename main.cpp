#include <Windows.h>
#include <iostream>

struct SharedData {
    bool buttonClicked;
    int buttonID;
};

int main()
{
    // Create shared memory for communication
    HANDLE sharedMemory = ::CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SharedData), L"SharedData");
    if (sharedMemory == NULL)
    {
        std::cerr << "Failed to create shared memory." << std::endl;
        return 1;
    }

    // Inject the DLL (omitted here, refer to previous examples)

    // Check for button click events in a loop
    SharedData* sharedData = (SharedData*)::MapViewOfFile(sharedMemory
    // Check for button click events in a loop
    SharedData* sharedData = (SharedData*)::MapViewOfFile(sharedMemory, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedData));
    if (sharedData != nullptr)
    {
        while (true)
        {
            if (sharedData->buttonClicked)
            {
                std::cout << "Button with ID " << sharedData->buttonID << " clicked." << std::endl;

                // Handle button click event here, e.g., by asking the user which button was clicked
                // and finding the function pointer based on the button ID

                // Reset the button click event
                sharedData->buttonClicked = false;
            }

            Sleep(100); // Sleep for a while to avoid excessive CPU usage
        }

        ::UnmapViewOfFile(sharedData);
    }

    ::CloseHandle(sharedMemory);

    return 0;
}
