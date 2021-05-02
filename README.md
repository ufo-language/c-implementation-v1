# UFO C implementation

This is a plain-vanilla C implementation of UFO. Currently it uses the C99 standard. I kind of want to make it ANSI C, but *crikey* C99 is so much easier to program in than ANSI C. And every day the C99 codebase grows. Rest assured, the entire codebase is in C, despite Github reporting that some small percentage of it is in C++ (see rant at bottom of page).

Why plain-vanilla C?

* Because what I find interesting is not what you can build using a bloated language that tries to give you everything, but with a small and clean language that gives you only what you need. And also there's a small SBC I would love to port it to.

Why not C++?

* Because C++ has become a Frankenstein's monster of a programming language and it really needs to die a horrible death. *With fire.* (That, and I've done it already in C++, also Java, Ada, Modula-3, Erlang, Scheme.)

You prolly want docs, right? We got docs: see https://github.com/ufo-language/reference-implementation/tree/master/doc for the complete UFO documentation (download the file, save it, and *then* open it in your web browser in order to view it). *Caveat programmator*: this version is not yet compliant with that spec, and I think it also diverges a bit from it.

----

Current status: there's a REPL that lets you enter many of the data structures and a few of the expressions, but there are no built-in functions.

Features:
* Virtual memory
* Automatic memory management with memory block defragmentation
* Recursive evaluator, but provides proper tail call elimination (impressed? *you should be*) ;)
* Expressions:
  * variable
  * if/then/else
  * do/end (sequence)
  * let (dynamic scope)
  * let/in (local scope)
  * abstraction & application (lexical scope, recursion, tail call elimination)
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
* Plenty of undocumented features! (i.e., bugs)

CS quiz: Why is an immuatable array useful? No fair peeking at my source code before you answer it. (hint: I said "CS quiz" and not "software engineering quiz")

Also, I'm thinking about combining the linked list & queue types into a doubly-linked list type. I can think of reasons to do it, and reasons not to do it.

----

The rant I promised you:

Apparently this problem of Github mis-reporting C code as C++ code is a problem that's been plaguing Github users since at least 2014, from all the complaints I've seen. Since the solution is simple but Github refuses to fix the problem, then I can only conclude that the issue is political and not technical: leaving it this way, Github gets to over-report how much C++ code it hosts. My guess is that there's a financial incentive for them to do this.
