# Da Dome
### An interaction encourager for Music Festivals, created by Just 5 Vibes
<img width="1000" height="630" alt="DaDome V1" src="https://github.com/user-attachments/assets/14b1c42f-0779-4a98-ba1f-9484ab0026b7" />
Our installation encourages user interaction by allowing participants to hold buttons on an acrylic dome. For each button held down in the same row, it will play more complex music tracks within that instrument. Thus, the only way you can create music is to either find more friends or ask familiar strangers to help play music together!

This enhancement was intended for the "dead" space when roaming inbetween large scale music festivals.

## Link to our Promotional Material
See our [Brochure](https://github.com/bobblejotcake/the-5-vibes/blob/476c3276f1447310003505a55dcb5afd2f129e90/Exhibit%20Promotional%20Material/Da%20Dome%20Brochure.pdf) and our [poster](https://github.com/bobblejotcake/the-5-vibes/blob/476c3276f1447310003505a55dcb5afd2f129e90/Exhibit%20Promotional%20Material/Da%20Dome%20Poster%20Final.pdf) for the Social & Mobile exhibit!

## Links to GitHub Wiki Design Process Overview.
Click [here](https://github.com/bobblejotcake/the-5-vibes/wiki/Design-Process-Overview) to go to the Overview Design Process.

Click [here](https://github.com/bobblejotcake/the-5-vibes/wiki/Standup--Meeting-Notes) for our Stand Up Meeting Notes.

Click [here](https://github.com/bobblejotcake/the-5-vibes/wiki/Ethical-Considerations) for the Ethical Considerations page.

Click [here](https://github.com/bobblejotcake/the-5-vibes/wiki/AI-Disclosure) for the AI Disclosure page.
## Prerequisites for Usage
 - Arduino R3 Uno Mega.
 - Soldered 8 by 4 pin diode matrix
 - 


### Softwares to Install
Since Arduino Uno does not send MIDI signals natively, the following softwares to be installed to transfer the signals to MIDI

LoopMidi at [https://www.tobias-erichsen.de/software/loopmidi.html](https://www.tobias-erichsen.de/software/loopmidi.html)

Serial EA Bridge at v0.1.1 at [https://github.com/ezequielabregu/EA-serialmidi-bridge/releases](https://github.com/ezequielabregu/EA-serialmidi-bridge/releases)

FL Studio (This software is paid, however this could be used with free DAW /Digital Audio Workstation alternatives). For the purposes of this tutorial, FL studio will be used.

### Setting up the hardware
- Da Dome is separated into two semicircles. To help with troubleshooting, each row will be labelled with letters (A,B,C,D) and each column of the Da Dome will be referenced by numbers.

<img width="937" height="671" alt="Top-Down Diagram of Da Dome" src="https://github.com/user-attachments/assets/8f75b45c-2845-448f-8b05-73c2652a3b01" />

The 8 by 4 matrix has color coordinated cables that connect to the arcade buttons.
On the arcade buttons side, the full color cable represents data and the striped color/white cable represent ground.


| Row Letter | Column 1  | Column 2 | Column 3 |  Column 4 | Column 5 | Column 6 | Column 7 | Column 8 |
| ------------- | ------------- | ------------- |------------- |------------- |------------- |------------- |------------- | ------------- |
| A | Content Cell  | Content Cell  | Content Cell  | Content Cell  |Content Cell  |Content Cell  |Content Cell  | Content Cell  |
| B | Content Cell  | Content Cell  | Content Cell  | Content Cell  |Content Cell  |Content Cell  |Content Cell |Content Cell  |
| C | Content Cell  | Content Cell  | Content Cell  | Content Cell  |Content Cell  |Content Cell  |Content Cell |Content Cell  |
| D | Content Cell  | Content Cell  | Content Cell  | Content Cell  |Content Cell  |Content Cell  |Content Cell |Content Cell  |

## How to use Solution
- Ensure that the most recent Arduino code from the Github is uploaded to the Arduino Mega.
- Open the LoopMidi app and ensure a new LoopMidi port is added.
<img width="607" height="402" alt="LoopMidi" src="https://github.com/user-attachments/assets/0b2cee12-05ab-45ce-bb7a-e2bf65080924" />

- Open the EA Serial Bridge app, check that which port the serial cable is connected to via the Arduino app, and select the correct Serial Port, Midi Out (loopMidi Port 1) and the correct baud rate of 115200.
- Download the FL studio file and open the zip folder within the FL Studio app. The ZIP file does not need to be unzipped before opening within FL studio.
- If you have recently opened a new LoopMidi port, ensure that you disable and re-enable the general midi controller within FL studio

- Check that the project is looping and then press play! Da Dome is now functional!
