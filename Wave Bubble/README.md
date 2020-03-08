# Wave Bubble

## Preparation

Original idea and design by Limor Fried and updated by Mictronics.

References:

- http://www.ladyada.net/make/wavebubble/
- https://forums.adafruit.com/viewtopic.php?f=16&t=14782
- http://ladyada.net/wiki/wavebubble/wave_bubble_2010

Software Prerequisites:

- [Autodesk EAGLE](https://www.autodesk.com/products/eagle/overview)
- [PuTTY](https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html)
- [Atmel Studio 7](https://www.microchip.com/mplab/avr-support/atmel-studio-7) or [AVR Toolchains (C Compilers)](https://www.microchip.com/mplab/avr-support/avr-and-arm-toolchains-c-compilers)
- [FTDI VCP Drivers](https://www.ftdichip.com/Drivers/VCP.htm)

Hardware and Tools Used:

- Fluke 87V
- Tektronix TDS2014C
- Arksen 305D
- Weller WES51
- Lenovo ThinkPad T480s
- Atmel AVRISP mkII

Lots of cables, probes, wires, etcs...

Datasheets:

- [FT232RL](https://www.ftdichip.com/Support/Documents/DataSheets/ICs/DS_FT232R.pdf)
- [FDN302P](https://www.onsemi.com/pub/Collateral/FDN302P-D.PDF)
- [FDV303N](https://www.onsemi.cn/PowerSolutions/document/FDV303N-D.PDF)
- [MMBT3906](https://www.onsemi.com/pub/Collateral/PZT3906-D.PDF)
- [LM358](https://www.onsemi.com/pub/Collateral/LM358-D.PDF)
- [LM2731](http://www.ti.com/lit/ds/symlink/lm2731.pdf)
- [LMX2433](http://www.ti.com/lit/ds/symlink/lmx2433.pdf)
- [LP2985](http://www.ti.com/lit/ds/symlink/lp2985.pdf)
- [TPS793](http://www.ti.com/lit/ds/symlink/tps793.pdf)
- [NE555](http://www.ti.com/lit/ds/symlink/ne555.pdf)
- [LT1173](https://www.analog.com/media/en/technical-documentation/data-sheets/lt1173.pdf)
- [LT1301](https://www.analog.com/media/en/technical-documentation/data-sheets/lt1301.pdf)
- [LTC1730-4.2](https://www.analog.com/media/en/technical-documentation/data-sheets/1730fs.pdf)
- [AD8402](https://www.analog.com/media/en/technical-documentation/data-sheets/AD8400_8402_8403.pdf)
- [ATmega168](http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48_88_168_megaAVR-Data-Sheet-40002074.pdf)
- [MIC2506](http://ww1.microchip.com/downloads/en/DeviceDoc/20005579A.pdf)
- [MIC2514](http://ww1.microchip.com/downloads/en/DeviceDoc/mic2514.pdf)
- [ROS-1300](https://www.minicircuits.com/pdfs/ROS-1300+.pdf)
- [ROS-2700-1819](https://www.minicircuits.com/pdfs/ROS-2700-1819+.pdf)
- [SGA-7489Z](<https://media.digikey.com/pdf/Data%20Sheets/Sirenza%20Microdevices/SGA-7489(Z)_Datasheet.pdf>)
- [ABMM-10.000MHZ](https://abracon.com/Resonators/ABMM.pdf)
- [SMAZ22-13-F](https://www.diodes.com/assets/Datasheets/ds18015.pdf)

## Technical Information

### ATmega168 Fuse Settings

- EXTENDED: 0xFF
- HIGH: 0xD5
- LOW: 0xE2
  See [documentation](http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48_88_168_megaAVR-Data-Sheet-40002074.pdf) or an [AVR fuse calculator](http://www.engbedded.com/fusecalc/) for details.

### Connections

Connect the battery board to the main board via the 4-pin jumper. Power the main board using ~4.8V (limit power supply current to 250mA).

## Hardware Test and Firmware Build Configuration

See [Makefile](Wave Bubble/Makefile) or [Visual Studio Solution](Wave Bubble.atsln).

### Flashing

Build and flash the `.hex` (or `.elp`) file using an AVR programmer.

Need to hold "Power On" button while flashing or burning fuses. Select ATmega168 device target in programmer.

### Serial Interface

Run the tests by connecting the battery board via USB and using a terminal program.

Plug in the USB into the computer, it should show up as a virtual COM port. Open that COM port with a terminal program (configuration settings below).

PuTTY configuration:

- Connection type: Serial
- Serial line to connect to: COM3
- Speed (baud): 19200
- Data bits: 8
- Stop bits: 1
- Parity: None
- Flow control: None
- Implicit CR in every LF: checked
- Implicit LF in every CR: checked

In Test Firmware, type characters to run tests from the menu. Note that there is no echo of characters during test, probe the Test Points with oscilloscope and/or multimeter for feedback.
In Operational Firmware, type character to enter the menu. Then follow the prompts to configure the Wave Bubble from the menu.

### Test Setup

Set current limit to power supply (e.g. 200mA).
Connect power supply to battery leads, set voltage above 4.2V so that fuel gauge IC thinks battery is fully charged.
Press and hold "Power On" button to bootstrap the board's regulators until the microcontroller is booted.
Once text is on serial terminal use keyboard to control (note: once again no visual feedback is shown in terminal - only changes in board behavior).

## RF Bands

| Name/Description    | Freq. Band                                                   | Notes                                                                    |
| ------------------- | ------------------------------------------------------------ | ------------------------------------------------------------------------ |
| GSM 850 Downlink    | 870-894MHz                                                   | Starting to become more popular, but used more in rural areas in the US. |
| GSM 900 Downlink    | 925-960MHz                                                   | Original & most common frequency band used in Europe.                    |
| GSM 1800 Downlink   | 1805-1880MHz                                                 | Starting to become more popular in Europe.                               |
| GSM (PCS) Broadband | 1900 Downlink 1930-1990MHz                                   | Original & most common frequency band used in the US.                    |
| Cordless Phones     | 900-910MHz (handset) 920-930MHz (base)                       | 900MHz band cordless phones                                              |
| Bluetooth           | 2400-2483MHz                                                 | Headsets, small electronics                                              |
| 802.11b/g           | 2400-2483MHz                                                 | Generic WiFi                                                             |
| ZigBee              | 868 (Europe) 915 (USA) and 2400-2483MHz                      | Some home automation stuff                                               |
| GPS                 | 1575.42 MHz and 1227.6 MHz (civilian), 1227.6 MHz (military) | Global Positioning System                                                |

## Bill of Materials

### USB/Battery Board

| Name | Value       | Package         | Qty |
| ---- | ----------- | --------------- | --- |
| BAT1 | LiPo        | 3.7V/1800mAh    | 1   |
| C1   | 0.10μF/50V  | 1206            | 1   |
| C2   | 0.10μF/50V  | 1206            | 1   |
| C3   | 0.10μF/50V  | 1206            | 1   |
| C4   | 0.10μF/50V  | 1206            | 1   |
| C5   | 1uF/25V     | 1206            | 1   |
| C6   | 220uF/6.3V  | SMD_R8X5.4_ELKO | 1   |
| C7   | 47uF/6.3V   | SMD_R5X5.4_ELKO | 1   |
| C8   | 1u          | 1206            | 1   |
| C9   | 47n         | 1206            | 1   |
| C10  | 22n         | 1206            | 1   |
| C11  | 100n        | 1206            | 1   |
| D1   | GREEN       | 1206-D          | 1   |
| IC1  | FT232RL     | TSSOP-28        | 1   |
| IC2  | LTC1730-4.2 | SO-8            | 1   |
| JP1  | Mini USB    | MINI-USB-SMD    | 1   |
| R1   | 0.05        | 1206            | 1   |
| R2   | 1k          | 1206            | 1   |
| R3   | 4.7         | 1206            | 1   |
| R4   | 10k         | 1206            | 1   |
| R5   | 10k NTC     | 1206            | 1   |
| R6   | 4.1k        | 1206            | 1   |

### Main Board

| Name                                 | Value                 | Package           | Qty |
| ------------------------------------ | --------------------- | ----------------- | --- |
| C1                                   | 100μF/6.3V            | SMD_R6.3X5.4_ELKO | 1   |
| C2,C3,C6,C7,C9,C16,C17,C19,C21,C22,  | 100nF                 | 1206              | 19  |
| C25,C26,C27,C28,C31,C32,C33,C61,C62  | "                     | "                 | "   |
| C4,C5,C8,C14,C15,C34,C35,C42,C45,C64 | 1uF                   | 1206              | 10  |
| C10                                  | 10nF                  | 1206              | 1   |
| C11                                  | 100μF/4V              | SMD_R5X5.4_ELKO   | 1   |
| C12                                  | 470pF                 | 1206              | 1   |
| C13                                  | 100μF/10V             | 6032              | 1   |
| C18                                  | 22μF/35V              | 7243              | 1   |
| C20                                  | 68μF/16V              | 6032              | 1   |
| C23,C24,C29,C30                      | 4.7μF                 | 1206              | 4   |
| C36,C40,C47                          | 68pF                  | 0603              | 3   |
| C38,C46,C48,C49,C50,C51,C52,C55      | 100pF                 | 0603              | 8   |
| C41,C44                              | 1000pF                | 0603              | 2   |
| C43,C53,C54                          | 22pF                  | 0603              | 3   |
| C56,C57,C58,C63                      | 10nF                  | 0603              | 4   |
| C59                                  | 100nF                 | 0603              | 1   |
| C60                                  | 1μF                   | 0603              | 1   |
| D1,D2,D3                             | SS14                  | SMA               | 3   |
| D4                                   | Green LED             | 1206-D            | 1   |
| D5,D6                                | SMAZ22-13-F           | SMA               | 2   |
| IC1                                  | TPS793                | SOT23-5           | 1   |
| IC2                                  | LM2731                | SOT23-5           | 1   |
| IC3                                  | LT1173                | SOIC-8            | 1   |
| IC4                                  | LT1301                | SOIC-8            | 1   |
| IC5                                  | ATMEGA168             | TQFP-32           | 1   |
| IC6                                  | AD8402                | SOIC-14           | 1   |
| IC7                                  | NE555                 | 8-SMD-1           | 1   |
| IC8,IC9                              | LM358                 | SOIC-8            | 2   |
| IC10,IC11                            | MIC2514               | SOT23-5           | 2   |
| IC12                                 | MIC2506               | SOIC-8            | 1   |
| IC13                                 | LMX2433               | TSSOP-20          | 1   |
| IC14                                 | LP2985                | SOT23-5           | 1   |
| JP1                                  | USB/Battery Board     | 1X04              | 1   |
| JP2                                  | ISP                   | 2X03-90           | 1   |
| JP5,JP6                              | SMA                   | SMA J629          | 2   |
| L1                                   | 7.3μH                 | CDRH6D28          | 1   |
| L2,L3                                | 33uH                  | CDRH6D28          | 2   |
| L4,L5                                | FERRIT                | 1206              | 2   |
| L6                                   | 22nH                  | 0603              | 1   |
| L7                                   | 33nH                  | 0603              | 1   |
| R1                                   | 510kΩ                 | 1206              | 1   |
| R2                                   | 330kΩ                 | 1206              | 1   |
| R3                                   | 47kΩ                  | 1206              | 1   |
| R4                                   | 13kΩ                  | 1206              | 1   |
| R5                                   | 820kΩ                 | 1206              | 1   |
| R6                                   | 50kΩ                  | 1206              | 1   |
| R7,R13,R16,R17,R18,R19,R38,R39       | 10kΩ                  | 1206              | 8   |
| R8,R9                                | 1kΩ                   | 1206              | 2   |
| R12                                  | 750Ω                  | 1206              | 1   |
| R14                                  | 20kΩ                  | 1206              | 1   |
| R15                                  | 68Ω                   | 1206              | 1   |
| R20                                  | 47kΩ                  | 1206              | 2   |
| R21                                  | 54.9kΩ                | 1206              | 2   |
| R23,R24,R25,R27                      | 0Ω                    | 0603              | 4   |
| R28,R29                              | 1Ω                    | 1206              | 2   |
| R30,R31                              | 50Ω                   | 0603              | 2   |
| R32,R33,R34,R35                      | 18Ω                   | 0603              | 4   |
| R36,R37                              | 10kΩ                  | 0603              | 2   |
| R40,R42                              | 10Ω                   | 0603              | 2   |
| R41                                  | 120Ω                  | 0603              | 1   |
| R43,R45                              | 22Ω                   | 0603              | 2   |
| R44                                  | 47Ω                   | 0603              | 1   |
| R46                                  | 100Ω                  | 1206              | 1   |
| R47,R48                              | 100kΩ                 | 1206              | 2   |
| S1,S2                                | Program, Power Switch | TL3330            | 2   |
| T1                                   | MMBT3906              | SOT23-3           | 1   |
| T2,T3                                | SGA-7489Z             | SOT-89            | 2   |
| T4                                   | FDN302P               | SSOT              | 1   |
| T5                                   | FDV303N               | SSOT              | 1   |
| VCO1                                 | ROS-2700-1819         | CK605             | 1   |
| VCO2                                 | ROS-1300              | CK605             | 1   |
| X1                                   | 10MHz                 | ABMM Crystal      | 1   |

Notes:

- R20 and R21 adjusted to match Vtune maximum of VCO. Adjusting LM-358 Max Vtune Voltage:
  Adjust the values for R20 and R21 to correspond more adequately with the VCO Tuning Voltage Limits and the input power you get at the LM-358 (IC9).
  The formula for the OP AMP in NonInverting Step Up Mode:  
  Vout = Vin\*((Rf/Ri)+1)  
  Examples:  
  For the ROS 1200  
  3.3V((30kohm/10kohm)+1) = 13.2V  
  Corrected:  
  3.3V((47.7/10)+1) = 19V ( max Vtune for ROS 1300 = 20V)  
  For the ROS2700-1819+:  
  3.3((64.9/10)+1) = 24.717V (Max Vtune for ROS 2700 is 25V)
- JP3 and JP4 shorted between pads 1 and 2.
- JP7 shorted between pads 2 and 3.
- Missing R43 pads in PCB used 1206 package version: https://forums.adafruit.com/viewtopic.php?f=16&t=22158
- Cut D1 ground pad to 5V trace short
- Oscillator calibration (i.e. OSCCAL) is not needed in firmware, produces garbage serial output.

## VCO Testing and Tuning References

http://www.ladyada.net/make/wavebubble/tuning.html
VCO Test Methods:
http://www.minicircuits.com/app/VCO15-15.pdf
General VCO Notes:
http://www.spectrummicrowave.com/pdf/VCO_application_notes.pdf

Voltage controlled oscillator, VCO, for PLLs

- an overview of the various types of voltage controlled oscillator, VCO, used in phase locked loops, PLLs and frequency synthesizers
  http://www.radio-electronics.com/info/rf-technology-design/pll-synthesizers/vco-voltage-controlled-oscillators.php

Phase locked loop, PLL, tutorial
http://www.radio-electronics.com/info/rf-technology-design/pll-synthesizers/phase-locked-loop-tutorial.php

PLL loop filter

- an overview of the loop filter used in a phase locked loop, PLL. This gives an overview of the requirements, and design.
  http://www.radio-electronics.com/info/rf-technology-design/pll-synthesizers/loop-filter-design.php

## Miscellaneous

Initialize ClangFormat configuration file:

```
> &"$env:PROGRAMFILES\LLVM\bin\clang-format.exe" -style="Google" -dump-config > .clang-format
```
