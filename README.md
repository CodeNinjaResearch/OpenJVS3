# OpenJVS3
[![Build status](https://github.com/bobbydilley/OpenJVS3/workflows/Build/badge.svg?branch=master)](https://github.com/bobbydilley/OpenJVS3/actions?query=branch%3Amaster)

This is a completely new version of OpenJVS built from the JVSCore project. The aim of this revision of the software is to be stable on all hardware and linux distributions, and built in a maintainable way.

## Installation

To install OpenJVS3 follow the instructions below to install the required packages and make the program.

```
sudo apt install git build-essential cmake
git clone https://github.com/bobbydilley/OpenJVS3
cd OpenJVS3
./build.sh
dpkg --install build/*.deb
```

Please note when installing the package again, any settings files you have modified will be overwritten. Map/Device files that you have __added__ your self will remain safe.

## Usage

OpenJVS3 can be run from the command line by typing the executable name.

```
sudo openjvs
```

## Setup

Settings files are located in `/etc/openjvs/` which contains the global config file and both mapping from USB devices to OpenJVS and OpenJVS to Arcade Hardware.

### Devices

Below are the list of commands they take:

- ABS 255 ACCEL
- KEY 25 BUTTON_1

### Arcade

- BUTTON 1 BUTTON_1
