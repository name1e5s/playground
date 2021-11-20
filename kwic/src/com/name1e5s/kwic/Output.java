package com.name1e5s.kwic;

import java.io.BufferedWriter;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

public class Output implements Stage {
    Optional<Stage> prevStage;
    BufferedWriter bufferedWriter;

    Output(BufferedWriter bufferedWriter) {
        this.bufferedWriter = bufferedWriter;
        prevStage = Optional.empty();
    }

    @Override
    public Iterator<List<String>> iter() {
        return new OutputIterator();
    }

    @Override
    public void setup(Stage prevStage) {
        this.prevStage = Optional.of(prevStage);
    }

    class OutputIterator implements Iterator<List<String>> {
        @Override
        public Optional<List<String>> next() {
            if (!prevStage.isPresent()) {
                return Optional.empty();
            }

            Iterator<List<String>> iter = prevStage.get().iter();
            Optional<List<String>> t = iter.next();
            while (t.isPresent()) {
                try {
                    bufferedWriter.write(String.join(" ", t.get()));
                    bufferedWriter.newLine();
                    bufferedWriter.flush();
                    t = iter.next();
                } catch (Exception e) {
                    return Optional.empty();
                }
            }
            return Optional.of(new ArrayList<>());
        }
    }
}
