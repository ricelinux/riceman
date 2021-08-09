# Riceman

The official rice manager for Rice Linux

## Usage

```
OPERATIONS
    -S, --sync
        Synchronize rices. 
    
    -R, --remove
        Remove rices.
    
    -Q, --query
        Query the rices database.

    -V, --version
        Display the current version.
    
    -h, --help
        Shows this help dialog. Can be paired with other operations.

SYNC OPTIONS
    -y, --refresh
        Download a fresh copy of the rices database.
    
    -u, --upgrade
        Upgrades all rices that are out-of-date.

```

> Note: This text above is only temporary

## Window Managers

These window managers are split into a few different groups. The main distinction amongst them is whether or not they allow a config file param. I'm really not to sure why they wouldn't all allow some kind of config file/dir specification, but there needs to be a different solution for those that don't.

### Supported, tested

- BSPWM

### Supported, untested 
- DWM (supportable, not yet implemented)
- Awesome
- Sway
- River
- i3 (and all of it's variants)
- Fluxbox

### Unsupported

- Xmonad
- Openbox
- Qtile
> Note: As of right now [qtile's docs](http://docs.qtile.org/en/latest/manual/config/index.html) say that a `-c` argument can be given for the configuration file, however, when used, it doesn't work

### Thank you to...

- the pacman devs