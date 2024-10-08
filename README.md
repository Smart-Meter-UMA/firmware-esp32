<p align="center">
<img src="./logoblack.png" width="100" align="center">
<br>
Accesible Smart Meters for Home  
</p>

# ESP32 Firmware

Firmware for ESP-32 based in Arduino over Platform-io for K-project Smart meter project (Link UMA Atech)

More information about this project can be found at this [link.](http://ertis.uma.es/k-project/)

## What we can expect of this firmware?

Not a lot right now, we are just starting!

- You can see the values measured by the sensors using BLE (that can be accesed with an app like [nRF Connect](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp&hl=es_419&gl=US) or a custom Android/Iphone App).


## How to run it!?

1. Clone this repository where you want (and can).
2. Open Platform.IO (information about how to install it can be found [here](https://docs.platformio.org/en/latest//integration/ide/vscode.html#installation))
3. Select "Open Project".
4. Choose the folder where you clone the repository. The project will be initialized and ask you if you trust the authors of the project. [Trust us, we are engineers!](https://www.youtube.com/watch?v=rp8hvyjZWHs)
5. This project was made to use the board "ESP32 Devkit V4" (like az-delivery-devkit-v4 board). Maybe it is necessary to choose another board if is different of the available board. You can find a list of boards [here](https://docs.platformio.org/en/latest/boards/). You can choose your board or a compatible one, it is not needed to get the exact model. After choose your board, you can find examples to change the platform.io file.
6. Upload the firmware (right arrow icon in the lower status bar of Visual Studio Code) and enjoy!
7. Optionally, you can open the serial monitor (outlet icon near the upload icon) to see more information from the ESP32 like errors, information about his flow of work, the data that is being handled...
