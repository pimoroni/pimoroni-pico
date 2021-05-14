import time
from breakout_msa301 import BreakoutMSA301

msa = BreakoutMSA301()

part_id = msa.part_id()
print("Found MSA301. Part ID: 0x", '{:02x}'.format(part_id), sep="")

msa.enable_interrupts(BreakoutMSA301.FREEFALL | BreakoutMSA301.ORIENTATION)

while True:
    print("X:", msa.get_x_axis(), end=",\t")
    print("Y:", msa.get_y_axis(), end=",\t")
    print("Z:", msa.get_z_axis(), end=",\t")
    print("Freefall?", msa.read_interrupt(BreakoutMSA301.FREEFALL), end=",\t")
    print("Orientation:", msa.get_orientation())
    time.sleep(0.1)
