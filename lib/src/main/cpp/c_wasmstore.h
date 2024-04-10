#include "c_base.h"

class WasmStore
{
private:
    WasmData *data;
    wasmtime_store_t *store;

public:
    WasmStore(WasmEngine *weng);
    ~WasmStore();
    wasmtime_context_t *GetContext();
};
