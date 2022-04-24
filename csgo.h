#pragma once
#include "classes.h"

//enemy visuals color
#define EnemyPen 0x000000FF

//setup GDI for visuals
HDC hdc = GetDC( FindWindowA( NULL, xorstr_( "Counter-Strike: Global Offensive - Direct3D 9" ) ) );
HBRUSH EnemyBrush = LI_FN( CreateSolidBrush ).cached()(0x000000FF);

vec3_t calc_aim( vec3_t& source, vec3_t& destination, vec3_t& viewAngles )
{
	vec3_t delta = source - destination;
	auto radians_to_degrees = []( float radians )
	{
		return radians * 180 / static_cast<float>(3.14159265358979323846);
	};
	vec3_t angles;
	angles.x = radians_to_degrees( atanf( delta.z / std::hypotf( delta.x, delta.y ) ) ) - viewAngles.x;
	angles.y = radians_to_degrees( atanf( delta.y / delta.x ) ) - viewAngles.y;
	angles.z = 0.0f;

	if ( delta.x >= 0.0 )
		angles.y += 180.0f;

	//determine rcs inside of calcangle to do our fancy recoil-based fov
	//you don't have to do it inside of calcangle at all
	//but my internal I ported it from it makes sense I swear. Pass a struct or something of recoil settings idk

	if ( true ) //is rcs enabled, currently no setting to disable
	{
		vec3_t vVecpunch;
		vec3_t Delta;

		auto aim_punch = g_localplayer.aim_punch_angle() * 2;

		//rcs-independant smoothing?

		Delta.x = aim_punch.x;
		Delta.y = aim_punch.y;

		angles.x -= Delta.x * settings->aimbot.rcs_x;
		angles.y -= Delta.y * settings->aimbot.rcs_y + 0.04f;

		angle_normalize( angles );
	}

	return angles;
}

bool world_to_screen( vec3_t pos, vec2_t& screen ) // converts 3D coords to 2D coords
{
	float screen_width = (float)globals::res_x;
	float screen_height = (float)globals::res_y;

	float Matrix[16]; // [4][4]: 4*4 = 16

	LI_FN( ReadProcessMemory ).cached()(globals::_handle, (PFLOAT)(globals::client + offsets::dwViewMatrix), &Matrix, sizeof( Matrix ), 0);

	//Matrix-vector Product, multiplying world(eye) coordinates by projection matrix = clipCoords
	vec4_t clipCoords;
	clipCoords.x = pos.x * Matrix[0] + pos.y * Matrix[1] + pos.z * Matrix[2] + Matrix[3];
	clipCoords.y = pos.x * Matrix[4] + pos.y * Matrix[5] + pos.z * Matrix[6] + Matrix[7];
	clipCoords.z = pos.x * Matrix[8] + pos.y * Matrix[9] + pos.z * Matrix[10] + Matrix[11];
	clipCoords.w = pos.x * Matrix[12] + pos.y * Matrix[13] + pos.z * Matrix[14] + Matrix[15];

	if ( clipCoords.w < 0.1f ) // if the camera is behind our player don't draw, i think?
		return false;


	vec3_t NDC;
	// Normalize our coords
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	// Convert to window coords (x,y)
	screen.x = (screen_width / 2 * NDC.x) + (NDC.x + screen_width / 2);
	screen.y = -(screen_height / 2 * NDC.y) + (NDC.y + screen_height / 2);
	return true;
}

int get_nearest_bone( c_player entity )
{
	float best_dist = 360.f;
	int aim_bone = 8;

	std::vector<int> target_bones{ 8, 7, 6, 5, 3 };

	for ( int i : target_bones )
	{
		vec3_t local_eye = g_localplayer.get_local_eye();
		vec3_t local_angles = g_engine.get_viewangles();
		vec3_t target_pos = entity.get_bone_pos( i );
		auto angle = calc_aim( local_eye, target_pos, local_angles );
		angle_normalize( angle );
		auto this_dist = std::hypotf( angle.x, angle.y );

		if ( best_dist > this_dist )
		{
			best_dist = this_dist;
			aim_bone = i;
			continue;
		}
	}

	return aim_bone;
}

int find_target()
{
	float best_fov = settings->aimbot.fov;
	auto best_target = 0;

	for ( int i = 1; i != 32; i++ )
	{
		c_player entity = c_player( i );
		auto entity_bone_position = entity.get_bone_pos( get_nearest_bone( entity ) );
		auto local_eye_pos = g_localplayer.get_local_eye();
		auto distance = local_eye_pos.distance_to( entity_bone_position );

		if ( !entity.address
			|| entity.address == g_localplayer.address
			|| entity.get_dormant()
			|| entity.get_health() < 1
			|| entity.get_team() == g_localplayer.get_team()
			)
			continue;

		vec3_t local_angles = g_engine.get_viewangles();
		vec3_t aim_angle = calc_aim( local_eye_pos, entity_bone_position, local_angles );
		angle_normalize( aim_angle );
		auto fov = std::hypotf( aim_angle.x, aim_angle.y ); //add distance based fov soon
		if ( fov < best_fov )
		{
			best_fov = fov;
			best_target = i;
		}
	}
	return best_target;
}

void draw_filled_rect( int x, int y, int w, int h )
{
	RECT rect = { x, y, x + w, y + h };
	FillRect( hdc, &rect, EnemyBrush );
}

void draw_box( int x, int y, int w, int h, int thickness )
{
	draw_filled_rect( x, y, w, thickness ); //Top horiz line
	draw_filled_rect( x, y, thickness, h ); //Left vertical line
	draw_filled_rect( (x + w), y, thickness, h ); //right vertical line
	draw_filled_rect( x, y + h, w + thickness, thickness ); //bottom horiz line
}