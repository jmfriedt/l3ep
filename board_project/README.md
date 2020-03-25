* Input:   1-PPS from GPS receiver connected to Timer 3 input capture
         also connected to GPIO INT2 pin
* Output:  PWM followed by a low pass filter to act as a D/A converter
* Control: microprocessor frequency

Requires ``simavr``, available at https://github.com/buserror/simavr
1. clone simavr
2. copy this directory in the examples subdirectory of ``simavr``
3. install ``libelf-dev`` if not available (required dependency)
4. in simavr: ``make``
will recursively compile all board examples including the content of this directory.
5. execute ``./examples/board_project/obj-x86_64-linux-gnu/PPScontrol.elf``

The graphical user interface requires libdevil [1], installed on Debian GNU/Linux with
``apt-get install libdevil-dev``

If not available, remove support for the GUI (not mandatory) by replacing
``#define with_GUI``
with
``#undef with_GUI``

[1] libdevil-dev - Cross-platform image loading and manipulation toolkit

When using the graphical user interface option, the red square indicates 1-PPS has been
triggered, and yellow square indicates input capture has occured:

<img src="illustration.jpg">