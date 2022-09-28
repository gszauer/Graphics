@echo off

C:/WASM/clang.exe -E GraphicsWASM.cpp -o GraphicsExpanded.cpp
C:/WASM/clang.exe inflate GraphicsWASM.cpp

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
    -o GraphicsSample.wasm ^
    WasmSample.cpp

python C:/WASM/wasm-sourcemap.py ^
    GraphicsSample.wasm -s ^
    -o GraphicsSample.wasm.map ^
    -u "./GraphicsSample.wasm.map" ^
    -w GraphicsSample.debug.wasm ^
    --dwarfdump=C:/WASM/llvm-dwarfdump.exe