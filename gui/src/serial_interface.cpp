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

#include "serial_interface.h"

/**
 * @brief SerialInterface
 * @param _portname address of the com port
 */
SerialInterface::SerialInterface(const std::string& _portname, int _baudrate) {
    this->portname = _portname;
    this->baudrate = _baudrate;
}

/**
 * @brief Create a new QSerialPort object and specify
 *        communication settings
 */
void SerialInterface::open_port() {
    if(this->portname.size() == 0) {
        throw std::runtime_error("No port has been set");
    }

    qDebug() << "Opening serial port.";

    this->port = std::make_unique<QSerialPort>(this->portname.c_str());
    this->port->setBaudRate(this->baudrate);
    this->port->setDataBits(QSerialPort::Data8);
    this->port->setStopBits(QSerialPort::OneStop);
    this->port->setParity(QSerialPort::NoParity);
    this->port->setFlowControl(QSerialPort::NoFlowControl);

    this->port->open(QIODevice::ReadWrite);
}

/**
 * @brief Close the communication port and destroy
 *        the QSerialPort object
 */
void SerialInterface::close_port() {
    this->port->close();
    this->port.reset();

    qDebug() << "Closing serial port.";
}

/********************************************************
 *  Cardreader interfacing routines
 ********************************************************/

/**
 * @brief Get identifier string of the board
 * @return identifier string
 */
std::string SerialInterface::get_board_info() {
    try {
        char command[] = "READINFO";
        QByteArray response_data = this->send_command_capture_response(command, 16);
        qDebug() << "Response: " << response_data;

        // store firmware data
        QString firmware_string = QString(response_data);
        QStringList firmware_items = firmware_string.split("-");
        if(firmware_items[1] == "AVR") {
            this->chipset = "32u4";
        } else if(firmware_items[1] == "8515") {
            this->chipset = "8515L";
        } else {
            throw std::runtime_error("Unidentified chipset");
        }
        QString firmware_version_qstring = firmware_items[2].remove(0,1);
        QStringList firmware_version_items = firmware_version_qstring.split(".");
        this->firmware_version = firmware_version_qstring.toStdString();
        this->firmware_major = firmware_version_items[0].toInt();
        this->firmware_minor = firmware_version_items[1].toInt();
        this->firmware_patch = firmware_version_items[2].toInt();

        // output chipset version information
        qDebug() << "Chipset: " << this->chipset.c_str();
        qDebug() << "Firmware version: " << this->firmware_major << "." << this->firmware_minor << "." << this->firmware_patch;

        return response_data.toStdString();
    }  catch (std::exception& e) {
        std::cerr << "Caught error: " << e.what() << std::endl;
        throw e;
    }
}

/**
 * @brief Get compile time of the firmware
 * @return time string
 */
std::string SerialInterface::get_compile_time() {
    char command[] = "COMPTIME";
    QByteArray response_data = this->send_command_capture_response(command, 32);

    QString compile_time = response_data;
    QRegularExpression re("([A-Za-z]{3}\\s+[0-9]+\\s+[0-9]{4}).*(\\d{2}:\\d{2}:\\d{2})");
    QRegularExpressionMatch match = re.match(compile_time);

    if(match.hasMatch()) {
        return (match.captured(1) + " " + match.captured(2)).toStdString();
    } else {
        qDebug() << "Cannot identify compile time.";
        return std::string("Unknown compile time");
    }
}

/**
 * @brief Read cartridge header (first 0x150 bytes)
 * @return cartridge header
 */
QByteArray SerialInterface::read_header() {
    std::vector<uint8_t> data(0x150);
    try {
        char command[] = "READHDR0";
        QByteArray response_data = this->send_command_capture_response(command, 0x150);

        return response_data;
    }  catch (std::exception& e) {
        std::cerr << "Caught error: " << e.what() << std::endl;
        throw e;
    }
}

/**
 * @brief Read a sector (0x1000 bytes) from cartridge at address location
 * @param address location
 * @return data at address
 */
QByteArray SerialInterface::read_sector(unsigned int sector_addr) {
    try {
        std::string command = QString("RDBK%1").arg(sector_addr * 0x1000, 4, 16, QChar('0')).toStdString();
        QByteArray response_data = this->send_command_capture_response(command, 0x1000);

        return response_data;
    }  catch (std::exception& e) {
        std::cerr << "Caught error: " << e.what() << std::endl;
        throw e;
    }
}

