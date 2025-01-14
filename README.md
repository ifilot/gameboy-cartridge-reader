# Gameboy Cartridge Reader

![GitHub tag (latest SemVer)](https://img.shields.io/github/v/tag/ifilot/gameboy-cartridge-reader?label=version)
[![build](https://github.com/ifilot/gameboy-cartridge-reader/actions/workflows/build.yml/badge.svg)](https://github.com/ifilot/gameboy-cartridge-reader/actions/workflows/build.yml)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![License: CC BY-SA 4.0](https://img.shields.io/badge/License-CC%20BY--SA%204.0-blue.svg)](https://creativecommons.org/licenses/by-sa/4.0/)

This project is a Gameboy cartridge reader designed around the Atmel ATmega32u4
microcontroller. It provides an easy way to interface with Gameboy cartridges,
enabling data reading, writing, and backup. The design includes a custom PCB,
firmware for the 32u4, and a simple GUI for user interaction.

> [!NOTE]
> Download a Windows 64-bit installer for the GUI [via this link](https://github.com/ifilot/gameboy-cartridge-reader/releases/latest/download/gbcr-installer-win64.exe)

<img src="img/gameboy-cartridge-reader-with-cartridge-02.png" style="width: 50%;" alt="PCB with components">

## Features

* **Custom PCB**: Compact and optimized PCB designed for seamless connection with
  Gameboy cartridges and the 32u4.
* **Firmware**: Lightweight firmware written for the ATmega32u4, handling cartridge
  communication and USB interfacing.
* **GUI**: User-friendly graphical interface to manage cartridge data operations on
  your computer.
* **Open Source**: All design files, firmware, and GUI are open-source and available
  for modification.
* **Writable Cartridge Support**: Includes a [PCB design](pcb/gb-cart-sst39sf010/) 
  with an SST39SF010 chip, enabling the writing of 32KiB Gameboy games.

> [!IMPORTANT] 
> Please be aware of copyright laws regarding Gameboy and Gameboy
> Color games. Making copies of game data is strictly prohibited in many
> countries unless you own the original cartridge and are creating a backup for
> personal use where such actions are legally permitted. It is your
> responsibility to ensure compliance with local laws. This project is intended
> solely for **educational purposes** and **lawful personal** use.

## GUI

The GUI allows users to interact with their Gameboy cartridges through a
straightforward interface. Users can read ROM data from cartridges, create
backups of save files, and write new save files to supported cartridges.
Additionally, the GUI provides options for verifying data integrity, viewing
cartridge metadata (such as title and game ID), and managing writable cartridges
for flashing custom or homebrew games. Designed for simplicity, the GUI ensures
that all operations are intuitive and accessible, even for beginners.

![Graphical User Interface](img/screenshot_gui.png)

## Bundled games

The following games are bundled with the GUI and can be readily flashed by the
user to a writeable cartridge.

* [Quartet](https://makrill.itch.io/quartet)
* [Flappybird](https://github.com/pashutk/flappybird-gameboy)

## PCB

The PCB primarily utilizes SMD components, with the exception of the 16MHz
crystal and the push button, which are through-hole components. To simplify the
soldering process, it is recommended that users order a solder paste stencil and
use a hot plate to solder the SMD components efficiently and accurately.

<img src="img/gameboy-cartridge-reader.png" style="width: 50%;" alt="PCB with components">

<img src="img/pcb.png" style="width: 50%;" alt="PCB">

### Schematic

![PCB schematic](pcb/32u4/dmg-gbc-cartridge-reader.svg)

### Writeable cartridge

The writable cartridge for the Game Boy, based on the SST39SF010 flash memory
chip, offers versatile support for both TTSOP32 and PLCC32 package variants.
This flexibility allows developers and enthusiasts to choose their preferred
chip packaging without requiring additional hardware modifications. The
cartridge is designed to leverage the high-speed erase and write capabilities of
the SST39SF010, making it ideal for prototyping or homebrew games.

<img src="img/gameboy-writeable-cartridge.png" style="width: 50%;" alt="Writeable cartridge">

## Firmware

The firmware for the ATmega32u4 must be compiled using Atmel Studio to ensure
compatibility and proper optimization. Once compiled, the firmware can be
flashed to the microcontroller using an ISP programmer, such as AVRDUDE, for
seamless and reliable programming.

### Fuses

See the `firmware/<DEVICE>/scripts` folder for reading and writing the fuses. 
The proper fuse settings for `lfuse`, `hfuse` and `efuse` should 
be `0xFF`, `0xD8` and `0xCB`, respectively.

## Case

A 3D-printable case for the Gameboy cartridge reader is available in the
[cases](cases) folder, providing a protective and aesthetically pleasing
enclosure for the assembled PCB.