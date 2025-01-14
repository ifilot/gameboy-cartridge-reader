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

#include "mainwindow.h"

/**
 * @brief      Class for main window.
 */
MainWindow::MainWindow(const std::shared_ptr<QStringList> _log_messages) :
    log_messages(_log_messages)
{
    // log window
    this->log_window = std::make_unique<LogWindow>(this->log_messages);
    this->log_window->setWindowTitle("Debug log");
    this->log_window->setWindowIcon(QIcon(":/assets/img/logo.ico"));

    // build menu
    this->build_dropdown_menu();

    // create interface
    this->create_interface();

    // set button connections
    connect(this->button_scan_ports, SIGNAL (released()), this, SLOT (scan_com_devices()));
    connect(this->button_select_serial, SIGNAL (released()), this, SLOT (select_com_port()));
    connect(this->button_read_cartridge, SIGNAL (released()), this, SLOT (read_cartridge()));
    connect(this->button_read_header, SIGNAL (released()), this, SLOT (read_header()));
    connect(this->button_read_ram, SIGNAL(released()), this, SLOT(read_ram()));
    connect(this->button_restore_ram, SIGNAL(released()), this, SLOT(write_ram()));

    // set icon
    setWindowIcon(QIcon(":/assets/img/logo.ico"));

    // status bar
    statusBar()->showMessage("Ready");
}

/**
 * @brief function that runs when window is closed
 * @param event
 */
void MainWindow::closeEvent(QCloseEvent *event) {
    UNUSED(event);
}

/****************************************************************************
 *  GRAPHICAL USER INTERFACE BUILDER ROUTINES
 ****************************************************************************/

/**
 * @brief Build the GUI and connect all buttons to signals
 */
void MainWindow::create_interface() {
    QWidget* w = new QWidget(this);
    this->setCentralWidget(w);
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    // create general layout
    QHBoxLayout *main_layout = new QHBoxLayout();
    w->setLayout(main_layout);

    // create two columns
    QWidget* container_left_column = new QWidget(this);
    QWidget* container_middle_column = new QWidget(this);
    QWidget* container_right_column = new QWidget(this);
    QVBoxLayout *layout_left_column = new QVBoxLayout();
    QVBoxLayout *layout_middle_column = new QVBoxLayout();
    QVBoxLayout *layout_right_column = new QVBoxLayout();
    main_layout->addWidget(container_left_column);
    main_layout->addWidget(container_middle_column);
    main_layout->addWidget(container_right_column);
    container_left_column->setLayout(layout_left_column);
    container_middle_column->setLayout(layout_middle_column);
    container_right_column->setLayout(layout_right_column);

    // build gameboy cartridge information layout
    this->build_cartridge_information_menu(layout_left_column);

    // build serial interface menu
    this->build_serial_interface_menu(layout_middle_column);

    // build data interface menu
    this->build_data_interface_menu(layout_middle_column);

    // interface for burning roms
    this->build_burn_interface_menu(layout_right_column);
}

/**
  * @brief Set dropdown menu
  */
void MainWindow::build_dropdown_menu() {
    // create menu bar
    QMenuBar *menuBar = new QMenuBar;

    // add drop-down menus
    QMenu *menuFile = menuBar->addMenu(tr("&File"));
    QMenu *menuHelp = menuBar->addMenu(tr("&Help"));

    // add actions to menus

//    // debug menu for gameboy camera
//    QAction *action_gameboy_camera = new QAction(menuFile);
//    action_gameboy_camera->setText("Gameboy Camera");
//    menuFile->addAction(action_gameboy_camera);
//    connect(action_gameboy_camera, &QAction::triggered, this, &MainWindow::gameboy_camera_test);

    // quit
    QAction *action_quit = new QAction(menuFile);
    action_quit->setText(tr("Quit"));
    action_quit->setShortcuts(QKeySequence::Quit);
    menuFile->addAction(action_quit);
    connect(action_quit, &QAction::triggered, this, &MainWindow::exit);

    // debug log
    QAction *action_debug_log = new QAction(menuHelp);
    action_debug_log->setText(tr("Debug Log"));
    action_debug_log ->setShortcut(Qt::Key_F2);
    menuHelp->addAction(action_debug_log);
    connect(action_debug_log, &QAction::triggered, this, &MainWindow::show_debug_log);

    // about
    QAction *action_about = new QAction(menuHelp);
    action_about->setText(tr("About"));
    menuHelp->addAction(action_about);
    action_about ->setShortcut(QKeySequence::WhatsThis);
    connect(action_about, &QAction::triggered, this, &MainWindow::show_about);

    // build menu
    setMenuBar(menuBar);
}

/**
 * @brief Build GUI showing cartridge information
 * @param layout position where to put this part of the GUI
 */
