# Notes

## Dealing with various types of options

Create a few arrays of options, one for operations, one for operation modifiers, one for global operations, and any other operations that aren't booleans will be manually implemented.

## General flow

The main function will call another function to validate the operation arguments (only one specified) and then set `config.op`. Then, construct an operation class (sync, remove, query, etc.) which will inherit a operation super/abstract class and set the constructed class to a variable with the type as the super/abstract class (not sure if this is possible but try it anyways). Then, inside of each operation class declare a run/execute method which will take the already parsed arguments from the constructor and execute them accordingly.

 ## Dealing with the AUR

So lets say that I decide to use an AUR helper instead of building in my own AUR helper.
A) That's super ineffcicient. I have already have libgit as a dependency, it would be stupid to spawn a child process for a program, written in some other language which has bindings for libgit. I don't even need to support any special search arguments like AUR helpers do. I'm literally taking the most direct road to installing something from the AUR
B) Permissions make me sad. yay and other AUR helpers need to drop permissions in order to run makepkg, which is a problem because makepkg calls sudo to install so we'd be asking the user for their password twice which is dumb. If I were to do it manually without an AUR helper, not only could I drop privaleges in the forked process for compiling the package, but I can then continue on with the original root permissions when running `pacman -U {PACKAGE.zst}`
C) The AUR isn't even that hard to nevegate. The URL is just https://aur.archlinux.org/{PACKAGENAME}.git THAT'S IT. I didn't need to look that up, its that easy.
D) I would need to support every AUR helper. Its not like it would be hard to do so since I could just ask for an install command in the config, but default to yay with a bunch of arguments to ignore user input and just install, but again that would be a pain in the butt for the user to have to do a bunch of research into how to ignore user input for their package manager that it would hurt this program as a whole.

In conclusion, DO IT YOURSELF.