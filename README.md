configure CMake
```
cmake -Bbuild -DCMAKE_BUILD_TYPE=RelWithDebInfo
```
build
```
cd build 
cmake --build . --target luaugame --config RelWithDepInfo
```
run
```
.\luaugame.exe
```
edit ```build\main.luau``` as you please.
