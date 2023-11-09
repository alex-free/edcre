# Changelog

## Version 1.0.5 (10/26/2023)

*	[edcre-v1.0.5-windows\_x86](https://github.com/alex-free/edcre/releases/download/v1.0.5/edcre-v1.0.5-windows-x86.zip) _For Windows 95 OSR 2.5 Or Newer (32-bit Windows)_
*	[edcre-v1.0.5-windows-x86\_64](https://github.com/alex-free/edcre/releases/download/v1.0.5/edcre-v1.0.5-windows-x86_64.zip) _For 64-bit Windows_
*	[edcre-v1.0.5-linux-x86](https://github.com/alex-free/edcre/releases/download/v1.0.5/edcre-v1.0.5-linux-x86_static.zip) _For x86 Linux Distros_
*	[edcre-v1.0.5-linux-x86\_64](https://github.com/alex-free/edcre/releases/download/v1.0.5/edcre-v1.0.5-linux-x86_64_static.zip) _For x86_64 Linux Distros_
*	[edcre-v1.0.5-source](https://github.com/alex-free/edcre/archive/refs/tags/v1.0.5.zip)

Changes:

*  Removed the `-z` argument. EDCRE now starts EDC/ECC regeneration at sector zero by default, this can be modified for any sector number starting by using the new `-s` argument followed by a sector number, i.e. `-s 16`.

*   Significantly improved argument handling and cleaned up code. 

## Version 1.0.4 (9/7/2023)

*	[edcre-1.0.4-windows\_x86](https://github.com/alex-free/edcre/releases/download/v1.0.4/edcre-1.0.4-windows_x86.zip) _For Windows 95 OSR 2.5 Or Newer (32-bit Windows)_
*	[edcre-1.0.4-windows\_x86\_64](https://github.com/alex-free/edcre/releases/download/v1.0.4/edcre-1.0.4-windows_x86_64.zip) _For 64-bit Windows_
*	[edcre-1.0.4-linux\_x86](https://github.com/alex-free/edcre/releases/download/v1.0.4/edcre-1.0.4-linux_x86_static.zip) _For x86 Linux Distros_
*	[edcre-1.0.4-linux\_x86\_64](https://github.com/alex-free/edcre/releases/download/v1.0.4/edcre-1.0.4-linux_x86_64_static.zip) _For x86_64 Linux Distros_
*	[edcre-1.0.4-source](https://github.com/alex-free/edcre/archive/refs/tags/v1.0.4.zip)

Changes:

*   Patches [Binmerged](https://github.com/putnam/binmerge) games (which have the data track and all audio tracks as one bin file) faster.

## Version 1.0.3 (9/3/2023)

*	[edcre-1.0.3-windows\_x86](https://github.com/alex-free/edcre/releases/download/v1.0.3/edcre-1.0.3-windows_x86.zip) _For Windows 95 OSR 2.5 Or Newer (32-bit Windows)_
*	[edcre-1.0.3-windows\_x86\_64](https://github.com/alex-free/edcre/releases/download/v1.0.3/edcre-1.0.3-windows_x86_64.zip) _For 64-bit Windows_
*	[edcre-1.0.3-linux\_x86](https://github.com/alex-free/edcre/releases/download/v1.0.3/edcre-1.0.3-linux_x86_static.zip) _For x86 Linux Distros_
*	[edcre-1.0.3-linux\_x86\_64](https://github.com/alex-free/edcre/releases/download/v1.0.3/edcre-1.0.3-linux_x86_64_static.zip) _For x86_64 Linux Distros_
*	[edcre-1.0.3-source](https://github.com/alex-free/edcre/archive/refs/tags/v1.0.3.zip)

Changes:

*   Added -t argument, which only reads the file for any sectors with invalid EDC/ECC data. Can be combined with the -v argument ot display each sector with invalid EDC/ECC. 

*   [Binmerged](https://github.com/putnam/binmerge) games (which have the data track and all audio tracks as one bin file) now work with EDCRE correctly.

## Version 1.0.2 (8/21/2023)

*	[edcre-1.0.2-windows\_x86](https://github.com/alex-free/edcre/releases/download/v1.0.2/edcre-1.0.2-windows_x86.zip) _For Windows 95 OSR 2.5 Or Newer (32-bit Windows)_
*	[edcre-1.0.2-windows\_x86\_64](https://github.com/alex-free/edcre/releases/download/v1.0.2/edcre-1.0.2-windows_x86_64.zip) _For 64-bit Windows_
*	[edcre-1.0.2-linux\_x86](https://github.com/alex-free/edcre/releases/download/v1.0.2/edcre-1.0.2-linux_x86_static.zip) _For x86 Linux Distros_
*	[edcre-1.0.2-linux\_x86\_64](https://github.com/alex-free/edcre/releases/download/v1.0.2/edcre-1.0.2-linux_x86_64_static.zip) _For x86_64 Linux Distros_
*	[edcre-1.0.2-source](https://github.com/alex-free/edcre/archive/refs/tags/v1.0.2.zip)

Changes:

*   Changed default behavior to start regen at sector 15 (to enable using this to update EDC after TOCPerfect patching).


## Version 1.0.1 (7/29/2023)

*	[edcre-1.0.1-windows\_x86](https://github.com/alex-free/edcre/releases/download/v1.0.1/edcre-1.0.1-windows_x86.zip) _For Windows 95 OSR 2.5 Or Newer (32-bit Windows)_
*	[edcre-1.0.1-windows\_x86\_64](https://github.com/alex-free/edcre/releases/download/v1.0.1/edcre-1.0.1-windows_x86_64.zip) _For 64-bit Windows_
*	[edcre-1.0.1-linux\_x86](https://github.com/alex-free/edcre/releases/download/v1.0.1/edcre-1.0.1-linux_x86_static.zip) _For x86 Linux Distros_
*	[edcre-1.0.1-linux\_x86\_64](https://github.com/alex-free/edcre/releases/download/v1.0.1/edcre-1.0.1-linux_x86_64_static.zip) _For x86_64 Linux Distros_
*	[edcre-1.0.1-source](https://github.com/alex-free/edcre/archive/refs/tags/v1.0.1.zip)

Changes:

*   Prettier output and usage instructions.
*   Improved documentation.

## Version 1.0 (7/25/2023)

*	[edcre-1.0-windows\_x86](https://github.com/alex-free/edcre/releases/download/v1.0/edcre-1.0-windows_x86.zip) _For Windows 95 OSR 2.5 Or Newer (32-bit Windows)_
*	[edcre-1.0-windows\_x86\_64](https://github.com/alex-free/edcre/releases/download/v1.0/edcre-1.0-windows_x86_64.zip) _For 64-bit Windows_
*	[edcre-1.0-linux\_x86](https://github.com/alex-free/edcre/releases/download/v1.0/edcre-1.0-linux_x86_static.zip) _For x86 Linux Distros_
*	[edcre-1.0-linux\_x86\_64](https://github.com/alex-free/edcre/releases/download/v1.0/edcre-1.0-linux_x86_64_static.zip) _For x86_64 Linux Distros_
*	[edcre-1.0-source](https://github.com/alex-free/edcre/archive/refs/tags/v1.0.zip)