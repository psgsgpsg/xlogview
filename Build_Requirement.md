# Introduction #

Build Requirement


# Details #

From SVN version 7, logview uses **boost::regex** to enhance text filter.

Requirement for boost includes:
  1. Multi thread
  1. Static Link
  1. Regex component.

**Boost** used in this project can be downloaded from http://code.google.com/p/xlogview/downloads/detail?name=boost_1.51.0_library.7z

So before you build **LogView**, you need to modify **"Additional Include Directory"** and **"Additional Library Directory"** to indicate the right position of **boost** library.