#include <jni.h>
#include <cstring>

int sn();

void int_cpptest();

extern "C"
{
    int usewasm(const char *szWasm, bool isWat);
    char *callsm(const char *szInput);
}

JNIEnv *genv = NULL;

char *callabove(const char *szInput)
{
    jclass cls = genv->FindClass("com/smwasm/smwasm/SmManager");
    if (cls != NULL)
    {
        jmethodID mid = genv->GetStaticMethodID(cls, "call", "(Ljava/lang/String;)Ljava/lang/String;");
        if (mid != NULL)
        {
            jstring message = genv->NewStringUTF(szInput);
            jstring ret = (jstring)genv->CallStaticObjectMethod(cls, mid, message);
            const char *szOutput = genv->GetStringUTFChars(ret, NULL);
            genv->DeleteLocalRef(message);
            return (char *)szOutput;
        }
    }

    char *sz = new char[3];
    strcpy(sz, "{}");
    return sz;
}

extern "C"
{
    JNIEXPORT void JNICALL Java_com_smwasm_smwasm_SmWasm_load(JNIEnv *env, jobject thisObj, jstring parameter, jboolean isWat)
    {
        if (genv == NULL)
        {
            printf("--- jni know env ---\n");
            genv = env;
            int_cpptest();
        }
        const char *param = env->GetStringUTFChars(parameter, NULL);
        usewasm(param, isWat == JNI_TRUE);
        env->ReleaseStringUTFChars(parameter, param);
        return;
    }

    JNIEXPORT jint JNICALL Java_com_smwasm_smwasm_SmWasm_sn(JNIEnv *env, jobject thisObj)
    {
        return sn();
    }

    JNIEXPORT jstring JNICALL Java_com_smwasm_smwasm_SmWasm_call(JNIEnv *env, jobject thisObj, jstring input)
    {
        genv = env;
        const char *param = env->GetStringUTFChars(input, NULL);
        char *szOutput = callsm(param);
        jstring v = env->NewStringUTF(szOutput);
        env->ReleaseStringUTFChars(input, param);
        delete szOutput;
        return v;
    }
}