void MainWindow::build_cartridge_information_menu(QVBoxLayout* target_layout) {
    // build cartridge information
    QGroupBox* cartridge_container = new QGroupBox("Cartridge Information");
    cartridge_container->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QGridLayout *cartridge_layout = new QGridLayout();
    cartridge_container->setLayout(cartridge_layout);
    target_layout->addWidget(cartridge_container);

    // add label titles
    cartridge_layout->addWidget(new QLabel("<b>Cartridge Title</b>"), 0, 0);
    cartridge_layout->addWidget(new QLabel("<b>Super Game Boy</b>"), 1, 0);
    cartridge_layout->addWidget(new QLabel("<b>Game Boy Color</b>"), 2, 0);
    cartridge_layout->addWidget(new QLabel("<b>Nintendo Logo</b>"), 3, 0);
    cartridge_layout->addWidget(new QLabel("<b>Header Checksum</b>"), 4, 0);
    cartridge_layout->addWidget(new QLabel("<b>ROM Checksum</b>"), 5, 0);
    cartridge_layout->addWidget(new QLabel("<b>Mapper Type</b>"), 6, 0);
    cartridge_layout->addWidget(new QLabel("<b>ROM size</b>"), 7, 0);
    cartridge_layout->addWidget(new QLabel("<b>RAM size</b>"), 8, 0);

    // add labels
    this->label_cartridge_title = new QLabel();
    this->label_super_game_boy = new QLabel();
    this->label_gameboy_color = new QLabel();
    this->label_nintendo_logo = new QLabel();
    this->label_header_checksum = new QLabel();
    this->label_rom_checksum = new QLabel();
    this->label_cartridge_type = new QLabel();
    this->label_rom_size = new QLabel();
    this->label_ram_size = new QLabel();

    // add widgets
    cartridge_layout->addWidget(this->label_cartridge_title, 0, 1);
    cartridge_layout->addWidget(this->label_super_game_boy, 1, 1);
    cartridge_layout->addWidget(this->label_gameboy_color, 2, 1);
    cartridge_layout->addWidget(this->label_nintendo_logo, 3, 1);
    cartridge_layout->addWidget(this->label_header_checksum, 4, 1);
    cartridge_layout->addWidget(this->label_rom_checksum, 5, 1);
    cartridge_layout->addWidget(this->label_cartridge_type, 6, 1);
    cartridge_layout->addWidget(this->label_rom_size, 7, 1);
    cartridge_layout->addWidget(this->label_ram_size, 8, 1);
}

/**
 * @brief Build GUI showing serial port interface
 * @param layout position where to put this part of the GUI
 */
void MainWindow::build_serial_interface_menu(QVBoxLayout* target_layout) {
    // create interface for serial ports
    QGroupBox* serial_container = new QGroupBox("Serial interface");
    serial_container->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QVBoxLayout *layout_serial_vertical = new QVBoxLayout();
    serial_container->setLayout(layout_serial_vertical);
    QHBoxLayout *serial_layout = new QHBoxLayout();
    target_layout->addWidget(serial_container);
    QWidget* container_serial_interface_selector = new QWidget();
    container_serial_interface_selector->setLayout(serial_layout);
    layout_serial_vertical->addWidget(container_serial_interface_selector);
    QLabel* comportlabel = new QLabel(tr("COM port"));
    serial_layout->addWidget(comportlabel);
    this->combobox_serial_ports = new QComboBox(this);
    serial_layout->addWidget(this->combobox_serial_ports);
    this->button_scan_ports = new QPushButton(tr("Scan"));
    serial_layout->addWidget(this->button_scan_ports);
    this->button_select_serial = new QPushButton(tr("Select"));
    this->button_select_serial->setEnabled(false);
    serial_layout->addWidget(this->button_select_serial);

    // create interface for currently selected com port
    QWidget* serial_selected_container = new QWidget();
    QHBoxLayout *serial_selected_layout = new QHBoxLayout();
    serial_selected_container->setLayout(serial_selected_layout);
    this->label_serial = new QLabel(tr("Please select a COM port from the menu above"));
    serial_selected_layout->addWidget(this->label_serial);
    this->label_board_id = new QLabel();
    serial_selected_layout->addWidget(this->label_board_id);
    layout_serial_vertical->addWidget(serial_selected_container);
}

/**
 * @brief Build GUI showing rom extraction and ram reading/writing
 * @param layout position where to put this part of the GUI
 */
void MainWindow::build_data_interface_menu(QVBoxLayout* target_layout) {
    // read and store data
    QGroupBox* data_container = new QGroupBox("Cartridge Interface");
    QGridLayout *data_layout = new QGridLayout();
    data_container->setLayout(data_layout);
    target_layout->addWidget(data_container);
    this->button_read_header = new QPushButton(tr("Read header"));
    data_layout->addWidget(this->button_read_header, 0, 0);
    this->button_read_header->setEnabled(false);
    this->button_read_cartridge = new QPushButton(tr("Backup ROM"));
    data_layout->addWidget(this->button_read_cartridge, 0, 1);
    this->button_read_cartridge->setEnabled(false);
    this->button_read_ram = new QPushButton(tr("Backup RAM"));
    data_layout->addWidget(this->button_read_ram, 1, 0);
    this->button_read_ram->setEnabled(false);
    this->button_restore_ram = new QPushButton(tr("Restore RAM"));
    data_layout->addWidget(this->button_restore_ram, 1, 1);
    this->button_restore_ram->setEnabled(false);

    // build progress indicator
    this->progress_bar_load = new QProgressBar();
    data_layout->addWidget(this->progress_bar_load, 2, 0, 1, 2);
}

/**
 * @brief Build GUI showing cart flashing
 * @param layout position where to put this part of the GUI
 */
