# Games 101 Assignments

***macOS***

`brew install eigen`  
`brew install opencv`

neovim lsp clangd config

```lua
local servers = {
  clangd = {
    cmd = {
      "clangd",
      "--function-arg-placeholders=0",
      "--enable-config",
      "--offset-encoding=utf-16",
      "--compile-commands-dir=build",
    },
  },
}
```
