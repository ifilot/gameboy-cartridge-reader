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

#include "readramthread.h"

/**
 * @brief read the ROM from a cartridge
 *
 * This routine will be called when a thread containing this
 * class is runned
 *
 * Note that enabling and disabling RAM needs to occur when the RAM pin is high (disabled). This is
 * also mentioned here: https://dragaosemchama.com/en/2016/02/downloading-game-boy-camera-pics/
 */
void ReadRAMThread::run() {
    unsigned int sector_counter = 0;

    this->serial_interface->open_port();

    // only scan regular ram
    if(this->ram_size_kb < 8) {
        this->serial_interface->set_ram(true);
        auto sectordata = this->serial_interface->read_sector(0xA);
        this->data.append(sectordata.mid(0, this->ram_size_kb * 1024));
    } else {
        // read upper banks
        for(unsigned int j=0; j<this->nr_ram_banks; j++) {
            if(this->nr_ram_banks > 1) {
                this->serial_interface->change_ram_bank(j);
            }
            this->serial_interface->set_ram(true);
            for(unsigned int i=0; i<2; i++) {  // 2 sectors per bank (each bank is 8k)
                auto sectordata = this->serial_interface->read_sector(0xA+i);
                this->data.append(sectordata);
                sector_counter++;
            }
            this->serial_interface->set_ram(false);
        }
    }

    this->serial_interface->set_ram(false);

    this->serial_interface->close_port();
    emit(read_ram_result_ready());
}