void MainWindow::build_burn_interface_menu(QVBoxLayout* target_layout) {
    // create interface for serial ports
    QGroupBox* burn_container = new QGroupBox("Flash Cartridge");
    burn_container->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QVBoxLayout *layout_vertical = new QVBoxLayout();
    burn_container->setLayout(layout_vertical);
    target_layout->addWidget(burn_container);

    this->label_cartridge_image = new QLabel();
    QPixmap pixmap(":/assets/img/gameboy_cartridge_blank.png");
    this->label_cartridge_image->setPixmap(pixmap);
    this->label_cartridge_image->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    layout_vertical->addWidget(this->label_cartridge_image);

    // flash rom
    this->button_flash_rom = new QToolButton();
    layout_vertical->addWidget(this->button_flash_rom);

    this->menu_flash_rom = new QMenu(tr("Flash ROM"));
    this->button_flash_rom->setMenu(this->menu_flash_rom);
    this->button_flash_rom->setText(tr("Flash ROM"));
    this->button_flash_rom->setPopupMode(QToolButton::MenuButtonPopup);

    // populate drop-down menu with roms
    static const std::vector<std::pair<std::string, std::string>> roms = {
        {"Select ROM from file", ""},
        {"Flappybird", ":/assets/roms/flappybird.gb"},
        {"Quartet", ":/assets/roms/quartet.gb"},
        {"Diagnostics cartridge", ":/assets/roms/diagnostics.gb"}
    };

    for(const auto& rom : roms) {
        std::cout << rom.second << std::endl;
        QAction* action = new QAction(this->menu_flash_rom);
        action->setData(QVariant(QString(rom.second.c_str())));
        action->setText(tr(rom.first.c_str()));
        action->setEnabled(false);
        connect(action, SIGNAL(triggered()), this, SLOT(flash_rom()));
        this->menu_flash_rom->addAction(action);
    }

    this->progress_bar_flash = new QProgressBar();
    layout_vertical->addWidget(this->progress_bar_flash);
}

/**
 * @brief Disable all interface boxes
 */
void MainWindow::disable_all_buttons() {
    this->button_select_serial->setEnabled(false);
    this->button_scan_ports->setEnabled(false);
    this->button_read_cartridge->setEnabled(false);
    this->button_read_header->setEnabled(false);

    // disable the following buttons based on cartridge settings
    this->button_flash_rom->setEnabled(false);
    this->button_read_ram->setEnabled(false);
    this->button_restore_ram->setEnabled(false);
}

/**
 * @brief Enable all interface boxes
 */
void MainWindow::enable_all_buttons() {
    this->button_select_serial->setEnabled(true);
    this->button_scan_ports->setEnabled(true);
    this->button_read_cartridge->setEnabled(true);
    this->button_read_header->setEnabled(true);

    if(this->gameboydata.get_ram_size_kb(this->header[0x149]) > 0) {
        this->button_read_ram->setEnabled(true);
        this->button_restore_ram->setEnabled(true);
    }

    this->button_flash_rom->setEnabled(true);
}

/****************************************************************************
 *  PRIVATE HELPER ROUTINES
 ****************************************************************************/

/**
 * @brief Parse cartridge header data and populate cartridge information
 */
void MainWindow::parse_header_data() {
    // set game title
    this->label_cartridge_type->setText(this->gameboydata.get_type(this->header[0x147]).c_str());
    this->label_cartridge_title->setText(this->header.mid(0x134, 16));

    // set game data
    if((uint8_t)this->header[0x146] == 0x03) {
        this->label_super_game_boy->setText("Yes");
    } else {
        this->label_super_game_boy->setText("No");
    }

    if((uint8_t)this->header[0x143] == 0x80 || (uint8_t)this->header[0x143] == 0xC0) {
        this->label_gameboy_color->setText("Yes");
    } else {
        this->label_gameboy_color->setText("No");
    }

    // check presence Nintendo logo
    std::vector<uint8_t> nintendo_logo =
        {0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,
         0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
         0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E};
    bool flag = true;
    for(unsigned int i=0; i<nintendo_logo.size(); i++) {
        if((uint8_t)this->header[0x104 + i] != nintendo_logo[i]) {
            flag = false;
            break;
        }
    }
    if(flag) {
        this->label_nintendo_logo->setText("<font color=\"green\">Valid</font>");
    } else {
        this->label_nintendo_logo->setText("<font color=\"red\">Invalid</font>");
    }

    // verify header checksum
    uint8_t checksum = 0;
    for(unsigned int i=0x134; i<=0x14C; i++) {
        checksum -= (uint8_t)this->header[i] + 1;
    }
    if(checksum == (uint8_t)this->header[0x14D]) {
        this->label_header_checksum->setText(tr("<font color=\"green\">0x") + tr("%1</font>").arg(checksum, 2, 16, QLatin1Char('0')).toUpper());
    } else {
        this->label_header_checksum->setText(tr("<font color=\"red\">0x") + tr("%1</font>").arg(checksum, 2, 16, QLatin1Char('0')).toUpper());
    }

    // show ROM checksum
    this->label_rom_checksum->setText(tr("0x") + tr("%1%2")
            .arg((uint8_t)this->header[0x014E], 2, 16, QLatin1Char('0'))
            .arg((uint8_t)this->header[0x014F], 2, 16, QLatin1Char('0')).toUpper());

    // set rom and ram size
    this->label_rom_size->setText(this->gameboydata.get_rom_size(this->header[0x148]).c_str());
    this->label_ram_size->setText(this->gameboydata.get_ram_size(this->header[0x149]).c_str());
    this->num_sectors = this->gameboydata.get_nr_sectors();
}

