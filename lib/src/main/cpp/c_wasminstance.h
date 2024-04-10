#include "c_base.h"

class WasmInstance
{
private:
    WasmModule *wmodule;
    wasmtime_instance_t instance;
    wasmtime_context_t *context;

public:
    wasmtime_extern_t smcall;
    wasmtime_extern_t smalloc;
    wasmtime_extern_t smdealloc;

public:
    WasmInstance(WasmModule *wmodule, wasmtime_context_t *context);
    ~WasmInstance();
    uint8_t *Init(wasmtime_extern_t *import, int num);

    wasmtime_instance_t *Instance();
    wasmtime_extern_t GetRun(const char *szFunc);
};
