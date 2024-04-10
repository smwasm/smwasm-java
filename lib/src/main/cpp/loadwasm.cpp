#include "c_base.h"
#include "c_wasmengine.h"
#include "c_wasmdata.h"
#include "c_wasmitem.h"
#include "c_wasmstore.h"
#include "c_wasminstance.h"
#include "c_wasmmodule.h"
#include "c_import.h"

std::unordered_map<std::string, void *> gwItems;
std::unordered_map<std::string, void *> gwFuncs;
std::unordered_map<std::string, void *> gwSmFuncs;
WasmEngine *gwEng = NULL;

bool sm_register(std::string name, SmFunc *func)
{
    gwSmFuncs[name] = func;
    return true;
}

char *call_wasm(WasmItem *wi, const char *szInput)
{
    int ptr = set_buffer_text(wi, (char*)szInput);

    wasmtime_val_t pack[2];
    wasm_trap_t *trap = NULL;
    wasmtime_val_t rp[1];

    pack[0].kind = WASMTIME_I32;
    pack[0].of.i32 = ptr;
    pack[1].kind = WASMTIME_I32;
    pack[1].of.i32 = 1;
    wasmtime_func_call(wi->context, &wi->wins->smcall.of.func, pack, 2, rp, 1, &trap);

    int ptr_ret = rp[0].of.i32;
    char* szOutput = get_buffer_text(wi->data->mem, ptr_ret);

    pack[0].kind = WASMTIME_I32;
    pack[0].of.i32 = ptr_ret;
    wasmtime_func_call(wi->context, &wi->wins->smdealloc.of.func, pack, 2, NULL, 0, &trap);

    return szOutput;
}

bool use_ins(wasmtime_context_t *context, WasmItem *wi)
{
    wi->context = context;
    wi->data = (WasmData *)wasmtime_context_get_data(context);
    wi->data->wi = wi;

    wasmtime_extern_t import[30];
    int impId = 0;

    wasm_importtype_vec_t imports;
    wasmtime_module_imports(wi->wmodule->Module(), &imports);
    for (size_t i = 0; i < imports.size; i++)
    {
        const wasm_importtype_t *one = imports.data[i];
        const wasm_externtype_t *ty = wasm_importtype_type(one);
        wasm_externkind_t kind = wasm_externtype_kind(ty);

        if (kind == WASM_EXTERN_FUNC)
        {
            const wasm_name_t *name = wasm_importtype_name(one);
            char *szName = new char[name->size + 1];
            memcpy(szName, name->data, name->size);
            szName[name->size] = 0;
            std::string txt = szName;
            if (txt.substr(0, 6) == "__wbg_")
            {
                memcpy(szName, name->data + 6, name->size - 23);
                szName[name->size - 23] = 0;
            }

            bool found = false;
            wasmtime_func_t func;
            if (strcmp(szName, "fd_write") == 0)
            {
                found = true;
                wasm_functype_t *fty = wasm_functype_new_4_1(wasm_valtype_new_i32(), wasm_valtype_new_i32(),
                                                             wasm_valtype_new_i32(), wasm_valtype_new_i32(), wasm_valtype_new_i32());
                wasmtime_func_new(context, fty, host_fd_write, NULL, NULL, &func);
            }
            else if (strcmp(szName, "hostputmemory") == 0)
            {
                found = true;
                wasm_functype_t *fty = wasm_functype_new_2_0(wasm_valtype_new_i32(), wasm_valtype_new_i32());
                wasmtime_func_new(context, fty, host_put_memory, NULL, NULL, &func);
            }
            else if (strcmp(szName, "hostgetms") == 0)
            {
                found = true;
                wasm_functype_t *fty = wasm_functype_new_0_1(wasm_valtype_new_i64());
                wasmtime_func_new(context, fty, host_get_ms, NULL, NULL, &func);
            }
            else if (strcmp(szName, "hostcallsm") == 0)
            {
                found = true;
                wasm_functype_t *fty = wasm_functype_new_1_1(wasm_valtype_new_i32(), wasm_valtype_new_i32());
                wasmtime_func_new(context, fty, host_callsm, NULL, NULL, &func);
            }
            else if (strcmp(szName, "hostdebug") == 0)
            {
                found = true;
                wasm_functype_t *fty = wasm_functype_new_2_0(wasm_valtype_new_i32(), wasm_valtype_new_i32());
                wasmtime_func_new(context, fty, host_debug, NULL, NULL, &func);
            }
            printf("--- (func) %s --- (%d) --- %d ---\n", szName, sn(), found ? 1 : 0);
            if (found)
            {
                import[impId].kind = WASMTIME_EXTERN_FUNC;
                import[impId].of.func = func;
                impId++;
            }

            delete szName;
        }
    }

    wi->data->mem = wi->wins->Init(&import[0], impId);
    bool ok = (wi->data->mem != NULL);
    if (!ok)
        exit_with_error("failed to instantiate", NULL, NULL);

    wasmtime_error_t *error;
    wasm_trap_t *trap = NULL;
    wasmtime_val_t results[1];

    wasmtime_extern_t run = wi->wins->GetRun("sminit");
    wasmtime_val_t para[1];
    para[0].kind = WASMTIME_I32;
    para[0].of.i32 = 0;
    error = wasmtime_func_call(context, &run.of.func, para, 1, results, 1, &trap);
    if (error != NULL || trap != NULL)
        exit_with_error("failed to call function", error, trap);

    wi->wins->smalloc = wi->wins->GetRun("smalloc");
    wi->wins->smdealloc = wi->wins->GetRun("smdealloc");
    wi->wins->smcall = wi->wins->GetRun("smcall");
    char szTemp[] = "{\"$usage\":\"smker.get.all\"}";
    char *szOutput = call_wasm(wi, szTemp);

    std::string err;
    smjson::Json jt = smjson::Json::parse(szOutput, err);

    for (const auto &kv : jt.object_items())
    {
        std::string key = kv.first;
        if (key != "smker.get.all")
        {
            printf("--- sm --- (%d) --- %s ---\n", sn(), key.c_str());
            gwFuncs[key] = wi;
        }
    }

    delete szOutput;
    return ok;
}

