package com.smwasm.smwasm;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.smwasm.smwasm.SmHub;

import org.junit.Test;

public class MySm {

    public static String smThirdHeartBeat(String inputText) {
        ObjectMapper objectMapper = new ObjectMapper();
        try {
            JsonNode jsonNode = objectMapper.readTree(inputText);
            String intxt = jsonNode.get("text").asText();
            String outtxt = intxt + "---[java]---third";

            ObjectNode objectNode = objectMapper.createObjectNode();
            objectNode.put("input", intxt);
            objectNode.put("output", outtxt);

            String jsonString = objectMapper.writeValueAsString(objectNode);
            return jsonString;
        } catch (JsonProcessingException e) {
        }
        return "{}";
    }

    public static String smHeartBeat(String inputText) {
        ObjectMapper objectMapper = new ObjectMapper();
        try {
            JsonNode jsonNode = objectMapper.readTree(inputText);
            String intxt = jsonNode.get("text").asText();
            String outtxt = intxt + "---[java]---heart";

            ObjectNode objectNode = objectMapper.createObjectNode();
            objectNode.put("input", intxt);
            objectNode.put("output", outtxt);

            String jsonString = objectMapper.writeValueAsString(objectNode);
            return jsonString;
        } catch (JsonProcessingException e) {
        }
        return "{}";
    }

    public static String smHeart0gBeat(String input) {
        System.out.println(">>> --- sm --- native.heart0g.beat ---");
        ObjectMapper objectMapper = new ObjectMapper();
        String inputText;

        try {
            JsonNode jsonNode = objectMapper.readTree(input);
            inputText = jsonNode.get("text").asText();
            String outputText = inputText + "---[java]---heart0g";

            ObjectNode objectNode = objectMapper.createObjectNode();
            objectNode.put("input", inputText);
            objectNode.put("output", outputText);

            String jsonString = objectMapper.writeValueAsString(objectNode);
            return jsonString;
        } catch (JsonProcessingException e) {
        }
        return "{}";
    }

    public static boolean register() {
        System.out.println(">>> --- MySm register ---");
        SmHub smh = SmHub.getInstance();
        smh.register("{\"$usage\": \"third.heart.beat\", \"text\": \"\"}", MySm::smThirdHeartBeat);
        smh.register("{\"$usage\": \"native.heart.beat\", \"text\": \"\"}", MySm::smHeartBeat);
        smh.register("{\"$usage\": \"native.heart0g.beat\", \"text\": \"\"}", MySm::smHeart0gBeat);

        return true;
    }

    @Test
    public void temp() {
    }
}
