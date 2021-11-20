package com.name1e5s.kwic;

import java.util.Optional;

public interface Iterator<T> {
    Optional<T> next();
}
