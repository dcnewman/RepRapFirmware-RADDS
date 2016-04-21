Traditionally, the Duet RepRapFirmware is built using the Eclipse IDE.
If that is to your liking, then by all means go ahead and do so.

This fork of the RepRapFirmware may also be built using the
[scons](http://scons.org/) command line tool,

1. Should your system lack Python 2.x, download and install it.

2. Download and install [scons](http://scons.org/).  Version 2.3.5 or
   later is required.

3. Install the [Arduino](https://www.arduino.cc/) application.
   Versions 1.5.8beta is known to work.  You will merely be using the
   gcc-arm toolchain installed with/by the Arduino application.
   
4. Run the Arduino application and install the ARM programming tools.
   Look under "Tools > Board > Boards Manager...".  Select the "Arduino
   SAM Boards (32-bits ARM Cortex-M3) by Arduino" and install it.

5. From this github repository, edit the file

         scons_tools/sample_rrf_arduino_paths.py

   as appropriate to indicate the location of your installed gcc-arm
   tool chain and location of the CoreDuet sources (Step 6).  Then save
   this file to your home directory as the file

         ~/.rrf_arduino_paths.py
   
6. Obtain and build a copy of the github [CoreDuet repo](https://github.com/dcnewman/CoreDuet).

7. From the top-level RepRapFirmware repository directory, build the
   firmware with the command

         scons platform=<board-name>
   
   where `<board-name>` is the target board.  As of this writing, `duet`
   and `radds` are the accepted target boards.  To see the supported
   target boards, you can issue the command
   
   	     python scons_tools/platforms.py
   	     
8. Once scons finishes a build, the resulting files will be in the directory

        build/<board-name>

   The final build product is a `.bin` file.

9. For boards with a Native Programming Port and the
   [ATMEGA16U2 Assistant](http://playground.arduino.cc/Bootloader/DueBootloaderExplained),
   the scons command
   
        scons platform=<board-name> upload port=<usb-device>
        
   may be used to automatically upload the firmware.  You do not need to
   press any buttons on the board to effect the upload.
   
   For boards which lack a Native Programming Port or the ATMEGA16U2
   Assistant, you can use the above command but first you will need to
   press the board's ERASE button.  Possibly the RESET button as well.
   
   Boards with an attached Arduino Due should have a Native Programming
   Port as well as the ATMEGA16U2 Assistant (e.g., RADDS).
