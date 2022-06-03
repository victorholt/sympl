# Sympl
Sympl Scripting Language Compiler

### Description
This language is mainly built for the purpose of embedding into my own games. As of 2022 the language was heavily refactored based on the word done here [Create Your Own Programming Language](https://www.youtube.com/watch?v=Eythq9848Fg&list=PLZQftyCk7_SdoVexSmwy_tBgs7P0b97yD&index=1) with some modifications. 

### Example
```
// Simple math example.
profiler_start("math");

func add(a, b) : int {
    return a + b;
}

var x = 0;
while (x < 250000) {
    x = add(x, 1);
}

printl(x);

profiler_stop("math");
```

