SIP Client
===========================

This repo was forked from the sip_call repo by chrta that was created for initiating a call from a doorbell using the ESP microcontroller. I have forked it and will make changes to it to turn it into a regular SIP Client that will support 2 way communication
This is an ongoing project

Building
++++++++

The project now uses cmake. To build this project for the pc (linux, e.g. ubuntu or fedora)
  mkdir <build dir>
  cd <build dir>
  cmake <this project's root dir>
  make

The sip server configuration must be done in the defines of the file <this project's root dir>/native/main.cpp. (Will be changed to be taken through cmd line)

The following libraries are required for this (e.g. on fedora)::

  sudo apt-get install libasio-dev libmbedtls-dev libsfml-dev


(NOTE: The below text is from the original chrta repo and is left alone for now)
Code formatting
+++++++++++++++

Clang-format is used to format the code. The settings are stored in .clangformat. The format of external files, e.g.
components/sip_client/include/boost/sml.hpp should not be changed. To run clang-format (e.g. version 12) over all files::

  find . -regex '.*\.\(cpp\|cc\|cxx\|h\)' -exec clang-format -style=file -i {} \;

Hardware
--------

An ESP32 board can be used. Only one external GPIO (input is sufficient) must be available, to detect the call trigger.
To test this, two PC817 opto coupler are used to detect the AC signal (about 12V from the bell transformer). The input diodes of the opto couplers are connected in parallel and opposing directions.
In series, a 2k Resistor is used. This may have to be tweaked according to the input voltage.
The output transistors of the opto couplers are connected in parallel in the same polarity to pull the signal to ground, if a current flows through one of the input diodes. A pull up resistor (either internal in the ESP32 or external) must be used to pull the signal to 3V3 if no input current is detected and the output transistors are switched off.

Instead of two PC817 opto couplers, one PC814 can be used to detect the AC signal. Because of the different CTR, the resistor values must be tweaked.
If it is sufficient to only detect one half-wave of the AC signal (this is normally the case) one PC817 opto coupler and a simple diode (e.g. 1N4148) is sufficient. The diode ensures that the voltage of the input diode of the opto coupler is not above the threshold. The 1N4148 must be connected anti-parallel to the input diode of the PC817.

.. image:: hw/door_bell_input_schematic.svg
	   :width: 600pt


If the bell transformer delivers enough power, the ESP32 can be powered from it. A bridge rectifier, a big capacitor and a cheap switching regulator board can be used for that.


License
-------

If not otherwise specified, code in this repository is Copyright (C) 2017-2021 Christian Taedcke <hacking@taedcke.com>, licensed under the Apache License 2.0 as described in the file LICENSE.

Misc Information
----------------

On the AVM Fritzbox the number \*\*9 can be used to let all connected phones ring.


.. _`Espressif IoT Development Framework`: https://esp-idf.readthedocs.io/
.. _`Selecting soc build target`: https://docs.espressif.com/projects/esp-idf/en/v4.3.1/esp32c3/api-guides/build-system.html#selecting-the-target
