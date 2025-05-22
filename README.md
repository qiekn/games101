# Games 101 Assignments

***macOS***

`brew install eigen opencv`

### neovim clangd lsp config

1. add `set(CMAKE_EXPORT_COMPILE_COMMANDS ON)` in CMakeLists.txt to generate `compile_commands.json`

2. config lsp to specify file location (The generated path of `compile_commands.json` is the path where cmake is executed. Assume you run `cmake ..` at `/path/to/your/project/assignment_?/build`)

```lua
local servers = {
  clangd = {
    cmd = {
      "clangd",
      "--compile-commands-dir=build",
    },
  },
}
```
