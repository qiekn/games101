# Assignment 5

Tidy code using neovim:

```vimscript
:args *.h *.cpp
:argdo %s/\.hpp/\.h/g | update
:argdo %s/#include "\zs[^"]\+\ze"/\L&/g | update
```
