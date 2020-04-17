#ifndef OPENJVS_H_
#define OPENJVS_H_

#include "config.h"
#include "io.h"
#include "version.h"

JVSConfig config;

int main(int argc, char **argv);
void handleSignal(int sig);
JVSCapabilities *getCapabilities();

#endif // OPENJVS_H_