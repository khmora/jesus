#pragma once

/*
global variables and other stuff that is probably too small to put in another 
file (and i'm too lazy to make files for all of them) would go in here.
Stuff like fonts and shit
*/
namespace GLOBAL
{
	extern HWND csgo_hwnd;

	extern bool should_send_packet;
	extern bool is_fakewalking;
	extern int choke_amount;

	extern Vector fake_pos;
	extern Vector real_angles;
	extern Vector fake_angles;
	extern float lineFakeAngle;
	extern Vector strafe_angle;
	extern float cheat_start_time;
	extern bool can_shoot_someone;
	extern int randomnumber;
	extern float flHurtTime;
	extern bool DisableAA;
	extern bool NewRound;
	extern 	bool disable_fake_lag;
	extern bool Aimbotting;
	extern 	bool do_max_fake_lag;
	extern 	bool should_choke_packets;
	using msg_t = void(__cdecl*)(const char*, ...);
	extern msg_t		Msg;
	extern std::string	old_tag;
	extern Vector FakePosition;
	extern int ground_tickz;
	extern bool CircleStraferActive;
	extern SDK::CUserCmd originalCMD;
}
namespace FONTS
{
	extern unsigned int visuals_gun_icon;
	extern unsigned int menu_tab_font;
	extern unsigned int menu_checkbox_font;
	extern unsigned int menu_slider_font;
	extern unsigned int menu_groupbox_font;
	extern unsigned int menu_combobox_font;
	extern unsigned int menu_window_font;
	extern unsigned int bigtext;
	extern unsigned int numpad_menu_font;
	extern unsigned int visuals_esp_font1;
	extern unsigned int visuals_esp_font;
	extern unsigned int visuals_xhair_font;
	extern unsigned int visuals_side_font;
	extern unsigned int visuals_name_font;
	extern unsigned int visuals_lby_font;
	extern unsigned int visuals_grenade_pred_font;
	extern 	unsigned int menulogofont;

	bool ShouldReloadFonts();
	void InitFonts();
}
namespace SETTINGS
{
	class CSettings
	{
	public:
		// returns true/false whether the function succeeds, usually returns false if file doesn't exist
		bool Save(std::string file_name);
		bool Load(std::string file_name);

		void CreateConfig(); // creates a blank config

		std::vector<std::string> GetConfigs();



		bool bhop_bool;
		bool strafe_bool;
		bool esp_bool;
		int chams_type;
		int metallic;
		int metallicw;
		bool wirehands;
		int xhair_type;
		bool tp_bool;
		bool CBasePlayerAnimState;
		bool override_keybind;
		bool aim_bool;

		bool trashtalk;
		bool trashtalk2;
		bool trashtalk3;

		int aim_type;
		bool Clantag;
		bool aa_bool;
		CColor lvmodel_col;
		bool sky_enabled;
		CColor skycolor;
		CColor night_col;
		CColor refixsign = CColor(255, 0, 0, 180);
		int aa_pitch;
		int aa_type;
		bool wolrd_enabled;
		int acc_type;
		bool bt_vis_chams_enabled;
		bool up_bool;
		bool misc_bool;
		bool HandleGameEvent;
		int config_sel;
		bool beam_bool;
		bool gradientbullet;
		bool stop_bool;
		bool auto_zeus;

		bool fakelag_adaptive_enabled = false;
		bool fakelag_on_peek = false;
		float fakelag_moving = 1.f;
		float fakelag_jumping = 1.f;
		bool vecvelocityprediction;
		bool auto_revolver_enabled;
		bool backtrack_bool;
		bool backtrack_show;
		int resolver__type;

		int positionadjustment;
		float positionadjustment_max_history = 1.f;

		int weap_bool;
		bool astro;
		int hsinair;
		bool onlyhs;
		bool bomb;
		bool bomb_beep;

		bool cs_win_panel_round;
		bool achievement_earned;
		bool secrettab;
		bool sky_enable;
		CColor spread_col;
		CColor backtrack_col;
		CColor sky_col;
		bool night_bool;
		bool buybot_enabled = false;
		bool buybot_zeus = false;
		bool buybot_grenade = false;
		int buybot_pistol = 0;
		int buybot_rifle = 0;
		int buybot_armor = 0;
		bool box_bool;
		bool esp_enemy_skeleton_enabled;
		CColor esp_enemy_skeleton_color;
		bool name_bool;
		CColor spread_Col;
		CColor backtrack_show_color;
		bool impacts;
		int AK47;
		int AWP;
		int SSG08;
		int SCAR20;
		int G3SG1;
		int M4A1;
		int M4A1S;
		int USP;
		int P2000;
		int MAC10;
		int MP9;
		int UMP45;
		int P90;
		int BIZON;
		int AUG;
		int SG553;
		int MP7;
		int P250;
		int Glock18c;
		int DUALB;
		int FAMAS;
		int Galil;
		int Deagle;
		int FiveSeven;
		int Tec9;
		int CZ75;
		bool skinenable;