/**
 * @brief Change memory bank
 * @param bank_id
 * @param mapper_type
 */
void SerialInterface::change_rom_bank(unsigned int bank_id, unsigned int mapper_type) {
    try {
        uint8_t bank = bank_id;

        switch(mapper_type) {
            case 0:
                // do nothing; no bank switching for this ROM
            break;
            case 1: // MBC 1
                if(bank_id < 0x020) {
                    this->write_address(0x2100, bank);
                } else {
                    this->write_address(0x6000, 0x00);          // set rom banking mode
                    this->write_address(0x4000, bank >> 5);     // set bits 5 and 6
                    this->write_address(0x2100, bank & 0x1F);   // sets lower five bits
                }
            break;
            case 2: // MBC 2
                this->write_address(0x2100, bank & 0x0F);
            break;
            case 3: // MBC 3
                this->write_address(0x2100, bank & 0x7F);
            break;
            case 4: // MMM01

            break;
            case 5: // MBC 5
                this->write_address(0x2100, bank & 0xFF);
                this->write_address(0x3000, ((uint16_t)bank >> 8) & 0x01);
            break;
            case 6: // MBC 6

            break;
            case 7: // MBC 7

            break;
            default:
                throw std::runtime_error("Unknown mapper type.");
            break;
        }
    }  catch (std::exception& e) {
        throw e;
    }
}

/**
 * @brief Change memory bank
 * @param bank_id
 */
void SerialInterface::change_ram_bank(unsigned int bank_id) {
    try {
        this->write_address(0x4000, bank_id);
    }  catch (std::exception& e) {
        throw e;
    }
}

/**
 * @brief Change ram memory bank
 * @param bank_id
 */
void SerialInterface::write_ram(const QByteArray& data, bool upper) {
    try {
        std::string command;
        if(data.size() == 2048) {
            command = "RMWR2k00";
        } else if(data.size() == 4096) {
            if(upper) {
                command = "RMWR4kB0";
            } else {
                command = "RMWR4kA0";
            }

        } else {
            throw std::runtime_error("Invalid data size received");
        }

        this->send_command(command);

        this->port->write(data, data.size());
        while(this->port->waitForBytesWritten(SERIAL_TIMEOUT_SECTOR)){}

    }  catch (std::exception& e) {
        std::cerr << "Caught error: " << e.what() << std::endl;
        throw e;
    }
}

/**
 * @brief Erase sector (4096 bytes) on SST39SF0x0 chip
 * @param start address
 */
void SerialInterface::erase_sector(unsigned int addr) {
    try {
        std::string command = QString("ESST%1").arg(addr, 4, 16, QChar('0')).toStdString();
        auto response = this->send_command_capture_response(command, 2);
        uint16_t nrcycles = 0;
        memcpy((void*)&nrcycles, (void*)&response.data()[0], 2);
        qDebug() << "Succesfully erased sector " << addr << " in " << nrcycles << " cyles.";
    }  catch (std::exception& e) {
        std::cerr << "Caught error: " << e.what() << std::endl;
        throw e;
    }
}

/**
 * @brief Burn block (256 bytes) to SST39SF0x0 chip
 * @param start address
 * @param data (256 bytes)
 */
void SerialInterface::burn_block(unsigned int addr, const QByteArray& data) {
    try {
        qDebug() << "Burning block.";
        std::string command = QString("WRST%1").arg(addr, 4, 16, QChar('0')).toStdString();
        this->send_command(command);
        this->port->write(data, 256);
        while(this->port->waitForBytesWritten(SERIAL_TIMEOUT_BLOCK)){}

        // discard any contents still left in read buffer
        this->flush_buffer();
    }  catch (std::exception& e) {
        std::cerr << "Caught error: " << e.what() << std::endl;
        throw e;
    }
}

/**
 * @brief get_chip_id check to verify this is a SST39SF0x0 chip
 * @return chip id
 */
uint16_t SerialInterface::get_chip_id() {
    try {
        std::string command = "DEVIDSST";
        auto response = this->send_command_capture_response(command, 2);
        uint16_t chip_id = (uint16_t)(response[0]+1) * 256 + (uint16_t)response[1];
        return chip_id;
    }  catch (std::exception& e) {
        std::cerr << "Caught error: " << e.what() << std::endl;
        throw e;
    }

}

