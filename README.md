# SADS

## Arduino IDE
* Download from https://www.arduino.cc/en/Main/Software
* Install software
* Open Arduino IDE => Files => Preferences
* Additional Boards Manager URLs: http://arduino.esp8266.com/stable/package_esp8266com_index.json
* Open Tools => Board xx => Boards Manager ...
* Find esp8266 by ESP8266 Community and Install version 2.5.0 (versions in dropdown)
* Connect NodeMCU board to you computer
* Connect Light Sensor based on schematics https://puu.sh/D9oFG/f33b3da7f0.png
* In the Tools menu set following properties:
    * Board: NodeMCU 1.0 (ESP-12E Module)
    * Upload Speed: 115200
    * CPU Frequency: 80Mhz
    * Flash size: 4M (1M SPIFFS)
    * Debug port: "Disabled"
    * Debug level: "None"
    * IwIP Variant: v2 Lower Memory
    * VTables: Flash
    * Erase Flash: Only Sketch
    * Port: Port on which your connected device shows up (Open Device Manager, and check COM & LPT)
* Compile sketch
* Upload it to the board
* Open logs Tools => Serial monitor (CTRL + SHIFT + M). Baud 115200

* Update wifi credentials (SSID, password)
* Head over to loop function and do your magic