/****************************************************************************
 *  SIGNALS :: Help interface
 ****************************************************************************/

/**
 * @brief show about menu
 */
void MainWindow::show_about() {
    QMessageBox message_box;
    //message_box.setStyleSheet("QLabel{min-width: 250px; font-weight: normal;}");
    message_box.setText(PROGRAM_NAME
                        " version "
                        PROGRAM_VERSION
                        ".\nCompile time: " + QString(__DATE__) + " " + QString(__TIME__) +
                        ".\n\nAuthor: Ivo Filot <ivo@ivofilot.nl>\n\n"
                        "GBCR is licensed under the GPLv3 license.\n"
                        "GBCR is dynamically linked to Qt, which is licensed under LGPLv3.\n");
    message_box.setIcon(QMessageBox::Information);
    message_box.setWindowTitle("About " + tr(PROGRAM_NAME));
    message_box.setWindowIcon(QIcon(":/assets/img/logo.ico"));
    //message_box.setIconPixmap(QPixmap(":/assets/img/logo.ico"));
    message_box.exec();

}

/**
 * @brief show debug log
 */
void MainWindow::show_debug_log() {
    this->log_window->show();
}

/****************************************************************************
 *  SIGNALS :: COMMUNICATION INTERFACE ROUTINES
 ****************************************************************************/

/**
 * @brief Scan all communication ports to populate drop-down box
 */
void MainWindow::scan_com_devices() {
    // clear all previous data
    this->combobox_serial_ports->clear();
    this->port_identifiers.clear();

    // pattern to recognise GBCR (same ids as Arduino Leonardo)
    static const std::vector<std::pair<uint16_t, uint16_t> > patterns = {
        std::make_pair<uint16_t, uint16_t>(0x2341, 0x36),   // Arduino Leonardo / 32u4
        std::make_pair<uint16_t, uint16_t>(0x0403, 0x6001)  // FTDI FT232RL
    };

    // get communication devices
    QSerialPortInfo serial_port_info;
    QList<QSerialPortInfo> port_list = serial_port_info.availablePorts();
    std::unordered_map<std::string, std::pair<uint16_t, uint16_t> > ports;
    QStringList device_descriptors;
    qInfo() << "Discovered COM ports.";
    for(int i=0; i<port_list.size(); i++) {
        auto ids = std::make_pair<uint16_t,uint16_t>(port_list[i].vendorIdentifier(), port_list[i].productIdentifier());
        for(int j=0; j<patterns.size(); j++) {
            if(ids == patterns[j]) {
                ports.emplace(port_list[i].portName().toStdString(), ids);
                device_descriptors.append(port_list[i].description());
                qInfo() << port_list[i].portName().toStdString().c_str()
                        << QString("pid=0x%1, vid=0x%2,").arg(port_list[i].vendorIdentifier(),2,16).arg(port_list[i].productIdentifier(),2,16).toStdString().c_str()
                        << QString("descriptor=\"%1\",").arg(port_list[i].description()).toStdString().c_str()
                        << QString("serial=\"%1\"").arg(port_list[i].serialNumber()).toStdString().c_str();
            }
        }
    }

    // populate drop-down menu with valid ports
    for(const auto& item : ports) {
        this->combobox_serial_ports->addItem(item.first.c_str());
        this->port_identifiers.push_back(item.second);
    }

    // if more than one option is available, enable the button
    if(this->combobox_serial_ports->count() > 0) {
        this->button_select_serial->setEnabled(true);
    }


    if(port_identifiers.size() == 1) {
        statusBar()->showMessage(tr("Auto-selecting ") + this->combobox_serial_ports->itemText(0) + tr(" (vid and pid match board)."));
        this->combobox_serial_ports->setCurrentIndex(0);
        this->select_com_port();
    } else if(port_identifiers.size() > 1) {
        QMessageBox msg_box;
        msg_box.setIcon(QMessageBox::Warning);
        msg_box.setText(tr(
              "There are at least %1 devices that share the same id. Please ensure that only a single GBCR device is plugged in."
              " If multiple devices are plugged in, ensure you select the correct port. Please also note that the device id overlaps"
              " with the one from the Arduino Leonardo bootloader. If you have an Arduino Leonardo or compatible device plugged in,"
              " take care to unplug it or carefully select the correct port."
        ).arg(port_identifiers.size()));
        msg_box.setWindowIcon(QIcon(":/assets/img/logo.ico"));
        msg_box.exec();
    } else {
        QMessageBox msg_box;
        msg_box.setIcon(QMessageBox::Warning);
        msg_box.setText("Could not find a communication port with a matching id. Please make sure the GBCR device is plugged in.");
        msg_box.setWindowIcon(QIcon(":/assets/img/logo.ico"));
        msg_box.exec();
    }
}

/**
 * @brief Select communication port for serial to 32u4
 */
