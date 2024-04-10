#include "c_base.h"

class WasmItem
{
public:
    char *szWasm;
    bool isWat;

    WasmInstance *wins;
    WasmStore *wstore;
    WasmModule *wmodule;
    WasmData *data;
    wasmtime_context_t *context;

public:
    WasmItem(const char *szWasm, bool isWat);

    ~WasmItem();

    bool read_bytes(wasm_byte_vec_t *pdata, const char *szMode);

    bool get_wat_data(wasm_byte_vec_t *pwasm);

    bool get_wasm_data(wasm_byte_vec_t *pwasm);
};
