# keystation-duino

An ardino sketch to replace the micro-controller in the M-Audio Keystation 49 with an arudino (cause my one died).

#### Working
* Currently have the basis of a velocity-sensing key matrix implemented. 
* Turn key timings into velocity in the range 0..127
* Sends midi note-on/note-off events

#### ToDo
* circuit board adaptor to get all keys working
* pitch wheel
* mod wheel
* volume slider
* Octave buttons

#### Maybe ToDo
* Send USB Midi _and_ standard midi over one of the other UARTs so that (for example) it could connect to a raspberry pi directly instead of over usb
* Maaaaybeee re-write in rust if the Due is fast enough




