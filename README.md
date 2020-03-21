# littlefs-utils

![Travis CI latest](https://img.shields.io/travis/com/mbikovitsky/littlefs-utils?label=Travis%20CI%20latest)
![AppVeyor latest](https://img.shields.io/appveyor/build/mbikovitsky/littlefs-utils?label=AppVeyor%20latest)

![Travis CI master](https://img.shields.io/travis/com/mbikovitsky/littlefs-utils/master?label=Travis%20CI%20master)
![AppVeyor master](https://img.shields.io/appveyor/build/mbikovitsky/littlefs-utils/master?label=AppVeyor%20master)

Various utilities for [littlefs](https://github.com/ARMmbed/littlefs).


## littlefs-extract

Extracts the contents of a littlefs image to a tar archive.

### Usage

```
littlefs-extract -i INPUT_FILE [-l LITTLEFS_VERSION] [-b BLOCK_SIZE] [-c BLOCK_COUNT] [-r READ_SIZE] [-p PROG_SIZE] [-o OUTPUT_FILE]
Allowed options:
  -h [ --help ]                      produce help message
  -v [ --version ]                   show version
  -l [ --littlefs-version ] arg (=2) littlefs version to use
  -b [ --block-size ] arg (=512)     filesystem block size
  -c [ --block-count ] arg           filesystem block count
  -r [ --read-size ] arg (=64)       filesystem read size
  -p [ --prog-size ] arg (=64)       filesystem prog size
  -i [ --input-file ] arg            littlefs image file
  -o [ --output-file ] arg (=-)      output tar file
```

If a block count is not specified, the application attemps to infer it from
the input file's size. On *nix systems this works even for block devices.
On Windows, when opening a physical disk the block count *must* be specified.

Note: to access a physical disk on Windows, use a path of the form:
`\\.\PhysicalDrive%d`. To get a list of physical disks, invoke, for instance:
`wmic diskdrive list brief /format:list`.
