#pragma once
#include "includes.h"
#include "globals.h"
namespace memory
{
    template <class t>
    bool write( uintptr_t address, t value )
    {
        return LI_FN(WriteProcessMemory).cached()( globals::_handle, ( PVOID ) address, &value, sizeof( t ), NULL );
    }

    template <class t>
    t read( uintptr_t address )
    {
        t data{};
        LI_FN(ReadProcessMemory).cached()( globals::_handle, ( PVOID ) address, &data, sizeof( t ), NULL );
        return data;
    }

    uintptr_t get_module( const wchar_t* modName )
    {
        uintptr_t modBaseAddr = 0;
        HANDLE hSnap = LI_FN(CreateToolhelp32Snapshot).cached()( TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, globals::process_id );
        if( hSnap != INVALID_HANDLE_VALUE )
        {
            MODULEENTRY32 modEntry;
            modEntry.dwSize = sizeof( modEntry );
            if( Module32First( hSnap, &modEntry ) )
            {
                do
                {
                    if( !_wcsicmp( modEntry.szModule, modName ) )
                    {
                        modBaseAddr = ( uintptr_t ) modEntry.modBaseAddr;
                        break;
                    }
                } while( Module32Next( hSnap, &modEntry ) );
            }
        }
        LI_FN(CloseHandle).cached()( hSnap );
        return modBaseAddr;
    }
}