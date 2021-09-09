This monitors, logs, and displays data from my Prius's CAN bus.

A Raspberry Pi 3 does the monitoring, with a CAN bus HAT providing the bus interface.
An attached 800 x 480 LCD displays data.
A microcontroller running a USB HID program will provide customized input.


### UI

User interfaces are hard.
By the far the hardest part of this project has been figuring out a UI that I don't hate.


#### Threading

This is multithreaded.

A single thread fetches frames from the CAN bus (perhaps a thousand or so a second) and processes them into individual vehicle status parameters.
Another thread handles UI and display.
I deal with synchronization between writer and reader threads by not trying; if the data displayed is slightly out of date, it'll surely get updated correctly on the next frame.
