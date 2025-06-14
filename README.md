# CMake
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
.\luaugame.exe tsoding_ball
```
# Make
clone Luau
```
git clone https://github.com/luau-lang/luau.git
```
build Luau 
```
make -C luau config=release luau
```
clone Raylib
```
git clone https://github.com/raysan5/raylib.git
```
build Raylib
```
make -C raylib/src
```
build
```
cd build
make -C ..
```
run
```
./luaugame.exe tsoding_ball
```
