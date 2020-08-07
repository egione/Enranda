/*
Enranda
Copyright 2016-2017 Russell Leidich
http://enranda.blogspot.com

This collection of files constitutes the Enranda Library. (This is a
library in the abstact sense; it's not intended to compile to a ".lib"
file.)

The Enranda Library is free software: you can redistribute it and/or
modify it under the terms of the GNU Limited General Public License as
published by the Free Software Foundation, version 3.

The Enranda Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Limited General Public License version 3 for more details.

You should have received a copy of the GNU Limited General Public
License version 3 along with the Enranda Library (filename
"COPYING"). If not, see http://www.gnu.org/licenses/ .
*/
TURN ON WORD WRAP SO YOU CAN READ THIS FILE AND ALSO SOURCE CODE COMMENTS,
ALTHOUGH DOING SO WILL RUIN TEXT ALIGNMENT.

VERSION

Version info is provided in the files starting with "flag".

ABOUT

Enranda
-------
Enranda is a library (in the abstract sense) of functions which generate true random numbers using your machine's CPU timestamp counter (timer). All the details are provided at the webpage linked above.

Enranda comes with a (probably outdated) version of Dyspoissometer, which is used for statistical analysis in the demo. See the header in dyspoissometer.c for a webpage where you can get the latest version with its own demo.

One-Time-Pad Maker with Enranda
-------------------------------
otpenranda is similar to /dev/random, except that (1) it only works in power-of-2 sizes and (2) it doesn't suck. Use it to generate power-of-2-sized chunks of true random data. Not that OTPs are useful in practice, but it provides everyone with an easy means to independently analyze Enranda's claims of randomness.

Timedelta Profile with Dyspoissometer
-------------------------------------
timedeltaprofile detects subtle changes in timing. It can be used to discover malware activity (especially blue pill attacks) or changes in: CPU frequency throttling behavior, application load, temperature, hardware interrupt latency, kernel bloat, etc. For an amazing demo, try: "timedeltaprofile 5 2 20" (remember that it's probably in the "temp" subfolder) in a terminal window while alternately playing and pausing a video in another window. Watch the timedelta log2 band populations fluctuate accordingly, creating a fuzzy yet distinct fingerprint of the video playing activity -- despite timedeltaprofile having no direct access to the video player! This suggests an obvious way in which to make educated guesses as to what processes are running in other windows, based on a support vector machine analysis of the local timedelta log2 band fingerprint.

In principle, even rare malware interrupts should show up as spikes in a distinct timedelta log2 band. It would be very hard to write malware code which could evade detection by this method (and also simultaneously all other known detection methods, such as ye olde byte signature analysis). It doesn't matter if the malware is encrypted or polymorphic or even obscured by opaque execution techniques such as homomorphic encryption; ironically, the harder the stealth technology, the more wildly its timedelta log2 band fingerprint -- as observed from a sandboxed and unprivileged process running on the same machine -- is likely to change. Wilder still, nearby uninfected machines might be able to "smell" the malware due to their own subtle but asymptotically distinct thermal aberrations induced by increased load on the infected machine -- virtual gravity waves, yo!

Therefore it bears emphasizing: be careful with allowed uses in published software, as this program is capable of acting as a receiver for covert thermal broadcasts across an airgap. Also, it might be used to reverse-engineer network topology based on the principle that physically proximate servers tend to have more similar timedelta log2 band fingerprints than distant ones; all that's needed is a virtual machine context on each machine whose relative location is to be mapped.

Timedelta Stream Saver
----------------------
timedeltasave samples raw timedeltas in a tight loop for your statistical analysis and malware sleuthing pleasure.

LANGUAGES

Currently only C (adhering to C11) is supported, but contact us if you want to port it.

TESTED ENVIRONMENTS

Linux 32/64-bit
Mac 64-bit
Windows (MinGW 32-bit with MSYS) or (MinGW-w64 32-bit with MSYS from MinGW)

UNTESTED ENVIRONMENTS (TELL US)

ARM 32/64-bit
Windows MinGW-w64 64-bit

GETTING STARTED

Linux
-----
In addition to a C compiler (either "gcc" or something aliased to "cc"), X86 and X64 platforms require the NASM assembler from http://nasm.us :

1. "sudo apt-get install nasm" at the Linux terminal prompt.
2. If that fails, try the following ultraslow commands: "sudo apt-get update" and then "sudo apt-get upgrade" and then go to step 1.
3. Worst case, download the latest RPM package. (It might be necessary to convert it to a DEB or TGZ package; search "alien convert rpm deb".)

Mac
---
In addition to the clang C compiler, you will need to install nasm (http://nasm.us).

Windows
-------
MinGW and MinGW-w64 are the only supported Windows compilers, although porting should be easy. As with Linux, NASM is required, but download the installer directly from the website. For example:

http://www.nasm.us/pub/nasm/releasebuilds/2.11.08/win32

(win32 seems to be the folder name even if you need to compile for a 64-bit target.)

Also, you will need to install MSYS from http://mingw.org . Just select "msys-base" and "mingw32-base" in the MinGW Installation Manager. Then go to Installation -> Apply Changes.

For its part, MinGW-w64 (which also compiles 32-bit code) seems to require MAKE from MinGW's MSYS package, so the following Path advice applies to both compilers:

MinGW and MinGW-w64 are frustating and unintuitive. For one thing, you need to change your Path variable at Start -> Computer -> Right Click -> Properties -> Advanced system settings -> Environment Variables -> System variabes -> Path, in order to point to your_mingw_folder\bin and your_mingw_folder\msys\[version]\bin (and for that matter, your_nasm_folder) . Then relauch the terminal window (Start -> Run -> cmd).

COMPILING

Build Cleanup
-------------
"make clean" will delete everything in the temp folder. (This must be done manually on Windows.)

Enranda
-------
Type "make demo" inside this folder, then follow the instructions from there to learn how Enranda works.

One-Time-Pad Maker with Enranda
-------------------------------
Type "make otpenranda" to build otpenranda.

Timedelta Profile with Dyspoissometer
-------------------------------------
Type "make timedeltaprofile" to build timedeltaprofile.

Timedelta Stream Saver
----------------------
Type "make timedeltasave" to build timedeltasave.
