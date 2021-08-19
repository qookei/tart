#!/bin/env python

import subprocess, sys, os

def mktemp():
	res = subprocess.run(['mktemp'], capture_output = True)
	return res.stdout.decode()[:-1]


c_prog = sys.argv[1]
objcopy_prog = sys.argv[2]

in_file = sys.argv[3]
out_file = sys.argv[4]

temp_elf = mktemp()
subprocess.run([c_prog, '-o', temp_elf, in_file, '-Wl,-Ttext,0', '-nostdlib', '-mthumb'])

temp_bin = mktemp()
subprocess.run([objcopy_prog, temp_elf, '--dump-section', f'.text={temp_bin}'])

with open(out_file, 'w') as out_f:
	with open(temp_bin, 'rb') as bin_f:
		for i in bin_f.read():
			out_f.write(hex(i) + ',')

os.remove(temp_elf)
os.remove(temp_bin)
