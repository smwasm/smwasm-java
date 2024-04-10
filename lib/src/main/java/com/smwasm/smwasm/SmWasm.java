package com.smwasm.smwasm;
//

// export JAVA_HOME=/usr/lib/jvm/java-17-openjdk-amd64
// ./gradlew build
//
// sudo sh -c 'echo "/home/jzr/repo/smmo/other/java/smwasm/src/main/cpp" > /etc/ld.so.conf.d/smwasm.conf'
// sudo ldconfig
// ldconfig -p | grep smwasm
//
// export LD_LIBRARY_PATH=/home/jzr/repo/smmo/other/java/smwasm/src/main/cpp
//
// cd bjavawasm/smwasm/src/main/cpp
// gcc -shared -fpic HelloJNI.c -o libhello.so -I ${JAVA_HOME}/include -I ${JAVA_HOME}/include/linux
//

public class SmWasm {
    static {
        System.loadLibrary("smwasm");
    }

    private static final Object lock = new Object();

    public native void load(String szWasm, boolean isWat);

    public native int sn();

    public native String call(String input);
}
