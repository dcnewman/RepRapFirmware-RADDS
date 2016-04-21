; homeall file for use with dc42 Duet firmware on CoreXY printers
; This file assumes X, Y max/high-end homing and Z min homing (no Z probing)
; Reverse the X and Y movement for low-end/min switches.

G91                       ; relative mode
G1 S1 X240 Y240 F3000     ; coarse home X or Y
G1 S1 X240                ; coarse home X
G1 S1 Y240                ; coarse home Y
G1 X-4 Y-4 F600           ; move away from the endstops
G1 S1 X10                 ; fine home X
G1 S1 Y10                 ; fine home Y
G1 S1 Z-300 F600          ; coarse home Z
G1 Z5 F300                ; backoff
G1 S1 Z-10 F100           ; fine home Z
G90                       ; absolute mode
