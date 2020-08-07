;
; Enranda
; Copyright 2016 Russell Leidich
; http://enranda.blogspot.com
;
; This collection of files constitutes the Enranda Library. (This is a
; library in the abstact sense; it's not intended to compile to a ".lib"
; file.)
;
; The Enranda Library is free software: you can redistribute it and/or
; modify it under the terms of the GNU Limited General Public License as
; published by the Free Software Foundation, version 3.
;
; The Enranda Library is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
; Limited General Public License version 3 for more details.
;
; You should have received a copy of the GNU Limited General Public
; License version 3 along with the Enranda Library (filename
; "COPYING"). If not, see http://www.gnu.org/licenses/ .
;
;--------------------------------------------------------------------------
;
; Timestamp Reader
;
; For use with:
;
;   32-bit X86 gcc, clang, and Microsoft compilers.
;
;   64-bit X64 gcc and clang.
;
; Compile with nasm, which is available from http://nasm.us . It can be
; installed in most Linux distros using "sudo apt-get intall nasm" in a terminal
; window. See makefile for sample invokations.
;
%IFDEF _32_
%IFDEF _64_
%FATAL #error "You have defined both _32_ and _64_. Chose one only."
%ELSE
USE32
%ENDIF
%ELIF _64_
USE64
%ELSE
%FATAL "Use \"nasm -D_64_\" for 64-bit or \"nasm -D_32_\" for 32-bit code."
%ENDIF

SECTION .text

%MACRO PROC 1
  GLOBAL %1
  %1:
%ENDMACRO

%MACRO ENDPROC 1
%ENDMACRO

%MACRO INT32 1
  %1: RESD 1
%ENDMACRO

%MACRO INT64 1
  %1: RESQ 1
%ENDMACRO

STRUC timestamp_cpu
  INT64 timestamp
  INT64 temp0
  INT64 temp1
  INT64 temp2
ENDSTRUC
;
; timestamp_cpu_get is (ALIGN)ed to a 32-byte boundary in order to minimize
; variances in CPU instruction pipeline alignment from platform to platform,
; in order to enable more accurate behavior characterization using multiple
; cores or multiple machines. (This is probably completely unnecessary, thanks
; to Enranda's sequence hashing, but the cost is negligible so we do it
; anyway.)
;
ALIGN 32
PROC timestamp_cpu_get
PROC _timestamp_cpu_get
;
; Read the Timestamp Counter (TSC)
;
; The C prototype is defined such that ECX/RDI will end up pointing to
; timestamp_cpu. See
;
; http://en.wikipedia.org/wiki/X86_calling_conventions for more information.
;
; Call it like this:
;
;   #include "timestamp_x86_x64.h"
;   timestamp_cpu_t timestamp_cpu;
;   timestamp_cpu_get(&timestamp_cpu);
;
;-----------------------------------------------------------
;
; IN
;
;   ECX/RDI = Base of timestamp_cpu.
;
;   DS = Segment of timestamp_cpu (32-bit environment only).
;
; OUT
;
;   EAX/RAX destroyed.
;
;   timestamp_cpu is undefined except for timestamp, which contains the latest
;   value read from the timestamp counter (TSC).
;
;-----------------------------------------------------------
;
; We can't easily use the stack due to the possible presence of the compiler red
; red zone below ESP or RSP. That's OK because we have ECX/RDI as the base of
; timestamp_cpu.
;
%IFDEF _32_
  mov [ecx+temp0], edx
  rdtsc
  mov [ecx+timestamp], eax
  mov [ecx+timestamp+4], edx
  mov edx, [ecx+temp0]
%ELSE
  mov [rdi+temp0], rdx
  rdtsc
  mov [rdi+timestamp], eax
  mov [rdi+timestamp+4], edx
  mov rdx, [rdi+temp0]
%ENDIF
  retn
ENDPROC _timestamp_cpu_get
ENDPROC timestamp_cpu_get
;
; ALIGN is unnecessary here, as autopseudorandomness is minimized by (ALIGN)ing
; read_timestamp_counter below.
;
PROC timestamp_x4_cpu_get
PROC _timestamp_x4_cpu_get
;
; Read the Timestamp Counter (TSC) 4 Times
;
; The C prototype is defined such that ECX/RDI will end up pointing to
; timestamp_x4_cpu. See
;
; http://en.wikipedia.org/wiki/X86_calling_conventions for more information.
;
; Call it like this:
;
;   #include "timestamp_x86_x64.h"
;   timestamp_x4_cpu_t timestamp_x4_cpu;
;   timestamp_x4_cpu_get(&timestamp_x4_cpu);
;
;-----------------------------------------------------------
;
; IN
;
;   ECX/RDI = Base of timestamp_cpu.
;
;   DS = Segment of timestamp_cpu (32-bit environment only).
;
; OUT
;
;   EAX/RAX destroyed.
;
;   timestamp_cpu is undefined except for timestamp, which contains 4
;   successive uint16_t timestamps, starting from bit 0.
;
;-----------------------------------------------------------
;
; We can't easily use the stack due to the possible presence of the compiler red
; red zone below ESP or RSP. That's OK because we have ECX/RDI as the base of
; timestamp_x4_cpu.
;
%IFDEF _32_
  mov [ecx+temp0], edi
  mov edi, ecx
  mov [ecx+temp0+4], ebx
  mov [ecx+temp1], edx
  mov [ecx+temp1+4], esi
  mov ecx, 4+1
  jmp SHORT loop_read_timestamp_counter

ALIGN 32
read_timestamp_counter:
;
; The first read will be entropy-rich -- and legitimately so, overwhelmingly
; due to the long time that has probably elapsed since this function was last
; called. So the additional autopseudorandomness induced by first-loop effects
; is negligible. We could eliminate those effects by discarding the first
; RDTSC, but the security advantage would be negligible (after passing through
; Enranda's extensive safeguards) and the performance hit would be significant.
;
  rdtsc
  shrd ebx, esi, 16
  shrd esi, eax, 16

loop_read_timestamp_counter:
;
; JMP here first to maximize predictable branch behavior after the first RDTSC.
; ...just another way to minimize autopseudorandomness. Furthermore, we use the
; LOOP instruction because it provides more boring and less speculative
; performance than a conditional JMP.
;
  loop read_timestamp_counter

  mov [edi+timestamp], ebx
  mov [edi+timestamp+4], esi
  mov esi, [edi+temp1+4]
  mov edx, [edi+temp1]
  mov ebx, [edi+temp0+4]
  mov ecx, edi
  mov edi, [edi+temp0]
%ELSE
  mov [rdi+temp0], rbx
  mov [rdi+temp1], rcx
  mov [rdi+temp2], rdx
  mov ecx, 4+1
  jmp SHORT loop_read_timestamp_counter

ALIGN 32
read_timestamp_counter:
  rdtsc
  shrd rbx, rax, 16

loop_read_timestamp_counter:
  loop read_timestamp_counter

  mov [rdi+timestamp], rbx
  mov rdx, [rdi+temp2]
  mov rcx, [rdi+temp1]
  mov rbx, [rdi+temp0]
%ENDIF
  retn
ENDPROC _timestamp_x4_cpu_get
ENDPROC timestamp_x4_cpu_get
