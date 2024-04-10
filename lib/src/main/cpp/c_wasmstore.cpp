#include "c_base.h"
#include "c_wasmengine.h"
#include "c_wasmdata.h"
#include "c_wasmstore.h"


WasmStore::WasmStore(WasmEngine *weng)
{
    this->data = new WasmData();
    this->store = wasmtime_store_new(weng->engine, this->data, NULL);
    printf("--- new wasm store --- (%d) --- %p ---\n", sn(), (void *)this->data);
}

WasmStore::~WasmStore()
{
    delete this->data;
    wasmtime_store_delete(this->store);
}

wasmtime_context_t *WasmStore::GetContext()
{
    wasmtime_context_t *context = wasmtime_store_context(this->store);
    return context;
}
