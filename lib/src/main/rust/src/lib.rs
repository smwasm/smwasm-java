use json::{JsonValue};
use lazy_static::lazy_static;
use std::sync::{Mutex, MutexGuard, RwLock};

use jni::objects::{JClass, JObject, JString, JValue};
use jni::sys::{jboolean, jint, jstring};
use jni::JNIEnv;

use smcore::{smh, smu};
use smdton::{SmDton, SmDtonBuffer, SmDtonBuilder, SmDtonMap};


lazy_static! {
    static ref LIB_DATA: RwLock<LibData> = RwLock::new(LibData { _sn: 0 });
}

struct LibData {
    _sn: u64,
}

thread_local! {
    static GLOBAL_ENV: Mutex<JniEnv> = Mutex::new(JniEnv { _env: None });
}

pub struct JniEnv {
    _env: Option<JNIEnv<'static>>,
}

pub fn keep_env(mut _env: JNIEnv) {
    GLOBAL_ENV.with(|env_lock| {
        let mut env_lock = env_lock.lock().unwrap();
        env_lock._env = Some(unsafe { std::mem::transmute::<JNIEnv<'_>, JNIEnv<'static>>(_env) });
    });
}

pub fn get_env() -> Option<MutexGuard<'static, JniEnv>> {
    GLOBAL_ENV.with(|env_lock| {
        let env_lock = env_lock.lock().unwrap();
        if env_lock._env.is_some() {
            Some(unsafe { std::mem::transmute(env_lock) })
        } else {
            None
        }
    })
}

pub fn use_env(intxt: &str, outtxt: &mut String) -> jni::errors::Result<()> {
    if let Some(guard) = get_env() {
        let jni_env: &JniEnv = &*guard;
        if let Some(env) = &jni_env._env {
            let mut tt = unsafe { env.unsafe_clone() };
            let class = tt.find_class("com/smwasm/smwasm/SmHub")?;

            

            let message = env.new_string(intxt)?;
            let jobject: JObject = message.into();
            let jvalue = JValue::from(&jobject);

            let result = tt.call_static_method(
                class,
                "callNative",
                "(Ljava/lang/String;)Ljava/lang/String;",
                &[jvalue],
            )?;
            
            let rr = result.l()?;

            *outtxt = tt.get_string(&rr.into())?.into();
            if outtxt.len() == 0 {
                *outtxt = "{}".to_string();
            }
        }
    } else {
    }

    Ok(())
}

pub fn inc_sn(_env: JNIEnv) -> u64 {
    let mut save_env = false;
    let sn: u64;
    {
        let mut hb = LIB_DATA.write().unwrap();
        if hb._sn == 0 {
            save_env = true;
        }
        hb._sn += 1;
        sn = hb._sn;
    }

    if save_env {
        smloadwasm::init();
        smsys::init();

        keep_env(_env);
    }

    return sn;
}

#[unsafe(no_mangle)]
pub extern "system" fn Java_com_smwasm_smwasm_SmWasm_load(mut _env: JNIEnv, 
    _class: jni::objects::JClass, 
    sz_wasm: JString, 
    space: jint
) {
    let wasm: String = _env.get_string(&sz_wasm).expect("").into();
    let space: i32 = space;
    smloadwasm::load_wasm(&wasm, space);
}

#[unsafe(no_mangle)]
pub extern "system" fn Java_com_smwasm_smwasm_SmWasm_sn(_env: JNIEnv, _class: JClass) -> jint {
    let ret = inc_sn(_env) as i32;
    return ret;
}

#[unsafe(no_mangle)]
pub extern "system" fn Java_com_smwasm_smwasm_SmWasm_call(mut _env: JNIEnv, _class: JClass, in_text: JString) -> jstring {
    let mut outtxt = "{}".to_string();

    let intxt: String = _env.get_string(&in_text).expect("").into();
     let jsn = json::parse(&intxt).unwrap();
    let smb = smu.build_buffer(&jsn);
    let ret = smh.call(smb);

    let op_ret = ret.stringify();
    match op_ret {
        Some(txt) => {
            outtxt = txt;
        },
        None => {
        },
    }

    let java_string = match _env.new_string(outtxt) {
        Ok(ret) => ret,
        Err(_) => _env.new_string("{}").unwrap(),
    };

    let raw_jstring = java_string.into_raw();
    raw_jstring
}

#[unsafe(no_mangle)]
pub extern "system" fn Java_com_smwasm_smwasm_SmWasm_register(mut _env: JNIEnv, _class: JClass, _usage: JString) -> jboolean {
    let txt: String = _env.get_string(&_usage).expect("").into();
    let _define = json::parse(&txt).unwrap();
    smh.register_by_json(&_define, _call_sm);

    let flag: jboolean = 1;
    return flag;
}

fn _call_sm(_input: &SmDtonBuffer) -> SmDtonBuffer {
    let sd = SmDton::new_from_buffer(_input);
    let intxt = sd.stringify().unwrap();

    let mut result_str: String = "{}".to_string();
    let _r = use_env(&intxt, &mut result_str);
   
    let parsed: Result<JsonValue, json::Error> = json::parse(&result_str);
    match parsed {
        Ok(jsn) => {
            let mut sdb = SmDtonBuilder::new_from_json(&jsn);
            return sdb.build();
        }
        Err(_e) => {
        }
    }

    let mut _map = SmDtonMap::new();
    return _map.build();
}
