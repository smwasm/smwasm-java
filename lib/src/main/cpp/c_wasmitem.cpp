#include "c_base.h"
#include "c_wasmitem.h"


WasmItem::WasmItem(const char *szWasm, bool isWat)
{
    this->szWasm = new char[strlen(szWasm) + 1];
    strcpy(this->szWasm, szWasm);
    this->isWat = isWat;

    this->wins = NULL;
    this->wstore = NULL;
    this->wmodule = NULL;
}

WasmItem::~WasmItem()
{
    delete this->szWasm;
}

bool WasmItem::read_bytes(wasm_byte_vec_t *pdata, const char *szMode)
{
    FILE *file = fopen(this->szWasm, szMode);
    if (file == NULL)
    {
        return false;
    }

    bool ok = true;
    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    wasm_byte_vec_new_uninitialized(pdata, file_size);
    if (fread(pdata->data, file_size, 1, file) != 1)
    {
        ok = false;
    }
    fclose(file);

    printf("--- read wasm --- (%d) --- %s --- %ld ---\n", sn(), this->szWasm, file_size);
    return ok;
}

bool WasmItem::get_wat_data(wasm_byte_vec_t *pwasm)
{
    wasm_byte_vec_t wat;
    bool ok = this->read_bytes(&wat, "r");

    if (ok)
    {
        wasmtime_error_t *error = wasmtime_wat2wasm(wat.data, wat.size, pwasm);
        if (error != NULL)
        {
            ok = false;
        }
    }
    wasm_byte_vec_delete(&wat);

    return ok;
}

bool WasmItem::get_wasm_data(wasm_byte_vec_t *pwasm)
{
    return this->read_bytes(pwasm, "rb");
}
