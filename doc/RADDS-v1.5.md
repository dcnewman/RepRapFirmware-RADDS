Firmware
--------

To install the RepRapFirmware, you will need to download a copy of
the `bossac` program.  The easiest way to do that is to install version
1.5 or later of the Arduino application and then locate the bossac
executable within it.

1. Download the Arduino application from [Arduino.cc](http://www.arduino.cc).
2. Run the Arduino application and tell it to install the ARM programming
   tools. Look under "Tools > Board > Boards Manager...".  Select the
   "Arduino SAM Boards (32-bits ARM Cortex-M3) by Arduino".
3. Search your computer for `bossac` (`bossac.exe` on Windows).  For example,
   with Arduino 1.5.8, `bossac` is found in `Java/hardware/tools/` under the
   Arduino application directory.  For Arduino 1.6 and later, it's stored
   under `packages/arduino/tools/` in a directory tree disjoint from the
   Arduino application's tree.
4. With a USB cable connected to the Arduino Due's programming port and your
   computer, (a) press the Due's RESET button, and then (b) issue the command

        bossac --port=PORT -e -w -v -U false -b RepRapFirmware-1.09r-dc42-radds-b.bin
        
   where `PORT` is the terminal device, not including `/dev`.  In place of
   `RepRapFirmware-1.09k-dc42-radds.bin` use the name and directory path for
   the specific firmware file you wish to load.
   
   Note that the programming port is the USB port on the Arduino Due which
   is closest to the Due's power jack.  If you prefer to use the native USB
   port then you will need to press the Due's ERASE button, then the RESET
   button, and then issue the above `bossac` command but with `-U true`
   instead of `-U false`.
   

SD Card Files
-------------

The RepRapFirmware expects an SD card to be present when it begins running.
While it is not critical that the SD card be present, when it is, the
firmware will execute the `sys/config.g` file from the card and establish
many of the printers settings (discussed in the guides referenced in the
next section, Firmware Configuration).  Also, "macro" files for homing and
other operations can be stored in the `sys/` folder of the SD card.  The
contents and nature of the files may depend upon the mechanics of the printer --
depend upon whether it is a Delta or non-Delta printer.  As a starting point
for files in `sys/` see the subdirectories of the `SD-Image/` image directory
of this source repository.  For example, the files in
`SD-Image/sys/sys-MiniKossel` are a suitable starting point for a Delta
printer's `sys/` directory.  You will want to remove or comment out the
M540, M552, M553, and M554 (network) commands as well as any M906 (motor
current) commands as those commands are not supported by RADDS hardware.



Firmware Configuration
----------------------

To commission a printer with the dc42 branch of the RepRapFirmware, refer
to these guides:

* **Delta:** [Configuring and calibrating RepRapFirmware for a delta printer](http://reprap.org/wiki/Configuring_and_calibrating_a_delta_printer_using_the_dc42_fork_of_RepRapFirmware)
* **Core XY:** [Configuring RepRapFirwmare for a Core XY printer](http://reprap.org/wiki/Configuring_RepRapFirmware_for_a_CoreXY_printer)
* **Normal Cartesian:** [Configuring RepRapFirmware for a Cartesian printer](http://reprap.org/wiki/Configuring_RepRapFirmware_for_a_Cartesian_printer)


   
Stepper Drivers
---------------

The RAPS-128 stepper drivers are excellent stepper drivers for 3D
printing: like the DRV8825, they can handle 2.2 amps of current but
with better overall performance than the DRV8825, particularly at
low speeds.  They do not have the high minimum rise time of the DRV8825
and they have better decay characteristics as well.  Strongly consider
using the RAPS-128 stepper drivers.

If you use the RAPS-128 stepper drivers, then know that their electical enable
signal is inverted from that used with most other 3D printing stepper drivers.
Use the [`M569` gcode](http://reprap.org/wiki/G-code#M569:_Set_axis_direction_and_enable_values)
to tell the firmware that the stepper drivers use an inverted enable signal,

    M569 P0 R1 ; X axis has inverted enable signal
    M569 P1 R1 ; Y axis has inverted enable signal
    M569 P2 R1 ; Z axis has inverted enable signal
    M569 P3 R1 ; E axis has inverted enable signal


Endstops
--------

The RepRapFirmware supports a single endstop per axis.  Through gcode, each
endstop is declared to be a minimum or maximum endstop and to send either
an electrical HIGH or LOW signal when activated.  This configuration is done
with the
[`M574` gcode command](http://reprap.org/wiki/G-code#M574:_Set_endstop_configuration),
typically in the `sys/config.g` file processed when the printer powers up.

On the RADDS board, the minimum endstop headers are used for the endstops:
X min, Y min, and Z min.  Wire your endstops to those headers and then, with
`M574` declare whether they are minimum (1) or maximum (2) endstops.  For
example, to use minimum endstops for X, Y, and Z use the command

    M574 X1 Y1 Z1 S0

The `S0` indicates that the endstops send a logical LOW when triggered.


Thermistors
-----------

On the RADDS board, thermistor position T4 is used for the heated bed.  This
is the thermistor header farthest from the board's corner.  Thermistor position
T0 is used for the first extruder, T1 for the second, etc.

The [`M305` gcode command](http://reprap.org/wiki/G-code#M305:_Set_thermistor_and_ADC_parameters)
is used to set characteristics of each thermistor (e.g., Beta values).  With
the `M305` command, `P0` specifies the bed thermistor, `P1` the first extruder,
`P2` the second extruder, etc.


Thermocouples
-------------

The RepRapFirmware on RADDS supports up to two external MAX31855 thermocouple
chips.  Physically wire the MAX31855s with their pins as follows

MAX31855 | RADDS
---------|-------
Vin | 3.3V [external SD card connector]
GND | GND  [external SD card connector]
Data out | MISO [external SD card connector]
CLK | CLK [external SD card connector]
CS | Y max or Z max (Due pins 36 and 38)

Then, to associate a MAX31855 with a temp sensor, use the
[`M305` gcode command](http://reprap.org/wiki/G-code#M305:_Set_thermistor_and_ADC_parameters),

    M305 Pn X10m

    n = 0 for HBP 
    n = 1 for 1st Extruder 
    n = 2 for 2nd Extruder 
    ... 

    m = 0 for MAX31855 with CS on Z max 
    m = 1 for MAX31855 with CS on Y max  

Note that the GND and 3.3V signals can be drawn from other locations
on the RADDS board.  They do not have to be taken from the "external SD
card" connector as suggested above.


Fans
----

Both Fan0 and Fan1 hookups on the RADDS board are supported.  These are the
two FET controlled screw down terminals next to the heated bed output and are
labelled on the underside of the RADDS board.

Control Fan0 with the command

    M106 Snnn

where `nnn` is a PWM duty cycle of 0 - 255 inclusive.  For off, use `S0`.  For
on full use `S255`.  Unlike some firmwares, the `Snnn` parameter is required.
To control Fan1, use the command

    M106 Snnn P1

Note that the command

    M106 Snnn P0

may also be used to control Fan0.  The `P` parameter defaults to `P0` when
omitted.


Inductive Z-Probe
-----------------
An inductive Z-probe may be connected to the RADDS PWM3 pin (Arduino Due
digital pin 39).  It then presents to the RepRapFirmware as the E0_AXIS
endstop.


USB
---

Use the Due's USB programming port for uploading firmware.  This is the USB port
closest to the Due board's power jack.  Use the Due's native USB port for
communication with the RepRapFirmware when it is running.  The native USB port
is configured to communicate at 115200 Baud.  


PanelDue
--------
A [PanelDue](https://www.think3dprint3d.com/PanelDue) may be used with the
RADDS board.  Wire it to the 5V, TX1, RX1, and GND pins on the RADDS' AUX1
header.
