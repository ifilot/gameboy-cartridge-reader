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

#ifndef READRAMTHREAD_H
#define READRAMTHREAD_H

#include <iostream>

#include "ioworker.h"

/**
 * @brief Worker Thread responsible for reading ROM from cartridge
 */
class ReadRAMThread : public IOWorker {

    Q_OBJECT

private:
    unsigned int nr_ram_banks = 0;      // number of banks to read
    unsigned int ram_size_kb = 0;       // ram size in kb

public:
    ReadRAMThread() {}

    ReadRAMThread(const std::shared_ptr<SerialInterface>& _serial_interface) :
        IOWorker(_serial_interface) {}
    /**
     * @brief read the ROM from a cartridge
     *
     * This routine will be called when a thread containing this
     * class is runned
     */
    void run() override;

    /**
     * @brief set the number of roms banks
     * @param number of rom banks
     */
    inline void set_number_ram_banks(unsigned int _nr_ram_banks) {
        this->nr_ram_banks = _nr_ram_banks;
    }

    /**
     * @brief set ram size in kb
     * @param ram size in kb
     */
    inline void set_ram_size_kb(unsigned int _ram_size_kb) {
        this->ram_size_kb = _ram_size_kb;
    }

signals:
    /**
     * @brief signal when ram has been read
     */
    void read_ram_result_ready();
};

#endif // READRAMTHREAD_H
