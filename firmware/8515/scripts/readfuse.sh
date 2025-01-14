#!/bin/bash

/d/avrdude/avrdude.exe \
-c usbasp -p m8515 \
-U lfuse:r:-:h \
-U hfuse:r:-:h \
