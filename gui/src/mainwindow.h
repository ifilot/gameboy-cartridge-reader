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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QStatusBar>
#include <QString>
#include <QtWidgets/QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QComboBox>
#include <QImage>
#include <QGraphicsScene>
#include <QPixmap>
#include <QMessageBox>
#include <QFile>
#include <QElapsedTimer>
#include <QMessageBox>
#include <QProgressBar>
#include <QGroupBox>
#include <QDateTime>

#include <fstream>

#include "config.h"
#include "serial_interface.h"
#include "readthread.h"
#include "readramthread.h"
#include "writeramthread.h"
#include "flashthread.h"
#include "gameboydata.h"
#include "gameboycamera.h"
#include "logwindow.h"

class InterfaceWindow; // forward declaration to avoid circular dependencies

/**
 * @brief      Class for main window.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    // constants
    static const unsigned int IM_BOXSIZE = 8;
    static constexpr unsigned int NR_SECTORS[] = {32, 64, 128};
    static const unsigned int COL_GREEN = 0x00006400;
    static const unsigned int COL_ORANGE = 0xFFFF4500;

    // Serial port selection
    QPushButton* button_select_serial;
    QPushButton* button_scan_ports;
    QLabel* label_serial;
    QLabel* label_board_id;
    QComboBox* combobox_serial_ports;
    std::vector<std::pair<uint16_t, uint16_t>> port_identifiers;

    // progress display
    QImage* image_progress;
    QLabel* label_progress_image;

    // load/save data
    unsigned int num_sectors = 0;
    QByteArray header;          // cartridge header
    QByteArray data;            // rom data
    QByteArray save_data;       // ram data
    QPushButton* button_read_header;
    QPushButton* button_read_cartridge;
    QPushButton* button_read_ram;
    QPushButton* button_restore_ram;
    QString current_filename;
    QString operation;
    std::unique_ptr<ReadThread> readerthread;
    std::unique_ptr<ReadRAMThread> readramthread;
    std::unique_ptr<WriteRAMThread> writeramthread;
    std::shared_ptr<SerialInterface> serial_interface;
    QProgressBar* progress_bar_load;

    // cartridge information
    QLabel* label_cartridge_title;
    QLabel* label_cartridge_type;
    QLabel* label_rom_size;
    QLabel* label_ram_size;
    QLabel* label_super_game_boy;
    QLabel* label_gameboy_color;
    QLabel* label_header_checksum;
    QLabel* label_rom_checksum;
    QLabel* label_nintendo_logo;

    // flash operations
    QMenu* menu_flash_rom;
    QToolButton* button_flash_rom;
    QProgressBar* progress_bar_flash;
    QLabel* label_cartridge_image;
    std::unique_ptr<FlashThread> flashthread;
    QByteArray flash_data;

    // time operations
    QElapsedTimer timer1;
    QElapsedTimer timer2;

    // gameboy data class
    GameboyData gameboydata;

    // storage for log messages
    std::shared_ptr<QStringList> log_messages;

    // window for log messages
    std::unique_ptr<LogWindow> log_window;

public:
    /**
     * @brief Default builder
     */
    MainWindow() {}
    /**
     * @brief      Constructs the object.
     */
    MainWindow(const std::shared_ptr<QStringList> _log_messages);

protected:
    /**
     * @brief function that runs when window is closed
     * @param event
     */
    void closeEvent(QCloseEvent *event) override;

