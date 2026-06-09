This folder shows how to integrate and use WARP compiler

## Build demo

build with cmake arg `-DENABLE_DEMO=1`

`vb_string_view_repro` also requires `wat2wasm` on `PATH`.

## Run demo on Unix and Windows:

```shell
./build/bin/vb_demo wasm_examples/log.wasm
```

## Run string_view repro on Windows and Unix:

```shell
./build/bin/vb_string_view_repro
```

For the standalone Windows/MSVC repro setup used here, configure only the demo-related features and force the x86_64 backend:

```shell
cmake -S . -B build_copilot_repro -DVB_ENABLE_DEV_FEATURE=OFF -DENABLE_DEMO=1 -DENABLE_EXTENSIONS=1 -DBACKEND=x86_64
cmake --build build_copilot_repro --target vb_string_view_repro --config Release --parallel 4
./build_copilot_repro/bin/Release/vb_string_view_repro
```

On this setup, `Debug` printed normal sizes `42` and `13`, while `Release` reproduced the bad `0` path.

## Run demo for embedded tricore

```shell
$TRICORE_QEMU_PATH/qemu-system-tricore -semihosting -display none -M tricore_tsim162 -kernel build_tricore_gcc/bin/vb_demo
```
