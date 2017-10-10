# README #

The is the README file for the BROS software.

### Software components ###

The software for the BROS setup consists out of 3 main components:

* MATLAB R2016B (9.1) & Simulink 8.8 (models), win64
* TwinCAT 3 (RT software)
* openFrameWorks (GUI & experiment backbone)

In case any MATLAB functions are called outside of the model, you can use MATLAB R2015B (win32) for compiling a shared DLL, which can be called from C/C++.

### How do I get set up? ###

1. Install MATLAB R2016B (including Simulink 8.8), both 64 bit versions.
2. Install Visual studio (2013 professional is what I use), TwinCAT XAE and the Simulink plugin, see the Symbitron [website wiki](https://www.symbitron.eu/wiki/index.php?title=Installing_TwinCAT) and [Bitbucket repo](https://bitbucket.org/ctw-bw/we1_experiments/wiki/Twincat%20Tips%20And%20Tricks) for more details.
3. Install Visual Studio 2015 (community) and openFrameworks using their [setup guide](http://openframeworks.cc/setup/vs/).

The openFrameworks GUI and experiment backbone talk to the TwinCAT RT model through the ADS. Since the ADS dll is only available in a 32bit version, compile only in 32bit (win32 configuration, default). If you'd like to call any MATLAB functionality (like an optimization routine), compile the DLL in MATLAB R2015B (win32). See the [wiki] for more info!

**Edit** Unfortunately the 2015b win32 version of MATLAB does not support the Parallel Computing Toolbox, so we decided to move to a MATLAB script which is running on an instance of MATLAB 2016b (win64). Communication between the experiment app and MATLAB is through XML files.

### Who do I talk to? ###

Niek Beckers: niekbeckers using Google's mail

[wiki]: https://bitbucket.org/ctw-bw/bros_experiments/wiki/Home