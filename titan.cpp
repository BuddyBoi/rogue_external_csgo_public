#include "includes.h"
#include "offsets.h"
#include "globals.h"
#include "memory.h"
#include "classes.h"
#include "config.h"
#include <filesystem>
#include "features.h"

namespace cheat
{
	//run once before features
	void setup()
	{
		while ( LI_FN( FindWindowA ).cached()(NULL, xorstr_( "Counter-Strike: Global Offensive - Direct3D 9" )) == 0 )
		{
			Sleep( 1000 );
			std::cout << xorstr_( "Waiting for counter-strike" ) << std::endl;
		}

		std::cout << xorstr_( "Starting setup" ) << std::endl;
		//get process id
		HWND _hwnd = LI_FN( FindWindowA ).cached()(NULL, xorstr_( "Counter-Strike: Global Offensive - Direct3D 9" ));
		LI_FN( GetWindowThreadProcessId ).cached()(_hwnd, &globals::process_id);
		LI_FN( CloseWindow ).cached()(_hwnd);

		if ( !_hwnd
			|| !globals::process_id )
		{
			Sleep( 1000 );
			setup();
		}


		std::cout << xorstr_( "[ setup ] window found : " ) << _hwnd << std::endl;
		std::cout << xorstr_( "[ setup ] process_id found : " ) << globals::process_id << std::endl;
		globals::client = memory::get_module( xorstr_( L"client.dll" ) );
		globals::engine = memory::get_module( xorstr_( L"engine.dll" ) );

		std::cout << xorstr_( "client :" ) << globals::client << std::endl;
		std::cout << xorstr_( "engine :" ) << globals::engine << std::endl;

		if ( !globals::client
			|| !globals::engine )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( 1500 ) );
			std::cout << xorstr_( "Modules not found. waiting" ) << std::endl;
			setup();
		}

		std::cout << xorstr_( "[ setup ] modules found : " ) << globals::client << " - " << globals::engine << std::endl;
		globals::_handle = LI_FN( OpenProcess )(FILE_ALL_ACCESS, false, globals::process_id);
		std::cout << xorstr_( "[ setup ] handle created : " ) << globals::_handle << std::endl;
	}

	void cache()
	{
		while ( true )
		{
			g_localplayer = c_player();

			//IMPROVEMENT:
			//create a cache in the player class to cache the localplayer's data
			//example - in cache save get_health to i_health
			//and in each function you read localplayer.get_health(), use localplayer.i_health instead
			//this limits reading the same variable over and over within a limited timeframe

			if ( !g_localplayer.address )
			{
				std::this_thread::sleep_for( std::chrono::milliseconds( 250 ) );
				continue;
			}

			if ( GetAsyncKeyState( VK_F3 ) )
			{
				settings->LoadConfig();
				LI_FN(Beep).forwarded_safe_cached()( 500, 150 );
				std::cout << xorstr_( "[ config ] loaded settings.cfg" ) << std::endl;
			}

			//insert delayed features here

			std::this_thread::sleep_for( std::chrono::milliseconds( 250 ) );
		}
	}

	void features()
	{
		//setup threads
		std::thread t_vs( features::visuals );
		std::thread t_bhop( features::bhop );

		while ( true )
		{
			if ( !g_localplayer.address )
			{
				std::this_thread::sleep_for( std::chrono::milliseconds( 250 ) );
				std::cout << xorstr_( "invalid localplayer" ) << std::endl;
				globals::b_run_features = false;
				continue;
			}
			globals::b_run_features = true;

			features::triggerbot();
			features::aimbot();

			std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
		}
	}

	void run()
	{
		setup();
		std::thread cache_thread( cache );
		std::thread features_thread( features );

		while ( !globals::b_unloading )
		{
			std::this_thread::sleep_for( std::chrono::milliseconds( 5000 ) );
		}

		LI_FN( CloseHandle ).cached()(globals::_handle);
	}
}

int main()
{
	//IMPROVEMENT:
	//if you want this to be Undetected or service this to others, you can do any variation of the following:
	//-inject the external into a native process or even a system process - research online. Injection is always easier to service to others
	//-don't write to memory. only do aim+trigger+visuals, maybe bhop with spacebar. This isn't required for VAC, but helps if you're desperate
	//-change Lazyimporter functions from .cached to another variation of .safe and .cached. Lookup lazyimporter for reference
	//-Please don't junkcode this. This source is very easy to make undetected, and VAC is a terrible usermode anti-cheat.

	SetConsoleTitleA( util::random_title().c_str() );

	if ( std::filesystem::exists( xorstr_( "settings.cfg" ) ) )
	{
		settings->LoadConfig();
		std::cout << xorstr_( "[ config ] loaded settings.cfg" ) << std::endl;
		cheat::run();
	}
	else
	{
		settings->SaveConfig();
		std::cout << xorstr_( "Config [settings.config] saved, Modify your settings and restart" ) << std::endl;
		std::this_thread::sleep_for( std::chrono::milliseconds( 10000 ) );
		return 0;
	}
}