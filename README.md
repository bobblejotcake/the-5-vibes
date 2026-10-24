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
 - Arduino R3 Mega
 - Soldered 8 by 4 pin diode matrix
 - Laptop to connect to Arduino R3 Mega


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
<img width="4624" height="3472" alt="PXL_20261021_040226204" src="https://github.com/user-attachments/assets/e13f6be2-be63-499a-ad39-cb8a27ced3de" />
> The arcade buttons side.
<img width="4624" height="3472" alt="PXL_20261021_040140821" src="https://github.com/user-attachments/assets/b8e1a763-a206-436e-9268-b5fd1f807274" />
> The Arduino connection side of the button. Ensure the "white" version of every colored wire is connected to the ground pin within the soldered board. The full colored wire will go into the data pin on the protoboard. Messing up these connection may cause the buttons to not work.

| Row Letter | Column 1  | Column 2 | Column 3 |  Column 4 | Column 5 | Column 6 | Column 7 | Column 8 |
| ------------- | ------------- | ------------- |------------- |------------- |------------- |------------- |------------- | ------------- |
| A | A1  | A2 | A3  | A4  | A5  | A6  | A7 | A8  |
| B | B1  | B2  | B3  | B4  | B5 | B6  | B7 | B8 |
| C | C1  | C2  | C3  | C4 | C5  | C6 | C7 | C8  |
| D | D1  | D2 | D3  | D4  |D5 |D6  |D7 |D8 |
> A table of what each of the soldered prototype is linked to within the code.
<img width="4624" height="3472" alt="SolderBoard2" src="https://github.com/user-attachments/assets/de6589a2-8a49-4000-a717-68c9b42235c6" />
> Picture of the soldered protoboard.


## How to use Solution
- Ensure that the most recent Arduino code from the Github is uploaded to the Arduino Mega. This would be GeminiMatrixFinal.
- Open the LoopMidi app and ensure a new LoopMidi port is added.
<img width="607" height="402" alt="LoopMidi" src="https://github.com/user-attachments/assets/0b2cee12-05ab-45ce-bb7a-e2bf65080924" />

- Open the EA Serial Bridge app, check that which port the serial cable is connected to via the Arduino app, and select the correct Serial Port, Midi Out (loopMidi Port 1) and the correct baud rate of 115200.
- Download the most recent FL studio file (ClarityFinal.zip) and open the zip folder within the FL Studio app. Do not unzip the folder before opening it within FL Studio.
- If you have recently opened a new LoopMidi port, ensure that you disable and re-enable the general midi controller within FL studio inside the MIDI options.
<img width="826" height="942" alt="UpdateMIDISelection" src="https://github.com/user-attachments/assets/db818235-f8d4-457c-9350-b690934c14fb" />


- Then check that the FL studio project is looping or that the top bar is red. If not, double click on the flag to make ensure the project loops.
<img width="1112" height="909" alt="NoLoopClick" src="https://github.com/user-attachments/assets/1fa75223-722b-4713-8913-91d43df2230d" />
<img width="1193" height="918" alt="Loop" src="https://github.com/user-attachments/assets/57ad28fb-c330-4d95-9473-4d96f5e485a9" />
- Once everything has been setup, press play and start playing on Da Dome!

*Please note that the ordering of applications is important. The Arduino app should be open first, then LoopMIDI and finally EA Serial Bridge. These applications are taking over the Serial Monitor function to transmit MIDI signals to the laptop 


