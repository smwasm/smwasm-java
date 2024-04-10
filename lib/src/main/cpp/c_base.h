#ifndef C_BASE
#define C_BASE

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <wasm.h>
#include <wasmtime.h>
#include <unordered_map>
#include <string>
#include <iostream>
#include <chrono>
#include "c_json.h"

class WasmEngine;
class WasmStore;
class WasmModule;
class WasmInstance;

class WasmData;
class WasmItem;

int sn();

void exit_with_error(const char *message, wasmtime_error_t *error,
                     wasm_trap_t *trap);

wasm_functype_t *wasm_functype_new_4_1(
    wasm_valtype_t *p1, wasm_valtype_t *p2,
    wasm_valtype_t *p3, wasm_valtype_t *p4,
    wasm_valtype_t *r1);

extern "C" char *callsm(const char *szInput);

typedef smjson::Json (*SmFunc)(smjson::Json);
bool sm_register(std::string name, SmFunc *func);

char *callabove(const char *szInput);

#endif
