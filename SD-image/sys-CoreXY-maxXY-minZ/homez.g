; Home Z min using endstop switch
G91                       ; relative mode
G1 S1 Z-300 F600          ; coarse home Z
G1 Z5 F300                ; backoff
G1 S1 Z-10 F100           ; fine home Z
G90                       ; absolute mode
