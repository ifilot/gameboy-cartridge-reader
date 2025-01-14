#ifndef WRITERAMTHREAD_H
#define WRITERAMTHREAD_H

#include "ioworker.h"

class WriteRAMThread : public IOWorker
{
    Q_OBJECT

private:
    unsigned int nr_ram_banks = 0;      // number of banks to read
    unsigned int ram_size_kb = 0;       // ram size in kb

public:
    WriteRAMThread() {}

    WriteRAMThread(const std::shared_ptr<SerialInterface>& _serial_interface) :
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
    void write_ram_result_ready();
};

#endif // WRITERAMTHREAD_H
