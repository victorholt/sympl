# Sympl
Sympl Scripting Language Compiler

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

