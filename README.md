qclosebutton
============

Provide a kill button overlay for fullscreen applications.

Build
-----

```
qmake && make
```

Use
---

```
./qclosebutton x_48x48.png xterm -maximized
```

In order to use PNG images with transparencies, you need to use a compositing window manager like e.g. `xcompmgr`.
