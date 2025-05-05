To import functions on Windows, add them with MAKE_STUB at the top of loader_win32.cpp and later in Base_InitLoader with
GET_FUNCTION or GET_FUNCTION_OPTIONAL with the right DLL, then add it to stubs_win32_*.inc with MAKE_STUB. You have to
get the right stdcall parameter size suffix, which is usually (number of parameters) * 4, except if it's cdecl, in which
case you just put a comma and no parameter to MAKE_STUB. If you add a new DLL, follow the stuff for the other ones. If
it's not on all versions, make it optional. It's also necessary to check if it's available with DECLARE_AVAILABLE and
<name>_Available() before using it. If it's not optional, you don't have to check as long as Base_InitLoader has run.