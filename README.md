# OpenJVS3

This is a completely new version of OpenJVS built from the JVSCore project. The aim of this revision of the software is to be stable on all hardware and linux distributions, and built in a maintainable way.

## Installation

To install OpenJVS3 follow the instructions below to install the required packages and make the program.

```
sudo apt install git build-essential
git clone https://github.com/bobbydilley/OpenJVS3
cd OpenJVS3
make
```

## Usage

OpenJVS3 can be run from the command line by typing the executable name and it will start emulating the specific IO defined in `openjvs.c`.

```
sudo ./bin/openjvs
```
