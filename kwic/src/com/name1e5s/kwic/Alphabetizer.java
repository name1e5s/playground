package com.name1e5s.kwic;

import java.util.*;

public class Alphabetizer implements Stage {
    Optional<Stage> prevStage = Optional.empty();

    @Override
    public Iterator<List<String>> iter() {
        return new AlphabetizerIterator();
    }

    @Override
    public void setup(Stage prevStage) {
        this.prevStage = Optional.of(prevStage);
    }

    class AlphabetizerIterator implements Iterator<List<String>> {
        boolean initialized = false;
        int index = 0;
        List<List<String>> result;

        @Override
        public Optional<List<String>> next() {
            if (!prevStage.isPresent()) {
                return Optional.empty();
            }
            if (!initialized) {
                ArrayList<String> list = new ArrayList<>();
                Iterator<List<String>> iter = prevStage.get().iter();
                Optional<List<String>> t = iter.next();
                while (t.isPresent()) {
                    list.add(String.join(" ", t.get()));
                    t = iter.next();
                }

                Collections.sort(list);

                List<String> toSort = new ArrayList<>(list);
                toSort.sort(null);

                List<List<String>> reSplitList = new ArrayList<>();
                for (String s : toSort) {
                    List<String> strings = new ArrayList<>(Arrays.asList(s.split(" ")));
                    reSplitList.add(strings);
                }

                result = reSplitList;
                index = 0;
                initialized = true;
            }

            if (index >= result.size()) {
                return Optional.empty();
            }

            List<String> t = result.get(index);
            index += 1;
            return Optional.of(t);
        }
    }
}
