BASICFLAGS=-D_FILE_OFFSET_BITS=64 -fno-stack-protector -mtune=native
BITS=32
CC=cc
DELTEMP=rm temp$(SLASH)*
EXE=
NASM=
OBJ=.o
OBJTYPE=elf
OPTIMIZEFLAGS=-O3
SLASH=/
TARGET=$(shell cc -dumpmachine)
ifeq (,$(TARGET))
	CC=gcc
	TARGET=$(shell gcc -dumpmachine)
endif
TIMESTAMP_CPU=
TIMESTAMP_CPU_OBJ=
TIMESTAMP_X86X64=0
WARNFLAGS=-Wall -Wextra -Wconversion -Wint-conversion
ifneq (,$(findstring 64,$(TARGET)))
	BITS=64
endif
ifneq (,$(findstring mingw,$(TARGET)))
ifneq (,$(findstring mingw32,$(TARGET)))
	BITS=32
endif
	DELTEMP=@echo clean does not work due to idiotic problems with MAKE under MSYS, but deleting everything in the temp subfolder accomplishes the same thing.
	EXE=.exe
	OBJ=.obj
	OBJTYPE=win
	SLASH=\\
	TIMESTAMP_X86X64=1
endif
ifneq (,$(findstring linux,$(TARGET)))
	TIMESTAMP_X86X64=1
endif
ifneq (,$(findstring darwin,$(TARGET)))
	OBJTYPE=macho
	TIMESTAMP_X86X64=1
	WARNFLAGS+= -Wconstant-conversion
else
ifneq (,$(findstring arm,$(TARGET)))
	TIMESTAMP_CPU=-DABSTIME
	WARNFLAGS+= -Wconstant-conversion
endif
endif
ifeq (1,$(TIMESTAMP_X86X64))
	TIMESTAMP_CPU=-DTIMESTAMP_X86_X64
        TIMESTAMP_CPU_OBJ=temp$(SLASH)timestamp_x86_x64$(OBJ)
	NASM=nasm -D_$(BITS)_ -O0 -f$(OBJTYPE)$(BITS) -o $(TIMESTAMP_CPU_OBJ) timestamp_x86_x64.asm
endif

clean:
	$(DELTEMP)

demo:
	make enranda
	$(CC) -c -fpic -D_$(BITS)_ -DDEBUG $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) -otemp$(SLASH)debug$(OBJ) debug.c
	$(CC) -c -fpic -D_$(BITS)_ -DDEBUG $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) -otemp$(SLASH)debug_quad$(OBJ) debug_quad.c
	$(CC) -c -fpic -D_$(BITS)_ -DDEBUG -DDYSPOISSOMETER_NUMBER_QUAD $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) -otemp$(SLASH)dyspoissometer$(OBJ) dyspoissometer.c
	$(CC) -D_$(BITS)_ -DDEBUG -DDYSPOISSOMETER_NUMBER_QUAD $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) temp$(SLASH)debug$(OBJ) temp$(SLASH)debug_quad$(OBJ) temp$(SLASH)dyspoissometer$(OBJ) temp$(SLASH)enranda$(OBJ) temp$(SLASH)timestamp$(OBJ) $(TIMESTAMP_CPU_OBJ) -otemp$(SLASH)demo$(EXE) demo.c -lm -lquadmath
	@echo
	@echo Learn how the demo works by following along with main\(\) in demo.c. See makefile
	@echo and flag\*.h for all the build magic. Standalone object files can be made with
	@echo \"make enranda\" or \"make enranda debug\". \(The debugger brings in a bunch of
	@echo other junk which is not appropriate for shipping products.\) \"clean\" will delete
	@echo the contents of the temp folder, which is where all object files and
	@echo executables are made. Beware that the make process overwrites them
	@echo automatically.
	@echo
	@echo In Windows, you may get some compilation errors about cc, fpic, or timestamp
	@echo link resolution. Ignore them.
	@echo
	@echo You can now run \"temp$(SLASH)demo\". Capture to a file if it scrolls too fast.

enranda:
	make timestamp
	$(CC) -c -fpic -D_$(BITS)_ -DDEBUG_OFF $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) -otemp$(SLASH)enranda$(OBJ) enranda.c

enranda_debug:
	make timestamp
	$(CC) -c -fpic -D_$(BITS)_ -DDEBUG $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) -otemp$(SLASH)enranda$(OBJ) enranda.c

otpenranda:
	make enranda
	$(CC) -D_$(BITS)_ -DDEBUG_OFF $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) temp$(SLASH)enranda$(OBJ) temp$(SLASH)timestamp$(OBJ) $(TIMESTAMP_CPU_OBJ) -otemp$(SLASH)otpenranda$(EXE) otpenranda.c
	@echo
	@echo You can now run \"temp$(SLASH)otpenranda\".

timedeltaprofile:
	make timestamp
	$(CC) -c -fpic -D_$(BITS)_ -DDEBUG_OFF -DDYSPOISSOMETER_NUMBER_QUAD $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) -otemp$(SLASH)dyspoissometer$(OBJ) dyspoissometer.c
	$(CC) -D_$(BITS)_ -DDEBUG_OFF -DDYSPOISSOMETER_NUMBER_QUAD $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) temp$(SLASH)dyspoissometer$(OBJ) temp$(SLASH)timestamp$(OBJ) $(TIMESTAMP_CPU_OBJ) -otemp$(SLASH)timedeltaprofile$(EXE) timedeltaprofile.c -lm -lquadmath
	@echo
	@echo You can now run \"temp$(SLASH)timedeltaprofile\".

timedeltasave:
	make timestamp
	$(CC) -D_$(BITS)_ -DDEBUG_OFF $(BASICFLAGS) -O0 $(WARNFLAGS) temp$(SLASH)timestamp$(OBJ) $(TIMESTAMP_CPU_OBJ) -otemp$(SLASH)timedeltasave$(EXE) timedeltasave.c
	@echo
	@echo You can now run \"temp$(SLASH)timedeltasave\".

timestamp:
	$(NASM)
	$(CC) -c -fpic -D_$(BITS)_ -DDEBUG_OFF $(TIMESTAMP_CPU) $(BASICFLAGS) $(OPTIMIZEFLAGS) $(WARNFLAGS) -otemp$(SLASH)timestamp$(OBJ) timestamp.c
