#pragma once

#pragma warning(push)
#pragma warning(disable : 4946) // reinterpret_cast used between related classes
#pragma warning(disable : 5204) // class has virtual functions, but its trivial destructor is not virtual
#pragma warning(disable : 5246) // '': the initialization of a sub-object should be wrapped in braces
#pragma warning(disable : 5260) // the constant variable has external\internal linkage
#include <winrt/base.h>
#pragma warning(pop)
