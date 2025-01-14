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

#ifndef FLASHTHREAD_H
#define FLASHTHREAD_H

#include <QMessageBox>
#include <QIcon>

#include "ioworker.h"

/**
 * @brief class for Flashing a cartridge
 *
 * Currently, only support for the AT28C256 is implemented.
 */
class FlashThread : public IOWorker {

    Q_OBJECT

private:
    /**
     * @brief flash card id
     *
     * 0: unknown
     * 1: AT28C256
     * 2: SST39SF0x0
     */
    uint8_t flash_card_id = 0;

    /**
     * @brief number of pages / blocks to flash
     */
    uint16_t num_pages = 0;

public:
    /**
     * @brief Default constructor
     */
    FlashThread() {}

    /**
     * @brief Constructor allocating SerialInterface
     * @param _serial_interface
     */
    FlashThread(const std::shared_ptr<SerialInterface>& _serial_interface) :
        IOWorker(_serial_interface) {}

    /**
     * @brief run cart flash routine
     */
    void run() override;

    /**
     * @brief set_flash_card_id
     * @param _flash_card_id
     */
    inline void set_flash_card_id(uint8_t _flash_card_id) {
        this->flash_card_id = _flash_card_id;
    }

    /**
     * @brief get_flash_id
     * @return
     */
    inline unsigned int get_flash_id() const {
        return this->flash_card_id;
    }

    /**
     * @brief get_num_pages
     * @return number of pages / blocks
     */
    inline uint16_t get_num_pages() const {
        return this->num_pages;
    }

private:
    /**
     * @brief run flash cart routine for a 28atc256 chip
     */
    void flash_28atc256();

    /**
     * @brief run flash cart routine for a sst39sf0x0 chip
     */
    void flash_sst39sf0x0();

signals:
    /**
     * @brief signal when flash process is ready
     */
    void flash_result_ready();

    /**
     * @brief signal when new page is about to be written
     * @param page_id
     */
    void flash_page_start(unsigned int page_id);

    /**
     * @brief signal when new page is written
     * @param page_id
     */
    void flash_page_done(unsigned int page_id);

    /**
     * @brief signal when flash chip id is incorrect
     * @param page_id
     */
    void flash_chip_id_error(unsigned int chip_id);
};

#endif // FLASHTHREAD_H
