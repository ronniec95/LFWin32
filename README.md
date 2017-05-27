
# LFWin32
Lightweight c++14 Windows GUI controls wrapper

# Motivation
* I enjoy coding in C++ especially with the latest C++11/14 constructs. As most people have realised, C++ is different from C and coding using a C style leads to brittle code.
* C++14 code tends to be small and efficient. I don't want to write a simple GUI and have it take up 100MB of memory.
* I want to knock up a GUI as fast as someone in C# without the overhead
* The MFC and WTL APIs are great but C++ has moved on. I want functional style interface with minimal thinking needed
* GUI libraries for window are either MFC, WTL or directly Win32. To really utilise MFC or WTL you need to understand the Win32 API as they are all based off that.

As a side effect, given the light weight nature of these wrappers, it can serve as a introduction to the win32 controls

# Design
* There should be 0 overhead beyond calling the win32 calls if at all possible. No vtables, no multiple indirections. Constexpr and template metaprogramming to do as much as possible at compile time. 
* Use static asserts wherever possible
* As much logging as possible in debug mode, but __noop in release
* Fluent style API to configure controls 
* Utilise signal-slot mechanism to decouple event handlers from their sources
* Use Win32 API constants and types so documentation is easy to find
* Support GUI controls such as filedialog
 
# Implementation
* Single header file

# Long term plan
* Add Direct2d and Direct3d charting, a 3d performant graph would be really useful
* Create an excel like grid based on list box and HeaderControl

