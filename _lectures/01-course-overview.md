---
title: "Lecture 1 — Course overview and the C toolchain"
reading: "Dive into Systems §1.1–1.3"
---

**Reading:** {{ page.reading }}

## What the book underplays

...

## Terminal session

```console
$ gcc -Wall -g hello.c -o hello
$ ./hello
```

{% comment %}
INSTRUCTOR: open two terminals before class, one in ~/demo.
Board: draw the four compilation stages before running gcc -save-temps.
{% endcomment %}

## Common mistakes