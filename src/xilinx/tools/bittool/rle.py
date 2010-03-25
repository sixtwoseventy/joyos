def rleEncode(raw):
    p = 0
    enc = []
    
    while (p<len(raw)):
        j = p
        curByte = raw[p]
        while (raw[j]==curByte):
            if ((j+1)==len(raw)):
                j+=1
                break;
            if (j-p)>=63:
                break;
            j+=1
        j-=p
        
        if (j==1 and (0xC0 != (ord(curByte) & 0xC0))):
            enc.append(curByte)
        else:
            enc.append(chr(j | 0xC0))
            enc.append(curByte)
        p+=j

    return enc

def rleBlockEncode(raw):
    p = 0
    blocks = []
    block = [chr(0)]
    
    while (p<len(raw)):
        j = p
        curByte = raw[p]
        while (raw[j]==curByte):
            if ((j+1)==len(raw)):
                j+=1
                break;
            if (j-p)>=63:
                break;
            j+=1
        j-=p
        
        if (j==1 and (0xC0 != (ord(curByte) & 0xC0))):
            if (len(block)<128):
                block.append(curByte)
            else:
                block[0] = chr(len(block))
                blocks.append(block)
                block = [chr(0)]
                block.append(curByte)
        else:
            if (len(block)<127):
                block.append(chr(j | 0xC0))
                block.append(curByte)
            else:
                block[0] = chr(len(block))
                blocks.append(block)
                block = [chr(0)]
                block.append(chr(j | 0xC0))
                block.append(curByte)

        p+=j

    if (len(block)>1):
        block[0] = chr(len(block))
        blocks.append(block)

    blockeddata = []
    bcount = 0
    for b in blocks:
        for d in b:
            blockeddata.append(d)
        if (len(b)==127):
            blockeddata.append(chr(0))
        bcount+=1
    
    return blockeddata


def rleDecode(enc):
    out = []
    p = 0
    while (p<len(enc)):
        count = 1
        ch = enc[p]
        p+=1
        if ((ord(ch) & 0xC0) == 0xC0):
            count = 0x3F & ord(ch)
            ch = enc[p]
            p+=1
        for j in range(count):
            out.append(ch)
    return out


# block format
# start | len   | use
# 0x00  | 1     | block length (0-127 bytes)
# 0x01  | 1-127 | data
# 0xNN  | 1-127 | filler

def rleBlockDecode(enc):
    out = []
    p = 0
    outcount = 0
    while (p<len(enc)):
        blocklen = ord(enc[p])
        blockend = p+blocklen
        p+=1
#        if (blocklen!=128 and blocklen!=127):
#            print "small block warning : last block?"
        #print blocklen, ord(enc[p+blocklen-2])
        while (p<blockend):
            count = 1
            ch = enc[p]
            p+=1
            if ((ord(ch) & 0xC0) == 0xC0):
                count = 0x3F & ord(ch)
                ch = enc[p]
                p+=1
            for j in range(count):
                out.append(ch)
                outcount+=1
        if (blocklen==127):
            p+=1
    print
    return out
