#include <stdio.h>
#include <stdlib.h>
#include <wasm.h>
#include <wasmtime.h>

int gwSN = 0;
int sn()
{
    gwSN++;
    return gwSN;
}

wasm_functype_t *wasm_functype_new_4_1(
    wasm_valtype_t *p1, wasm_valtype_t *p2,
    wasm_valtype_t *p3, wasm_valtype_t *p4,
    wasm_valtype_t *r1)
{
    wasm_valtype_t *ps[4] = {p1, p2, p3, p4};
    wasm_valtype_t *rs[1] = {r1};
    wasm_valtype_vec_t params, results;
    wasm_valtype_vec_new(&params, 4, ps);
    wasm_valtype_vec_new(&results, 1, rs);
    return wasm_functype_new(&params, &results);
}

void exit_with_error(const char *message, wasmtime_error_t *error,
                     wasm_trap_t *trap)
{
    fprintf(stderr, "error: %s\n", message);
    wasm_byte_vec_t error_message;
    if (error != NULL)
    {
        wasmtime_error_message(error, &error_message);
        wasmtime_error_delete(error);
    }
    else
    {
        wasm_trap_message(trap, &error_message);
        wasm_trap_delete(trap);
    }
    fprintf(stderr, "%.*s\n", (int)error_message.size, error_message.data);
    wasm_byte_vec_delete(&error_message);
    exit(1);
}
