# QIcomPCR V0.1.0

## Description :

  This become to be a Qt interface for ICOM PCR 2500 devices. I can only test on PCR2500 without any accessories.
  There is a Qt GUI and also a Web Interface through nodejs to control remotely the ICOM in realtime.
  
  [Website here] (http://fcrohas.github.io/QIcomPCR/)
  
## Dependencies :

	* For sound system Pulseaudio or PortAudio
	* Optional If you want Acars, you can install libacarsd for a robust decoding
	* Optional If FFTW3 is provided it will be used else with performance issue default DFT is used
	* Optional LibSndFile to load wav file and record it
	* Optional libSamplerate to allow over sampling in a correct way
  
## How to build :

	* On linux
		Using CMake is just enougth and all optional library above if you want. Actually only tested on Opensuse
		
	* On Windows
		Using CMake , Visual Studio and QT for windows.
		Optional library should be copied under 3rdparty/<library name>/include and /lib

	Note : Portaudio and ICOM controls are not tested under windows.
	
## Todo :
	* Add ICOM memory manager
	* Add more and more decoder
	* Add RTLSDR support
	

Contact me if you want to help fcrohas@gmail.com
