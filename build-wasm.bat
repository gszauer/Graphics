@echo off

REM Expand GraphicsWASM.cpp to GraphicsWASMExpanded.cpp
REM -Dd will include all #defines
REM -E makes it stop after preprocessor
REM -P omits markers
REM -C maintains comments
REM C:/WASM/clang.exe -x c++ ^
REM     -E -P -C ^
REM     --target=wasm32 ^
REM     -nostdinc ^
REM     -nostdlib ^
REM     -O0 ^
REM     -g ^
REM     -fno-threadsafe-statics ^
REM     -D WASM32=1 ^
REM     -D _WASM32=1 ^
REM     -D DEBUG=1 ^
REM     -D _DEBUG=1 ^
REM     -o GraphicsWASMExpanded.cpp ^
REM     GraphicsWASM.cpp


REM Compile GraphicsWASMExpanded.cpp to Graphics.wasm
C:/WASM/clang.exe -x c++ ^
    --target=wasm32 ^
    -nostdinc ^
    -nostdlib ^
    -O0 ^
    -g ^
    -fno-threadsafe-statics ^
    -Wl,--allow-undefined ^
    -Wl,--import-memory ^
    -Wl,--no-entry ^
    -Wl,--export-dynamic ^
    -Wl,-z,stack-size=4194304 ^
    -D WASM32=1 ^
    -D _WASM32=1 ^
    -D DEBUG=1 ^
    -D _DEBUG=1 ^
    -o Graphics.wasm ^
    GraphicsWASM.cpp

REM Expand WasmSample.cpp to WasmSampleExpanded.cpp
REM C:/WASM/clang.exe -x c++ ^
REM     -E -P -C ^
REM     --target=wasm32 ^
REM     -nostdinc ^
REM     -nostdlib ^
REM     -O0 ^
REM     -gfull ^
REM     -fno-threadsafe-statics ^
REM     -D WASM32=1 ^
REM     -D _WASM32=1 ^
REM     -D DEBUG=1 ^
REM     -D _DEBUG=1 ^
REM     -o WasmSampleExpanded.cpp ^
REM     WasmSample.cpp

REM Expand Compile WasmSampleExpanded.cpp to GraphicsSample.wasm
C:/WASM/clang.exe -x c++ ^
    --target=wasm32 ^
    -nostdinc ^
    -nostdlib ^
    -O0 ^
    -gfull ^
    -fno-threadsafe-statics ^
    -Wl,--allow-undefined ^
    -Wl,--import-memory ^
    -Wl,--no-entry ^
    -Wl,--export-dynamic ^
    -Wl,-z,stack-size=4194304 ^
    -D WASM32=1 ^
    -D _WASM32=1 ^
    -D DEBUG=1 ^
    -D _DEBUG=1 ^
    -o GraphicsSample.wasm ^
    WasmSample.cpp

REM  Build GraphicsSample.wasm.map from GraphicsSample.wasm
python C:/WASM/wasm-sourcemap.py ^
    GraphicsSample.wasm -s ^
    -o GraphicsSample.wasm.map ^
    -u "./GraphicsSample.wasm.map" ^
    -w GraphicsSample.debug.wasm ^
    --dwarfdump=C:/WASM/llvm-dwarfdump.exe