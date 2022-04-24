#include "config.h"
#include "file.h"
#include <stdio.h>
#include <string>
#include <windows.h>

Settings* settings = new Settings();

void Settings::Setup()
{
	if( CofigExists() )
	{
		LoadConfig();
	}
	else
	{
		SaveConfig();
	}
}

void Settings::SaveConfig()
{
	files->OnSetup();
	files->WriteBool( "Aimbot", "enabled", aimbot.enabled );
	files->WriteInt( "Aimbot", "type", aimbot.type );
	files->WriteInt( "Aimbot", "key", aimbot.key );
	files->WriteFloat( "Aimbot", "fov", aimbot.fov );
	files->WriteFloat( "Aimbot", "smoothing", aimbot.smooth );
	files->WriteFloat( "Aimbot", "rcs_x", aimbot.rcs_x );
	files->WriteFloat( "Aimbot", "rcs_y", aimbot.rcs_y );	
	files->WriteFloat( "Aimbot", "sens", aimbot.sens );

	files->WriteBool( "Triggerbot", "enabled", triggerbot.enabled );
	files->WriteInt( "Triggerbot", "key", triggerbot.key );

	files->WriteBool( "Visuals", "enabled", visuals.enabled );
	files->WriteBool( "Visuals", "recoil_crosshair", visuals.recoil_crosshair);

	files->WriteBool( "Misc", "bhop", misc.bhop );
	files->WriteBool( "Misc", "radar", misc.radar );	
	files->WriteBool( "Misc", "custom_flash_enable", misc.custom_flash );
	files->WriteFloat( "Misc", "custom_flash_amount", misc.custom_flash_amount );
	files->WriteBool( "Misc", "custom_fov_enable", misc.custom_fov );
	files->WriteInt( "Misc", "custom_fov_amount", misc.custom_fov_amount );
	files->WriteBool( "Misc", "fast_esp", misc.multi_threads );

	Beep( 195, 200 );
}

void Settings::LoadConfig()
{
	if( CofigExists() )
	{
		files->OnSetup();
		aimbot.enabled = files->ReadBool( "Aimbot", "enabled" );
		aimbot.type = files->ReadInt( "Aimbot", "type" );
		aimbot.key = files->ReadInt( "Aimbot", "key" );
		aimbot.fov = files->ReadFloat( "Aimbot", "fov" );
		aimbot.smooth = files->ReadFloat( "Aimbot", "smoothing" );
		aimbot.rcs_x = files->ReadFloat( "Aimbot", "rcs_x" );
		aimbot.rcs_y = files->ReadFloat( "Aimbot", "rcs_y" );
		aimbot.sens = files->ReadFloat( "Aimbot", "sens" );

		triggerbot.enabled = files->ReadBool( "Triggerbot", "enabled" );
		triggerbot.key = files->ReadInt( "Triggerbot", "key" );

		visuals.enabled = files->ReadBool( "Visuals", "enabled" );
		visuals.recoil_crosshair = files->ReadBool( "Visuals", "recoil_crosshair" );

		misc.bhop = files->ReadBool( "Misc", "bhop" );
		misc.radar = files->ReadBool( "Misc", "radar" );
		misc.custom_flash = files->ReadBool( "Misc", "custom_flash_enable" );
		misc.custom_flash_amount = files->ReadFloat( "Misc", "custom_flash_amount" );
		misc.custom_fov = files->ReadBool( "Misc", "custom_fov_enable" );
		misc.custom_fov_amount = files->ReadInt( "Misc", "custom_fov_amount" );
		misc.multi_threads = files->ReadBool( "Misc", "fast_esp" );
	}
}

bool Settings::CofigExists()
{
	struct stat buffer;
	return ( stat( "settings.cfg", &buffer ) == 0 );
}
