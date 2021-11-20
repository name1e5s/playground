package com.name1e5s.kwic;

import java.util.List;
import java.util.Optional;

public class CircularShift implements Stage {
    Optional<Stage> prevStage;
    Iterator<List<String>> prevIter;

    CircularShift() {
        prevStage = Optional.empty();
    }

    @Override
    public Iterator<List<String>> iter() {
        return new CircularShiftIterator();
    }

    @Override
    public void setup(Stage prevStage) {
        this.prevStage = Optional.of(prevStage);
        this.prevIter = prevStage.iter();
    }

    class CircularShiftIterator implements Iterator<List<String>> {
        int index = 0;
        boolean done = false;
        List<String> currentList = null;

        @Override
        public Optional<List<String>> next() {
            if (!prevStage.isPresent() || done) {
                return Optional.empty();
            }
            if (currentList == null || index >= currentList.size()) {
                Optional<List<String>> nextList = prevIter.next();
                if (!nextList.isPresent()) {
                    done = true;
                    return Optional.empty();
                } else if (nextList.get().isEmpty()) {
                    while (nextList.get().isEmpty()) {
                        nextList = prevIter.next();
                        if (!nextList.isPresent()) {
                            done = true;
                            return Optional.empty();
                        }
                    }
                } else {
                    index = 1;
                    currentList = nextList.get();
                    return Optional.of(currentList);
                }
            }

            String first = currentList.remove(0);
            currentList.add(first);
            index += 1;
            return Optional.of(currentList);
        }
    }
}
