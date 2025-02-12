# EzRe GNUMakefile Variables for Linux/Windows. See https://github.com/alex-free/ezre for more info.

# REQUIRED: executable name in release (.exe file extension is appended for Windows builds). I.e. hello.
PROGRAM=edcre
# REQUIRED: source files to be compiled into $(PROGRAM) target. Can use wildcard (i.e. *.c, *.cpp, etc) or specify files specifically. These files are looked for in the same directory that the EZRE `Makefile` and `variables.mk` files are in (relative).
SOURCE_FILES=edcre.cc
# REQUIRED: Basename of all release files (.zip, .deb). I.e. hello-world.
RELEASE_BASE_NAME=edcre
# REQUIRED: Version number, passed as 'VERSION' string to $(SOURCE_FILES). I.e. v1.0.
VERSION=1.0.9

# OPTIONAL: additional files included in all portable zip releases. I.e. readme.md.
RELEASE_FILES=*.md
# OPTIONAL: files included only in the Linux portable releases (.zip).
LINUX_SPECIFIC_RELEASE_FILES=
# OPTIONAL: files included only in the Windows portable releases (.zip).
WINDOWS_SPECIFIC_RELEASE_FILES=

# All dependencies required to build the software, EzRe style. These deps allow for:
# C and C++ programs.
# RPM and DEB package creation for Linux.
# Windows i686 and x86_64 builds.
# Linux i386 and x86_64 builds.
# Zip releases for all builds.
# EZRE uses the make deps rule to find either dnf and apt, and installs the below deps accordingly.
# For APT:
BUILD_DEPENDS_APT=build-essential g++-multilib gcc-multilib mingw-w64-tools g++-mingw-w64 zip dpkg-dev rpm
# For DNF:
BUILD_DEPENDS_DNF=gcc g++ libstdc++-static.i686 glibc-static.i686 libstdc++-static.x86_64 mingw64-gcc mingw32-gcc mingw32-gcc-c++ mingw64-gcc-c++ zip dpkg-dev rpmdevtools

# REQUIRED: Appended to end of release file name. Release file format is $(RELEASE_BASE_NAME)-$(VERSION)-$(RELEASE_NAME_SUFFIX).
LINUX_I386_RELEASE_NAME_SUFFIX=linux-i386-static
LINUX_X86_64_RELEASE_NAME_SUFFIX=linux-x86_64-static
WINDOWS_I686_RELEASE_NAME_SUFFIX=windows-i686-static
WINDOWS_X86_64_RELEASE_NAME_SUFFIX=windows-x86_64-static

# REQUIRED: Linux Compiler For i386 and x86_64.
LINUX_COMPILER=g++
# REQUIRED: Windows Cross Compiler For i686.
WINDOWS_I686_COMPILER=i686-w64-mingw32-g++
# REQUIRED: Windows Cross Compiler For x86_64.
WINDOWS_X86_64_COMPILER=x86_64-w64-mingw32-g++
# REQUIRED: Host system compiler.
COMPILER=$(LINUX_COMPILER)

# REQUIRED Linux AR command (for building libraries with EZRE used by the target program).
LINUX_AR=ar
# REQUIRED: Windows i686 AR command (for building libraries with EZRE used by the target program).
WINDOWS_I686_AR=i686-w64-mingw32-ar
# REQUIRED: Windows x86_64 AR command (for building libraries with EZRE used by the target program).
WINDOWS_X86_64_AR=x86_64-w64-mingw32-ar
# REQUIRED: Host system ar
AR=$(LINUX_AR)

# REQUIRED: Linux strip command.
LINUX_STRIP=strip
# REQUIRED: Windows i686 strip command (for building libraries with EZRE used by the target program).
WINDOWS_I686_STRIP=i686-w64-mingw32-strip
# REQUIRED: Windows x86_64 strip command (for building libraries with EZRE used by the target program).
WINDOWS_X86_64_STRIP=x86_64-w64-mingw32-strip
# REQUIRED: Host system strip.
STRIP=$(LINUX_STRIP)

# REQUIRED: compiler flags used to compile $(SOURCE_FILES). To make a C/C++ program portable, you probably at least want `-static` as shown below. I like using `-Wall -Wextra -Werror -pedantic -static` or some variation.
COMPILER_FLAGS=-Wall -Wextra -Werror -pedantic -static
# REQUIRED: compiler flag appended to $(COMPILER_FLAGS) to compile $(SOURCE_FILES) for Linux x86 builds. This tells GCC to build i386 code on an x86_64 system.
COMPILER_FLAGS_LINUX_I386=-m32
# OPTIONAL: You may compile a library with different CFLAGS set here. (i.e. `-Wall -Wextra -Werror -pedantic -Wno-unused-function`)
COMPILER_FLAGS_LIB=
# REQUIRED: set to `YES` to build additional libraries (must edit Makefile with relevant info). By default this is set to `NO`.
BUILD_LIB=NO
# REQUIRED: create builds in this directory relative to $(SOURCE_FILES). THIS DIRECTORY WILL BE DELETED WHEN EXECUTING `make clean-build` SO BE EXTREMELY CAREFUL WITH WHAT YOU SET THIS TOO.
BUILD_DIR=build