		bool noflash;
		bool no_vis_recoil;
		bool health_bool;
		bool info_bool;
		bool back_bool;
		bool lag_bool;
		float alphatrans;
		bool autozeus_bool;
		bool autoknife_bool;
		bool baimifawp;
		bool baimifhpbool;
		float baimifhp;
		bool baimonkey;
		int baimkey;
		bool autorevolver_bool;
		bool fakewalk;
		int box_type;
		bool reverse_bool;
		bool multi_bool;
		struct
		{
			bool HITBOX_HEAD;
			bool HITBOX_NECK;
			bool HITBOX_PELVIS;
			bool HITBOX_BELLY;
			bool HITBOX_THORAX;
			bool HITBOX_LOWER_CHEST;
			bool HITBOX_UPPER_CHEST;
			bool HITBOX_RIGHT_THIGH;
			bool HITBOX_LEFT_THIGH;
			bool HITBOX_RIGHT_CALF;
			bool HITBOX_LEFT_CALF;
			bool HITBOX_RIGHT_FOOT;
			bool HITBOX_LEFT_FOOT;
			bool HITBOX_RIGHT_HAND;
			bool HITBOX_LEFT_HAND;
			bool HITBOX_RIGHT_UPPER_ARM;
			bool HITBOX_RIGHT_FOREARM;
			bool HITBOX_LEFT_UPPER_ARM;
			bool HITBOX_LEFT_FOREARM;

		}hitboxes;

		struct {
			float HITBOX_HEAD;
			float HITBOX_NECK;
			float HITBOX_PELVIS;
			float HITBOX_BELLY;
			float HITBOX_THORAX;
			float HITBOX_LOWER_CHEST;
			float HITBOX_UPPER_CHEST;
			float HITBOX_RIGHT_THIGH;
			float HITBOX_LEFT_THIGH;
			float HITBOX_RIGHT_CALF;
			float HITBOX_LEFT_CALF;
			float HITBOX_RIGHT_FOOT;
			float HITBOX_LEFT_FOOT;
			float HITBOX_RIGHT_HAND;
			float HITBOX_LEFT_HAND;
			float HITBOX_RIGHT_UPPER_ARM;
			float HITBOX_RIGHT_FOREARM;
			float HITBOX_LEFT_UPPER_ARM;
			float HITBOX_LEFT_FOREARM;

		}multipoints;
		bool fakefix_bool;
		bool angle_bool;
		bool tp_angle_bool;
		bool glow_bool;
		bool dist_bool;
		bool fov_bool;
		bool smoke_bool;
		bool scope_bool;
		bool predict_bool;
		bool fake_bool;
		int media_type;
		bool novis_bool;
		bool localglow_bool;
		bool duck_bool;
		bool money_bool;
		int delay_shot;
		int lag_type;
		bool cham_bool;
		bool resolve_bool;
		bool resolver2;
		bool ammo_bool;
		bool Hitmarker;
		bool spread_bool;
		bool zeusrange;
		bool freestandind;
		CColor awcolor;
		bool hitmarker;
		bool prediction;
		bool lby_timer;

		float move_lag;
		bool hitmarker_screen;
		float jump_lag;

		bool debug_bool;

		CColor vmodel_col;
		CColor imodel_col;
		CColor box_col;
		CColor name_col;
		CColor weapon_col;
		CColor distance_col;
		CColor localchams_col;
		CColor grenadepredline_col;
		CColor grenadepredbox_col;

		CColor bulletlocal_col;
		CColor bulletenemy_col;
		CColor bulletteam_col;

		CColor menu_col = LIGHTGREY;
		CColor title_bar = CColor(132, 0, 218);

		CColor menu_text = CColor(255, 255, 255, 255);

		CColor groupbox_body = PURPLE;
		CColor groupbox_outline = BLACK;

		CColor scrollbar_body = BLACK;

		CColor separator_text_color = GREY;

		CColor checkbox_not_clicked_col = GREY;
		CColor checkbox_col = PURPLE;
		CColor button_color = PURPLE;
		CColor combobox_selected = PURPLE;
		CColor combobox_item = PURPLE;
		CColor slider_col = GREY;
		CColor slider_color_value = PURPLE;
		CColor tab_col = WHITE;
		CColor tab_body_col = WHITE;
		CColor glow_col;
		CColor glowlocal_col;
		CColor fov_col;

		CColor lbytimer_color;

		int weapon;
		float auto_chance_val;
		float auto_damage_val;
		float scout_damage_val;
		float scout_chance_val;
		float awp_damage_val;
		float awp_chance_val;
		float deagle_damage_val;
		float deagle_chance_val;
		float dual_damage_val;
		float dual_chance_val;
		float other_damage_val;
		float other_chance_val;
		float chance_val;
		float damage_val;
		float delta_val;
		float point_val;
		float body_val;
		bool misc_clantag;
		bool BacktrackChams;
		bool localesp;
		bool gradient;
		float gradientspeed;
		bool localchams;
		float fov_val = 90;
		float viewfov_val = 68;

