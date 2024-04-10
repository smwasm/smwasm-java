#include "c_base.h"
#include "c_wasmitem.h"
#include "c_wasmengine.h"
#include "c_wasminstance.h"
#include "c_wasmmodule.h"


WasmModule::WasmModule(WasmItem *witem)
{
    this->witem = witem;
    this->module = NULL;
}

WasmModule::~WasmModule()
{
    if (this->module != NULL)
    {
        wasmtime_module_delete(this->module);
    }
}

bool WasmModule::Init(WasmEngine *weng)
{
    printf("--- Inst2antiating module --- (%d) --- \n", sn());

    wasm_byte_vec_t wasm;
    if (witem->isWat)
    {
        this->witem->get_wat_data(&wasm);
    }
    else
    {
        this->witem->get_wasm_data(&wasm);
    }

    wasmtime_error_t *error = wasmtime_module_new(weng->engine, (uint8_t *)wasm.data, wasm.size, &this->module);
    printf("--- compiling module --- (%d) --- %ld --- %d ---\n", sn(), wasm.size, error == NULL ? 1 : 0);
    wasm_byte_vec_delete(&wasm);
    if (error != NULL)
    {
        return false;
    }

    return true;
}

const wasmtime_module_t *WasmModule::Module()
{
    return (const wasmtime_module_t *)this->module;
}
