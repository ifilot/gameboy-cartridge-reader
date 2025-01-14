/****************************************************************************
 *                                                                          *
 *   GBCR                                                                   *
 *   Copyright (C) 2021 Ivo Filot <ivo@ivofilot.nl>                         *
 *                                                                          *
 *   This program is free software: you can redistribute it and/or modify   *
 *   it under the terms of the GNU Lesser General Public License as         *
 *   published by the Free Software Foundation, either version 3 of the     *
 *   License, or (at your option) any later version.                        *
 *                                                                          *
 *   This program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU General Public License for more details.                           *
 *                                                                          *
 *   You should have received a copy of the GNU General Public license      *
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>. *
 *                                                                          *
 ****************************************************************************/
#ifndef GAMEBOYDATA_H
#define GAMEBOYDATA_H

#include <string>
#include <vector>
#include <stdexcept>

/**
 * @brief Class holding information of Gameboy cartridge
 *        such as mappers, rom and ram sizes, etc.
 */
class GameboyData {

private:
    // list of cartridge types
    std::vector<std::string> cartridge_types = {
        "ROM ONLY",                 // 00h
        "MBC1",                     // 01h
        "MBC1+RAM",                 // 02h
        "MBC1+RAM+BATTERY",         // 03h
        "unknown",                  // 04h
        "MBC2",                     // 05h
        "MBC2+BATTERY",             // 06h
        "unknown",                  // 07h
        "ROM+RAM",                  // 08h
        "ROM+RAM+BATTERY",          // 09h
        "unknown",                  // 0Ah
        "MMM01",                    // 0Bh
        "MMM01+RAM",
        "MMM01+RAM+BATTERY",
        "unknown",
        "MBC3+TIMER+BATTERY",
        "MBC3+TIMER+RAM+BATTERY",
        "MBC3",
        "MBC3+RAM",
        "MBC3+RAM+BATTERY",
        "unknown",
        "unknown",
        "unknown",
        "unknown",
        "unknown",
        "MBC5",
        "MBC5+RAM",
        "MBC5+RAM+BATTERY",
        "MBC5+RUMBLE",
        "MBC5+RUMBLE+RAM",
        "MBC5+RUMBLE+RAM+BATTERY",
        "unknown",
        "MBC6",
        "unknown",
        "MBC7+SENSOR+RUMBLE+RAM+BATTERY",
    };

    // list of ROM sizes
    std::vector<std::string> rom_sizes = {
        "32KByte (no ROM banking)",
        "64KByte (4 banks)",
        "128KByte (8 banks)",
        "256KByte (16 banks)",
        "512KByte (32 banks)",
        "1MByte (64 banks)",
        "2MByte (128 banks)",
        "4MByte (256 banks)",
        "8MByte (512 banks)"
    };

    // rom size in bytes
    std::vector<unsigned int> rom_sizes_nrbytes = {
        32 * 1024,
        64 * 1024,
        128 * 1024,
        256 * 1024,
        512 * 1024,
        1024 * 1024,
        2 * 1024 * 1024,
        4 * 1024 * 1024,
        8 * 1024 * 1024
    };

    // number of rom banks
    std::vector<unsigned int> nr_banks_data = {
        2,      // no bank switching
        4,
        8,
        16,
        32,
        64,
        125,
        256,
        512
    };

    // ram sizes in bytes
    std::vector<std::string> ram_sizes = {
        "None",
        "2 KBytes",
        "8 Kbytes",
        "32 KBytes (4 banks of 8KBytes each)",
        "128 KBytes (16 banks of 8KBytes each)",
        "64 KBytes (8 banks of 8KBytes each)"
    };

    // number of RAM banks to loop over
    std::vector<uint8_t> nr_ram_banks_data = {
        1,
        1,
        1,
        4,
        16,
        64
    };

    // ram size to collect
    std::vector<uint8_t> ram_size_data = {
        0,
        2,
        8,
        32,
        128,
        64
    };

    // mapper types
    std::vector<std::vector<uint8_t> > rom_mapper_types = {
        {0x00},                                   // ROM ONLY
        {0x01, 0x02, 0x03},                       // MBC1
        {0x05,0x06},                              // MBC2
        {0x0F, 0x10, 0x11, 0x12, 0x13},           // MBC3
        {0x0B, 0x0C},                             // MMM01
        {0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E},     // MBC5
        {0x20},                                   // MBC6
        {0x22},                                   // MBC7
    };

    unsigned int rom_size = 0;
    unsigned int nr_banks = 0;
    unsigned int mapper_type = 0;

public:
    /**
     * @brief GameboyData constructor
     */
    GameboyData();

    /**
     * @brief get number of sectors (0x1000 byte segments)
     * @return number of sectors
     */
    unsigned int get_nr_sectors() const;

    /**
     * @brief get_mapper_id
     * @return
     */
    inline unsigned int get_mapper_id() const {
        return this->mapper_type;
    }

    /**
     * @brief get Memory Bank Controller (MBC) type
     * @param header byte designating cartridge type
     * @return MBC type id
     */
    const std::string& get_type(uint8_t type_id);

    /**
     * @brief Get rom size in bytes
     * @param header byte designating rom size
     * @return rom size in bytes
     */
    const std::string& get_rom_size(uint8_t id);

    /**
     * @brief Get ramsize in bytes
     * @param header byte designating ram size
     * @return ram size in bytes
     */
    const std::string& get_ram_size(uint8_t id) const;

    /**
     * @brief get number of ram banks
     * @param id
     * @return number of ram banks
     */
    inline uint8_t get_nr_ram_banks(uint8_t id) const {
        return this->nr_ram_banks_data[id];
    }

    /**
     * @brief get the ram size in kb
     * @param id
     * @return ram size in kb
     */
    inline uint8_t get_ram_size_kb(uint8_t id) const {
        return this->ram_size_data[id];
    }

    /**
     * @brief Get number of banks
     * @param header byte designating ram size
     * @return number of rom banks
     */
    unsigned int get_nr_banks(uint8_t id) const;

private:
    /**
     * @brief get mapper id using MBC type
     * @param mapper id
     *
     * Mapper id is used internally as an index that determines
     * the bank switching method. Several different MBC type id
     * use the same method and these are captures in mapper ids.
     */
    void determine_mapper_id(uint8_t type_id);
};

#endif // GAMEBOYDATA_H
