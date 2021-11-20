package com.name1e5s.kwic;

import java.util.List;

public interface Stage extends Iterable<List<String>> {
    void setup(Stage prevStage);

    default Stage andThen(Stage nextStage) {
        nextStage.setup(this);
        return nextStage;
    }
}
