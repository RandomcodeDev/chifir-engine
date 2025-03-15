ntdll.dll has the subset of the CRT available to native applications, and the
launcher uses some of those functions. Microsoft only ships the ntdllp.lib
import library that includes those functions in the WDK. Since people don't
usually have that (I don't even know if I do), these import libraries with the
functions the launcher uses are here.
