/*
* HL Loader
* Copyright (c) goldfinder (goldfinder@ifrance.com) 2002
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/


#include "RedirectFunction.h"


#define MakePtr( cast, ptr, addValue )( cast )( ( DWORD )( ptr ) + ( DWORD )( addValue ) )


void *InterceptDllCall( HMODULE hModule, char *szDllName, char *szFunctionName, DWORD pNewFunction )
{
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
	PIMAGE_THUNK_DATA pThunk;
	DWORD dwOldProtect;
	DWORD dwOldProtect2;
	void *pOldFunction;

	if( !( pOldFunction = GetProcAddress( GetModuleHandle( szDllName ), szFunctionName ) ) )
		return 0;

	pDosHeader = ( PIMAGE_DOS_HEADER )hModule;
	if( pDosHeader->e_magic != IMAGE_DOS_SIGNATURE )
		return( NULL );

	pNTHeader = MakePtr( PIMAGE_NT_HEADERS, pDosHeader, pDosHeader->e_lfanew );
	if( pNTHeader->Signature != IMAGE_NT_SIGNATURE
	|| ( pImportDesc = MakePtr( PIMAGE_IMPORT_DESCRIPTOR, pDosHeader, pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress ) ) == ( PIMAGE_IMPORT_DESCRIPTOR )pNTHeader )
		return( NULL );

	while( pImportDesc->Name )
	{
		char *szModuleName = MakePtr( char *, pDosHeader,	pImportDesc->Name );
		if( !stricmp( szModuleName, szDllName ) )
			break;
		pImportDesc++;
	}
	if( pImportDesc->Name == NULL )
		return( NULL );

	pThunk = MakePtr( PIMAGE_THUNK_DATA, pDosHeader,	pImportDesc->FirstThunk );
	while( pThunk->u1.Function )
	{
		if( pThunk->u1.Function == ( DWORD * )pOldFunction )
		{
			VirtualProtect( ( void * )&pThunk->u1.Function, sizeof( DWORD ), PAGE_EXECUTE_READWRITE, &dwOldProtect );
			pThunk->u1.Function = ( DWORD * )pNewFunction;
			VirtualProtect( ( void * )&pThunk->u1.Function, sizeof( DWORD ), dwOldProtect, &dwOldProtect2 );
			return( pOldFunction );
		}
		pThunk++;
	}
	return( NULL ); 
}