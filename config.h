#pragma once
class Settings
{
	public:
	void Setup();
	void SaveConfig();
	void LoadConfig();

	struct aimbot
	{
		bool enabled = false;
		int key = 0x01;
		float fov = 2.0f;
		float smooth = 7;
		float rcs_x = 1.0f;
		float rcs_y = 1.0f;
		float sens = 2.0f;
		int type = 1;
	} aimbot;

	struct triggerbot
	{
		bool enabled = false;
		int key = 0x06;
	} triggerbot;

	struct visuals
	{
		bool enabled = false;
		bool recoil_crosshair = false;		
	} visuals;

	struct misc
	{
		bool bhop = false;
		bool radar = false;
		bool custom_flash = false;
		float custom_flash_amount = 255.0f;
		bool custom_fov = false;
		int custom_fov_amount = 90;
		bool multi_threads = false;
	} misc;

	private:
	bool CofigExists();
};
extern Settings* settings;