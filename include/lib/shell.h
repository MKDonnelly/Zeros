#pragma once

#include <arch/x86/vgacommon/vgacommon.h>
#include <arch/x86/keyboard.h>
#include <arch/x86/cpu.h>
#include <kernel/sched.h>
#include <lib/memory.h>
#include <fs/fs.h>
#include <kernel/kmalloc.h>
#include <lib/string.h>

#define PROMPT "Root@Ring0-> "

//A horrible hack to start the shell
//once the boot demo is done
extern int startShell;

//The main loop for the shell
void zeros_shell();

//Command parser
void parse_command(char*);

//Individual commands
void help();
void ls_files();
void cat_file(char*);
void sysuptime(char);
void open_file(char*);
void clear_screen();
void sysinfo();
void write_file(char*, char*);
