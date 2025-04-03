# c+text
A simple GUI text editor, made in C++

#### Features
- Vim motions
- Easy configuration

and more to come!

# Installation

> [!NOTE]
> This program requires SDL3, SDL3_ttf, and fontconfig to be installed on your system

### Building from source

```bash
# Clone the repository
$ git clone https://github.com/RQuarx/c-text/
$ cd c-text

# Build
$ CC=clang CXX=clang++ meson setup build
$ meson compile -C target

# Running
$ ./build/c+text -h
```
## Links

- [**SDL3**](https://wiki.libsdl.org/)
- [**Font Config**](https://www.freedesktop.org/wiki/Software/fontconfig/)


## License

This program is licensed under the GNU General Public License V3