void MainWindow::select_com_port() {
    auto port_id = this->port_identifiers[this->combobox_serial_ports->currentIndex()];
    QString vendor_id;

    if(port_id == std::make_pair<uint16_t, uint16_t>(0x2341, 0x36)) {          // Arduino Leonardo / 32u4
        qDebug() << "Connecting to 32u4; setting baud rate to 115200.";
        this->serial_interface = std::make_shared<SerialInterface>(this->combobox_serial_ports->currentText().toStdString(), 115200);
        vendor_id = tr("0x%1 0x%2").arg(port_id.first, 0, 16).arg(port_id.second, 0, 16);
    } else if(port_id == std::make_pair<uint16_t, uint16_t>(0x0403, 0x6001)) {  // FTDI FT232RL
        qDebug() << "Connecting to 8515L; setting baud rate to 512000.";
        this->serial_interface = std::make_shared<SerialInterface>(this->combobox_serial_ports->currentText().toStdString(), 512000);
        vendor_id = tr("0x%1 0x%2").arg(port_id.first, 0, 16).arg(port_id.second, 0, 16);
    } else {
        throw std::runtime_error("Invalid port id.");
    }

    this->serial_interface->open_port();
    std::string board_info = this->serial_interface->get_board_info();
    std::string compile_time = this->serial_interface->get_compile_time();
    this->serial_interface->close_port();
    this->label_serial->setText(tr("<b>Port:</b> %1<br><b>ID:</b> %2").arg(this->combobox_serial_ports->currentText()).arg(vendor_id));
    this->label_board_id->setText(tr("<b>Board id:</b> %1<br><b>Compile time:</b> %2").arg(board_info.c_str()).arg(compile_time.c_str()));
    if(board_info.substr(0,4) == "GBCR") {
        this->button_read_header->setEnabled(true);
        for (QAction *action : this->menu_flash_rom->actions()) {
            action->setEnabled(true);
        }
    }
}

/****************************************************************************
 *  SIGNALS :: READ ROM ROUTINES
 ****************************************************************************/

/*
 * @brief Read cartridge header
 */
void MainWindow::read_header() {
    try {
        // read header data
        this->timer1.start();
        this->serial_interface->open_port();
        this->header = this->serial_interface->read_header();
        this->serial_interface->close_port();
        this->button_read_cartridge->setEnabled(true);

        this->parse_header_data();

        // enable read ram button if ram size is larger than 0kb
        if(this->gameboydata.get_ram_size_kb(this->header[0x149]) > 0) {
            this->button_read_ram->setEnabled(true);
            this->button_restore_ram->setEnabled(true);
        }

        this->progress_bar_load->reset();
        this->progress_bar_load->setMaximum(this->num_sectors);
        this->progress_bar_load->setMinimum(0);

        double seconds_passed = (double)this->timer1.elapsed() / 1000.0;
        statusBar()->showMessage(tr("Header read in %1 seconds.").arg(seconds_passed));
    } catch(const std::exception& e) {
        QMessageBox msg_box;
        msg_box.setIcon(QMessageBox::Critical);
        msg_box.setText(e.what());
        msg_box.exec();
        this->progress_bar_load->reset();
        this->button_read_cartridge->setEnabled(false);
    }
}

/**
 * @brief Read data from chip
 */
void MainWindow::read_cartridge() {
    // get number of sectors depending on chip
    if(this->num_sectors == 0) {
            QMessageBox messageBox;
            messageBox.critical(0, "Error", "Unknown chip, please first identify the chip");
            messageBox.setFixedSize(320,240);
    }

    // ask where to store file
    QString target = this->label_cartridge_title->text();
    if(this->label_gameboy_color->text() == "YES") {
        target += ".gbc";
    } else {
        target += ".gb";
    }
    this->current_filename = QFileDialog::getSaveFileName(this, tr("Select save location"), target, tr("Images (*.gb *.gbc *.bin *.dat)"));
    if(this->current_filename.length() == 0) {
        return;
    }

    // start reading chip
    statusBar()->showMessage("Reading from chip, please wait...");
    this->timer1.start();

    // disable all buttons so that the user cannot interrupt this task
    this->disable_all_buttons();

    // dispatch thread
    this->operation = "Reading"; // message for statusbar
    this->readerthread = std::make_unique<ReadThread>(this->serial_interface);
    this->readerthread->set_serial_port(this->combobox_serial_ports->currentText().toStdString());
    this->readerthread->set_data_package(this->num_sectors, this->gameboydata.get_mapper_id());
    this->readerthread->set_number_rom_banks(this->gameboydata.get_nr_banks(this->header[0x148]));
    connect(this->readerthread.get(), SIGNAL(read_result_ready()), this, SLOT(read_result_ready()));
    connect(this->readerthread.get(), SIGNAL(read_sector_start(uint)), this, SLOT(read_sector_start(uint)));
    connect(this->readerthread.get(), SIGNAL(read_sector_done(uint)), this, SLOT(read_sector_done(uint)));
    this->readerthread->start();
}



/**
 * @brief Slot to accept when a sector is ready
 */
void MainWindow::read_sector_start(unsigned int sector_id) {
    this->progress_bar_load->setValue(sector_id);
}

/**
 * @brief Slot to accept when a sector is ready
 */
