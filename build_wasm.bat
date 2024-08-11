clang main.c --target=wasm32 -O3 -nostdlib -Wl,--export-all,--no-entry,--allow-undefined -o wasm.wasm
wasm2wat wasm.wasm > out2.wat