private:

    /****************************************************************************
     *  GRAPHICAL USER INTERFACE BUILDER ROUTINES
     ****************************************************************************/

    /**
     * @brief Build the GUI and connect all buttons to signals
     */
    void create_interface();

    /**
      * @brief Set dropdown menu
      */
    void build_dropdown_menu();

    /**
     * @brief Build GUI showing cartridge information
     * @param layout position where to put this part of the GUI
     */
    void build_cartridge_information_menu(QVBoxLayout* target_layout);

    /**
     * @brief Build GUI showing serial port interface
     * @param layout position where to put this part of the GUI
     */
    void build_serial_interface_menu(QVBoxLayout* target_layout);

    /**
     * @brief Build GUI showing rom extraction and ram reading/writing
     * @param layout position where to put this part of the GUI
     */
    void build_data_interface_menu(QVBoxLayout* target_layout);

    /**
     * @brief Build GUI showing cart flashing
     * @param layout position where to put this part of the GUI
     */
    void build_burn_interface_menu(QVBoxLayout* target_layout);

    /**
     * @brief Disable all interface boxes
     */
    void disable_all_buttons();

    /**
     * @brief Enable all interface boxes
     */
    void enable_all_buttons();

    /****************************************************************************
     *  PRIVATE HELPER ROUTINES
     ****************************************************************************/

    /**
     * @brief Parse cartridge header data and populate cartridge information
     */
    void parse_header_data();

private slots:
    /****************************************************************************
     *  SIGNALS :: Help interface
     ****************************************************************************/

    /**
     * @brief show about menu
     */
    void show_debug_log();

    /**
     * @brief show about menu
     */
    void show_about();

    /****************************************************************************
     *  SIGNALS :: COMMUNICATION INTERFACE ROUTINES
     ****************************************************************************/

    /**
     * @brief Scan all communication ports to populate drop-down box
     */
    void scan_com_devices();

    /**
     * @brief Select communication port for serial to 32u4
     */
    void select_com_port();

    /****************************************************************************
     *  SIGNALS :: READ ROM ROUTINES
     ****************************************************************************/

    /*
     * @brief Read cartridge header
     */
    void read_header();

    /**
     * @brief Read data from chip
     */
    void read_cartridge();

    /**
     * @brief Slot to indicate that a sector is about to be read / written
     */
    void read_sector_start(unsigned int sector_id);

    /**
     * @brief Slot to accept that a sector is read / written
     */
    void read_sector_done(unsigned int sector_id);

    /*
     * @brief Signal that a read operation is finished
     */
    void read_result_ready();

    /****************************************************************************
     *  SIGNALS :: READ RAM ROUTINES
     ****************************************************************************/

    /*
     * @brief Read cartridge header
     */
    void read_ram();

    /*
     * @brief Signal that a RAM read operation is finished
     */
    void read_ram_result_ready();

    /****************************************************************************
     *  SIGNALS :: WRITE RAM ROUTINES
     ****************************************************************************/

    /*
     * @brief Read cartridge header
     */
    void write_ram();

    /*
     * @brief Signal that a RAM read operation is finished
     */
    void write_ram_result_ready();

    /****************************************************************************
     *  SIGNALS :: FLASH ROM
     ****************************************************************************/

    /**
     * @brief Put rom on flash cartridge
     */
    void flash_rom();

    /**
     * @brief Slot to indicate that a page is about to be written
     */
    void flash_page_start(unsigned int page_id);

    /**
     * @brief Slot to accept that a page is written
     */
    void flash_page_done(unsigned int page_id);

    /*
     * @brief Signal that a flash operation is finished
     */
    void flash_result_ready();

    /*
     * @brief Signal that a flash operation is finished
     */
    void flash_chip_id_error(unsigned int chip_id);

    /**
     * @brief Slot to accept when a sector is about to be verified
     */
    void verify_sector_start(unsigned int page_id);

    /**
     * @brief Slot to accept when a sector is verified
     */
    void verify_sector_done(unsigned int page_id);

    /*
     * @brief Signal that a verified operation is finished
     */
    void verify_result_ready();

    /****************************************************************************
     *  SIGNALS :: OTHER
     ****************************************************************************/

    /**
     * @brief      Test the Gameboy Camera Interface
     */
    void gameboy_camera_test();

    /**
     * @brief      Close the application
     */
    void exit();
};

#endif
