@echo off

REM Expand GraphicsWASM.cpp to GraphicsWASMExpanded.cpp
REM -Dd will include all #defines
REM -E makes it stop after preprocessor
REM -P omits markers
REM -C maintains comments
C:/WASM/clang.exe -x c++ ^
    -E -P -C ^
    --target=wasm32 ^
    -nostdinc ^
    -nostdlib ^
    -O0 ^
    -g ^
    -fno-threadsafe-statics ^
    -D WASM32=1 ^
    -D _WASM32=1 ^
    -D DEBUG=1 ^
    -D _DEBUG=1 ^
    -o GraphicsWASMExpanded.cpp ^
    GraphicsWASM.cpp


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
    GraphicsWASMExpanded.cpp

REM Expand WasmSample.cpp to WasmSampleExpanded.cpp
C:/WASM/clang.exe -x c++ ^
    -E -P -C ^
    --target=wasm32 ^
    -nostdinc ^
    -nostdlib ^
    -O0 ^
    -gfull ^
    -fno-threadsafe-statics ^
    -D WASM32=1 ^
    -D _WASM32=1 ^
    -D DEBUG=1 ^
    -D _DEBUG=1 ^
    -o WasmSampleExpanded.cpp ^
    WasmSample.cpp

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