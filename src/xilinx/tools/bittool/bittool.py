#!/usr/bin/env python

import sys
import getopt
import rle
import time
import serial
import struct
import os

TASK_NONE       = 0
TASK_COMPRESS   = 1
TASK_DECOMPRESS = 2
TASK_UPLOAD     = 4

def crc16_update(crc, data):
    crc = crc ^ (data << 8)
    for i in range(8):
        if crc & 0x8000 != 0:
            crc = ((crc << 1) ^ 0x1021)
        else:
            crc = crc << 1
    return crc & 0xFFFF

def crc16_update_arr16(crc, arr):
    for a in arr:
        crc = crc16_update(crc,a&0xFF)
        crc = crc16_update(crc,a>>8)
    return crc


class Bittool:

    algorithms = {
            "rle" : ["Simple RLE compression",rle.rleEncode,rle.rleDecode],
            "brle" : ["Block level RLE compression",rle.rleBlockEncode,rle.rleBlockDecode],
    }

    def usage(self):
        print "Bittool v0.02 05/01/08"
        print "Usage: bittool [options] files..."
        print "Options:"
        print "  -a <algo>   Use compression algorithm <algo>"
        print "  -c          Compress .bit file"
        print "  -d          Decompress .rle file"
        print "  -f <file>   use bittool config file"
        print "  -h          Display this text"
        print "  -o <file>   Output to <file>"
        print "  -p <port>   Use com port <port> for happyboard communication"
        print "  -u          Upload to Happyboard after compression"
        sys.exit(1)

    def parse_config(self):
        self.conf = {}
        f = file(self.conffile,"rb")
        lines = f.readlines()
        f.close()
        for l in lines:
            if l[0]=="#" or l.rstrip()=="":
                continue
            ls = l.split("=")
            if len(ls)!=2:
                print "invalid config option: %s" % l
                sys.exit(1)
            n = ls[0].rstrip()
            v = ls[1].lstrip().rstrip()
            self.conf[n] = v

        #print "Config:"
        #keys = self.conf.keys()
        #keys.sort()
        #for c in keys:
        #print "  %s%s= %s" % (c, " "*(20-len(c)), self.conf[c])

    def compress(self):
        if self.outputFile==None:
            self.outputFile = os.path.splitext(self.args[0])[0]+".rle"
        f = file(self.args[0],"rb")
        din = []
        for i in range(7):
            f.readline()
        while (1):
            v=0
            line = f.readline().strip()
            if len(line)==0:
                break;
            if len(line)!=32:
                print "error!",len(line)
            for i in range(32):
                v <<= 1
                if (line[i]=="1"):
                    v |= 1
            din.append(chr((v>>24)&0xFF))
            din.append(chr((v>>16)&0xFF))
            din.append(chr((v>>8 )&0xFF))
            din.append(chr((v    )&0xFF))
        f.close()
        dout = self.compressor(din)
        b_ver = int(self.conf["BOARD_VERSION"],16)
        b_id = int(self.conf["BOARD_ID"],16)
        fpga_ver = int(self.conf["FPGA_VERSION"],16)
        fpga_len = len(dout)
        crc = crc16_update_arr16(0, [b_ver, b_id, fpga_ver, fpga_len])
        header = struct.pack("<HHHHH", b_ver, b_id, fpga_ver, fpga_len, crc)
        #print "%04x %04x %04x %04x %04x" % ( b_ver, b_id, fpga_ver, fpga_len, crc)
        f = file(self.outputFile,"wb")
        for d in header:
          f.write(d)
        for i in range(256-len(header)):
          f.write(chr(0xFF))
        for d in dout:
            f.write(d)
        f.close();

    def decompress(self):
        if self.outputFile==None:
            self.outputFile = self.args[0].split(".")[0]+".rbt"
        sourceName = self.args[0].split(".")[0]+".ncd"
        archName = "spartan3"
        partName = "3s50vq100"
        f = file(self.args[0],"rb")
        size = f.read(2)
        print (ord(size[0])<<8) + ord(size[1])
        din = f.read()
        f.close()
        dout = self.decompressor(din)
        f = file(self.outputFile,"wt")
        f.write("Xilinx ASCII Bitstream\nCreated by Bittool\nDesign name: \t%s\nArchitecture:\t%s\nPart:        \t%s\nDate:        \t%s\nBits:        \t%d\n" % (
                sourceName,
                archName,
                partName,
                time.asctime(time.gmtime(time.time())),
                len(dout)*8
                ));
        for i in range(len(dout)/4):
            n = i*4
            v = (ord(dout[n])<<24) | (ord(dout[n+1])<<16) | (ord(dout[n+2])<<8) | (ord(dout[n+3]))
            s = ""
            for j in range(32):
                s = str(v&1)+s
                v >>= 1
            f.write(s+"\n")

    def waitForResp(self, port, l, timeout=0.1):

        start = time.time()
        resp = ""
        #print "waiting for response of len %d" % l
        while (time.time()-start)<timeout:
            if port.inWaiting():
                resp+=port.read(1)
                if len(resp)==l:
                    return resp
        raise Exception("timeout waiting for response from happyboard")

    def upload(self):
        port = serial.Serial(self.port,19200)

        # check for happyboard
        port.write("1")
        port.write(" ")
        ret = self.waitForResp(port,9,1.0);
        if (ret[1:-1]!="AVR ISP"):
            raise Exception("Happyboard not found")


        # set address
        port.write("U")
        port.write(chr(0))
        port.write(chr(0))
        port.write(" ")
        self.waitForResp(port,2)

        # upload blocks
        f = file(self.args[0],"rb")
        din = f.read()
        f.close()
        dataLen = len(din)
        dataSent = 0

        print "Writing %dbytes:" % dataLen
        while (dataSent<dataLen):
            sys.stdout.write(".")
            sys.stdout.flush()
            port.write("d")
            if (dataLen-dataSent)>=256:
                blockLen = 256
            else:
                blockLen = dataLen-dataSent
            port.write(chr(blockLen>>8))
            port.write(chr(blockLen&0xFF))
            port.write("X")
            for i in range(blockLen):
                port.write(din[dataSent])
                dataSent+=1
            port.write(" ")

            self.waitForResp(port,2,0.5)
        print
        print "Done."

        port.close()

    def error(self,message):
        print "ERROR:",message
        sys.exit(1)

    def main(self,args):
        self.prog = args[0]
        self.args = args[1:]
        
        optlist,self.args = getopt.getopt(self.args,"a:cdfho:p:u")
        self.outputFile = None
        self.task = TASK_NONE
        self.algo = "rle"
        self.port = "com7"
        self.conffile = os.path.join(os.path.dirname(self.prog),"bittool.conf")

        for o, a in optlist:
            if (o=="-a"):
                self.algo = a
            if (o=="-c"):
                self.task = TASK_COMPRESS
            if (o=="-d"):
                self.task = TASK_DECOMPRESS
            if (o=="-f"):
                self.conffile = a
            if (o=="-h"):
                self.usage()
            if (o=="-o"):
                self.outputFile = a
            if (o=="-p"):
                self.port = a
            if (o=="-u"):
                self.task = TASK_UPLOAD

        if len(self.args)!=1:
            self.usage()

        if (self.algo not in self.algorithms):
            self.usage()

        self.compressor = self.algorithms[self.algo][1]
        self.decompressor = self.algorithms[self.algo][2]

        self.parse_config()

        if (self.task==TASK_NONE):
            self.usage()
        elif (self.task==TASK_COMPRESS):
            self.compress()
        elif (self.task==TASK_DECOMPRESS):
            self.decompress()
        elif (self.task==TASK_UPLOAD):
            self.upload()
        else:
            self.usage()


if __name__=="__main__":
    bt = Bittool()
    bt.main(sys.argv)
