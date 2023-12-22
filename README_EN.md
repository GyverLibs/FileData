This is an automatic translation, may be incorrect in some places. See sources and examples!

# FILEDATA
Replacing EEPROM for ESP8266/32 for storing any data in files
- the mechanism of the automatic "flag" of the first record
- Support for all file systems (Littlefs, Spiffs, SDFS)
- support for any type of static data
- Putting an entry by timeout
- "Update" data - the file will not be rewritten if the data has not changed

## compatibility
ESP8266, ESP32

## Content
- [installation] (# Install)
- [initialization] (#init)
- [use] (#usage)
- [Example] (# Example)
- [versions] (#varsions)
- [bugs and feedback] (#fedback)

<a id="install"> </a>
## Installation
- The library can be found by the name ** filleda ** and installed through the library manager in:
    - Arduino ide
    - Arduino ide v2
    - Platformio
- [download the library] (https://github.com/gyverlibs/filedata/archive/refs/heads/main.zip). Zip archive for manual installation:
    - unpack and put in * C: \ Program Files (X86) \ Arduino \ Libraries * (Windows X64)
    - unpack and put in * C: \ Program Files \ Arduino \ Libraries * (Windows X32)
    - unpack and put in *documents/arduino/libraries/ *
    - (Arduino id) Automatic installation from. Zip: * sketch/connect the library/add .Zip library ... * and specify downloaded archive
- Read more detailed instructions for installing libraries [here] (https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%BD%D0%BE%BE%BE%BED0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Update
- I recommend always updating the library: errors and bugs are corrected in the new versions, as well as optimization and new features are added
- through the IDE library manager: find the library how to install and click "update"
- Manually: ** remove the folder with the old version **, and then put a new one in its place.“Replacement” cannot be done: sometimes in new versions, files that remain when replacing are deleted and can lead to errors!

<a id="init"> </a>
## initialization

`` `CPP
FILEDATA;
FILEDATA (fs :: fs* fs);
FILEDATA (fs :: fs* fs, const char* Path);
FILEDATA (fs :: fs* fs, const char* Path, uint8_t key);
FILEDATA (FS :: fs* fs, const char* Path, Uint8_t Key, Void* Data, Uint16_T SIZE);
FILEDATA (FS :: fs* fs, const chaar* Path, Uint8_t Key, Void* Data, Uint16_T Size, Uint16_T Tout);

// fs - file system, address (& littlefs, & sdfs ..)
// Path - the path (name) of the file.It can be any, as well as the extension ("/mydata", "/data/settings.dat")
// Key - the key of the first record.It is not recommended to set 0 and 255. It is recommended to use the characters ('a', 'f')
// DATA - link to a variable (array, structure, class)
// size - the size of the variable, can be conveyed as sizeof (variable)
// Tout - Timesout updates in milliseconds (silent 5000)
`` `

<a id="usage"> </a>
## Usage

In ESP8266/32 EEPROM, memory is emulated from Flash memory, the implementation of Eeprom in the built -in library has the following disadvantages:
- size is limited 4 kb
- all indicated in `eeprom.begin ()` Memory volume is duplicated in RAM before the call `eeprom.end ()` ``
- In case of any change in the "eeprom" memory (even the 1st byte) and the call `eeprom.commit ()` the entire sector 4 KB is completely erased and rewritten.That is, memory wear does not occur in the cells, but completely the entire EEPROM region!About 10-20 thousandCranberries of data from data counts and eeprom memory no longer

It is proposed to use the file system (for example, built -in Littlefs), which itself takes care of the rewriting of memory and is engaged in the rotation of files in the designated area, which repeatedly increases the memory resource and the reliability of data storage.It will also allow you to "download" saved data, make backups and so on.This library is an analogue [eemanager] (https://github.com/gyverlibs/eemanager) and has similar mechanisms and opportunities:
- "Connection" of static variables of any type, the library itself will read and write their contents to the file
- the “first launch key” mechanism - if the file does not exist or does not contain the specified key - the “default” data will be written into the file
- the mechanism of the postponed record by timeout - after changing the data, it is enough to give the library the command to update, and it will update the data after the time of the timeout

`` `CPP
// install the file system and the path to the file
VOID setfs (fs :: fs* nfs, const char* Path);

// set the key
VOID Setkey (Uint8_t Key);

// connect data (variable)
VOID Setdata (Void* Data, Uint16_T Size);

// Install the timing of records
VOID settimeout (uint16_t tout);

// Read the file in the variable
// Return: fd_fs_err/fd_file_err/fd_write/fd_add/fd_read
Fdstat_t read ();

// update now
// Return: fd_fs_err/fd_file_err/fd_write/fd_no_dif
Fdstat_t updatatenow ();

// postpone the update for a given timaut
VOID update ();

// ticker, upgrade data by timeout
// Return: fd_fs_err/fd_file_err/fd_write/fd_no_dif/fd_wait/fd_idle
Fdstat_t tick ();

// write data to the file
// Return: fd_fs_err/fd_file_err/fd_write
FDSTAT_T WRITE ();

// Reset the key
// Return: fd_fs_err/fd_file_err/fd_reset
Fdstat_t reset ();

// enable the data increase mode with the addition to the file without cleaning
VOID Addwithoutwipe (Bool Addw);

// ======================================
Fd_idle // 0 - idle work
Fd_wait // 1 - expectation of a timeout
Fd_fs_err // 2 - file system error
Fd_file_err // 3 - file opening error
FD_WRITE // 4 - Write data to the file
Fd_read // 5 - reading data from a file
Fd_add // 6 - Adding data to the file
FD_NO_DIF // 7 - data do not differ (not recorded)
FD_Reset // 8 - the key is discharged
`` `

### Procedure (global data)
To store the "settings" of the program in the global region, to always have access to them:
1. Create a global variable and object `filledata`
2. Pass the variable to the object
3. Read the data `read ()` when starting
4. Call `tick ()` in `loop ()`
5. After changing the data, call `update ()`
6. At the expiration of the timeout, the data themselves will be written to the file

> Thus, it is safe to call `update ()` several times in a row, for example, when the "configuration" changes "buttons - the data will be updated only after the input and the timeage is completed

### Procedure (local data)
To store settings in the file and reading/changing the functions in the program:
1. Create a variable and object `filledata`
2. Pass the variable to the object
3. Read the data `read ()`
4. If the data has changed and they need to be saved - call `updatenow ()`

`` `CPP
VOID FUNC () {
  Data Mydata;
  Finedata Data (& Littlefs, "/data.dat", 'a', & mydata, sizeof (mydata));
  Data.read ();
  // ...
  Data.updatatenow ();
}
`` `

### data change
- when changing the size of the data (when changing the number of fields in the structure), a discharge will be carried out when reading - a new structure will be written into the file
- When installing the flag `Addwithoutwipe (True)` and with ** increasing ** data size: the data of the old size will be read from the file, then new data will be added to the file (the difference with the old size).This is convenient when developing a project - adding new "settings" will not discard the old

<a id="EXAMPLE"> </a>
## Example

`` `CPP
#include <arduino.h>
#incLude <filleda.h>
#include <Littlefs.h>

Struct Data {
  uint8_t val8;
  uint16_t val16;
  uint32_t val32 = 12345;
  Chard [20];
};
Data Mydata;

Finedata Data (& LittlefsCranberry, "/data.dat", 'b', & mydata, sizeof (mydata));

VOID setup () {
  Serial.Begin (115200);
  DELAY (1000);
  Serial.println ();

  Littlefs.Begin ();
  
  // Read the data from the file to the variable
  // at the first launch to the file, data from the structure will be written
  Fdstat_t stat = data.read ();

  Switch (stat) {
    Case fd_fs_err: serial.println ("fs error");
      Break;
    Case fd_file_err: serial.println ("error");
      Break;
    Case fd_write: serial.println ("Data Write");
      Break;
    Case fd_add: serial.println ("Data Add");
      Break;
    Case fd_read: serial.println ("Data Read");
      Break;
    Default:
      Break;
  }

  Serial.println ("Data Read:");
  Serial.println (mydata.val8);
  Serial.println (mydata.val16);
  Serial.println (mydata.val32);
  Serial.println (mydata.str);
}

VOID loop () {
  // DATA.Tick ();// call a ticker in look
  
  // you can catch the moment of recording
  if (Data.tick () == fd_write) serial.println ("Data updated!");

  // write to the data from the port monitor
  // and also assign the rest of the variable random values
  if (serial.available ()) {
    int len = serial.Readbytes (mydata.str, 20);
    mydata.str [len] = '\ 0';
    MyData.val8 = Random (255);
    MyData.val16 = Random (65000);
    Serial.println ("update");

    // postpone the update
    Data.update ();
  }
}
`` `

<a id="versions"> </a>
## versions
- V1.0

<a id="feedback"> </a>
## bugs and feedback
Create ** Issue ** when you find the bugs, and better immediately write to the mail [alex@alexgyver.ru] (mailto: alex@alexgyver.ru)
The library is open for refinement and your ** pull Request ** 'ow!

When reporting about bugs or incorrect work of the library, it is necessary to indicate:
- The version of the library
- What is MK used
- SDK version (for ESP)
- version of Arduino ide
- whether the built -in examples work correctly, in which the functions and designs are used, leading to a bug in your code
- what code has been loaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed.Not a canvas of a thousand lines, but a minimum code