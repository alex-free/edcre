# This variables.mk file is part of the EzRe build system v1.1.2.
# https://github.com/alex-free/ezre

# REQUIRED: executable name in release (.exe file extension is appended for Windows builds). I.e. hello.
PROGRAM=edcre
# REQUIRED: Version number, passed as 'VERSION' string to $(SOURCE_FILES). I.e. 1.0. Prepended with a v.
VERSION=1.1.0
# REQUIRED: source files to be compiled into $(PROGRAM) target. Can use wildcard (i.e. *.c, *.cpp, etc) or specify files specifically. These files are looked for in the same directory that the EZRE `Makefile` and `variables.mk` files are in (relative).
SOURCE_FILES=edcre.cc

# REQUIRED: Basename of all release files (.zip, .deb). I.e. hello-world.
RELEASE_BASE_NAME=edcre
# REQUIRED: Appended to end of release file name. Release file format is $(RELEASE_BASE_NAME)-$(VERSION)-$(RELEASE_NAME_SUFFIX).
RELEASE_NAME_SUFFIX_LINUX_I386=linux-i386-static
RELEASE_NAME_SUFFIX_LINUX_X86_64=linux-x86_64-static
RELEASE_NAME_SUFFIX_WINDOWS_I686=windows-i686-static
RELEASE_NAME_SUFFIX_WINDOWS_X86_64=windows-x86_64-static
# Because uname -m = Power Macintosh, etc on 10.4. On 10.12 it is x86_64
RELEASE_NAME_SUFFIX_MAC_OS=mac-os-$(shell uname -m)
# Because uname -p = powerpc, etc. on 10.4. On 10.12 it is i386.
RELEASE_NAME_SUFFIX_MAC_OS_LEGACY=mac-os-$(shell uname -p)
# OPTIONAL: additional files included in all zip releases. I.e. readme.md.
RELEASE_FILES=*.md

# For RPM and DNF packages:
SOFTWARE_DESCRIPTION=Regenerates EDC/ECC data in CD image files.
YOUR_NAME=Alex Free
WEBSITE=https://github.com/alex-free/edcre
LICENSE=3-BSD

# All dependencies required to build the software, EzRe style.
# For APT:
BUILD_DEPENDS_APT=build-essential g++-multilib gcc-multilib mingw-w64-tools g++-mingw-w64 zip dpkg-dev rpm
# For DNF:
BUILD_DEPENDS_DNF=gcc g++ libstdc++-static.i686 glibc-static.i686 libstdc++-static.x86_64 mingw64-gcc mingw32-gcc mingw32-gcc-c++ mingw64-gcc-c++ zip dpkg-dev rpmdevtools
# For MacPorts (Note we do also have these: rpm dpkg but no cross compiler for linux via MacPorts...):
BUILD_DEPENDS_MACPORTS=mingw-w64

# REQUIRED: Host system compiler. gcc for c. g++ for C++.
COMPILER_HOST=g++
# OPTIONAL: Alternative Mac OS compiler. Otherwise set to $(COMPILER_HOST).
COMPILER_MAC=
# OPTIONAL: Alternative Mac OS compiler if LEGACY=TRUE. Otherwise set to $(COMPILER_HOST).
COMPILER_MAC_LEGACY=
# REQUIRED: Windows Cross Compiler For i686.  i686-w64-mingw32-gcc for C. i686-w64-mingw32-g++ for C++
COMPILER_WINDOWS_I686=i686-w64-mingw32-g++
# REQUIRED: Windows Cross Compiler For x86_64. x86_64-w64-mingw32-gcc for C. x86_64-w64-mingw32-g++ for C++.
COMPILER_WINDOWS_X86_64=x86_64-w64-mingw32-g++

# REQUIRED: compiler flags used to compile $(SOURCE_FILES). To make a C/C++ program portable, you probably at least want `-static` as shown below. I like using `-Wall -Wextra -Werror -pedantic -static` or some variation. We can't use `-static` on Mac OS though. COMPILER_FLAGS_MAC_LEGACY is provided for i.e. PowerPC specific flags (-arch ppc, whatever).
COMPILER_FLAGS_HOST=-Wall -Wextra -Werror -pedantic -static
COMPILER_FLAGS_MAC=-Wall -Wextra -Werror -pedantic
COMPILER_FLAGS_MAC_LEGACY=
COMPILER_FLAGS_WINDOWS_I686=
COMPILER_FLAGS_WINDOWS_X86_64=
# REQUIRED: compiler flag appended to $(COMPILER_FLAGS) to compile $(SOURCE_FILES) for Linux x86 builds. This tells GCC to build i386 code on an x86_64 system.
COMPILER_FLAGS_LINUX_I386=-m32

# OPTIONAL: LDFlags.
LDFLAGS_HOST=
LDFLAGS_MAC=
LDFLAGS_MAC_LEGACY=
LDFLAGS_WINDOWS_I686=
LDFLAGS_WINDOWS_X86_64=

# REQUIRED: Host system strip.
STRIP_HOST=strip
# OPTIONAL: alt strip for Mac OS. If not set defaults to $(STRIP)
STRIP_MAC=
# OPTIONAL: alt strip for Mac OS if LEGACY=TRUE is defined. If not set defaults to $(STRIP)
STRIP_MAC_LEGACY=
# REQUIRED: Windows i686 strip command (for building libraries with EZRE used by the target program).
STRIP_WINDOWS_I686=i686-w64-mingw32-strip
# REQUIRED: Windows x86_64 strip command (for building libraries with EZRE used by the target program).
STRIP_WINDOWS_X86_64=x86_64-w64-mingw32-strip

# REQUIRED: create builds in this directory relative to $(SOURCE_FILES). THIS DIRECTORY WILL BE DELETED WHEN EXECUTING `make clean-build` SO BE EXTREMELY CAREFUL WITH WHAT YOU SET THIS TOO.
BUILD_DIR=build

# OPTIONAL: Execute a script or set of commands before compiling each target.
SHELL_SCRIPT_HOST=
SHELL_SCRIPT_MAC=
SHELL_SCRIPT_MAC_LEGACY=
SHELL_SCRIPT_WINDOWS_I686=
SHELL_SCRIPT_WINDOWS_X86_64=
