#include "writeramthread.h"

/**
 * @brief read the ROM from a cartridge
 *
 * This routine will be called when a thread containing this
 * class is runned
 */
void WriteRAMThread::run() {
    unsigned int sector_counter = 0;

    this->serial_interface->open_port();

    // always start at RAM bank 0
    this->serial_interface->change_ram_bank(0);

    // only scan regular ram
    if(this->ram_size_kb < 8) {
        this->serial_interface->set_ram(true);
        this->serial_interface->write_ram(this->data, false);
        this->serial_interface->set_ram(false);
    } else {
        // write to ram banks
        for(unsigned int j=0; j<this->nr_ram_banks; j++) {
            if(this->nr_ram_banks > 1) {
                this->serial_interface->change_ram_bank(j);
            }
            this->serial_interface->set_ram(true);
            for(unsigned int i=0; i<2; i++) {  // 2 sectors per bank (each bank is 8k)
                unsigned int sidx = (i+j*2) * 0x1000;
                this->serial_interface->write_ram(this->data.mid(sidx, 0x1000), i);
                sector_counter++;
            }
            this->serial_interface->set_ram(false);
        }
    }

    // disable RAM
    this->serial_interface->set_ram(false);

    this->serial_interface->close_port();
    emit(write_ram_result_ready());
}
