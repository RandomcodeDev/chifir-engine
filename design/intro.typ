= Introduction
This document outlines the design of the custom-made 3D game engine for False King and subsequent Randomcode Developers games.

== Language
The engine will be written in C++03, likely using a custom replacement for the STL and/or the C runtime.

== Design
The engine will be based on an entity component system, fairly clean separation between independant components with certain
common ones, like platform abstraction, and simple data formats.

== Systems
Each system will be a static or dynamically loaded shared library, and expose a general interface in addition to ECS systems.
This will make it easy to add and integrate new features.
