#!/bin/bash

/d/avrdude/avrdude.exe \
-c usbasp -p m32u4 \
-U lfuse:w:0xFF:m \
-U hfuse:w:0xD8:m \
-U efuse:w:0xCB:m
