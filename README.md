# c-implementation

ANSI C implementation of UFO.

Why ANSI C?

Because what's become interesting to me is not what you can build using a language that gives you everything, but with a language that gives you very little.

See https://github.com/ufo-language/reference-implementation/tree/master/doc (download file in order to view) for the complete UFO documentation. Caveat emptor: this version is not up to that spec.

----

Current status: there's a REPL that lets you enter many of the data structures and a few of the expressions, but there are no build-in functions.

Features:
* Virtual memory
* Automatic memory management with memory block defragmentation
* Recursive evaluator
* Expressions:
  * variable
  * if/then/else
  * do/end (sequence)
  * let (variable definition)
  * let/in (local scope)
  * abstraction & application (function with lexical binding & function call)
  * exception throwing (exception catching not yet implemented)
* Data types:
  * integer, real, boolean, string
  * symbolic constant
  * array
  * linked list
  * finite map / hash table
  * set
  * queue
  * tuple (immutable array)
