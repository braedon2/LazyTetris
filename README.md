# LazyTetris
Tetris that plays itself

## Building on windows
install chocolatey, then run choco install for the following packages
* cmake
* llvm
* make

Then in VS Code install the CMake Tools extension (The one by Microsoft).

The configure command that the extension uses is:
```
"C:\Program Files\CMake\bin\cmake.EXE" -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE "-DCMAKE_C_COMPILER:FILEPATH=C:\Program Files\LLVM\bin\clang exe" "-DCMAKE_CXX_COMPILER:FILEPATH=C:\Program Files\LLVM\bin\clang++.exe" --no-warn-unused-cli -SE:/Projects/LazyTetris -Be:/Projects/LazyTetris/build -G "Unix Makefiles"
```