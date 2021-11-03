# littlefs-utils

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
On Windows and macOS, when opening a physical disk the block count _must_ be specified.

*Note*: to access a physical disk on Windows, use a path of the form:
`\\.\PhysicalDrive%d`. To get a list of physical disks, invoke, for instance:
`wmic diskdrive list brief /format:list`.

## littlefs-format

Formats a storage device for littlefs.

*Note*: On Windows, only formatting of files is supported.

### Usage

```
Usage: littlefs-format -i INPUT_FILE [-l LITTLEFS_VERSION] [-b BLOCK_SIZE] [-c BLOCK_COUNT] [-r READ_SIZE] [-p PROG_SIZE]
Allowed options:
  -h [ --help ]                      produce help message
  -v [ --version ]                   show version
  -l [ --littlefs-version ] arg (=2) littlefs version to use
  -b [ --block-size ] arg (=512)     filesystem block size
  -c [ --block-count ] arg           filesystem block count
  -r [ --read-size ] arg (=64)       filesystem read size
  -p [ --prog-size ] arg (=64)       filesystem prog size
  -i [ --input-file ] arg            littlefs image file
```

The `-i` parameter expects either a file or a block device (`/dev/...`). If a file is
specified, it *must* already exist and be of the correct size.

If a block count is not specified, the application attemps to infer it from
the input file's size. On *nix systems this works even for block devices.
On macOS, when opening a physical disk the block count _must_ be specified.
