#!/bin/bash

/d/avrdude/avrdude.exe \
-c usbasp -p m32u4 \
-U lfuse:r:-:h \
-U hfuse:r:-:h \
-U efuse:r:-:h -v
