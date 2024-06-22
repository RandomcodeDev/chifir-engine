// Implementation details of Windows DLL shenanigans

#pragma once

// Get the address of NTDLL, find LdrGetProcedureAddress manually, load any other system DLLs/functions
extern bool Plat_InitLoader();
