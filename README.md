cyclomatic_complexity
=====================

Compute cyclomatic complexity of functions based on the gcc internal representation.

http://en.wikipedia.org/wiki/Cyclomatic_complexity

The complexity M is then defined as `M = E − N + 2P` where

    E = the number of edges of the graph
    N = the number of nodes of the graph
    P = the number of connected components (exit nodes).


Compiling & Usage
-----------------

##### gcc 4.5 - 5:

```shell
$ make clean ; make
```

##### Usage

```shell
$ make run
```