extern "C" int usewasm(const char *szWasm, bool isWat)
{
    printf("--- usewasm --- (%d) --- 0 ---\n", sn());
    int ret = 0;

    if (gwEng == NULL)
    {
        gwEng = new WasmEngine();
    }

    auto iter = gwItems.find((char *)szWasm);
    if (iter != gwItems.end())
    {
        auto item = gwItems[(char *)szWasm];
        if (item == NULL)
        {
            return 2;
        }
        return 1;
    }

    WasmItem *wi = new WasmItem(szWasm, isWat);
    gwItems[(char *)szWasm] = wi;
    printf("--- wasm in table --- (%d) --- %s --- %p ---\n", sn(), szWasm, (void *)wi);

    wi->wstore = new WasmStore(gwEng);
    wi->wmodule = new WasmModule(wi);

    wasmtime_context_t *context = wi->wstore->GetContext();
    wi->wmodule->Init(gwEng);
    wi->wins = new WasmInstance(wi->wmodule, context);

    bool ok = use_ins(context, wi);
    if (!ok)
        exit_with_error("failed to instantiate", NULL, NULL);

    printf("--- wasm ready --- (%d) --- %s ---\n", sn(), szWasm);
    return ret;
}

smjson::Json call_smfunc(std::string usage, smjson::Json sjInput)
{
    auto it = gwSmFuncs.find(usage);
    if (it != gwSmFuncs.end())
    {
        SmFunc func = (SmFunc)gwSmFuncs[usage];
        smjson::Json ret = func(sjInput);
        return ret;
    }

    std::string txt;
    sjInput.dump(txt);
    char *szOutput = callabove(txt.c_str());
    std::string err;
    smjson::Json jt = smjson::Json::parse(szOutput, err);
    delete szOutput;
    return jt;
}

extern "C" char *callsm(const char *szInput)
{
    std::string err;
    smjson::Json jt = smjson::Json::parse(szInput, err);

    std::string usage = jt["$usage"].string_value();
    auto iter = gwFuncs.find(usage);

    if (iter != gwFuncs.end())
    {
        WasmItem *wi = (WasmItem *)gwFuncs[usage];
        if (wi != NULL)
        {
            char *szOutput = call_wasm(wi, szInput);
            return szOutput;
        }
    }
    else
    {
        smjson::Json ret = call_smfunc(usage, jt);
        std::string txt = ret.dump();
        char *szOutput = new char[txt.length() + 1];
        strcpy(szOutput, txt.c_str());
        return szOutput;
    }

    char *szOutput = new char[3];
    strcpy(szOutput, "{}");
    return szOutput;
}
