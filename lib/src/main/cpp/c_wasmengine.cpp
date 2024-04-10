#include "c_base.h"
#include "c_wasmengine.h"


WasmEngine::WasmEngine()
{
    this->engine = wasm_engine_new();
    printf("--- wasm engine --- 102 --- (%d) ---\n", sn());
}

WasmEngine::~WasmEngine()
{
    wasm_engine_delete(this->engine);
}
