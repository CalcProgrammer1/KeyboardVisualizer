# Keyboard Visualizer ![Icon](https://raw.githubusercontent.com/CalcProgrammer1/KeyboardVisualizer/master/KeyboardVisualizerQT/Icon.png)

## OpenRGB Audio Visualizer for Windows, Linux, and MacOS

Keyboard Visualizer turns your OpenRGB-supported RGB setup into a spectacular light show for your music.  Keyboard Visualizer is able to listen to any input or loopback audio device on your computer so it can visualize audio from any music player, website, game, or other application.

To use Keyboard Visualizer with your setup, you must first install OpenRGB and set it up to detect all of your RGB devices.  OpenRGB can be found here:

https://gitlab.com/CalcProgrammer1/OpenRGB

## What about the old Keyboard Visualizer with Razer Chroma SDK, Corsair SDK, etc?

Development on this version has stalled as I'm not interested in supporting a multitude of proprietary SDKs that change constantly and only work on Windows.  This version remains as Keyboard Visualizer 3.x and downloads are still available in the Release submodule.  I will accept pull requests for this version if others wish to keep it alive, but I plan to focus primarily on the OpenRGB version (4.x) going forwards.

# Download/Build

## Windows
  *  Pre-built binaries are available under the Releases section on GitLab.

  *  If you wish to build the application yourself:

      1. Download the latest Visual Studio Community Edition and Qt Creator.
      2. git clone https://gitlab.com/CalcProgrammer1/KeyboardVisualizer
      3. git submodule update --init --recursive
      4. Open the KeyboardVisualizer.pro project in Qt Creator.
      5. Use the MSVC compiler kit, either 32- or 64-bit, to build the application.
      6. Run the project from Qt Creator.  If you want to use your custom build standalone, download the latest matching Release package and replace the OpenRGB.exe in it with your new build.

## Linux
  *  Pre-built binaries are not currently available for Linux

  *  You can build the project using Qt Creator or on the command line.  The commands listed here work for Debian-based distros.

      1.  sudo apt install build-essential qtcreator qt5-default libopenal-dev
      2.  git clone https://gitlab.com/CalcProgrammer1/KeyboardVisualizer
      3.  cd KeyboardVisualizer
      4.  git submodule update --init --recursive
      5.  qmake KeyboardVisualizer.pro
      6.  make -j8
    
  *  Run the application with ./KeyboardVisualizer

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
    * Filter Constant - A low pass filter constant between 0 and 1.  A value less than 1 slows the response of the
                        visualization, which makes for a less jumpy, smoother effect the lower the value is.
    * Background Timeout - How long to wait (in multiples of 10 milliseconds) before fading the background in.

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

# Available Visual Effects

    * Spectrograph - Shows vertical bars for each frequency in the audio
    * Bar - Shows a single bar representing bass frequencies
    * Single Color - Brightness and color represent strength of bass frequencies

# Donations

   * I created this project for fun mostly, and Razer's support of sending free products for me to integrate is awesome, but several people have asked if I'd accept donations.  I've gotten back into cryptocurrency and mining recently so if you want to send some Bitcoin, Dogecoin, or Ethereum my way you can do so with the addresses below.  I don't want to bother with PayPal and its ugly fees and I detest online advertising and block all ads on my PCs so I would never put ads on my videos or programs.
   
   * Bitcoin:  1N83YPu7btXYadPS1neB9zX7X1QTdpyZQ
   
   * Dogecoin:  D7nqJZ3dyfCMRvS6J5jpoxerpteUG5z7Pm
   
   * Ethereum:  0xb5023019E6789af4887aED2056E471DF0f4c8e93
   
   * Monero:  44p8QfKTHxA65PeY1HabFsaZnayVu36h2eNHsTVcHzg5EeUDszmK91aTaAc4otCRdL4644gMzatb85bWyZB7vfK78siS962
