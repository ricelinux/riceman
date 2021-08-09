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

## Goals

The goal of riceman is to make easy the installation of window manager configurations while allowing for multiple configurations of one window manager. In other words, it does not limit the user in the number of configurations per window manager. 

Part of riceman's design philosophy is the idea that a user's home directory should not be modified, and the configurations for various software should be all sourced from one location on the device as opposed to many locations. As an example, lets say you want to install a rice of bspwm which configures bspwm, sxhkd, alacritty, polybar, various themes (icon, GTK, cursor, etc), rofi, and picom. This would be slightly annoying if you didn't configure those programs already, but if you did you would have to archive those configs, change your GTK theme, etc. After all of that you have your config, but now you have to do it again once you want to configure bspwm again, or use a different window manager that uses the same programs. The point is, its a pain to install a clean rice.

## Challenges

Some window managers, as you will see below, make it hard, if not impossible in the case of Xmonad, to not modify the home directory. There are a few ways to go at this issue. Riceman could implement a pre-start script in which the config directory would be unlinked and then linked again before the window manager starts. This entire process could also be a built in feature into how riceman generates the .desktop file.

## Window Managers

These window managers are split into a few different groups. The main distinction amongst them is whether or not they allow a config file param. I'm really not to sure why they wouldn't all allow some kind of config file/dir specification, but there needs to be a different solution for those that don't.

### Supported, tested

- bspwm

### Supported, untested 

- dwm (supportable, not yet implemented)
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

## Desktop Environments

DEs are a tricky business. I'm honestly not sure how all the configs are stored for Gnome, KDE, xfce, mate, etc., and although xfce and mate are admittedly minimal, as far as I can see there isn't a good way of handling multiple configuration installations without a hell of problems.

### Thank you to...

- the pacman devs