; Y max homing file for dc42 Duet firmware
G91                       ; relative mode
G1 S1 Y240 F3000          ; coarse home Y
G1 Y-4 F600               ; move away from the endstops
G1 S1 Y10                 ; fine home Y
G90                       ; absolute mode
