#include "c_base.h"
#include "c_wasmdata.h"
#include "c_wasmitem.h"
#include "c_wasminstance.h"

wasm_trap_t *host_fd_write(void *env, wasmtime_caller_t *caller,
                           const wasmtime_val_t *args, size_t nargs,
                           wasmtime_val_t *results, size_t nresults)
{
    printf("--- fd_write --- (%d) ---\n", sn());
    return NULL;
}
