# OpenJVS3

This is a completely new version of OpenJVS built from the JVSCore project. The aim of this revision of the software is to be stable on all hardware and linux distributions.

## Aims for this version

- OpenJVS should run on any hardware running any linux distribution that can support a USB to RS485 converter.
- OpenJVS should run without any 'checksum error' issues, even if they don't fully stop it working.
- OpenJVS code should be clean and easily maintainable.

## Project Milestones

- Create an application with no input, that can talk to a Sega Naomi using sync to ground and functions well.
- Create an application that can support any evdev device.

## Installation

To install OpenJVS3 follow the instructions below to install the required packages and make the program.

```
sudo apt install git build-essential
git clone https://github.com/bobbydilley/OpenJVS3
cd OpenJVS3
make
sudo make install
```