void MainWindow::read_sector_done(unsigned int sector_id) {
    double seconds_passed = (double)this->timer1.elapsed() / 1000.0;
    double seconds_per_sector = seconds_passed / (double)(sector_id+1);
    double seconds_remaining = seconds_per_sector * (this->num_sectors - sector_id - 1);
    if(sector_id < (this->num_sectors - 1)) {
        statusBar()->showMessage(QString("%1 sector %2 / %3 : %4 seconds remaining.").arg(this->operation).arg(sector_id+1).arg(this->num_sectors).arg(seconds_remaining));
    }
    this->progress_bar_load->setValue(sector_id+1);
}

/*
 * @brief Signal that a read operation is finished
 */
void MainWindow::read_result_ready() {
    this->progress_bar_load->setValue(this->progress_bar_load->maximum());
    this->data = this->readerthread->get_data();
    this->readerthread.reset(); // delete object
    QFile file(this->current_filename);
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();

    // verify global checksum
    uint16_t global_checksum = 0;
    uint16_t global_checksum_cartridge = (uint8_t)this->header[0x14E] << 8 | (uint8_t)this->header[0x14F];
    for(unsigned int i=0; i<0x14E; i++) {
        global_checksum += (uint8_t)this->data[i];
    }
    for(int i=0x150; i<this->data.size(); i++) {
        global_checksum += (uint8_t)this->data[i];
    }
    if(global_checksum == global_checksum_cartridge) {
        statusBar()->showMessage("Ready - Done reading in " + QString::number((double)this->timer1.elapsed() / 1000) + " seconds. Global checksum valid.");
        this->label_rom_checksum->setText(tr("<font color=\"green\">0x") + tr("%1%2</font>")
                .arg((uint8_t)this->header[0x014E], 2, 16, QLatin1Char('0'))
                .arg((uint8_t)this->header[0x014F], 2, 16, QLatin1Char('0')).toUpper());
    } else {
        statusBar()->showMessage("Ready - Done reading in " + QString::number((double)this->timer1.elapsed() / 1000) + " seconds. Global checksum invalid.");
        this->label_rom_checksum->setText(tr("<font color=\"red\">0x") + tr("%1%2</font>")
                .arg((uint8_t)this->header[0x014E], 2, 16, QLatin1Char('0'))
                .arg((uint8_t)this->header[0x014F], 2, 16, QLatin1Char('0')).toUpper());
    }

    // re-enable all buttons when data is read
    this->enable_all_buttons();
}

/****************************************************************************
 *  SIGNALS :: READ RAM ROUTINES
 ****************************************************************************/

/*
 * @brief Read cartridge header
 */
void MainWindow::read_ram() {
    // ask where to store file
    this->current_filename = QFileDialog::getSaveFileName(this, tr("Select save location"), this->label_cartridge_title->text() + ".sav", tr("Images (*.sav)"));
    if(this->current_filename.length() == 0) {
        return;
    }

    // start reading chip
    statusBar()->showMessage("Reading from chip, please wait...");
    this->timer1.start();

    // disable all buttons so that the user cannot interrupt this task
    this->disable_all_buttons();

    // dispatch thread
    this->operation = "Reading"; // message for statusbar
    this->readramthread = std::make_unique<ReadRAMThread>(this->serial_interface);
    this->readramthread->set_serial_port(this->combobox_serial_ports->currentText().toStdString());
    this->readramthread->set_data_package(this->num_sectors, this->gameboydata.get_mapper_id());
    this->readramthread->set_number_ram_banks(this->gameboydata.get_nr_ram_banks(this->header[0x149]));
    this->readramthread->set_ram_size_kb(this->gameboydata.get_ram_size_kb(this->header[0x149]));

    // connect signals
    connect(this->readramthread.get(), SIGNAL(read_ram_result_ready()), this, SLOT(read_ram_result_ready()));

    // start reading RAM
    this->readramthread->start();
}

/*
 * @brief Signal that a RAM read operation is finished
 */
void MainWindow::read_ram_result_ready() {
    // store data
    this->save_data = this->readramthread->get_data();
    this->readramthread.reset(); // delete object
    QFile file(this->current_filename);
    file.open(QIODevice::WriteOnly);
    file.write(this->save_data);
    file.close();

    statusBar()->showMessage("Ready - Done reading RAM");

    // check if RAM image corresponds to gameboy camera
    if(this->label_cartridge_title->text().startsWith("GAMEBOYCAMERA")) {
        QMessageBox msg_box;
        msg_box.setIcon(QMessageBox::Question);
        msg_box.setWindowTitle("Gameboy Camera savefile detected");
        msg_box.setText("Would you like to extract the pictures from this savefile?");
        msg_box.setWindowIcon(QIcon(":/assets/img/logo.ico"));
        msg_box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msg_box.setDefaultButton(QMessageBox::Yes);
        int ret = msg_box.exec();

        if(ret == QMessageBox::Yes) {
            GameboyCamera *gbcam_window = new GameboyCamera(this->save_data);
            gbcam_window->show();
        }
    }

    // re-enable all buttons when data is read
    this->enable_all_buttons();

    // complete progress bar
    this->progress_bar_load->reset();
}

