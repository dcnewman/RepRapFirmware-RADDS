__This repository is based upon https://github.com/dc42/RepRapFirmware and is a port to RADDS of the RepRapFirmware.  All work by dcnewman on this repository was suspended on April 21, 2016.__

## 1.0 Introduction

_This `README.md` file is a slightly updated version of the original
`README` file also found in this repository._

This firmware is intended to be a fully object-oriented highly modular
control program for RepRap self-replicating 3D printers.

It owes a lot to Marlin and to the original RepRap FiveD_GCode.

This is the version for the RepRap Duet,
[blog.think3dprint3d.com/2013/12/Duet-Arduino-Due-compatible-3DPrinter-controller.html](http://blog.think3dprint3d.com/2013/12/Duet-Arduino-Due-compatible-3DPrinter-controller.html  )

A complete uploadable executable version is in the directory
`Release/RepRapFirmware-<version>.bin` in this repository.  For details
of how to flash it to a Duet see,
[Installation_8211_Flashing_the_Firmware](https://reprappro.com/documentation/commissioning-introduction/maintenance-duet/#Installation_8211_Flashing_the_Firmware)

For details of how to compile the source code see the `INSTALL.md`
document contained in this repository or [Section #4.0](#sec40) below.


## 2.0 General design principles

* Control by RepRap G Codes.  These are taken to be machine independent,
  though some may be unsupported.
* Full use of C++ OO techniques,
* Make classes hide their data,
* Make everything except the Platform class (see below) as stateless as
  possible,
* No use of conditional compilation except for #include guards - if you
  need that, you should be forking the repository to make a new branch
  - let the repository take the strain,
* 90% concentration of all machine-dependent definitions and code in
  `Platform.h` and `Platform.cpp`,
* No specials for (X,Y) or (Z) - all movement is 3-dimensional,
* Except in `Platform.h`, use real units (mm, seconds, etc.) throughout
  the rest of the code wherever possible,
* Try to be efficient in memory use, but this is not critical,
* Labour hard to be efficient in time use, and this is critical,
* Don't abhor floats - they work fast enough if you're clever,
* Don't avoid arrays and structs/classes,
* Don't avoid pointers,
* Use operator and function overloading where appropriate.

### 2.1 Naming conventions

* \#defines are all CAPITALS_WITH_OPTIONAL_UNDERSCORES_BETWEEN_WORDS
* No underscores in other names - MakeReadableWithCapitalisation
* Class names and functions start with a CapitalLetter
* Variables start with a lowerCaseLetter (i.e., camel case)
* Use veryLongDescriptiveNames

### 2.2 Structure

There are six main classes:

* RepRap
* GCodes
* Heat
* Move
* Platform, and
* Webserver

#### 2.2.1 RepRap

This is just a container class for the single instances of all the others,
and otherwise does very little.

#### 2.2.2 GCodes

This class is fed GCodes, either from the web interface, or from GCode
files, or from a serial interface, interprets them, and requests actions
from the RepRap machine via the other classes.

#### 2.2.3 Heat

This class implements all heating and temperature control in the RepRap
machine.

#### 2.2.4 Move

This class controls all movement of the RepRap machine, both along its 
axes, and in its extruder drives.

#### 2.2.5 Platform

This is the only class that knows anything about the physical setup of the
RepRap machine and its controlling electronics.  It implements the interface
between all the other classes and the RepRap machine.  All the other classes
are completely machine-independent (though they may declare arrays dimensioned
to values `#defined` in `Platform.h`).

#### 2.2.6 Webserver

This class talks to the network (via Platform) and implements a simple 
webserver to give an interactive interface to the RepRap machine.  It uses
the Knockout and Jquery Javascript libraries to achieve this.

When the software is running there is one single instance of each main class,
and all the memory allocation is done on initialization.  `new`/`malloc` should
not be used in the general running code, and `delete` is never used.  Each class
has an `Init()` function that resets it to its boot-up state; the constructors
merely handle that memory allocation on startup.  Calling `RepRap.Init()` calls
all the other `Init()` functions in the right sequence.

There are other ancillary classes that are declared in the `.h` files for the
master classes that use them.  For example, Move has a DDA class that implements
a Bresenham/digital differential analyser.

### 2.3 Timing

There is a single interrupt chain entered via `Platform.Interrupt()`.  This
controls movement step timing, and this chain of code should be the only
place that volatile declarations and structure/variable-locking are required.
All the rest of the code is called sequentially and repeatedly as follows:

As of version `057r-dc42` the tick interrupt (which is set up by the Arduino
core) is also used to set up ADC conversions, read the result of the last
conversion, and shut down heaters when temperature errors are detected.

All the main classes have a `Spin()` function.  These are called in a loop
by the `RepRap.Spin()` function and implement simple time sharing.  No class
does, or ever should, wait inside one of its functions for anything to happen
or call any sort of `delay()` function.  The general rule is:

	Can I do a thing?
    	Yes - do it
    	 No - set a flag/timer to remind me to do it next time I'm called
    		  at a future time and return.

The restriction this strategy places on almost all the code in the firmware
(that it must execute quickly and never cause waits or delays) is balanced
by the fact that none of that code needs to worry about synchronization,
locking, or other areas of code accessing items upon which it is working.
As mentioned, only the interrupt chain needs to concern itself with such
problems.  Unlike movement, heating (including PID controllers) does not
need the fast precision of timing that interrupts alone can offer.  Indeed,
most heating code only needs to execute a couple of times a second.

Most data is transferred byte-wise, with classes' `Spin()` functions
typically containing code like this:

	Is a byte available for me?
    	Yes
      		read it and add it to my buffer
      		Is my buffer complete?
         		Yes
           			Act on the contents of my buffer
         		No
           			Return
  	No	
    	Return

Note that it is simple to raise the "priority" of any class's activities 
relative to the others by calling its `Spin()` function more than once from
`RepRap.Spin()`.


## 3.0 Compiling from Source (scons)

Follow the directions in `INSTALL.md`.


## [4.0 Compiling from Source (Eclipse)](id:sec40)


RepRap Firmware was developed using the Eclipse IDE, which is much more
powerful for big software projects than the Arduino IDE.  [But, they are both
IDEs with all the failings of IDEs.  If you dislike IDEs, then use scons
as per `INSTALL.md`.]

We use the [Eclipse C/C++ IDE](http://www.eclipse.org/downloads/).

You will also need the [Eclipse Arduino](http://www.baeyens.it/eclipse/)
support. ***BIG FAT WARNING:*** In July 2015, this Arduino plugin for
Eclipse had a bug by wich doing a "clean" would `rm -rf /`.  You read
that correctly, it would delete your entire file system!  ***Use with
caution!***

And the [Arduino IDE](http://arduino.cc/en/Main/Software) itself (make
sure you get the one for the Due/Duet - version 1.6.5 at the time of
writing).

As of this firmware version 0.57r-dc42, the Arduino IDE must be patched to
enable watchdog support.   Follow Step #6 of `INSTALL.md`.

Start by getting the Arduino IDE programming your Duet with a simple
Hello World program that prints to the USB,

    SerialUSB.print("Hello World");
 
on the Due/Duet, not

    Serial.print("Hello World");... )

Then install Eclipse and the Arduino plugin.

Make temporary copies of `RepRapFirmware.cpp` and `RepRapFirmware.h` from
your download in another folder (you will only need to do this once).

Finally use Eclipse to open the Arduino project called RepRapFirmware in the
folder where you have downloaded the RepRap Firmware code.  Tell Eclipse to
use the Arduino-libraries files you downloaded as the local libraries.
Eclipse will complain that the project already exists (which it does - it
is your download).  Ignore this and it will open the project anyway.  

Annoyingly the first time it may also overwrite `RepRapFirmware.cpp` and
`RepRapFirmware.h`.  So close the project, overwrite its overwrites with
the two files you saved, open the project again and refresh it.  Everything 
should now be ship-shape.  Add the libraries 

* Wire
* EMAC
* Lwip
* MCP4461
* SamNonDuePin
* SD_HSMCI

to your project.  Under no circumstances be tempted to add standard Arduino
libraries for devices like Ethernet - these are for the Due, and will not
work on the Duet.

You should now be able to compile the code and upload the result to the Duet.
***OR NOT!***  dcnewman found the above directions to be insufficient.  A
number of Eclipse project settings needed to be tweaked (e.g., include paths,
include file orderings, etc.).  Also be warned that the Arduino Eclipse plugin,
aside from possibly doing a `rm -rf /`, will faithfully follow the compile
settings from the Arduino `platform.txt` file and will thus turn off all
compiler warnings (`-w`) and effect other unwanted compiler flags.


## 5.0  Note on dc42 fork of this firmware

As well as containing various bug fixes and performance improvements, I have
added various functionality compared to the original RepRapPro version.
The major changes are:

* Z probe reading is included in the web server poll response
* Homed status (i.e. whether homes since reset) is maintained for all 3
  axes and included in the web server poll response
* Once the X and Y axes have been homed, movement is limited to the range 
  of the axis, except when the G0 or G1 command includes the check endstops
  (S1) parameter. Use the M208 command to set axis travel if the default
  values in the firmware are incorrect for your machine.
* Implemented M301 (hot end PID parameters) and M304 (bed PID parameters)
  commands. Extended these commands to allow setting of thermistor resistance
  at 25C (R parameter) and thermistor beta (B parameter). A negative P
  parameter means don't use PID, use bang-bang control.
* Implemented M999. This resets the Duet.
* Added temperature (T) and temperature coefficient of height (C) parameters
  to the G31 command
* Added support for modulated IR sensor (M558 P2) and ultrasonic sensor
  (M558 P3)
* Various parameters are now saved to flash memory (Z-probe parameters,
  network parameters, PID parameters)

--------

Version 0.mn beta

Started: 2012-11-18
This README dated: 2013-12-30

Adrian Bowyer
RepRap Professional Ltd
http://reprappro.com

Licence: GPL
