#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include <xstring>
#include <string_view>
#include <TlHelp32.h>
#include <random>
#include <ShlObj_core.h>
#include <string>
#include "lazyimporter.h"
#include "xorstr.h"

namespace util
{
	std::string to_lower( std::string str )
	{
		std::transform( str.begin(), str.end(), str.begin(), static_cast< int( * )( int ) >( ::tolower ) );
		return str;
	}

	std::string to_utf8( std::wstring wstr )
	{
		if( wstr.empty() )
			return {};

		const auto size = WideCharToMultiByte( CP_UTF8, 0, wstr.data(), ( int ) wstr.size(), 0, 0, 0, 0 );
		auto ret = std::string( size, 0 );

		WideCharToMultiByte( CP_UTF8, 0, wstr.data(), ( int ) wstr.size(), (LPSTR)ret.data(), size, 0, 0 );

		return ret;
	}

	std::wstring to_unicode( std::string str )
	{
		if( str.empty() )
			return {};

		const auto size = MultiByteToWideChar( CP_UTF8, 0, str.data(), ( int ) str.size(), 0, 0 );
		auto ret = std::wstring( size, 0 );

		MultiByteToWideChar( CP_UTF8, 0, str.data(), ( int ) str.size(), (LPWSTR)ret.data(), size);

		return ret;
	}

	bool is_process_open( const std::vector<std::pair<std::uint32_t, std::string>>& vec_processes, std::string str_proc )
	{
		if ( vec_processes.empty() )
			return {};

		if ( str_proc.empty() )
			return {};

		auto target = to_lower( str_proc.data() );
		for ( const auto& ctx : vec_processes )
		{
			auto ep = to_lower( ctx.second );
			if ( target.find( ".exe" ) == std::string::npos )
			{
				if ( ep.find( target ) == std::string::npos )
					continue;
			}
			else
			{
				if ( ep != target )
					continue;
			}

			const auto h_process = LI_FN( OpenProcess ).cached()(PROCESS_VM_READ, false, ctx.first);
			if ( h_process != nullptr )
			{
				LI_FN( CloseHandle ).cached()(h_process);
				return true;
			}
		}

		return {};
	}

	bool kill_process( const std::vector<std::pair<std::uint32_t, std::string>>& vec_processes, std::string str_proc )
	{
		if ( vec_processes.empty() )
			return {};

		if ( str_proc.empty() )
			return {};

		auto executed = false;
		auto target = to_lower( str_proc.data() );
		for ( const auto& ctx : vec_processes )
		{
			auto ep = to_lower( ctx.second );
			if ( target.find( ".exe" ) == std::string::npos )
			{
				if ( ep.find( target ) == std::string::npos )
					continue;
			}
			else
			{
				if ( ep != target )
					continue;
			}

			const auto h_process = LI_FN( OpenProcess )(PROCESS_TERMINATE, false, ctx.first);
			if ( h_process != nullptr )
			{
				LI_FN( TerminateProcess ).cached()(h_process, 9);
				LI_FN( CloseHandle ).cached()(h_process);

				executed = true;
			}
		}

		return executed;
	}

	std::uint32_t get_process_id_by_name( const std::vector<std::pair<std::uint32_t, std::string>>& vec_processes, std::string str_proc )
	{
		if ( vec_processes.empty() )
			return {};

		if ( str_proc.empty() )
			return {};

		auto target = to_lower( str_proc.data() );
		for ( const auto& ctx : vec_processes )
		{
			auto ep = to_lower( ctx.second );
			if ( target.find( ".exe" ) == std::string::npos )
			{
				if ( ep.find( target ) == std::string::npos )
					continue;
			}
			else
			{
				if ( ep != target )
					continue;
			}

			return ctx.first;
		}

		return {};
	}

	std::vector<std::pair<std::uint32_t, std::string>> get_process_list()
	{
		std::vector<std::pair<std::uint32_t, std::string>> vec_list{};

		const auto h_handle = LI_FN( CreateToolhelp32Snapshot ).cached()(TH32CS_SNAPPROCESS, NULL);

		PROCESSENTRY32 m_entry{};
		m_entry.dwSize = sizeof( m_entry );

		if ( !Process32First( h_handle, &m_entry ) )
			return {};

		while ( Process32Next( h_handle, &m_entry ) )
			vec_list.emplace_back( m_entry.th32ProcessID, to_utf8( m_entry.szExeFile ) ); // string::to_utf8

		LI_FN( CloseHandle ).cached()(h_handle);

		return vec_list;
	}	

	DWORD get_hwid()
	{
		//rem
	}

	void log( std::string message )
	{
		std::cout << xorstr_( "[cheat-name-here]" ) + message << std::endl;
	}

	template <typename T>
	T random_int( T start, T end )
	{
		std::random_device rd;
		std::mt19937_64 rng( rd() );
		std::uniform_int_distribution<T> uni( start, end );

		return ( T ) uni( rng );
	}

	template <typename T>
	T random_float( T start, T end )
	{
		std::random_device rd;
		std::mt19937_64 rng( rd() );
		std::uniform_real_distribution<T> uni( start, end );

		return ( T ) uni( rng );
	}

	std::pair<int, int> get_window_size( HWND h_wind )
	{
		static RECT rect_window;
		LI_FN(GetWindowRect).cached()( h_wind, &rect_window );
		return std::make_pair( rect_window.right, rect_window.bottom );
	}

	bool file_exists( std::string path )
	{
		return ( (LI_FN(GetFileAttributesA).cached() ) ( path.data() ) != 0xFFFFFFFF );
	}

	bool file_directory_exists( const char* path )
	{
		const auto dw_attributes = (LI_FN(GetFileAttributesA).cached() ) ( path );
		if( dw_attributes == INVALID_FILE_ATTRIBUTES )
			return false;

		return ( dw_attributes & FILE_ATTRIBUTE_DIRECTORY );
	}

	std::string random_title()
	{
		std::string console_title;
		console_title += std::to_string( util::random_int( 100000000000, 999999999999 ) );
		console_title += std::to_string( util::random_int( 100000000000, 999999999999 ) );
		console_title += std::to_string( util::random_int( 100000000000, 999999999999 ) );
		console_title += std::to_string( util::random_int( 100000000000, 999999999999 ) );
		console_title += std::to_string( util::random_int( 100000000000, 999999999999 ) );
		console_title += std::to_string( util::random_int( 100000000000, 999999999999 ) );
		console_title += std::to_string( util::random_int( 100000000000, 999999999999 ) );
		console_title += std::to_string( util::random_int( 100000000000, 999999999999 ) );
		console_title += std::to_string( util::random_int( 100000000000, 999999999999 ) );
		return console_title;
	}
}