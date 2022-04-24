#pragma once
#include <iostream>
#include <Windows.h>
#include <xstring>
#include <string_view>
#include <TlHelp32.h>
#include <random>
#include <ShlObj_core.h>
#include <string>
#include <thread>
#include <vector>

#include "util.h"
#include "lazyimporter.h"
#include "xorstr.h"

class vec2_t
{
public:
    float x, y;

    vec2_t()
    {
        x = 0; y = 0;
    };
    vec2_t( float _x, float _y )
    {
        x = _x; y = _y;
    };

    inline vec2_t operator*( const float n ) const
    {
        return vec2_t( x * n, y * n );
    }
    inline vec2_t operator+( const vec2_t& v ) const
    {
        return vec2_t( x + v.x, y + v.y );
    }
    inline vec2_t operator-( const vec2_t& v ) const
    {
        return vec2_t( x - v.x, y - v.y );
    }
    inline void operator+=( const vec2_t& v )
    {
        x += v.x;
        y += v.y;
    }
    inline void operator-=( const vec2_t& v )
    {
        x -= v.x;
        y -= v.y;
    }

    bool operator==( const vec2_t& v ) const
    {
        return (v.x == x && v.y == y);
    }
    bool operator!=( const vec2_t& v ) const
    {
        return (v.x != x || v.y != y);
    }

    inline float length()
    {
        return sqrt( (x * x) + (y * y) );
    }
    float distance_to( const vec2_t& other )
    {
        return sqrt( pow( other.x - this->x, 2.0 ) + pow( other.y - this->y, 2.0 ) );
    }
};

class vec3_t
{
public:
    float x, y, z;

    vec3_t()
    {
        x = 0; y = 0; z = 0;
    };
    vec3_t( float _x, float _y, float _z )
    {
        x = _x; y = _y; z = _z;
    };

    vec3_t& operator+=( const vec3_t& v )
    {
        x += v.x; y += v.y; z += v.z; return *this;
    }
    vec3_t& operator-=( const vec3_t& v )
    {
        x -= v.x; y -= v.y; z -= v.z; return *this;
    }
    vec3_t& operator*=( float v )
    {
        x *= v; y *= v; z *= v; return *this;
    }
    vec3_t operator+( const vec3_t& v )
    {
        return vec3_t{ x + v.x, y + v.y, z + v.z };
    }
    vec3_t operator-( const vec3_t& v )
    {
        return vec3_t{ x - v.x, y - v.y, z - v.z };
    }
    vec3_t operator*( float fl ) const
    {
        return vec3_t( x * fl, y * fl, z * fl );
    }
    vec3_t operator*( const vec3_t& v ) const
    {
        return vec3_t( x * v.x, y * v.y, z * v.z );
    }
    vec3_t& operator/=( float fl )
    {
        x /= fl;
        y /= fl;
        z /= fl;
        return *this;
    }
    vec3_t operator/( float& fl )
    {
        return vec3_t( x / fl, y / fl, z / fl );
    }
    float length_sqr( void )
    {
        auto sqr = []( float n )
        {
            return static_cast<float>(n * n);
        };

        return (sqr( x ) + sqr( y ) + sqr( z ));
    }

    float distance_to( const vec3_t& other )
    {
        vec3_t delta;

        delta.x = x - other.x;
        delta.y = y - other.y;
        delta.z = z - other.z;

        return delta.length_sqr();
    }

    void normalize()
    {
        x = std::isfinite( x ) ? std::remainderf( x, 360.0f ) : 0.0f;
        y = std::isfinite( y ) ? std::remainderf( y, 360.0f ) : 0.0f;
        z = 0.0f;
    }
};

class vec4_t
{
public:
    float x, y, z, w;
};

struct bone_matrix
{
    BYTE _junk1[0xC];
    float x;
    BYTE _junk2[0xC];
    float y;
    BYTE _junk3[0xC];
    float z;
};

bool angle_clamp( vec3_t& angles )
{
    if ( isfinite( angles.x ) &&
        isfinite( angles.y ) &&
        isfinite( angles.z ) )
    {
        angles.x = std::clamp( angles.x, -89.0f, 89.0f );
        angles.y = std::clamp( angles.y, -180.0f, 180.0f );
        angles.z = 0.0f;
        return true;
    }

    return false;
}

bool angle_normalize( vec3_t& angles )
{
    if ( isfinite( angles.x ) &&
        isfinite( angles.y ) &&
        isfinite( angles.z ) )
    {
        angles.x = remainder( angles.x, 360.0f );
        angles.y = remainder( angles.y, 360.0f );
        return true;
    }

    return false;
}

bool angle_check( const vec3_t& angles )
{
    if ( angles.x > 89.0f
        || angles.x < -89.0f
        || angles.y > 180.0f
        || angles.y < -180.0f
        || angles.z != 0.0f )
    {
        return false;
    }

    return true;
}

bool angle_sanitize( vec3_t& angles )
{
    if ( !angle_normalize( angles )
        || !angle_clamp( angles ) )
    {
        return false;
    }

    if ( !angle_check( angles ) )
    {
        return false;
    }

    return true;
}