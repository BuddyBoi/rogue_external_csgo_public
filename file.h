#pragma once

class Files
{
	public:
	void OnSetup();
	int	ReadInt( const char* element, const char* item );
	float ReadFloat( const char* element, const char* item );
	bool ReadBool( const char* element, const char* item );
	void WriteInt( const char* element, const char* item, int value );
	void WriteFloat( const char* element, const char* item, float value );
	void WriteBool( const char* element, const char* item, bool value );
	private:
	char m_fileName[ 255 ];
};

extern Files* files;