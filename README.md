# UFO C implementation

Plain-vanilla C implementation of UFO. (Currently it uses the C99 standard. I kind of want to make it ANSI C, but crikey C99 is so much easier to program in than ANSI C. And every day the C99 codebase grows...)

Why plain-vanilla C?

Because what's become interesting to me is not what you can build using a language that gives you everything, but with a language that gives you very little.

See https://github.com/ufo-language/reference-implementation/tree/master/doc (download the file in order to view it) for the complete UFO documentation. Caveat emptor: this version is not up to that spec.

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

CS quiz: Why is an immuatable array useful? No fair peeking at my source code before you answer it.

Also, I'm thinking about combining the linked list & queue types into a single doubly-linked list type. I can think of reasons to do it, and reasons not to do it.
