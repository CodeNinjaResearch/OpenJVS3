#ifndef OPENJVS_H_
#define OPENJVS_H_

#include <stdio.h>
#include <signal.h>

#include "jvs.h"
#include "input.h"
#include "io.h"

int main(int argc, char **argv);
void handleSignal(int sig);

#endif // OPENJVS_H_