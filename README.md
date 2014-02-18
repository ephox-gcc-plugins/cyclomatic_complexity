cyclomatic_complexity
=====================

Compute cyclomatic complexity of functions based on the gcc internal representation.

http://en.wikipedia.org/wiki/Cyclomatic_complexity

The complexity M is then defined as `M = E − N + 2P` where

    E = the number of edges of the graph
    N = the number of nodes of the graph
    P = the number of connected components (exit nodes).

Homepage
--------

http://www.grsecurity.net/~ephox/

Compiling & Usage
-----------------

##### gcc 4.5 - 4.7 (C):

```shell
$ gcc -I`gcc -print-file-name=plugin`/include \
      -I`gcc -print-file-name=plugin`/include/c-family \
      -fPIC -shared -O2 \
      -o cyc_complexity_plugin.so cyc_complexity_plugin.c
```

##### gcc 4.7 (C++) - 4.8+

```shell
$ g++ -I`g++ -print-file-name=plugin`/include \
      -I`g++ -print-file-name=plugin`/include/c-family \
      -fPIC -shared -O2 \
      -o cyc_complexity_plugin.so cyc_complexity_plugin.c
```

##### Usage

```shell
$ gcc -fplugin=./cyc_complexity_plugin.so test.c -O2
```
