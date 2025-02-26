package com.smwasm.smwasm;

import java.util.HashMap;
import java.util.function.Function;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;

public class SmHub {

    private static final SmWasm WASM = new SmWasm();
    private final HashMap<String, Function<String, String>> fmap = new HashMap<>();

    private SmHub() {
    }

    private static class SmHubHolder {

        private static final SmHub INSTANCE = new SmHub();
    }

    public static SmHub getInstance() {
        return SmHubHolder.INSTANCE;
    }

    // Internal use for Rust
    public static String callNative(String input) {
        ObjectMapper objectMapper = new ObjectMapper();

        try {
            JsonNode jsonNode = objectMapper.readTree(input);
            String name = jsonNode.get("$usage").asText();
            Function<String, String> fn = getInstance().fmap.get(name);
            if (fn != null) {
                System.out.println(">>> --- call java usage --- " + name);
                String output = fn.apply(input);
                return output;
            }
        } catch (JsonProcessingException e) {
        }

        return "";
    }

    public boolean register(String itdef, Function<String, String> func) {
        ObjectMapper objectMapper = new ObjectMapper();

        try {
            JsonNode jsonNode = objectMapper.readTree(itdef);
            String name = jsonNode.get("$usage").asText();
            Function<String, String> fn = this.fmap.get(name);
            if (fn == null) {
                this.fmap.put(name, func);
                WASM.register(itdef);
                return true;
            }
        } catch (JsonProcessingException e) {
        }

        return false;
    }

    public void load(String szWasm, int space) {
        WASM.load(szWasm, space);
    }

    public String call(String input) {
        String ret = callNative(input);
        if (ret.length() > 0) {
            return ret;
        }

        ret = WASM.call(input);
        return ret;
    }
}
