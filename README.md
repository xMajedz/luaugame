git submodules
```
git submodule update --init --recursive
```
# Build with CMake
generate build files.
```
cmake -B build
```
build
```
cmake --build build
```
run
```
build/luaugame tsoding_ball

```
# Build web demo with Emscripten + CMake
generate build files.
```
emcmake cmake -B build-web -DWEB_DEMO=ON -DBUNDLE=tsoding-ball
```
build
```
cmake --build build-web
```
run
```
npx http-server build-web
or
python -m http.server -d build-web

```
