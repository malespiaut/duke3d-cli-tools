# [![duke3d-cli-tools](https://raw.githubusercontent.com/malespiaut/duke3d-cli-tools/master/.media/logo.svg?sanitize=true)](https://github.com/malespiaut/duke3d-cli-tools)

**duke3d-cli-tools** is a set of command line tools for creating Duke Nukem 3D content.

## Tools
- `grp`: Add files to a GRP file.
- `ungrp`: Extract files from a GRP file.
- `rts`: Create an RTS file from VOC files.
- `unrts`: Extract sounds from a RTS file.
- `dmoinfo`: Display informations about a DMO file.
- `mapinfo`: Display informations about a MAP file
- `vocinfo`: Display informations about a VOC file.
- `art2img`: Extract frames from an ART file.
- `img2art`: Create an ART file from image files.
- `voc2wav`: Convert VOC to WAV.
- `wav2voc`: Convert WAV to VOC.
- `map2svg`: Draw an SVG file from a MAP file.

## Releases
Prebuilt binaries for every tagged version are on the
[releases page](https://github.com/malespiaut/duke3d-cli-tools/releases):

| Archive | Target |
| --- | --- |
| `linux-x86_64.tar.gz` | Linux 64-bit, statically linked |
| `linux-i686.tar.gz` | Linux 32-bit, statically linked |
| `windows-x86_64.zip` | Windows 7+ 64-bit |
| `windows-i686.zip` | Windows 7+ 32-bit |
| `macos-universal.tar.gz` | macOS 10.13+ (Intel) / 11.0+ (Apple Silicon) |

Check the download against `SHA256SUMS.txt` with `sha256sum -c`.

The macOS and Windows binaries are unsigned.
On macOS, clear the quarantine flag before the first run:

```sh
xattr -d com.apple.quarantine grp mapinfo vocinfo
```

## Building
All `.c` files are meant to be independent executables.

```sh
gcc grp.c -o grp
```

## Contributing
Have a bug fix or a new feature you'd like to see in duke3d-cli-tools?
Send it our way!
Please make sure you create an issue that addresses your fix/feature so we can discuss the contribution.

1. Create an [issue](https://github.com/malespiaut/duke3d-cli-tools/issues/new).
2. Fork this repo!
3. Create your feature branch:
```sh
git checkout -b features/add-cool-new-tool
```
4. Commit your changes:
```sh
git commit -m 'Adding must have new tool!'
```
5. Push the branch:
```sh
git push origin features/add-cool-new-tool
```
6. Submit a pull request.

## License
This code is under the MIT license.
See [LICENSE](./LICENSE) for the full text.
