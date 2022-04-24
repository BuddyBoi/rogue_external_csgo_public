#pragma once
#include "includes.h"
#include "memory.h"
#include "offsets.h"
#include <math.h>

class c_engine
{
public:
	DWORD get_clientstate()
	{
		return memory::read<DWORD>( globals::engine + offsets::dwClientState );
	}
	bool is_ingame()
	{
		return memory::read<DWORD>( get_clientstate() + offsets::dwClientState_State ) == 6;
	}
	vec3_t get_viewangles()
	{
		DWORD client_state = memory::read<DWORD>( globals::engine + offsets::dwClientState );
		vec3_t view_angles = memory::read<vec3_t>( client_state + offsets::dwClientState_ViewAngles );
		return view_angles;
	}
	void set_viewangles( vec3_t view_angles )
	{
		memory::write<vec3_t>( get_clientstate() + offsets::dwClientState_ViewAngles, view_angles );
	}
};
c_engine g_engine;

class c_client
{
public:
	uintptr_t get_entity_from_index( int index )
	{
		return memory::read<uintptr_t>( globals::client + offsets::dwEntityList + (index * 0x10) );
	}
	void force_attack()
	{
		memory::write<int>( globals::client + offsets::dwForceAttack, 6 );
	}
	DWORD get_glow_object()
	{
		return memory::read<DWORD>( globals::client + offsets::dwGlowObjectManager );
	}
};
c_client g_client;

//improve by creating a player-cache
class c_player
{
public:
	std::uintptr_t address;
	c_player()
	{
		address = memory::read<uintptr_t>( globals::client + offsets::dwLocalPlayer );
	}
	c_player( int index )
	{
		address = g_client.get_entity_from_index( index );
	}
	uint32_t get_health()
	{
		return memory::read<uint32_t>( address + offsets::m_iHealth );
	}
	uint32_t get_team()
	{
		return memory::read<uint32_t>( address + offsets::m_iTeamNum );
	}
	bool get_spotted()
	{
		return memory::read<uint32_t>( address + offsets::m_bSpotted );
	}
	bool set_spotted( bool value )
	{
		if ( memory::write<uint32_t>( address + offsets::m_bSpotted, value ) )
			return true;
	}
	bool get_dormant()
	{
		return memory::read<uint32_t>( address + offsets::m_bDormant );
	}
	float get_flashalpha()
	{
		return memory::read<float>( address + offsets::m_flFlashMaxAlpha );
	}
	bool set_flashalpha( float value )
	{
		if ( memory::write<float>( address + offsets::m_flFlashMaxAlpha, value ) )
			return true;
	}
	uint32_t get_flags()
	{
		return memory::read<uint32_t>( address + offsets::m_fFlags );
	}
	uint32_t get_classid()
	{
		return memory::read<uint32_t>( memory::read<DWORD>( memory::read<DWORD>( memory::read<DWORD>( address + 0x8 ) + 0x8 ) + 0x1 ) + 0x14 );
	}
	bool is_defusing()
	{
		return memory::read<bool>( address + offsets::m_bIsDefusing );
	}
	uint32_t get_armor()
	{
		return memory::read<uint32_t>( address + offsets::m_ArmorValue );
	}
	uint32_t get_comp_wins()
	{
		return memory::read<uint32_t>( address + offsets::m_iCompetitiveWins );
	}
	bool has_kit()
	{
		return memory::read<bool>( address + offsets::m_bHasDefuser );
	}
	bool is_scoped()
	{
		return memory::read<bool>( address + offsets::m_bIsScoped );
	}
	bool get_immunity()
	{
		return memory::read<bool>( address + offsets::m_bGunGameImmunity );
	}
	DWORD32 get_bone_matrix()
	{
		return memory::read<DWORD32>( address + offsets::m_dwBoneMatrix );
	}
	uint32_t get_glow_index()
	{
		return memory::read<uint32_t>( address + offsets::m_iGlowIndex );
	}
	float get_sensitivity()
	{
		DWORD sensitivity = memory::read<DWORD>( globals::client + offsets::dwSensitivity ) ^ (DWORD)(globals::client + offsets::dwSensitivityPtr);
		return *(float*)&sensitivity;
	}
	bool set_sensitivity( DWORD value )
	{
		if ( memory::write<DWORD>( (globals::client + offsets::dwSensitivity) ^ (DWORD)(globals::client + offsets::dwSensitivityPtr), value ) )
			return true;
	}
	DWORD32 get_crosshair_id()
	{
		return memory::read<DWORD32>( address + offsets::m_iCrosshairId );
	}
	c_player get_crosshair_entity()
	{
		c_player enemy;
		enemy.address = memory::read<DWORD>( globals::client + offsets::dwEntityList + (c_player().get_crosshair_id() - 1) * 0x10 );
		return enemy;
	}
	vec3_t aim_punch_angle()
	{
		return memory::read<vec3_t>( address + offsets::m_aimPunchAngle );
	}
	vec3_t get_origin()
	{
		return memory::read<vec3_t>( address + offsets::m_vecOrigin );
	}
	vec3_t get_bone_pos( int bone )
	{
		DWORD bone_matrix_ptr = memory::read<DWORD>( address + offsets::m_dwBoneMatrix );
		bone_matrix bone_pos = memory::read<bone_matrix>( bone_matrix_ptr + bone * 0x30 );
		return vec3_t( { bone_pos.x, bone_pos.y, bone_pos.z } );
	}
	vec3_t get_local_eye()
	{
		vec3_t vec_origin = get_origin();
		vec3_t vec_offset = memory::read<vec3_t>( address + offsets::m_vecViewOffset );

		return vec_origin + vec_offset;
	}
	uint32_t get_weapon()
	{
		return memory::read<uint32_t>( address + offsets::m_hActiveWeapon );
	}
	uint32_t get_weapon_entity()
	{
		return memory::read<uint32_t>( globals::client + offsets::dwEntityList + ((get_weapon() & 0xFFF) - 1) * 0x10 );
	}
	uint32_t get_weapon_index()
	{
		return memory::read<uint32_t>( get_weapon_entity() + 0x2d90 + 0x40 + 0x1ea );
	}
};
c_player g_localplayer;