# Keyboard Visualizer
Razer Chroma and Corsair RGB Keyboard Spectrograph Visualizer for Windows and Linux

Binary Windows builds can be downloaded from the Release submodule

You must have the Microsoft Visual C++ 2015 32-bit redistributable package installed to run this program (Windows).

To build this project in Linux, install QT Creator and libopenal-dev, libhidapi-dev and open/build the .pro file.

# Settings

There are many settings you can tweak to make Keyboard Visualizer work with your audio setup and to personalize it to your liking.

    * Amplitude - How reactive the visualizer is to sound.  If it is not responding, or is responding with a low level,
                  increase this value.
    * Background Brightness - How bright the background layer is (0-100%)
    * Average Size - How many bars of input to combine, increase for a less detailed output that may look better on
                     devices with low LED counts.
    * Decay - How much of the previous value is retained from step to step.  Higher values make the visualizer less
              flickery but seem slower to react, while low values are fast and flickery.  (0-100%)
    * Delay - How many milliseconds to wait between each device update.  Lower values give a higher "frame rate"
              but can cause flicker or other issues on certain setups.
    * Normalization Offset - Adjusts amplitude of bass frequencies, increase if your song lacks bass and decrease if
                             your song is bass heavy
    * Normalization Scale - Amount to scale up each consecutive frequency bar over normalization offset, to amplify
                            higher frequencies.  Increase if the right side of the visualizer is low.
    * Filter Constant - Amount of signal to blend per update, 0-1 floating point.  A value of 1 disables filtering
                        and updates the spectrum every cycle, a value between 0 and 1 blends the new value with the
                        previous value, slowing the response.  A value of 0 holds the existing value and ignores
                        new samples.
    * FFT Window Mode - Select between None, Hamming, Hanning, and Blackman windowing functions
                        (see https://en.wikipedia.org/wiki/Window_function for the math behind FFT windowing)
    * Background Mode - Select between various static and animated background colors and patterns
    * Foreground Mode - Select between various static foreground colors and patterns
    * Single Color Mode - Select between various color options for devices that use the single color effect
    * Average Mode - Select between discrete bars (binning) or a smoothed line (low pass) output
    * Animation Speed - Speed of background and foreground animated patterns, default 100, can be negative
                        to reverse the direction of the pattern, can be increased to speed up the pattern or
                        decreased to slow it down.

# Command Line Options and Settings File

Keyboard Visualizer allows you to save your custom settings in two different ways.  You can pass parameters in on the command line (which is useful for creating shortcuts) or you can save a settings file in the same directory that the .exe is stored called settings.txt.  The parameters are as follows:

    Available command line commands:
        help              - Display this message and quit
        startminimized    - Start in system tray
    
    Available command line arguments:
        Usage: argument1=value1 argument2=value2 ...

        amplitude         - Adjust the amplitude of the visualizer
        bkgd_bright       - Adjust the background brightness
        avg_size          - Number of points to average
        decay             - Percentage of value to decay every step
        delay             - Milliseconds between each device update
        nrml_ofst         - Normalization offset, floating point value
        nrml_scl          - Normalization scale, floating point value
        fltr_const        - Low pass filter constant, floating point value 0-1
        window_mode       - FFT windowing mode selector, values are:
                          - 0:  No windowing
                          - 1:  Hanning window
                          - 2:  Hamming window
                          - 3:  Blackman window
        bkgd_mode         - Background mode, values are:
                          - 0  Black
                          - 1  White
                          - 2  Red
                          - 3  Orange
                          - 4  Yellow
                          - 5  Green
                          - 6  Cyan
                          - 7  Blue
                          - 8  Purple
                          - 9  Green/Yellow/Red
                          - 10 Green/White/Red
                          - 11 Blue/Cyan/White
                          - 12 Red/White/Blue
                          - 13 Rainbow Bars
                          - 14 Rainbow Bars Inverse
                          - 15 Original
                          - 16 Rainbow
                          - 17 Color Wheel
                          - 18 Spectrum Cycle
        frgd_mode         - Foreground mode, values are:
                          - 0  Black
                          - 1  White
                          - 2  Red
                          - 3  Orange
                          - 4  Yellow
                          - 5  Green
                          - 6  Cyan
                          - 7  Blue
                          - 8  Purple
                          - 9  Green/Yellow/Red
                          - 10 Green/White/Red
                          - 11 Blue/Cyan/White
                          - 12 Red/White/Blue
                          - 13 Rainbow Bars
                          - 14 Rainbow Bars Inverse
                          - 15 Original
                          - 16 Rainbow
                          - 17 Color Wheel
                          - 18 Spectrum Cycle
        single_color_mode - Single color mode, values are:
                          - 0  Black
                          - 1  White
                          - 2  Red
                          - 3  Orange
                          - 4  Yellow
                          - 5  Green
                          - 6  Cyan
                          - 7  Blue
                          - 8  Purple
                          - 9  Background
                          - 10 Follow Background
                          - 11 Follow Foreground
        avg_mode          - Visualizer averaging mode
                          - 0:  Binning
                          - 1:  Low-pass filtering
        anim_speed        - Animation Speed (percent)
        server            - Configure this instance as a server for synchronization
                          -  Takes what port to serve on as argument, i.e. server=1234
        client            - Configure this instance as a client for synchronization
                          -  Takes the IP/hostname of the server and port as arguments,
                          -  i.e. client=192.168.1.100,1234
        ledstrip          - LED config strings :
                          - Serial : ledstrip=port,baud,num_leds
                          - (ex.ledstrip=COM1,115200,30)
                          - UDP : ledstrip=udp:client,port,num_leds
                          - (ex.ledstrip=udp:192.168.1.5,1234,30)
        xmas              - COM port, ex. xmas=COM2

# Available Visual Effects

    * Spectrograph - Shows vertical bars for each frequency in the audio
    * Bar - Shows a single bar representing bass frequencies
    * Single Color - Brightness and color represent strength of bass frequencies

# Supported Devices (Windows)
    
    * Razer Chroma SDK
    
        Keyboards
        - BlackWidow Chroma (spectrograph)
        - BlackWidow Chroma Tournament Edition (spectrograph)
        - DeathStalker Chroma (horizontal bar)
        - BlackWidow X Chroma (spectrograph)
        - BlackWidow X Chroma Tournament Edition (spectrograph)
        - Razer Blade Stealth (spectrograph)
        - Razer Blade (spectrograph)
        - Razer Ornata Chroma (spectrograph)
        
        Keypads
        - Razer Orbweaver Chroma (spectrograph)
        - Razer Tartarus Chroma (single color)

        Mice
        - Diamondback Chroma (bar and single color)
        - Mamba Tournament Edition (bar and single color)
        - Mamba Chroma Wireless (bar and single color, wired and wireless)
        - DeathAdder Chroma (single color)
        - Naga Epic Chroma (single color, wired only)
        - Naga Chroma (single color)
        - Orochi Chroma (single color, wired only)
        
        Headsets
        - Kraken 7.1 Chroma (single color)
        - Kraken 7.1 Chroma V2 (single color)
        - ManO'War (single color)
        
        Mouse Mats
        - Firefly (bar)
    
        External Graphics Dock
        - Razer Core (bar)

        Other
        - Razer Chroma Mug (bar)

    * Corsair CUE SDK
    
        Keyboards
        - K70 RGB (spectrograph)
        - Likely other CUE SDK supported keyboards (spectrograph), untested

    * Cooler Master RGB

    	Keyboards
    	- Masterkeys Pro L
    	- Masterkeys Pro S

    * SteelSeries

        Keyboards
        - Apex M800 (spectrograph)

    * MSI SteelSeries
        
        Keyboards
        - MSI 3-zone laptop keyboards and additional LED zones (MSI GS63VR, etc) (bar, single color)

    * WS28XX Pixel LED Strips
    
        - Arduino (Serial/USB-Serial) or ESP8266 (UDP over WiFi) may be used for controller
        - WS2812B, WS2811, and compatible LED strips/strings supported
        - Adjust the LEDs value in the Arduino sketch to match the number of LEDs on your LED strip/string
        - Recommended maximum of 90 or so LEDs on Arduino using 115200 baud rate
        - Configure LED strip using the ledstrip command line/settings file parameter
        - Number of LEDs in Arduino sketch must match num_leds parameter in LED command string
        - For serial connected LEDs, -ledstrip=port,baud,num_leds (ex. -ledstrip=COM1,115200,30)
        - For WiFi connected LEDs, -ledstrip=client,port,num_leds (ex. -ledstrip=192.168.1.5,1234,30)

        - For up to 200 LEDs on Arduino using 1000000 baud rate, see my pixel_controller repository
        - This requires AVR Studio 4 + WinAVR to build, but is more efficient by skipping Arduino IDE

# Supported Devices (Linux)

    * Razer Linux Drivers (https://github.com/terrycain/razer-drivers)

        Keyboards
        - BlackWidow Chroma (spectrograph)
        - BlackWidow Chroma Tournament Edition (spectrograph)
        - DeathStalker Chroma (horizontal bar)
        - Razer Ornata Chroma (spectrograph)
        - Razer Blade Stealth (spectrograph)
        - Razer Blade Pro (spectrograph)

        Keypads
        - Razer Tartarus Chroma (single color)

        Mice
        - Diamondback Chroma (bar and single color)
        - Mamba Tournament Edition (bar and single color)
        - DeathAdder Chroma (single color)
        
        Mouse Mats
        - Firefly (bar)

        Other
        - Razer Chroma Mug (bar)

    * SteelSeries

        Keyboards
        - Apex M800 (spectrograph)

    * MSI SteelSeries
        
        Keyboards
        - MSI 3-zone laptop keyboards and additional LED zones (MSI GS63VR, etc) (bar, single color)

    * WS28XX Pixel LED Strips
    
        - See the information under the Windows supported devices list.
