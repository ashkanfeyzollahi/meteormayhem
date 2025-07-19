# Meteor Mayhem

![screenshot](screenshot.png)

<p align="center">🚀 Fly through space, while dodging and destroying meteors and trying to gain score!</p>
<p align="center">
    <a href="#features">Features</a> |
    <a href="#compiling">Compiling</a> |
    <a href="#acknowledgements">Acknowledgements</a>
</p>

```
Usage: meteormayhem [OPTION...]

 User Interface Options:
  -n, --nerdfont             Prefer using NerdFont with utf-8 encoding
  -s, --statistics[=VISIBLITY]   Change player statistics' VISIBLITY

 Information Options:
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to ashkanfeyzollahi@gmail.com.
```

## Features

- [x] **Terminal Colors**: Use terminal colors if supported.
- [x] **NerdFont Support**: Support *NerdFont* characters through *utf-8* encoding.
- [x] **Firing Mechanism**: Allow players to fire rays when <kbd>e</kbd> pressed.
- [ ] **TODO: Prevent Memory Leaks And Game Crashes**: When window resizes, it sometimes may cause game crashes.
- [ ] **TODO: Add Difficulties/Do Speed Adjustments**

## Compiling

Compiling the program is as easy as stealing a candy from a baby.

1. Clone this repository

```bash
git clone https://github.com/ashkanfeyzollahi/meteormayhem
```

2. Change directory to `meteormayhem` (obviously where it is been cloned to)

```bash
cd meteormayhem
```

3. Do `make`

```bash
make
```

## Acknowledgements

Thanks to these resources which taught me a lot:

- [NCURSES Programming HOWTO (book)](https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/)
- [Step-By-Step Into Argp (book)](http://nongnu.askapache.com/argpbook/step-by-step-into-argp.pdf)

