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

#include "gameboydata.h"

/**
 * @brief GameboyData constructor
 */
GameboyData::GameboyData() {
    this->cartridge_types.resize(0xFF, "");
    this->cartridge_types[0xFC] = "Gameboy Camera";
}

/**
 * @brief get number of sectors (0x1000 byte segments)
 * @return number of sectors
 */
unsigned int GameboyData::get_nr_sectors() const {
    return this->rom_size / 4096;
}

/**
 * @brief get Memory Bank Controller (MBC) type
 * @param header byte designating cartridge type
 * @return MBC type id
 */
const std::string& GameboyData::get_type(uint8_t type_id) {
    if(type_id >= cartridge_types.size()) {
        throw std::runtime_error((std::string("Invalid id received for cartridge type: ") + std::to_string(type_id)).c_str());
    }
    this->determine_mapper_id(type_id);
    return this->cartridge_types[type_id];
}

/**
 * @brief Get rom size in bytes
 * @param header byte designating rom size
 * @return rom size in bytes
 */
const std::string& GameboyData::get_rom_size(uint8_t id) {
    if(id >= rom_sizes.size()) {
        throw std::runtime_error((std::string("Invalid id received for rom size: ") + std::to_string(id)).c_str());
    }
    this->rom_size = this->rom_sizes_nrbytes[id];
    this->nr_banks = this->nr_banks_data[id];
    return this->rom_sizes[id];
}

/**
 * @brief Get ramsize in bytes
 * @param header byte designating ram size
 * @return ram size in bytes
 */
const std::string& GameboyData::get_ram_size(uint8_t id) const {
    if(id >= ram_sizes.size()) {
        throw std::runtime_error((std::string("Invalid id received for rom size: ") + std::to_string(id)).c_str());
    }
    return this->ram_sizes[id];
}

/**
 * @brief Get number of banks
 * @param header byte designating ram size
 * @return number of rom banks
 */
unsigned int GameboyData::get_nr_banks(uint8_t id) const {
    if(id >= nr_banks_data.size()) {
        throw std::runtime_error((std::string("Invalid id received for rom size: ") + std::to_string(id)).c_str());
    }
    return this->nr_banks_data[id];
}

/**
 * @brief get mapper id using MBC type
 * @param mapper id
 *
 * Mapper id is used internally as an index that determines
 * the bank switching method. Several different MBC type id
 * use the same method and these are captures in mapper ids.
 */
void GameboyData::determine_mapper_id(uint8_t type_id) {
    for(unsigned int i=0; i<this->rom_mapper_types.size(); i++) {
        for(unsigned int j=0; j<this->rom_mapper_types[i].size(); j++) {
            if(this->rom_mapper_types[i][j] == type_id) {
                this->mapper_type = i;
                return;
            }
        }
    }
}
