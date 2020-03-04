# Wave Bubble

## Preparation

Original idea and design by Limor Fried and updated by Mictronics.

References:

- http://www.ladyada.net/make/wavebubble/
- http://ladyada.net/wiki/wavebubble/wave_bubble_2010

Software Prerequisites:

- Autodesk EAGLE: https://www.autodesk.com/products/eagle/overview
- PuTTY: https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html
- Atmel Studio 7: https://www.microchip.com/mplab/avr-support/atmel-studio-7
- FTDI VCP Drivers: https://www.ftdichip.com/Drivers/VCP.htm

Hardware and Tools Used:

- Fluke 87V
- Tektronix TDS2014C
- Arksen 305D
- Weller WES51
- Lenovo ThinkPad T480s
- Atmel AVRISP mkII

Lots of cables, probes, wires, etcs...

Datasheets:

- FT232RL: https://www.ftdichip.com/Support/Documents/DataSheets/ICs/DS_FT232R.pdf
- FDN302P: https://www.onsemi.com/pub/Collateral/FDN302P-D.PDF
- FDV303N: https://www.onsemi.cn/PowerSolutions/document/FDV303N-D.PDF
- MMBT3906: https://www.onsemi.com/pub/Collateral/PZT3906-D.PDF
- LM358: https://www.onsemi.com/pub/Collateral/LM358-D.PDF
- LM2731: http://www.ti.com/lit/ds/symlink/lm2731.pdf
- LMX2433: http://www.ti.com/lit/ds/symlink/lmx2433.pdf
- LP2985: http://www.ti.com/lit/ds/symlink/lp2985.pdf
- TPS793: http://www.ti.com/lit/ds/symlink/tps793.pdf
- NE555: http://www.ti.com/lit/ds/symlink/ne555.pdf
- LT1173: https://www.analog.com/media/en/technical-documentation/data-sheets/lt1173.pdf
- LT1301: https://www.analog.com/media/en/technical-documentation/data-sheets/lt1301.pdf
- LTC1730-4.2: https://www.analog.com/media/en/technical-documentation/data-sheets/1730fs.pdf
- AD8402: https://www.analog.com/media/en/technical-documentation/data-sheets/AD8400_8402_8403.pdf
- ATmega168: http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48_88_168_megaAVR-Data-Sheet-40002074.pdf
- MIC2506: http://ww1.microchip.com/downloads/en/DeviceDoc/20005579A.pdf
- MIC2514: http://ww1.microchip.com/downloads/en/DeviceDoc/mic2514.pdf
- ROS-1300: https://www.minicircuits.com/pdfs/ROS-1300+.pdf
- ROS-2700-1819: https://www.minicircuits.com/pdfs/ROS-2700-1819+.pdf
- GALI-84: https://www.minicircuits.com/pdfs/GALI-84+.pdf
- ABMM-10.000MHZ: https://abracon.com/Resonators/ABMM.pdf

## Technical Information

ATmega168 Fuse settings:

- EXTENDED: 0xFF
- HIGH: 0xD5
- LOW: 0xE2

Build and flash the .hex (or .elp) file using a AVR programmer.

Run the tests by connecting the battery board via USB and using a terminal program. Set the terminal to .

Connect the battery board to the main board, solder in the 4-pin jumper if you havent yet
Plug in the USB into your computer, it should show up as a COM port. Open up that COM port with your favorite terminal program (19.2Kbps 8N1 no handshake,etc)

PuTTY config (19200 Baud 8N1):

- Serial line to connect to: COM3
- Implicit CR in every LF: checked
- Speed (baud): 19200
- Data bits: 8
- Stop bits: 1
- Parity: None
- Flow control: None

Type characters to run tests from the menu (note: no echo of characters during test).

## Test Setup

Set current limit to power supply (e.g. 200mA).
Connect power supply to battery leads, set voltage above 4.2V so that fuel gauge IC thinks battery is fully charged.
Press and hold "Power On" button to bootstrap the board's regulators until the microcontroller is booted.
Once text is on serial terminal use keyboard to control (note: once again no visual feedback is shown in terminal - only changes in board behavior).

## Flashing and Fusing

Need to hold "Power On" button while flashing or burning fuses.
Select ATmega168 device target in programmer.

## Hardware Test and Firmware Build Configuration

See [Makefile](Wave Bubble/Makefile) or [Visual Studio Solution](Wave Bubble.atsln).

## Miscellaneous

```
> &"$env:HOME\.vscode\extensions\ms-vscode.cpptools-<version>\LLVM\bin\clang-format.exe" -style="Google" -dump-config > .clang-format
```
