/*
Enranda
Copyright 2016 Russell Leidich
http://enranda.blogspot.com

This collection of files constitutes the Dyspoissometer Library. (This is a
library in the abstact sense; it's not intended to compile to a ".lib"
file.)

The Dyspoissometer Library is free software: you can redistribute it and/or
modify it under the terms of the GNU Limited General Public License as
published by the Free Software Foundation, version 3.

The Dyspoissometer Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Limited General Public License version 3 for more details.

You should have received a copy of the GNU Limited General Public
License version 3 along with the Dyspoissometer Library (filename
"COPYING"). If not, see http://www.gnu.org/licenses/ .
*/
/*
Timedelta Profile Version Info
*/
/*
BUILD_ID must increase with every code release, even if only a comment changes. It allows the user to discern one build from another, but is invisible to other libraries. If this library depends on other libraries, then it should be expressed as the sum of the local build ID plus the build IDs of the dependencies, and in any event must increase monotonically even if some of the latter are removed.
*/
#define TIMEDELTAPROFILE_BUILD_ID (4+DYSPOISSOMETER_BUILD_ID+TIMESTAMP_BUILD_ID)