/****************************************************************************
 *  SIGNALS :: WRITE RAM ROUTINES
 ****************************************************************************/

    /*
     * @brief Read cartridge header
     */
    void MainWindow::write_ram() {
        // select file
        QString filename = QFileDialog::getOpenFileName(this, tr("Select savefile"), "", tr("Images (*.sav)"));
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox msg_box;
            msg_box.setIcon(QMessageBox::Critical);
            msg_box.setText("Could not open file. Please try again.");
            msg_box.exec();
            return;
        } else {
            this->save_data = file.readAll();
            file.close();
        }

        // check that the savefile has the correct size for this rom
        if(this->save_data.size()/1024 != this->gameboydata.get_ram_size_kb(this->header[0x149])) {
            QMessageBox msg_box;
            msg_box.setIcon(QMessageBox::Critical);
            msg_box.setText(tr("This cartridge has %1 kb of memory whereas the savefile selected has %2 kb of data. "
                               " This savefile is not compatible with this cartridge. Please select a new file and try again.")
                            .arg(this->gameboydata.get_ram_size_kb(this->header[0x149])).arg(this->save_data.size()/1024));
            msg_box.exec();
            this->save_data.clear();
            return;
        }

        // disable all buttons so that the user cannot interrupt this task
        this->disable_all_buttons();

        // dispatch thread
        this->operation = "Writing RAM"; // message for statusbar
        this->timer1.start();
        this->writeramthread = std::make_unique<WriteRAMThread>(this->serial_interface);
        this->writeramthread->set_serial_port(this->combobox_serial_ports->currentText().toStdString());
        this->writeramthread->set_data(this->save_data);
        this->writeramthread->set_number_ram_banks(this->gameboydata.get_nr_ram_banks(this->header[0x149]));
        this->writeramthread->set_ram_size_kb(this->gameboydata.get_ram_size_kb(this->header[0x149]));
        connect(this->writeramthread.get(), SIGNAL(write_ram_result_ready()), this, SLOT(write_ram_result_ready()));
        writeramthread->start();
    }

    /*
     * @brief Signal that a RAM read operation is finished
     */
    void MainWindow::write_ram_result_ready() {
        // store data
        this->writeramthread.reset(); // delete object

        statusBar()->showMessage("Ready - Done writing to RAM");

        // re-enable all buttons when data is read
        this->enable_all_buttons();
    }

/****************************************************************************
 *  SIGNALS :: FLASH ROM
 ****************************************************************************/

/**
 * @brief Put rom on flash cartridge
 */
void MainWindow::flash_rom() {
    QString url = qobject_cast<QAction*>(sender())->data().toString();
    QFile file;
    if(url.isEmpty()) {
        // select file
        QString filename = QFileDialog::getOpenFileName(this, tr("Select ROM"), "", tr("Images (*.bin *.dat *.gb *.gbc)"));
        if(filename.length() == 0) {
            return;
        }
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox msg_box;
            msg_box.setIcon(QMessageBox::Critical);
            msg_box.setText("Could not open file. Please try again.");
            msg_box.exec();
            return;
        } else {
            this->flash_data = file.readAll();
            file.close();
        }
    } else {
        QFile file(url);
        if (file.open(QIODevice::ReadOnly)) {
            this->flash_data = file.readAll();
        }
    }

    // check that the savefile has the correct size for this rom
    if(this->flash_data.size()/1024 != 32) {
        QMessageBox msg_box;
        msg_box.setIcon(QMessageBox::Critical);
        msg_box.setText(tr("This flash cartrdige has %1 kb of memory whereas the ROM file selected has %2 kb of data. "
                           " This ROM file is not compatible with this cartridge. Please select a new file and try again.")
                        .arg(32).arg(this->flash_data.size()/1024));
        msg_box.exec();
        this->save_data.clear();
        return;
    }

    // dispatch thread
    this->operation = "Flashing"; // message for statusbar
    this->timer1.start();

    static const int flash_id = 2; // SST39SF0x0-based cartridge
    unsigned int num_pages = 0;
    switch(flash_id) {
    case 1:
        num_pages = 32 * 1024 / 64;
    break;
    case 2:
        num_pages = 32 * 1024 / 256;
    break;
    default:
        num_pages = 0;
    break;
    }

    this->progress_bar_flash->setMaximum(num_pages);
    this->flashthread = std::make_unique<FlashThread>(this->serial_interface);
    this->flashthread->set_serial_port(this->combobox_serial_ports->currentText().toStdString());
    this->flashthread->set_data(this->flash_data);
    this->flashthread->set_flash_card_id(flash_id);

    connect(this->flashthread.get(), SIGNAL(flash_result_ready()), this, SLOT(flash_result_ready()));
    connect(this->flashthread.get(), SIGNAL(flash_page_start(uint)), this, SLOT(flash_page_start(uint)));
    connect(this->flashthread.get(), SIGNAL(flash_page_done(uint)), this, SLOT(flash_page_done(uint)));
    connect(this->flashthread.get(), SIGNAL(flash_chip_id_error(uint)), this, SLOT(flash_chip_id_error(uint)));
    flashthread->start();

    // disable all buttons
    this->disable_all_buttons();
}

/**
 * @brief Slot to indicate that a page is about to be written
 */
void MainWindow::flash_page_start(unsigned int page_id) {
    this->progress_bar_flash->setValue(page_id);
}

/**
 * @brief Slot to accept that a page is written
 */
