#pragma once

#include <vgacommon.h>
#include <keyboard.h>
#include <cpu.h>
#include <sched.h>
#include <memory.h>
#include <fs.h>
#include <kmalloc.h>

#define PROMPT "Root@Ring0-> "

//The main loop for the shell
void zeros_shell();

//Command parser
void parse_command(char*);

void help();
