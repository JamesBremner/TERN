# TERN
Event simulator with tasks or flows

The Event RuNer is a C++17 discrete event simulator.

Objects wander between event handlers, triggering events as they arrive and depart.

`tern_in_console` is a standalone simulator running from the command line.  Model layout is specified by writing C++ code, results are shown in text.

`tern_in_vase` runs simulation models specified by `vase'

`vase` is a graphical user interface.  Model layout is specified by pointing and clicking.  Results are displayed in graphs.  Runs are stored in and can be recovered from a database.

