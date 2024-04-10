#include "c_base.h"
#include "c_wasmmodule.h"
#include "c_wasminstance.h"


WasmInstance::WasmInstance(WasmModule *wmodule, wasmtime_context_t *context)
{
    this->wmodule = wmodule;
    this->context = context;
}

WasmInstance::~WasmInstance()
{
}

uint8_t *WasmInstance::Init(wasmtime_extern_t *import, int num)
{
    wasm_trap_t *trap = NULL;
    wasmtime_error_t *error = wasmtime_instance_new(
        this->context, this->wmodule->Module(), import, num, &this->instance, &trap);
    if (error != NULL || trap != NULL)
    {
        exit_with_error("failed to instantiate", error, trap);
        return NULL;
    }

    wasmtime_extern_t item;
    bool ok = wasmtime_instance_export_get(this->context, &this->instance,
                                            "memory", strlen("memory"), &item);
    assert(ok && item.kind == WASMTIME_EXTERN_MEMORY);
    wasmtime_memory_t memory = item.of.memory;

    uint8_t *mem = wasmtime_memory_data(this->context, &memory);

    printf("--- wasm memory --- (%d) --- %p ---\n", sn(), (void *)mem);
    return mem;
}

wasmtime_instance_t *WasmInstance::Instance()
{
    return &this->instance;
}

wasmtime_extern_t WasmInstance::GetRun(const char *szFunc)
{
    wasmtime_extern_t run;
    bool ok = wasmtime_instance_export_get(this->context, &this->instance, szFunc, strlen(szFunc), &run);
    assert(ok);
    assert(run.kind == WASMTIME_EXTERN_FUNC);
    return run;
}
