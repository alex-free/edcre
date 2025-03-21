# [EDCRE](readme.md) -> Test Suite

EDCRE source now comes with a test suite. You will need the following files to use this:

* `Parasite Eve (USA) (Disc 1).bin`
* `Ridge Racer (USA) (Track 01).bin`

To run the test, execute `./test <path to Parasite Eve (USA) (Disc 1).bin> <path to Ridge Racer (USA) (Track 01).bin>`

What is tested:

1) Verification that both input files are Redump format with matching correct checksums.
2) Verification that after `edcre` is ran `Ridge Racer (USA) (Track 01).bin` is modified to the correct checksum (this game shipped on real CD-ROM with invalid EDC/ECC).
3) Verification that after `edcre` is ran `Parasite Eve (USA) (Disc 1).bin` is not modified and still has the same correct checksum (this game shipped on real CD-ROM with valid EDC/ECC in all sectors).