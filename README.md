# PixSDLib

Note: currently in development

PixSDLib is a sprite-based foundational 2D/3D game engine core written in C++,
built on top of SDL2. It provides core systems such as input handling, audio,
sprite rendering, and a traditional double-buffered game loop lifecycle.

---

## Design Philosophy

PixSDLib assumes an SDL2-based environment, including rendering, and a
traditional double-buffered game loop architecture. Beyond this assumption,
the library is designed as a scalable foundation for real-time applications,
with a strong focus on flexibility, robustness, and clarity.

The architecture favors modular building blocks with clear and minimal
responsibilities. Object-oriented design is used where it provides clear value,
while avoiding unnecessary abstraction.

---

## C++ Usage

PixSDLib intentionally uses a restricted subset of C++, sometimes described as
"lightweight C++" or "C with classes". The focus is on enforcing invariants,
supporting polymorphism where appropriate, and maintaining predictable runtime
behavior.

Templates are used sparingly and only where they provide clear practical
benefits.

---

## Build

- Visual Studio 2026
- C++14 (by design, to maintain simplicity and wide compatibility)
- SDL2

---

## Author

PixSDLib is developed by RetroMasters  
Real name: Juri Schupilo