/**
 * @brief whether to enable external RAM
 * @param enable
 */
void SerialInterface::set_ram(bool enable) {
    std::string command;

    if(enable) {
        command = "RAMON000";
    } else {
        command = "RAMOFF00";
    }

    this->send_command(command);
}

/********************************************************
 *  PRIVATE ROUTINES
 ********************************************************/

/**
 * @brief Write single byte to address
 * @param address to write at
 * @param byte to write
 */
void SerialInterface::write_address(uint16_t address, uint8_t value) {
    try {
        std::string command = QString("WR%1%2").arg(address, 4, 16, QChar('0')).arg(value, 2, 16, QChar('0')).toStdString();
        this->send_command(command);

    }  catch (std::exception& e) {
        std::cerr << "Caught error: " << e.what() << std::endl;
        throw e;
    }
}

/**
 * @brief send a single command and capture the echo
 * @param command to send
 */
void SerialInterface::send_command(const std::string& command) {
    // send the command
    qDebug() << "Send command: " << command.c_str();
    this->port->write(command.c_str(), 8);
    while(this->port->waitForBytesWritten(SERIAL_TIMEOUT)){}

    // capture command response
    this->wait_for_response(8);
    auto response = this->port->readAll();

    // check that response is identifical to command,
    // else throw an error
    if(strcmp(response.toStdString().c_str(), command.c_str()) != 0) {
        throw std::runtime_error("Invalid response received (" + response.toStdString() + ") from command " + command);
    } else {
        qDebug() << "Response succesfully received: " << response;
    }
}

/**
 * @brief send a single command and capture the echo
 * @param command to send
 */
QByteArray SerialInterface::send_command_capture_response(const std::string& command, int nrbytes) {

    while(true) {
        // send the command
        qDebug() << "Send command: " << command.c_str();
        this->port->write(command.c_str(), 8);
        while(this->port->waitForBytesWritten(SERIAL_TIMEOUT)){}

        // capture command response
        this->wait_for_response(8 + nrbytes);

        // read bytes from port
        auto response = this->port->readAll();

        // separate command and response
        auto cmdres = response.mid(0,8);
        response = response.mid(8,nrbytes);

        // verify response
        if(strcmp(cmdres.toStdString().c_str(), command.c_str()) == 0) {
            qDebug() << "Response succesfully received: " << cmdres;
            return response;
        } else {
            qDebug() << "Invalid response received (" << response << ") from command " << QString(command.c_str());
        }

        qDebug() << "Done, returning " << response.size() << " bytes.";
    }
}


/**
 * @brief Capture any bytes left in read buffer and destroy them
 */
void SerialInterface::flush_buffer() {
    QByteArray response;

    if(this->port->waitForReadyRead(SERIAL_TIMEOUT)) {
        response += this->port->readAll(); //discard bytes
    }

    if(response.size() > 0) {
        qDebug() << "Flushing buffer, discarding the following bytes: " << response;
    }
}

/**
 * @brief Convenience function waiting for response
 */
void SerialInterface::wait_for_response(int nrbytes) {
    size_t ctr = 0;
    int bytes_available = 0;
    while(this->port->waitForReadyRead(SERIAL_TIMEOUT) || this->port->bytesAvailable() < nrbytes){
        // check if number of bytes available is increasing, if not, increment counter
        if(this->port->bytesAvailable() == bytes_available) {
            ctr++;
        }
        bytes_available = this->port->bytesAvailable();

        // if counter reaches a maximum number of tries, terminate the procedure
        if(ctr > 100) {
            qDebug() << "Failed to capture response, outputting buffer:";
            qDebug() << this->port->readAll();
            throw std::runtime_error("Too many tries waiting for response to command, terminating.");
        }
    }
}

bool SerialInterface::firmware_version_greater_than(int major, int minor, int patch) {
    if(this->firmware_major > major) {
        return true;
    } else if(this->firmware_major < major) {
        return false;
    }

    // major is equal, so check minor
    if(this->firmware_minor > minor) {
        return true;
    } else if(this->firmware_minor < minor) {
        return false;
    }

    // major and minor are both equal, so check patch
    if(this->firmware_patch > patch) {
        return true;
    }

    // either patch is also equal or smaller, so return false
    return false;
}
