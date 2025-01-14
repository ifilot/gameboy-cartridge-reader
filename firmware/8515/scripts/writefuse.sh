#!/bin/bash

/d/avrdude/avrdude.exe \
-c usbasp -p m8515 \
-U lfuse:w:0xFF:m \
-U hfuse:w:0xD9:m \
