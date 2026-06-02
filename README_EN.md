This is an automatic translation and may be incorrect in some places. See the source README and examples for authoritative information.

[![latest](https://img.shields.io/github/v/release/GyverLibs/FileData.svg?color=brightgreen)](https://github.com/GyverLibs/FileData/releases/latest/download/FileData.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/FileData.svg)](https://registry.platformio.org/libraries/gyverlibs/FileData)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/FileData?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# FileData
Replace EEPROM for ESP8266/32 to store any data in files
- Automatic "flag" mechanism of the first entry
- Support for all file systems (LittleFS, SPIFFS, SDFS)
- Support for all types of static data
- Delayed timeout recording
- “Update” data – the file will not be overwritten unless the data has changed

> Note: The library stores **binary**, i.e. unreadable data. If you want to store data in a file in a readable format, use the library.[PairsFile](https://github.com/GyverLibs/Pairs)

### Compatibility
ESP8266, ESP32

## Contents
- [Installation](#install)
- [Initialization](#init)
- [Use of use](#usage)
- [Example](#example)
- [Versions](#versions)
- [Bugs and feedback](#feedback)

<a id="install"></a>
## Installation
- The library can be found under the name **FileData** and installed through the library manager in:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Download the library](https://github.com/GyverLibs/FileData/archive/refs/heads/main.zip).zip archive for manual installation:
    - Unpack and put in *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Unpack and put in *C:\Program Files\Arduino\libraries* (Windows x32)
    - Unpack and put in *Documents/Arduino/libraries/ *
    - (Arduino IDE) Automatic installation from .zip: *Sketch/Connect library/Add .ZIP library...* and specify downloaded archive
- Read more detailed instructions for installing libraries[here](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Update
- I recommend always updating the library: new versions fix errors and bugs, as well as optimize and add new features.
- Through the library manager IDE: find the library as when installing and click "Update"
- Manually: **Delete the folder with the old version** and then put the new one in its place. “Replacement” can not be done: sometimes new versions delete files that will remain when replaced and can lead to errors!

<a id="init"></a>
## Initialization

```cpp
FileData;
FileData(fs::FS* fs);
FileData(fs::FS* fs, const char* path);
FileData(fs::FS* fs, const char* path, uint8_t key);
FileData(fs::FS* fs, const char* path, uint8_t key, void* data, uint16_t size);
FileData(fs::FS* fs, const char* path, uint8_t key, void* data, uint16_t size, uint16_t tout);

// fs - file system, address (&LittleFS, &SDFS.)
// path is the path (name) of the file. It can be any extension ("/myData", "/data/settings.dat")
// The key is the first entry key. It is not recommended to set 0 and 255. It is recommended to use symbols ('A', 'F')
// Data - reference to a variable (array, structure, class)
// Size is the size of a variable, which can be translated as sizeof
// tout - update timeout in milliseconds (silent 5000)
```

<a id="usage"></a>
## Use of use

In ESP8266/32 EEPROM, memory is emulated from Flash memory, and the implementation of EEPROM in the embedded library has the following disadvantages:
- Size limited to 4 kB
- All of the`EEPROM.begin()`The amount of memory is duplicated in RAM before the call`EEPROM.end()`
- Any change in EEPROM memory (even 1 byte) and call`EEPROM.commit()`The entire 4kB sector is completely erased and overwritten. That is, memory wear does not occur on cells, but completely throughout the EEPROM area! About 10-20 thousand rewrites of data and EEPROM memory are no longer

It is proposed to use a file system for data storage (for example, the built-in LittleFS), which itself takes care of memory overwriting and rotates files over the designated area, which greatly increases the memory resource and data storage reliability. It will also allow you to "download" stored data, make backups and so on. This library is an analogue of[EEManager](https://github.com/GyverLibs/EEManager)and has similar mechanisms and capabilities:
- “Connect” static variables of any type, the library will read and write their contents into a file.
- The mechanism of the "first start key" - if the file does not exist or does not contain the specified key - the file will be written "by default"
- The mechanism of delayed recording by timeout - after changing the data, it is enough to give the library a command to update, and it will update the data after the timeout has expired

```cpp
// install file system and file path
void setFS(fs::FS* nfs, const char* path);

// key
void setKey(uint8_t key);

// connect data (variable)
void setData(void* data, uint16_t size);

// timeline
void setTimeout(uint16_t tout);

// variableize
// return: FD FS ERR/FD FILE ERR/FD WRITE/FD ADD/FD READ
FDstat_t read();

// update
// FD FS ERR/FD FILE ERR/FD WRITE/FD NO DIF
FDstat_t updateNow();

// postpone the update to a given timeout
void update();

// ticker, update timeout data
// return: FD FS ERR/FD FILE ERR/FD WRITE/FD NO DIF/FD WAIT/FD IDLE
FDstat_t tick();

// file
// Return: FD FS ERR/FD FILE ERR/FD WRITE
FDstat_t write();

// drop off
// Return: FD FS ERR/FD FILE ERR/FD RESET
FDstat_t reset();

// enable data augmentation mode with addition to file without cleaning
void addWithoutWipe(bool addw);

// =====================================================
FD_IDLE      // 0 - single work
FD_WAIT      // 1 - waiting timeout
FD_FS_ERR    // 2 - File system error
FD_FILE_ERR  // 3 - file opening error
FD_WRITE     // 4 - recording data into a file
FD_READ      // 5 - reading data from the file
FD_ADD       // 6 - Adding data to the file
FD_NO_DIF    // 7 - data are not different (not recorded)
FD_RESET     // 8 - key reset made
```

### Modalities (global data)
To store the "settings" of the program in the global area, so that you can always access them:
1. Create a global variable and object`FileData`
2. Transmit a variable to an object
3. Read the data`read()`launch
4. Call in`tick()`into`loop()`
5. After changing the data, call`update()`
6. After the timeout, the data will be recorded in the file.

> So it's safe to call`update()`several times in a row, for example, when changing the "setting" buttons - the data will be updated only after the end of the input and timeout

### Procedure (local data)
To store settings in a file and read/change within functions in the program:
1. Create a variable and an object`FileData`
2. Transmit a variable to an object
3. Read the data`read()`
4. If the data has changed, it needs to be saved.`updateNow()`

```cpp
void func() {
  Data mydata;
  FileData data(&LittleFS, "/data.dat", 'A', &mydata, sizeof(mydata));
  data.read();
  // ...
  data.updateNow();
}
```

### Changes in data
- When changing the data size (when changing the number of fields in the structure), a reset will be performed when reading - a new structure will be written into the file.
- When setting the flag`addWithoutWipe(true)`and when **increase ** data size: the old size data is read from the file, then the new data will be added to the file (the difference with the old size). This is convenient when developing a project - adding new "settings" will not reset the old ones.

<a id="example"></a>
## Example

```cpp
#include <Arduino.h>
#include <FileData.h>
#include <LittleFS.h>

struct Data {
  uint8_t val8;
  uint16_t val16;
  uint32_t val32 = 12345;
  char str[20];
};
Data mydata;

FileData data(&LittleFS, "/data.dat", 'B', &mydata, sizeof(mydata));

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();

  LittleFS.begin();
  
  // read data from file to variable
  // At the first start, data from the structure will be written into the file.
  FDstat_t stat = data.read();

  switch (stat) {
    case FD_FS_ERR: Serial.println("FS Error");
      break;
    case FD_FILE_ERR: Serial.println("Error");
      break;
    case FD_WRITE: Serial.println("Data Write");
      break;
    case FD_ADD: Serial.println("Data Add");
      break;
    case FD_READ: Serial.println("Data Read");
      break;
    default:
      break;
  }

  Serial.println("Data read:");
  Serial.println(mydata.val8);
  Serial.println(mydata.val16);
  Serial.println(mydata.val32);
  Serial.println(mydata.str);
}

void loop() {
  // data.tick(); // call the ticker in the loop
  
  // can capture the moment of recording
  if (data.tick() == FD_WRITE) Serial.println("Data updated!");

  // Write this line from the port monitor
  // assigning other variables random values
  if (Serial.available()) {
    int len = Serial.readBytes(mydata.str, 20);
    mydata.str[len] = '\0';
    mydata.val8 = random(255);
    mydata.val16 = random(65000);
    Serial.println("Update");

    // postpone
    data.update();
  }
}
```

<a id="versions"></a>
## Versions
- v1.0

<a id="feedback"></a>
## Bugs and feedback
If you find bugs, create **Issue**, or better write to the mail immediately.[alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
The library is open for revision and your **Pull Requests*!

When reporting bugs or incorrect work of the library, it is necessary to specify:
- Library version
- What is used by the IC
- SDK version (for ESP)
- Arduino IDE version
- Are embedded examples that use features and designs that cause bugs in your code working correctly?
- What code was downloaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed. Not a canvas of a thousand lines, but a minimum code.
