import argparse
import os
import sys
import struct

from numpy import empty, uint32, fromfile,uint16

#Unicode keys
TAB = unichr(9)
EOL = unichr(10)


FID_nSNPsRead = 1000
FID_IlluminaID = 102
FID_SD = 103
FID_Mean = 104
FID_NBeads = 107
FID_MidBlock = 200
FID_RunInfo = 300
FID_RedGreen = 400
FID_MostlyNull = 401
FID_Barcode = 402
FID_ChipType = 403



FIDS = [FID_nSNPsRead,FID_IlluminaID,FID_SD,FID_Mean,FID_NBeads,FID_MidBlock,FID_RunInfo,FID_RedGreen,FID_MostlyNull,FID_Barcode,FID_ChipType]

class SampleEntry:
    def __init__(self, pid, fs):
        self.pid = pid
        self.fs = fs


def getiDatHash(idat_dir):
    tree = os.walk(idat_dir)
    hash = {}
    for(d, subds, fs) in tree:
        for f in fs:
            if f.endswith(".idat"):
                hash[f] = os.path.join(d,f)
    return hash


fileName = "202105000056_R01C01_Grn.idat"
def read():
    with open(fileName) as mf:
        idats = []
        for line in mf:
            recs = ","
            pid = 0
            barcode = 3 
            pos = 4
            curr_fs = [] 
            ok = True

            warning = ""
            
            for colour in ["Red", "Grn"]:
                base_file = "{barcode}_{pos}_{colour}.idat".format(barcode = barcode, pos = pos, colour = colour)
                # f = idat_hash[base_file]                f = getiDatHash(base_file)
                
                this_ok = True##os.access(f,os.R_OK)
                if not this_ok: warning = warning + "Warning: File does not exist or readable{}".format(EOL)
                ok = ok &  this_ok
                # curr_fs.append(f)
            if not ok:
                if True:
                    if not False:
                        sys.sterr(warning+EOL)
                        continue
                else:
                    sys.exit("Missing idat files: " + EOL+warning)
            idats.append(SampleEntry(pid, curr_fs))


        print(idats[3].pid)
        print(idats[4].pid)
        print(idats[5].pid)
read()
