#include "c_base.h"

wasm_trap_t *host_debug(void *env, wasmtime_caller_t *caller,
                        const wasmtime_val_t *args, size_t nargs,
                        wasmtime_val_t *results, size_t nresults)
{
    printf("--- < < --- (%d) --- %d %d ---\n", sn(), args[0].of.i32, args[1].of.i32);
    return NULL;
}

wasm_trap_t *host_get_ms(void *env, wasmtime_caller_t *caller,
                         const wasmtime_val_t *args, size_t nargs,
                         wasmtime_val_t *results, size_t nresults)
{
    results[0].kind = WASMTIME_I64;

    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    results[0].of.i64 = millis;
    return NULL;
}
