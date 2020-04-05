# OpenJVS3
[![Build status](https://github.com/bobbydilley/OpenJVS3/workflows/Build/badge.svg?branch=master)](https://github.com/bobbydilley/OpenJVS3/actions?query=branch%3Amaster)

This is a completely new version of OpenJVS built from the JVSCore project. The aim of this revision of the software is to be stable on all hardware and linux distributions, and built in a maintainable way.

## Installation

To install OpenJVS3 follow the instructions below to install the required packages and make the program.

```
sudo apt install git build-essential cmake
git clone https://github.com/bobbydilley/OpenJVS3
cd OpenJVS3
mkdir build && cd build
cmake ..
make
cd ..
```

## Usage

OpenJVS3 can be run from the command line by typing the executable name and it will start emulating the specific IO defined in `openjvs.c`.

```
sudo ./build/openjvs
```

## Setup

Settings files are located in `/docs/maps/` and contain both mapping from USB devices to OpenJVS and OpenJVS to Arcade Hardware.

### Devices

Below are the list of commands they take:

- ABS 255 ACCEL
- KEY 25 BUTTON_1

### Arcade

- BUTTON 1 BUTTON_1

### Create an Installation Package

To create a debian package that can be installed follow these instructions from the root of the project:

```
mkdir -p build && cd build
cmake ..
cmake --build .
cpack
```

This will make a .deb file which can be installed like this:

```
dpkg --install <file_name>.deb
```

and removed like this:

```
dpkg --remove openjvs
```
