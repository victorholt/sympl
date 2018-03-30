# Code Style Ideas
#### version 0.0.1

~~~~
// Comments are done using this line.
/**
 * Block comments are done like this.
 */

/**
 * @ - denotes that the parser will perform a 'lookup' for the proceeding name.
 *
 * Everything is handled in memory for speed, and designed to use a little memory
 * as possible. Garbage collection is not done at all! Freeing memory is done by
 * deleting an object manually.
 */

// Define flag for the app.
flag("global_flag")

// Remove global flag
remove_flag("global_flag")

// Sympl class
// All classes are inherited to the 'ref' built-in object.
@obj some_obj : ref {
  @int my_int = 1
  @str my_str = "testing"
  @real my_dbl = 1.5
  @var my_auto_int = 1 // Auto assign the integer

  // Array examples
  @int[] int_arr = { 1, 3, 5 }
  @str[] str_arr = { "1", "3", "5" }
  @real[] real_arr = { 1.0, 3.0, 5.0 }

  // Constructor called when the object is created.
  @func construct() {
    print("${root_name} object has been created!")

    // Push an event to the global event stack.
    push_event("my_event", obj.my_event)
  }

  // Destructor called when the object is destroyed.
  @func destroy() {
    print("${root_name} object has been destroyed!")
  }

  // Inline object example
  @obj my_obj = {
    // Note that variables are scoped to this object.
    @int my_int = 1
  }

  // Method example.
  @func my_func : int {
    return 1 + 2
  }

  // Method example with args.
  @func my_func(x) : int {
    return x + 1
  }

  // Creating an event that can be called throughout the system.
  // @event is a special-case function which has the purpose of
  // executing as an event.
  @event my_event {
    print("Running event with ${args.length} arguments")

    // Delay this to run after 1 sec.
    @delay 1000 {
      print("Event has been called for object ${root_name} | ${name}!")
    }
  }
}

// file: main.sympl
@func main() {
  // Object's can be assigned 'null',
  // preventing memory allocation.
  @some_obj null_obj = null
  @some_obj is_null_obj = null_obj || null
  if !is_null_obj {
    print("We have a null object boys!")
  }

  // Build-in profiling (does not effect performance is profiling is off)
  profile_start("profile_name")
  @some_obj obj
  obj.name = "ObjectNameChanged" // dangerous?!
  obj.my_str = "Hello World"
  profile_end()

  // Print object variable.
  print(obj.my_str)

  // Print object.
  print(obj)

  // Loops
  for (@int i = 0; i < 10; i++) {
    print(i)
  }

  // Don't do this...
  while (true) {
    print("infinite-loop?")
    break
  }

  // Create object from json.
  @obj json_object = parse_json("{ my_var: 1 }")

  // Run our event (no args).
  event("my_event", {})

  // (optional) Delete our object to free up memory.
  delete(obj)
}
~~~~ ```