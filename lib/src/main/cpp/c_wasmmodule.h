#include "c_base.h"

class WasmModule
{
private:
    WasmItem *witem;
    wasmtime_module_t *module;

public:
    WasmModule(WasmItem *witem);

    bool Init(WasmEngine *weng);

    ~WasmModule();

    const wasmtime_module_t *Module();
};
