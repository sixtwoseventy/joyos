XST = xst
NGDBUILD = ngdbuild
MAP = map
PAR = par
BITGEN = bitgen
BITTOOL = /usr/bin/python ../tools/bittool/bittool.py

# base source/output name
BASENAME = happyio
# fpga part
PARTNAME = xc3s50-4vq100
# verilog sources
SRC = $(BASENAME).v \
			Motor.v \
			Servo.v \
			Encoder.v \
			Quadrature.v \
			Pwm.v \

all: $(BASENAME).rle

checksyntax: $(BASENAME).ngc

# Verilog compile
%.ngc: $(SRC)
	echo "run -ifn $(BASENAME).v -ifmt Verilog -ofn $@ -ofmt NGC -p $(PARTNAME)  -opt_mode Speed -opt_level 1 -ent $(BASENAME)" -top ${BASENAME} > xst.scr
	$(XST) -ifn xst.scr
	rm xst.scr

# Build Xilinx project DB
%.ngd: %.ngc %.ucf
	$(NGDBUILD) -aul -p $(PARTNAME) -nt timestamp -uc $(BASENAME).ucf $< $@

# Map
%.ncd: %.ngd
	$(MAP) -p $(PARTNAME) $<

# Place and route
%-placed.ncd: %.ncd
	$(PAR)  $< $@

# Generate bitfile
%.bit: %-placed.ncd
	$(BITGEN) -b -w $< $@

# RLE compress bitfile
%.rle: %.bit
	$(BITTOOL) -c -o $@ $(BASENAME).rbt

# Upload to FPGA
program: $(BASENAME).rle
	$(BITTOOL) -p com4 -u $<

# clean temporary files
cleantemp:
	rm -f $(BASENAME).ngc
	rm -f $(BASENAME).ngd
	rm -f $(BASENAME).ncd
	rm -f $(BASENAME).bld
	rm -f $(BASENAME).mrp
	rm -f $(BASENAME).ngm
	rm -f $(BASENAME).pcf
	rm -f $(BASENAME).bgn
	rm -f $(BASENAME).drc
	rm -f $(BASENAME).bit
	rm -f $(BASENAME)-placed*
	rm -f $(BASENAME)_usage.xml
	rm -f xst.srp
	rm -f netlist.lst
	rm -rf xst

# clean everything
clean: cleantemp
	rm -f $(BASENAME).rbt
	rm -f $(BASENAME).rle

.PHONY: clean cleantemp
	

