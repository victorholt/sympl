# Sympl
Sympl Scripting Language Compiler

### Description
This language is mainly built for the purpose of embedding into my own games. As of 2022 the language was heavily refactored based on the word done here [Create Your Own Programming Language](https://www.youtube.com/watch?v=Eythq9848Fg&list=PLZQftyCk7_SdoVexSmwy_tBgs7P0b97yD&index=1) with some modifications. 

### Example
```
# Structure to hold the value.
object Data
    var value = 0
end

# Simple method to add two values.
func add(a, b) -> a + b

# Shorthand definition for a method.
var start = time()

# Loop through and add to the x variable.
var data = new Data
while data.value < 2500 then
    data.value = add(data.value, 1)
end

# print our results.
print(data.value)

var endTime = (time() - start)
print("Took: " + str(endTime) + "ms to run")
```

