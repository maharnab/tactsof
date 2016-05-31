# tactsof

Thirty-meter-telescope Actuator Control Testing SOFtware (tactsof) is a client-side GUI developed in Qt-C++ to test TMT actuator control and parameters for Linux/Windows/Embedded systems. The Server-side C application runs on an embedded board and interacts with one or multiple GUIs via the TCP/IP protocol. 

## Getting started

For compilation and executing the debug/release version of the GUI application source, Qt 5.0.1 or above is required.

## Working

The working of the GUI is fairly basic and comprises of the following components:
 * protocol and connection establishment with the server-side embedded board
 * obtain status of various sensors, DAQ's, etc connected to the embedded board
 * depending on the status send requests to the embedded board for execution of VCM, Off-Loader motor, etc related functions
 * upon successful execution receive the data and plot real-time for the user for further actions

## License

None. Feel free to use as is or modified at your own risk.

####### PS: If it is of any help & wish to use it for any purpose or contribute to the project, the code can be optimized or supplied with more info as required. 
