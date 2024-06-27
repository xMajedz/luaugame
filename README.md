Configure CMake
```
cmake -G"MinGW Makefiles" -Bbuild -DCMAKE_BUILD_TYPE=RelWithDebInfo
```
Build
```
cmake --build build --target luaugame --config RelWithDepInfo
```
Run
```
.\build\luaugame.exe
```

you can edit ```build\main.luau```
