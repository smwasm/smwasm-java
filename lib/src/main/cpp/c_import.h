#ifndef C_IMPORT
#define C_IMPORT


#include "c_base.h"


wasm_trap_t *host_fd_write(void *env, wasmtime_caller_t *caller,
                                  const wasmtime_val_t *args, size_t nargs,
                                  wasmtime_val_t *results, size_t nresults);

wasm_trap_t *host_get_ms(void *env, wasmtime_caller_t *caller,
                                const wasmtime_val_t *args, size_t nargs,
                                wasmtime_val_t *results, size_t nresults);

wasm_trap_t *host_put_memory(void *env, wasmtime_caller_t *caller,
                                    const wasmtime_val_t *args, size_t nargs,
                                    wasmtime_val_t *results, size_t nresults);

wasm_trap_t *host_debug(void *env, wasmtime_caller_t *caller,
                               const wasmtime_val_t *args, size_t nargs,
                               wasmtime_val_t *results, size_t nresults);

wasm_trap_t *host_callsm(void *env, wasmtime_caller_t *caller,
                               const wasmtime_val_t *args, size_t nargs,
                               wasmtime_val_t *results, size_t nresults);

char* get_buffer_text(uint8_t* mem, int ptr);
int set_buffer_text(WasmItem* wi, char* szText);


#endif