		bool flip_bool;
		int aa_side;

		bool legit_bool;
		int legit_key;
		bool rcs_bool;
		float legitfov_val;
		int legitbone_int;
		float rcsamount_min;
		float rcsamount_max;
		float legitaim_val;
		bool legittrigger_bool;
		int legittrigger_key;

		int thirdperson_int;
		float transparency_when_scoped;
		int flip_int;
		int flip_int1;

		bool glowenable;
		int glowstyle;
		bool gradientglow;
		bool glowlocal;
		int glowstylelocal;
		int hitmarker_val;

		int aa_mode;
		int a_mode;

		int aa_real_type;
		int aa_real1_type;
		int aa_real2_type;

		int aa_fake1_type2;
		int aa_fake2_type2;
		int aa_fake_type2;
		int aa_fake_type1;
		int aa_fake_type;
		int aa_fake1_type;
		int aa_fake2_type;

		int aa_pitch_type;
		int aa_pitch1_type;
		int aa_pitch2_type;

		float aa_realadditive_val;
		float aa_fakeadditive_val;

		float aa_realadditive1_val;
		float aa_fakeadditive1_val;
		float delta1_val;

		float aa_realadditive2_val;
		float aa_fakeadditive2_val;
		float delta2_val;

		float spinangle;
		float spinspeed;

		float spinangle1;
		float spinspeed1;

		float spinangle2;
		float spinspeed2;

		float spinanglefake;
		float spinspeedfake;

		float spinanglefake1;
		float spinspeedfake1;

		float spinanglefake2;
		float spinspeedfake2;

		bool lbyflickup;
		bool lbyflickup1;
		bool lbyflickup2;

		bool aa_fakeangchams_bool;

		int chamstype;
		float fov_time;
		bool rifk_arrow;

		int glowteamselection;
		bool glowteam;

		int chamsteamselection;
		int chamsteam;

		int espteamselection;
		int espteamcolourselection;
		bool boxteam;
		bool nameteam;
		bool weaponteam;
		bool flagsteam;
		bool healthteam;
		bool moneyteam;
		bool ammoteam;
		bool arrowteam;
		CColor boxteam_col;
		CColor nameteam_col;
		CColor weaponteam_col;
		CColor arrowteam_col;

		CColor teamvis_color;
		CColor teaminvis_color;
		CColor teamglow_color;

		bool matpostprocessenable;
		bool removescoping;
		bool fixscopesens;
		bool forcecrosshair;

		int autoconfigb;
		int scoutconfigb;
		int pistolconfigb;

		int quickstopkey;
		bool stop_flip;
		bool chamsmetallic;
		int flashlightkey;
		int overridekey;
		int autostopmethod;
		int overridemethod;
		bool overridething;
		bool baimthing;
		bool nospread;
		bool overrideenable;
		bool lbyenable;
		int circlestrafekey;
		float Retrack = 0.f;
		bool secretwindow;
		float circlstraferetract;
		float fakewalkspeed;
		float PreserveKillfeed;
		float daytimevalue = 50;
		bool norecoil;
		float circlemin;
		bool forcehair;
		float circlemax;
		float circlevel;
		float circlenormalizemultiplier;

		bool skinchangerenable;

		int knifeToUse;
		int bayonetID, karambitID, flipID, gutID, m9ID, huntsmanID;
		int gloveToUse;
		int bloodhoundID, driverID, handwrapsID, motoID, specialistID, sportID, hydraID;

		int uspID, p2000ID, glockID, dualberettaID, p250ID, fivesevenID, tech9ID, r8ID, deagleID;
		int novaID, xm1014ID, mag7ID, sawedoffID, m249ID, negevID;
		int mp9ID, mac10ID, mp7ID, ump45ID, p90ID, ppbizonID;
		int famasID, galilID, ak47ID, m4a4ID, m4a1sID, ssg08ID, augID, sg553ID, awpID, scar20ID, g3sg1ID;
		bool logs;

	private:
	}; extern CSettings settings;
}

extern bool using_fake_angles[65];
extern bool full_choke;
extern bool is_shooting;

extern bool in_tp;
extern bool fake_walk;

extern int resolve_type[65];

extern int target;
extern int shots_fired[65];
extern int shots_hit[65];
extern int shots_missed[65];
extern int backtrack_missed[65];

extern bool didShot;
extern bool didMiss;

extern float tick_to_back[65];
extern float lby_to_back[65];
extern bool backtrack_tick[65];

extern float lby_delta;
extern float update_time[65];
extern float walking_time[65];

extern float local_update;

extern int hitmarker_time;
extern int random_number;
extern bool hurtcalled;
extern bool menu_hide;

extern int oldest_tick[65];
extern float compensate[65][12];
extern Vector backtrack_hitbox[65][20][12];
extern float backtrack_simtime[65][12];