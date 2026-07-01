/*
    Замена EEPROM для ESP8266/32 для хранения любых данных в файлах
    Документация:
    GitHub: https://github.com/GyverLibs/FileData
    Возможности:
    - Механизм автоматического "флага" первой записи
    - Поддержка всех файловых систем (LittleFS, SPIFFS, SDFS)
    - Поддержка любых типов статических данных
    - Отложенная запись по таймауту
    - "Обновление" данных - файл не перезапишется, если данные не изменились

    AlexGyver, alex@alexgyver.ru
    https://alexgyver.ru/
    MIT License

    Версии:
    v1.0 - релиз
*/

#pragma once

#include <Arduino.h>
#include <FS.h>

// статусы
enum FDstat_t {
    FD_IDLE,      // 0 - холостая работа
    FD_WAIT,      // 1 - ожидание таймаута
    FD_FS_ERR,    // 2 - ошибка файловой системы
    FD_FILE_ERR,  // 3 - ошибка открытия файла
    FD_WRITE,     // 4 - запись данных в файл
    FD_READ,      // 5 - чтение данных из файла
    FD_ADD,       // 6 - добавление данных в файл
    FD_NO_DIF,    // 7 - данные не отличаются (не записаны)
    FD_RESET,     // 8 - произведён сброс ключа
};

class FileData {
   public:
    FileData(fs::FS* nfs = nullptr, const char* path = nullptr, uint8_t key = 'A', void* data = nullptr, size_t size = 0, uint16_t tout = 5000) {
        setFS(nfs, path);
        setKey(key);
        setData(data, size);
        setTimeout(tout);
    }

    // установить файловую систему
    void setFS(fs::FS* nfs, const char* path) {
        _fs = nfs;
        _path = path;
    }

    // установить ключ
    void setKey(uint8_t key) {
        _key = key;
    }

    // подключить данные (переменную)
    void setData(void* data, size_t size) {
        _data = data;
        _size = size;
    }

    // установить таймаут записи
    void setTimeout(uint16_t tout) {
        _tout = tout;
    }

    // прочитать файл в переменную
    // возврат: FD_FS_ERR/FD_FILE_ERR/FD_WRITE/FD_ADD/FD_READ
    FDstat_t read() {
        if (!_valid()) return FD_FS_ERR;
        if (!_fs->exists(_path)) return write();

        File file = _fs->open(_path, "r+");
        if (!file) return FD_FILE_ERR;

        size_t size = file.size();
        int key = file.read();

        if (key == _key && size) {
            if (size > _size + 1) {
                file.close();
                return write();
            } else if (size < _size + 1) {
                if (_addw) {
                    size_t oldSize = size - 1;
                    size_t read = file.read((uint8_t*)_data, oldSize);
                    if (read != oldSize) return FD_FILE_ERR;

                    file.close();
                    return write() == FD_WRITE ? FD_ADD : FD_FILE_ERR;
                } else {
                    file.close();
                    return write();
                }
            } else {
                size_t read = file.read((uint8_t*)_data, _size);
                return read == _size ? FD_READ : FD_FILE_ERR;
            }
        } else {
            file.close();
            return write();
        }
    }

    // обновить сейчас
    // возврат: FD_FS_ERR/FD_FILE_ERR/FD_WRITE/FD_NO_DIF
    FDstat_t updateNow() {
        _updf = false;
        if (!_valid()) return FD_FS_ERR;
        if (!_fs->exists(_path)) return FD_FILE_ERR;

        File file = _fs->open(_path, "r");
        if (!file) return FD_FILE_ERR;

        if (file.size() != _size + 1 || file.read() != _key) {
            file.close();
            return write();
        }

        for (size_t i = 0; i < _size; i++) {
            int value = file.read();
            if (value < 0) return FD_FILE_ERR;

            if (((uint8_t*)_data)[i] != value) {
                file.close();
                return write();
            }
        }
        return FD_NO_DIF;
    }

    // отложить обновление на заданный таймаут
    void update() {
        _tmr = millis();
        _updf = true;
    }

    // тикер, обновит данные по таймауту
    // возврат: FD_FS_ERR/FD_FILE_ERR/FD_WRITE/FD_NO_DIF/FD_WAIT/FD_IDLE
    FDstat_t tick() {
        if (_updf && (uint16_t)((uint16_t)millis() - _tmr) >= _tout) {
            _updf = false;
            return updateNow();
        }
        return _updf ? FD_WAIT : FD_IDLE;
    }

    // записать данные в файл
    // возврат: FD_FS_ERR/FD_FILE_ERR/FD_WRITE
    FDstat_t write() {
        if (!_valid()) return FD_FS_ERR;
        File file = _fs->open(_path, "w");
        if (!file) return FD_FILE_ERR;

        bool written = file.write(_key) == 1 && file.write((const uint8_t*)_data, _size) == _size;
        file.close();
        return written ? FD_WRITE : FD_FILE_ERR;
    }

    // сбросить ключ
    // возврат: FD_FS_ERR/FD_FILE_ERR/FD_RESET
    FDstat_t reset() {
        if (!_valid()) return FD_FS_ERR;
        if (!_fs->exists(_path)) return FD_FILE_ERR;

        File file = _fs->open(_path, "r+");
        if (!file) return FD_FILE_ERR;

        int key = file.read();
        if (key < 0 || !file.seek(0) || file.write((uint8_t)(key + 1)) != 1) return FD_FILE_ERR;
        return FD_RESET;
    }

    // включить режим увеличения данных с добавлением в файл без очистки
    void addWithoutWipe(bool addw) {
        _addw = addw;
    }

   private:
    bool _valid() const {
        return _fs && _path && _path[0] && _data;
    }

    fs::FS* _fs;
    const char* _path;
    void* _data;
    size_t _size;
    uint16_t _tout;
    uint16_t _tmr = 0;
    uint8_t _key;
    bool _addw = false;
    bool _updf = false;
};
