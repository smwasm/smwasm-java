#include "c_base.h"
#include "c_import.h"
#include "c_wasmdata.h"
#include "c_wasmitem.h"
#include "c_wasminstance.h"


wasm_trap_t *host_put_memory(void *env, wasmtime_caller_t *caller,
                                    const wasmtime_val_t *args, size_t nargs,
                                    wasmtime_val_t *results, size_t nresults)
{
    wasmtime_context_t *context = wasmtime_caller_context(caller);
    WasmData *data = (WasmData *)wasmtime_context_get_data(context);

    int ptr = args[0].of.i32;
    int ty = args[1].of.i32;
    if (ty != 10) {
        return NULL;
    }

    char* buf = get_buffer_text(data->mem, ptr);
    printf("%s\n", buf);
    delete[] buf;
    return NULL;
}

wasm_trap_t *host_callsm(void *env, wasmtime_caller_t *caller,
                               const wasmtime_val_t *args, size_t nargs,
                               wasmtime_val_t *results, size_t nresults)
{
    wasmtime_context_t *context = wasmtime_caller_context(caller);
    WasmData *data = (WasmData *)wasmtime_context_get_data(context);

    int ptr = args[0].of.i32;
    char* szCall = get_buffer_text(data->mem, ptr);

    char* szOutput = callsm(szCall);
    delete[] szCall;

    int ptr_ret = set_buffer_text(data->wi, szOutput);
    delete[] szOutput;

    results[0].of.i32 = ptr_ret;
    return NULL;
}

char* get_buffer_text(uint8_t* mem, int ptr) {
    int len = mem[ptr] | (mem[ptr+1] << 8) | (mem[ptr+2] << 16) | (mem[ptr+3] << 24);
    char* buf = new char[len + 1];
    memcpy(buf, mem + ptr + 4, len);
    buf[len] = 0;
    return buf;
}

int set_buffer_text(WasmItem* wi, char* szText) {
    int len = strlen(szText);
    wasmtime_val_t pack[1];
    pack[0].kind = WASMTIME_I32;
    pack[0].of.i32 = len;

    wasmtime_val_t rp[1];
    rp[0].kind = WASMTIME_I32;

    wasm_trap_t *trap = NULL;
    wasmtime_func_call(wi->context, &wi->wins->smalloc.of.func, pack, 1, rp, 1, &trap);
    int ptr = rp[0].of.i32;
    memcpy(wi->data->mem + ptr + 4, szText, len);
    return ptr;
}
