# Breakfast-Keyboard-Intercept (WIP)
These programs are built off of the wonderful work of [Francisco Lopes.](http://www.oblita.com/interception.html)
These programs are designed to allow the use of a second keyboard to act like a soundboard. It intercepts keystrokes from the specified device, meaning that the system gets none of the keystrokes from the keyboard, and instead that key can be used to play a certain sound on the soundboard.

# Programs
The deviceID program is opened, listens for the next keypress on any connected device, and returns that over 127.0.0.1 to the Soundboard, which is listening for it. The Soundboard uses this to create an `interception.properties` file that the main interception program can then use.

The main interception program listens to the device with the specified deviceID, set by the other program, and intercepts and blocks the keystrokes for that device. It sends the string representation of the key over localhost to the soundboard, where the rest of the processing can be done.
# Usage
This is nowhere near being implemented into our Soundboard, and implementation is not guaranteed.
If you feel like improving these, just submit a pull request and we will review the code and accept the request.
