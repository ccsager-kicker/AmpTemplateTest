
Config ToDo:
    - strip unnecessary code to create bare-bones template.
    - separate embedded libraries into 32-bit and other.
    - create repo for Sigma, and AKM DSPs.
    - try and create repo for Harmony USB and use as git submodule.
    - add libraries as git submodules.
    - add configurations for bootloader.
    - add bootloader as git submodule.
    - Pass as much data as possible as pointers.

ReadMe ToDo: 
    - Change microcontroller.
    - Update git submodules.
        - KickerUtils
    - Instructions for generating a hexmate CRC post-build command.
    - Instructions for linking the bootloader.


- Timer 2 is occupied by USB. Do not try to use or make adjustments to Timer 2.

- Place microcontroller drivers in the McuDrivers folder. This folder is already included in the compiler search path for ease of use.

- Rename the project in MPLAB X by right-clicking the project in the Projects window.
- Rename the project's .X directory under firmware/

- Complete and review TODOs by opening the "Action Items" window located in the menu bar under: Window > Action Items.

- Do not forget to add new directories to the search path.
    - Right click on the project in the 'Projects' window and at the bottom click on 'Properties.'
    - Navigate to your desired configuration, and select xc32-gcc.
    - From the drop-down menu 'Option Categories' select 'Preprocessing and Messages.'
    - In the updated window below the drop-down menu, click on the ellipses (...) next to 'Search Directories.'
    - Click 'Browse...' at the top of the new window and navigate to the directory you wish to include.

- After you select a programming/debug tool in the configuration, make sure to unfreeze the CCP peripherals during debug. If a CCP peripheral freezes during a debug 
  breakpoint, your POWER SUPPLIES will SHORT.