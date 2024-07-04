# Variables of EzRe GNUMakefile for Linux/Windows by Alex Free https://github.com/alex-free/ezre

# Basename of all release files (.zip, .deb)
RELEASE_BASE_NAME=edcre
# Version number, passed as 'VERSION' string to $(SOURCE_FILES)
VERSION=v1.0.7
# Appeneded to end of release file name
LINUX_I386_RELEASE_NAME_SUFFIX=linux-i386-static
LINUX_X86_64_RELEASE_NAME_SUFFIX=linux-x86_64-static
WINDOWS_I686_RELEASE_NAME_SUFFIX=windows-i686-static
WINDOWS_X86_64_RELEASE_NAME_SUFFIX=windows-x86_64-static
# Release file format is $(RELEASE_BASE_NAME)-$(VERSION)-$(RELEASE_NAME_SUFFIX)

# Files included in all portable releases (.zip)
RELEASE_FILES=readme.md changelog.md license.md images
# OPTIONAL: files included only in the Linux portable releases (.zip)
LINUX_SPECIFIC_RELEASE_FILES=
# OPTIONAL: files included only in the Windows portable releases (.zip)
WINDOWS_SPECIFIC_RELEASE_FILES=

# All dependencies required to build the software, to be installed when using deps-apt EzRe Makefile rule (For Debian/Ubuntu)
BUILD_DEPENDS_APT=build-essential g++-multilib gcc-multilib mingw-w64-tools g++-mingw-w64code Makefile zip dpkg-dev
# All dependencies required to build the software, to be installed when using deps-dnf EzRe Makefile rule (For Fedora/Red Hat)
BUILD_DEPENDS_DNF=gcc g++ libstdc++-static.i686 glibc-static.i686 libstdc++-static.x86_64 mingw64-gcc mingw32-gcc mingw32-gcc-c++ mingw64-gcc-c++ zip

# Executable name in release (.exe file extension is appended for Windows builds)
PROGRAM=edcre
# Source files to be compiled into $(PROGRAM) target
SOURCE_FILES=lec.cc
# Compiler flags used to compile $(SOURCE_FILES)
COMPILER_FLAGS=-Wall -Wextra -Werror -pedantic
# Compiler flag appended to $(COMPILER_FLAGS) to compile $(SOURCE_FILES) for Linux x86 builds
COMPILER_FLAGS_LINUX_X86=-m32
# Create builds in this directory relative to $(SOURCE_FILES)
BUILD_DIR=build

# Linux Compiler For i386 and x86_64
LINUX_COMPILER=gcc
# Windows Cross Compiler For i686
WINDOWS_X86_COMPILER=i686-w64-mingw32-gcc
# Windows Cross Compiler For x86_64
WINDOWS_X86_64_COMPILER=x86_64-w64-mingw32-gcc
# Host system compiler
COMPILER=$(LINUX_COMPILER)
