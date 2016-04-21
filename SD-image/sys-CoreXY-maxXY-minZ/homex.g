; X max homing file for dc42 Duet firmware
G91                       ; relative mode
G1 S1 X240 F3000          ; coarse home X
G1 X-4 F600               ; move away from the endstops
G1 S1 X10                 ; fine home X
G90                       ; absolute mode