void MainWindow::flash_page_done(unsigned int page_id) {
    unsigned int num_pages = this->flashthread->get_num_pages();
    double seconds_passed = (double)this->timer1.elapsed() / 1000.0;
    double seconds_per_page = seconds_passed / (double)(page_id+1);
    double seconds_remaining = seconds_per_page * (num_pages - page_id - 1);
    if(page_id < (num_pages - 1)) {
        statusBar()->showMessage(QString("%1 page %2 / %3 : %4 seconds remaining.").arg(this->operation).arg(page_id+1).arg(num_pages).arg(seconds_remaining));
    }
    this->progress_bar_flash->setValue(page_id+1);
}

/*
 * @brief Signal that a flash operation is finished
 */
void MainWindow::flash_result_ready() {
    this->progress_bar_flash->setValue(this->progress_bar_flash->maximum());
    statusBar()->showMessage("Ready - Done flashing in " + QString::number((double)this->timer1.elapsed() / 1000) + " seconds.");

    // dispatch thread
    this->operation = "Verifying"; // message for statusbar
    this->timer1.start();
    this->num_sectors = 8;
    this->readerthread = std::make_unique<ReadThread>(this->serial_interface);
    this->readerthread->set_serial_port(this->combobox_serial_ports->currentText().toStdString());
    this->readerthread->set_data_package(8, 0x00); // 8 sectors, no rom banking
    this->readerthread->set_number_rom_banks(2);   // 2 banks

    // set progress bar
    this->progress_bar_flash->reset();
    this->progress_bar_flash->setMaximum(8);

    connect(this->readerthread.get(), SIGNAL(read_result_ready()), this, SLOT(verify_result_ready()));
    connect(this->readerthread.get(), SIGNAL(read_sector_start(uint)), this, SLOT(verify_sector_start(uint)));
    connect(this->readerthread.get(), SIGNAL(read_sector_done(uint)), this, SLOT(verify_sector_done(uint)));
    readerthread->start();
}

/*
 * @brief Response that the chip id could not be verified
 */
void MainWindow::flash_chip_id_error(unsigned int chip_id) {
    QMessageBox msg_box;
    msg_box.setIcon(QMessageBox::Warning);
    msg_box.setText(tr("The chip id (%1) does not match the proper value for a SST39SF0x0 chip. Please verify that you inserted"
                       " and/or selected the right FLASH cartridge. If so, resocket the cartridge and try again.").arg(chip_id,0,16));
    msg_box.setWindowIcon(QIcon(":/assets/img/logo.ico"));
    msg_box.exec();

    // reset flash button
    this->button_flash_rom->setEnabled(true);
}

/**
 * @brief Slot to accept when a sector is about to be verified
 */
void MainWindow::verify_sector_start(unsigned int sector_id) {
    this->progress_bar_flash->setValue(sector_id);
}

/**
 * @brief Slot to accept when a sector is verified
 */
void MainWindow::verify_sector_done(unsigned int sector_id) {
    double seconds_passed = (double)this->timer1.elapsed() / 1000.0;
    double seconds_per_sector = seconds_passed / (double)(sector_id+1);
    double seconds_remaining = seconds_per_sector * (this->num_sectors - sector_id - 1);
    if(sector_id < (this->num_sectors - 1)) {
        statusBar()->showMessage(QString("%1 sector %2 / %3 : %4 seconds remaining.").arg(this->operation).arg(sector_id+1).arg(this->num_sectors).arg(seconds_remaining));
    }
    this->progress_bar_flash->setValue(sector_id+1);
}

/*
 * @brief Signal that a verified operation is finished
 */
void MainWindow::verify_result_ready() {
    this->progress_bar_flash->setValue(this->num_sectors);
    QByteArray verify_data = this->readerthread->get_data();
    this->readerthread.reset(); // delete object

    if(verify_data == this->flash_data) {
        statusBar()->showMessage("Ready - Done verification in " + QString::number((double)this->timer1.elapsed() / 1000) + " seconds.");
        QMessageBox msg_box(QMessageBox::Information,
                "Flash complete",
                "Cartridge was successfully flashed. Data integrity verified.",
                QMessageBox::Ok, this);
        msg_box.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        msg_box.exec();

        // read header
        this->header = this->flash_data.mid(0, 0x150);
        this->parse_header_data();
    } else {
        QMessageBox msg_box(QMessageBox::Critical,
                "Error",
                "Data integrity could not be verified. Please try to reflash the cartridge. It might help to resocket the flash cartridge.",
                QMessageBox::Ok, this);
        msg_box.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        msg_box.exec();
    }

    // re-enable all buttons when data is read
    this->enable_all_buttons();
}

/****************************************************************************
 *  SIGNALS :: OTHER
 ****************************************************************************/

/**
 * @brief      Test the Gameboy Camera Interface
 */
void MainWindow::gameboy_camera_test() {
    QFile file("GAMEBOYCAMERA.sav");
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox msg_box;
        msg_box.setIcon(QMessageBox::Critical);
        msg_box.setText("Could not open file. Please try again.");
        msg_box.exec();
        return;
    } else {
        QByteArray example_data = file.readAll();
        file.close();
        GameboyCamera *gbcam_window = new GameboyCamera(example_data);
        gbcam_window->show();
    }
}

/**
 * @brief      Close the application
 */
void MainWindow::exit() {
    QApplication::quit();
}
