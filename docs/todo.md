# Bugs

Create a pointer/object registry to tackle classes
* Currently running into issue where a 'returned' class isn't copied over. However, we
should not use copy for objects, but instead pass by reference through the use of a
pointer registry class.
