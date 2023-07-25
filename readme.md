# [alex-free.github.io](https://alex-free.github.io)

# EDCRE: EDC/EEC Regenerator For BIN+CUE CD Disc Images

EDCRE provides a solution to update EDC/EEC data to match any edits/patching done to a data track bin file of a CD image. It is meant to allow successful patching/editing of PSX games with [EDC Protection](#edc-protected-psx-games) with programs such as [PS1 DemoSwap Patcher](https://alex-free.github.io/ps1demoswap#tocperfect-patch) and [APrip](https://alex-free.github.io/aprip/#patching-the-cd-image) using a clever [workaround](#psx-edc-protection-workaround-with-edcre) turned on by default. 

EDCRE is not limited to EDC Protected PSX games however, just specify the `-z` argument to enforce all sectors to be checked and fixed with updated EDC/EEC data if needed.

## Table of Contents

* [Downloads](#downloads)
* [EDC/EEC Data](#edceec-data)
* [PSX EDC Anti-Piracy Protection](#psx-edc-anti-piracy-protection)
* [PSX EDC Protection Workaround With EDCRE](#psx-edc-protection-workaround-with-edcre)
* [Burning EDC Protected PSX Games Correctly](#burning-edc-protected-psx-games-correctly)
* [EDC Protected PSX Games](#edc-protected-psx-games)
* [Usage](#usage)
* [License](#license)
* [Credits](#credits)

## Links

*	[Homepage](https://alex-free.github.io/edcre)
*	[Github](https://github.com/alex-free/edcre)
*   [CDRDAO-PLED](https://alex-free.github.io/cdrdao)
*   [APrip](https://alex-free.github.io/aprip)
*   [PS1 DemoSwap Patcher](https://alex-free.github.io/ps1demoswap)
*	[Tonyhax International](https://alex-free.github.io/tonyhax-international)
*	[Tonyhax International APv2 Bypass System](https://alex-free.github.io/tonyhax-international/anti-piracy-bypass.html)


## Downloads

### Version 1.0 (7/25/2023)

*	[edcre-1.0-windows\_x86](https://github.com/alex-free/edcre/releases/download/v1.0/edcre-1.0-windows_x86.zip) _For Windows 95 OSR 2.5 Or Newer (32-bit Windows)_
*	[edcre-1.0-windows\_x86\_64](https://github.com/alex-free/edcre/releases/download/v1.0/edcre-1.0-windows_x86_64.zip) _For 64-bit Windows_
*	[edcre-1.0-linux\_x86](https://github.com/alex-free/edcre/releases/download/v1.0/edcre-1.0-linux_x86_static.zip) _For x86 Linux Distros_
*	[edcre-1.0-linux\_x86\_64](https://github.com/alex-free/edcre/releases/download/v1.0/edcre-1.0-linux_x86_64_static.zip) _For x86_64 Linux Distros_
*	[edcre-1.0-source](https://github.com/alex-free/edcre/archive/refs/tags/v1.0.zip)

## EDC/EEC Data

EDCRE updates EDC (Error Detection Code) and EEC data (Error Correction Code) in a given data track bin file.

EDC/EEC provide a way to 'correct' read errors caused by i.e. slightly scratched discs. The EDC is a special checksum that verifies that the contents of the user data portion of a sector in a CD disc image data track are correct and expected. If during a sector read this is not found to be the case the EEC data provides a way to correct the data to what was expected (unless a signifigant amount of the sector is unreadable).

When you edit a data track bin file of a CDimage (the user data of a sector), the EDC and EEC data needs to be updated to work with the changes made to the file. Because the old EDC and EEC data where based on the original file and they now mismatch the new contents of the user data in a sector, the changes won't take proper effect, with 2 exceptions:

*   If you use an emulator, as most ignore the EDC/EEC data in sectors by design.

*   If you use a CD burning software that writes updated EDC/EEC on the fly while burning to the disc.

These exceptions cover a lot of cases. Most CD burning software always writes updated EDC/EEC data to burned discs:

*   [IMGBurn](https://www.imgburn.com/) always writes updated EDC/EEC data, and there isn't a way to disable that behavior.

*   [CDRDAO](https://github.com/cdrdao/cdrdao) always writes updated EDC/EEC data when using the default `generic-mmc` driver. It is possible however specify the `generic-mmc-raw` be used instead which **does not modify EDC/EEC data and leaves it as is**.

*   [CloneCD](https://www.redfox.bz/en/clonecd.html) always writes updated EDC/EEC data **unless you use the RAW writing mode**.

Writing updated EDC data to disc is usually what you want, that way the correct matching EDC/EEC data correlates to the edits of the disc image. But what if you want to edit a data track in a disc image and then write it raw? That is exactly what I want to do, as it defeats the EDC-based anti-piracy protection measure found in almost all of the [Dance Dance Revolution PSX games](#edc-protected-psx-games).

## PSX EDC Anti-Piracy Protection

The idea of EDC/EEC based additional anti-piracy protection is a brilliantly flawed one. See, Sony's tools to generate disc images back in the day were [buggy](http://www.psxdev.net/forum/viewtopic.php?t=1475). One such bug appears to be that the [reserved sectors 12-15](http://problemkaputt.de/psx-spx.htm#cdromisovolumedescriptors), which are zero filled in the user data portion of the sector, _also_ **have an EDC checksum of zero**. The correct checksum for a zero-filled user data sector _should be_ `3F 13 B0 BE`, _but it isn't_. It's `00 00 00 00` like the rest of the sector besides the sync data. This actually doesn't matter in practice, so the bug went unoticed and the technically invalid sector 12-15s shipped on real licensed PSX CD-ROMs. This apparently got fixed eventually in some newer version of the `cdgen` Sony tool that created disc images.

Someone working on the Dance Dance Revolution PSX games noticed this strange behavior and figured out that it could be exploited as an additional anti-piracy protection measure. If the real licensed PSX CD-ROM discs were shipped with an EDC checksum of zero in sector 12-15, then when someone went to rip the real licensed PSX CD-ROM disc and then burn it back to a CD-R, the EDC checksum in sector 12-15 would no longer be `00 00 00 00`, it would be the expected `3F 13 B0 BE`. [Game code](https://github.com/socram8888/tonyhax/issues/121#issuecomment-1341381549) can read the EDC checksum on the disc at sector 12, and a routine could then lock up the game if the EDC data is non-zero to deter piracy.

## PSX EDC Protection Workaround With EDCRE

EDCRE has a simple solution to allow edited/patched PSX disc images that have EDC Protection to work on real PSX hardware. By default (unless you specify the `-z` argument to specify updating all sectors in a disc image, starting at sector 0) It regenerates all sectors in a disc image starting at the 16th sector (LBA 15 in disc image/165 on disc) instead of starting at the first sector (LBA 0 in disc image/150 on disc). Because the 'reserved' zero-filled sectors 12-15 are untouched by EDCRE, the EDC protection never triggers in-game if you burn the disc image RAW (without enforcing/modifying EDC/EEC data written to disc). At the same time any edits/patches made to a PSX disc image will have matching EDC data allowing the changes made to take effect and actually work properly on real hardware.

## Burning EDC Protected PSX Games Correctly

I recommend using the latest CDRDAO v1.2.5 which unlike previous versions supports burning EDC Protected PSX games with CD audio tracks correctly using the `generic-mmc-raw` driver. There are pre-built portable releases of a new enough CDRDAO for Linux [available](https://alex-free.github.io/cdrdao).

If you modified the disc image in any way (such as using the [PS1 DemoSwap Patcher's](https://alex-free.github.io/ps1demoswap) [TOCPerfect Patch](https://alex-free.github.io/ps1demoswap#tocperfect-patch) or the [APrip](https://alex-free.github.io/aprip) [CD Image Patching Option](https://alex-free.github.io/aprip#patching-the-cd-image) **you must next run `edcre` on the first data track bin file of the disc image before burning with the CDRDAO RAW driver** as described below.

`cdrdao write --speed 1 --driver generic-mmc-raw --swap -n --eject yourgame.cue`

![TOCPerfect Patching And EDCRE Patching Dance Dance Revolution 2nd Remix Japan](images/ddr2j-tp.png)

![Burning Dance Dance Revolution 2nd Remix Japan](images/ddr2j-burning.png)

### CDRDAO Command Explanation

The `--speed 1` argument sets the writing speed to the slowest your CD burner supports.

The `--driver generic-mmc-raw` arguments specifies CDRDAO to use the `generic-mmc-raw` driver, which burns the CD image exactly as it is. The default driver used without specifiying these arguments is the **`generic-mmc` driver, which like the other drivers in CDRDAO will auto-regenerate EDC data as the CD image is burned.** This can change the EDC data read from the burned disc later, which some PSX games use as an additional anti-piracy check which if failed will lock up [the game](https://alex-free.github.io/tonyhax-international/anti-piracy-bypass.html#games-with-edc-protection).

The `--swap` argument is neccesary if the BIN/CUE CD image contains CD audio. Without it, you will get loud static when the CD audio tracks are played as they are by default byte-swapped by CDRDAO if this argument is not specified.

The `-n` argument disables the 10 second waiting period before burning.

The `--eject` argument will automatically eject the disc immediately after a successful burn.


## EDC Protected PSX Games

Every Dance Dance Revolution PSX game has EDC protection with the exception of the game [Dance Dance Revolution: Disney's Rave](http://redump.org/disc/37012/) / [Dance Dance Revolution: Disney Mix](http://redump.org/disc/13669/).

 Almost every Dance Dance Revolution PSX game containing EDC protection also have either [APv2](https://alex-free.github.io/tonyhax-international/anti-piracy-bypass.html#apv2) or [APv1](https://alex-free.github.io/tonyhax-international/anti-piracy-bypass.html#apv1) based anti-piracy detection code. While [Tonyhax International](https://alex-free.github.io/tonyhax-international) has [Anti-Piracy Screen Bypasses](https://alex-free.github.io/tonyhax-international/anti-piracy-bypass.html#apv2-bypasses) implemented to ensure these games run correctly on stock consoles (only APv2 can trigger Tonyhax International if an APv2 bypass is not implemented), **if your console has a non-stealth modchip** you'll need to [patch the first data track bin file](https://alex-free.github.io/aprip/#patching-the-cd-image) with [APrip](https://alex-free.github.io/aprip) first and then use EDCRE, or use [GameShark Codes](https://alex-free.github.io/aprip/#generating-gameshark-codes) generated by APrip to get these games working (if disc image patching is not possible with APrip, see below for per-game details). Please note that [Tonyhax International](https://alex-free.github.io/tonyhax-international) does have the ability to apply user supplied [GameShark Codes](https://alex-free.github.io/tonyhax-international/gameshark-code-support.html).

### Dance Dance Revolution

- Versions Tested: [Japan](http://redump.org/disc/1562/), [USA](http://redump.org/disc/16075/).
- Versions With EDC Protection: Japan, USA.
- When Is The EDC Protection: First `NOW LOADING` blinking text screen after selecting the first default option in the main  menu at start.
- Versions With Anti-Piracy Screen ([APv1](https://alex-free.github.io/tonyhax-international/anti-piracy-bypass.html#apv1)): Japan.
- When Is The Anti-Piracy Screen Check: Immeditely.

Tonyhax International Anti-Piracy Screen Bypass Support For Stock Consoles? - Not needed ([APv1](https://alex-free.github.io/tonyhax-international/anti-piracy-bypass.html#apv1)).

[APrip](https://alex-free.github.io/aprip) compatibility: [CD Image Patching](https://alex-free.github.io/aprip/#patching-the-cd-image) - Failure (no match), [GameShark Codes](https://alex-free.github.io/aprip/#generating-gameshark-codes) - Success:

`D01B9B04 0119`
`801B9B04 0000`
`D01B9B06 0304`
`801B9B06 0000`
`D01B9B10 0119`
`801B9B10 0000`
`D01B9B12 0302`
`801B9B12 0000`

### Dance Dance Revolution: Best Hits

- Versions Tested: [Japan](http://redump.org/disc/30601/).
- When Is The EDC Protection: First `NOW LOADING` blinking text screen after selecting the first default option in the main  menu at start.
- Versions With Anti-Piracy Screen ([APv2](https://alex-free.github.io/tonyhax-international/anti-piracy-bypass.html#apv2)): Japan.
- When Is The Anti-Piracy Screen Check: Immeditely.

Tonyhax International Anti-Piracy Screen Bypass Support For Stock Consoles? - [Yes, Implemented](https://alex-free.github.io/tonyhax-international/anti-piracy-bypass.html#dance-dance-revolution-best-hits).

[APrip](https://alex-free.github.io/aprip) compatibility: [CD Image Patching](https://alex-free.github.io/aprip/#patching-the-cd-image) - Success, [GameShark Codes](https://alex-free.github.io/aprip/#generating-gameshark-codes) - Success:

`D0102F94 0119`
`80102F94 0109`
`D0102F98 0119`
`80102F98 0103`
`D0102FA0 001E`
`80102FA0 0000`

### Dance Dance Revolution Konamix

- Versions Tested: [USA](http://redump.org/disc/1238/).
- When Is The EDC Protection: Immedietly there is a `NOW LOADING` blinking text screen and the CD drive goes crazy.
- Versions With Anti-Piracy Screen: None

### Dance Dance Revolution Extra Mix

- Versions Tested: [Japan](http://redump.org/disc/44438/).
- Versions With EDC Protection: Japan.
- When Is The EDC Protection: First `NOW LOADING` blinking text screen after selecting the first option in the  start menu.
- Versions With Anti-Piracy Screen ([APv2](https://alex-free.github.io/tonyhax-international/anti-piracy-bypass.html#apv2)): Japan.
- When Is The Anti-Piracy Screen Check: Immeditely.
- Versions With Anti-Piracy Bypass Support: Japan.

[APrip](https://alex-free.github.io/aprip) compatibility: [CD Image Patching](https://alex-free.github.io/aprip/#patching-the-cd-image) - Success. [GameShark Codes](https://alex-free.github.io/aprip/#generating-gameshark-codes) - Success:

`D00EB358 0119`
`800EB358 0109`
`D00EB35C 0119`
`800EB35C 0103`
`D00EB364 001E`
`800EB364 0000`

### Dance Dance Revolution 2nd Remix

- Versions Tested: [Japan](http://redump.org/disc/9477/).
- Versions With EDC Protection: Japan.
- When Is The EDC Protection: Immedietly at first screen with text.
- Versions With Anti-Piracy Screen ([APv2](https://alex-free.github.io/tonyhax-international/anti-piracy-bypass.html#apv2)): Japan. _Note:_ this game also contains (disabled) [APv1](https://alex-free.github.io/tonyhax-international/anti-piracy-bypass.html#apv1) code.
- When Is The Anti-Piracy Screen Check: Immeditely.
- Versions With Anti-Piracy Bypass Support: Japan.

[APrip](https://alex-free.github.io/aprip) compatibility: [CD Image Patching](https://alex-free.github.io/aprip/#patching-the-cd-image) - Success. [GameShark Codes](https://alex-free.github.io/aprip/#generating-gameshark-codes) - Success:

`D01C6738 0119`
`801C6738 0109`
`D01C673C 0119`
`801C673C 0103`
`D01C6744 001E`
`801C6744 0000`

### Dance Dance Revolution 3rd Mix

- Versions Tested: [Japan](http://redump.org/disc/9536/).
- Versions With EDC Protection: Japan.
- When Is The EDC Protection: First `NOW LOADING` blinking text screen after selecting the first option in the  start menu.
- Versions With Anti-Piracy Screen ([APv2](https://alex-free.github.io/tonyhax-international/anti-piracy-bypass.html#apv2)): Japan.
- When Is The Anti-Piracy Screen Check: Immeditely.
- Versions With Anti-Piracy Bypass Support: Japan.

[APrip](https://alex-free.github.io/aprip) compatibility: [CD Image Patching](https://alex-free.github.io/aprip/#patching-the-cd-image) - Success. [GameShark Codes](https://alex-free.github.io/aprip/#generating-gameshark-codes) - Success:

`D00C4254 0119`
`800C4254 0109`
`D00C4258 0119`
`800C4258 0103`
`D00C4260 001E`
`800C4260 0000`

### Dance Dance Revolution 4th Mix

- Versions Tested: [Japan](http://redump.org/disc/34157/).
- Versions With EDC Protection: Japan.
- When Is The EDC Protection: First `NOW LOADING` blinking text screen after selecting the `TRAINING` option in the  start menu.
- Versions With Anti-Piracy Screen ([APv2](https://alex-free.github.io/tonyhax-international/anti-piracy-bypass.html#apv2)): Japan.
- When Is The Anti-Piracy Screen Check: Immeditely.
- Versions With Anti-Piracy Bypass Support: Japan.

[APrip](https://alex-free.github.io/aprip) compatibility: [CD Image Patching](https://alex-free.github.io/aprip/#patching-the-cd-image) - Success. [GameShark Codes](https://alex-free.github.io/aprip/#generating-gameshark-codes) - Success:

`D00EB3D8 0119`
`800EB3D8 0109`
`D00EB3DC 0119`
`800EB3DC 0103`
`D00EB3E4 001E`
`800EB3E4 0000`

### Dance Dance Revolution 5th Mix

- Versions Tested: [Japan](http://redump.org/disc/34157/).
- Versions With EDC Protection: Japan.
- When Is The EDC Protection: First `NOW LOADING` blinking text screen after selecting the `TRAINING` option in the  start menu.
- Versions With Anti-Piracy Screen ([APv2](https://alex-free.github.io/tonyhax-international/anti-piracy-bypass.html#apv2)): Japan.
- When Is The Anti-Piracy Screen Check: Immeditely.
- Versions With Anti-Piracy Bypass Support: Japan.

[APrip](https://alex-free.github.io/aprip) compatibility: [CD Image Patching](https://alex-free.github.io/aprip/#patching-the-cd-image) - Success. [GameShark Codes](https://alex-free.github.io/aprip/#generating-gameshark-codes) - Success:

`D0177134 0119`
`80177134 0109`
`D0177138 0119`
`80177138 0103`
`D0177140 001E`
`80177140 0000`

## Usage

EDCRE is a command line program. On Windows and most Linux distributions, you can simply drag and drop the "track 01.bin" file of the PSX game you want to update EDC/EEC data for.

If you want to see more verbose info, and or if you want to update EDC/EEC data for all sectors (what you probably want if the data track bin file is not an EDC Protected PSX game but rather something else), you need to execute `edcre` with command line options:

`edcre <original data track>`
`edcre -v <original data track>    (display verbose info)`
`edcre -x <original data track>    (disc image is not a PSX EDC game, start at sector 0 when correcting EDC/EEC data)`
`edcre -x -v <original data track>    (disc image is not a PSX EDC game, start at sector 0 when correcting EDC/EEC data AND display verbose info)`

### Windows

*   Start cmd.exe and provide the executable file.
*   Provide any additional arguments (optional) (`-z` and or `-v`).
*   Provide the disc image data track bin file as the last argument and execute the command, such as:
    `edcre.exe -v "track 01.bin"`

### Linux CLI

*   Start Terminal and provide the executable file.
*   Provide any additional arguments (optional) (`-z` and or `-v`).
*   Provide the disc image data track bin file as the last argument and execute the command, such as:
    `./edcre" -v "track 01.bin"`

## Licenese

EDCRE is modified [CDRDAO](https://github.com/cdrdao/cdrdao) source code, which is licensed under the GPLv2 license. Please see the file `license.txt` in each release for full info.

## Credits

*   [CDRDAO](https://github.com/cdrdao/cdrdao) source code.
*   [Socram8888](https://github.com/socram8888) for providing info on [how the game code detects the EDC checksum](https://github.com/socram8888/tonyhax/issues/121#issuecomment-1341365357).
*   [MottZilla](https://github.com/mottzilla) for coming up with the workaround idea: "Just don't update those sectors" lol.