= Introduction
This document outlines the design of the custom-made 3D game engine for False King and subsequent Randomcode Developers games.

== Language
The engine is being written in C++14, with a custom runtime/container library and almost no libraries, maximizing portability.

== Target platforms
The engine is designed to be extremely portable. It currently supports Windows and Linux, and will target Xbox Series X|S,
PlayStation 5, and Nintendo Switch/Switch 2.

== Benefits for developers
The engine will be as open source as possible (essentially, anything that I'm legally allowed to open source). It will give me
and any interested developers a solid foundation to build advanced games on. It also benefits me as a huge learning experience.

== Design
The engine will be based on an entity component system, fairly clean separation between independant components with certain
common ones, like platform abstraction, and simple data formats.

== Systems
Each system will be a static or dynamically loaded shared library, and expose a general interface in addition to ECS systems.
This will make it easy to add and integrate new features.
