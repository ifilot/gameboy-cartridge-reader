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

#ifndef SERIAL_INTERFACE_H
#define SERIAL_INTERFACE_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDateTime>
#include <QDebug>

#include <string>
#include <iostream>
#include <exception>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <QString>
#include <QRegularExpression>

/**
 * @brief Interface class handling serial communication
 */
class SerialInterface {

private:
    static const unsigned int SERIAL_TIMEOUT = 100;             // timeout for regular serial communication
    static const unsigned int SERIAL_TIMEOUT_SECTOR = 0;        // timeout when reading sector data (0x1000 bytes)
    static const unsigned int SERIAL_TIMEOUT_BLOCK = 3000;      // timeout when reading sector data (0x1000 bytes)
    std::string portname;                                       // communication port address
    std::unique_ptr<QSerialPort> port;                          // pointer to QSerialPort object
    int baudrate;

    // variables to store cartridge firmware version
    int firmware_major = 0;
    int firmware_minor = 0;
    int firmware_patch = 0;
    std::string firmware_version;
    std::string chipset;

public:
    /**
     * @brief SerialInterface
     * @param _portname address of the com port
     */
    SerialInterface(const std::string& _portname, int baudrate = 115200);

    /**
     * @brief get_port
     * @return string with port address
     */
    inline const std::string& get_port() const {
        return this->portname;
    }

    /**
     * @brief Create a new QSerialPort object and specify
     *        communication settings
     */
    void open_port();

    /**
     * @brief Close the communication port and destroy
     *        the QSerialPort object
     */
    void close_port();

    /********************************************************
     *  Cardreader interfacing routines
     ********************************************************/

    /**
     * @brief Get identifier string of the board
     * @return identifier string
     */
    std::string get_board_info();

    /**
     * @brief Get compile time of the firmware
     * @return time string
     */
    std::string get_compile_time();

    /**
     * @brief Read cartridge header (first 0x150 bytes)
     * @return cartridge header
     */
    QByteArray read_header();

    /**
     * @brief Read a sector (0x1000 bytes) from cartridge at address location
     * @param address location
     * @return data at address
     */
    QByteArray read_sector(unsigned int sector_addr);

    /**
     * @brief Change rom memory bank
     * @param bank_id
     * @param mapper_type
     */
    void change_rom_bank(unsigned int bank_id, unsigned int mapper_type);

    /**
     * @brief Change ram memory bank
     * @param bank_id
     */
    void change_ram_bank(unsigned int bank_id);

    /**
     * @brief Change ram memory bank
     * @param whether to write to upper ram sector 0xB000
     */
    void write_ram(const QByteArray& data, bool upper);

    /**
     * @brief Erase sector (4096 bytes) on SST39SF0x0 chip
     * @param start address
     */
    void erase_sector(unsigned int addr);

    /**
     * @brief Burn block (256 bytes) to SST39SF0x0 chip
     * @param start address
     * @param data (256 bytes)
     */
    void burn_block(unsigned int addr, const QByteArray& data);

    /**
     * @brief get_chip_id check to verify this is a SST39SF0x0 chip
     * @return chip id
     */
    uint16_t get_chip_id();

    /**
     * @brief whether to enable external RAM
     * @param enable
     */
    void set_ram(bool enable);

    /**
     * @brief get user statistics
     * @return user statistics
     */
    std::vector<uint32_t> get_user_statistics();

private:

    /**
     * @brief Write single byte to address
     * @param address to write at
     * @param byte to write
     */
    void write_address(uint16_t address, uint8_t value);

    /**
     * @brief send a single command and capture the echo
     * @param command to send
     */
    void send_command(const std::string& command);

    /**
     * @brief send a single command and capture the response
     * @param command
     * @return
     */
    QByteArray send_command_capture_response(const std::string& command, int nrbytes);

    /**
     * @brief get variable stored in EEPROM at address addr
     * @param addr
     * @return dword
     */
    uint32_t get_variable_eeprom(uint16_t addr);

    /**
     * @brief Capture any bytes left in read buffer and destroy them
     */
    void flush_buffer();

    /**
     * @brief Convenience function waiting for response
     */
    void wait_for_response(int nrbytes);

    /**
     * @brief Convenience function for comparing two version numbers
     */
    bool firmware_version_greater_than(int major, int minor, int patch);
};

#endif // SerialInterface_H
