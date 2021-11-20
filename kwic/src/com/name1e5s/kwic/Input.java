package com.name1e5s.kwic;

import java.io.BufferedReader;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

public class Input implements Stage {
    BufferedReader bufferedReader;

    Input(BufferedReader bufferedReader) {
        this.bufferedReader = bufferedReader;
    }

    @Override
    public Iterator<List<String>> iter() {
        return new InputIterator();
    }

    @Override
    public void setup(Stage prevStage) {
    }

    class InputIterator implements Iterator<List<String>> {
        @Override
        public Optional<List<String>> next() {
            try {
                String line = bufferedReader.readLine();
                if (line != null) {
                    if (line.equals("-1")) {
                        return Optional.empty();
                    }
                    String[] s = line.trim().split(" ");
                    ArrayList<String> result = new ArrayList<>();
                    for (String t : s) {
                        String trimString = t.trim();
                        if (!trimString.isEmpty()) {
                            result.add(t);
                        }
                    }
                    return Optional.of(result);
                } else {
                    return Optional.empty();
                }
            } catch (Exception e) {
                return Optional.empty();
            }
        }
    }
}