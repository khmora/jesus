#include "../includes.h"
#include "../UTILS/interfaces.h"
#include "../SDK/IEngine.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CInputSystem.h"
#include "../UTILS/render.h"
#include "../xdxdxd.h"
#include "../SDK/ConVar.h"
#include "Components.h"
#include "..\ImGui\imgui.h"
#include <d3d9.h>
#include "menu_framework.h"
#include "../HOOKS/hooks.h"
#include <winuser.h>

int menutab;

int AutoCalc(int va)
{
	if (va == 1)
		return va * 35;
	else if (va == 2)
		return va * 34;
	else
		return va * 25 + 7.5;
}

namespace MENU
{
	const std::string currentDateTime() {
		time_t     now = time(0);
		struct tm  tstruct;
		char       buf[80];
		tstruct = *localtime(&now);
		strftime(buf, sizeof(buf), "%Y | %d | %X", &tstruct);

		return buf;
	}
	void InitColors()
	{
		using namespace PPGUI_PP_GUI;


		colors[WINDOW_BODY] = CColor(22, 22, 22, 170);
		colors[WINDOW_TITLE_BAR] = CColor(28, 28, 28, 0); //255, 75, 0
		colors[WINDOW_TEXT] = CColor(255, 255, 255);

		colors[GROUPBOX_BODY] = CColor(40, 40, 40, 0);
		colors[GROUPBOX_OUTLINE] = CColor(60, 60, 60);
		colors[GROUPBOX_TEXT] = WHITE;

		colors[SCROLLBAR_BODY] = SETTINGS::settings.menu_col;

		colors[SEPARATOR_TEXT] = WHITE;
		colors[SEPARATOR_LINE] = YELLOW;

		colors[CHECKBOX_CLICKED] = YELLOW;
		colors[CHECKBOX_NOT_CLICKED] = CColor(60, 60, 60, 255);
		colors[CHECKBOX_TEXT] = WHITE;

		colors[BUTTON_BODY] = CColor(40, 40, 40, 255);
		colors[BUTTON_TEXT] = WHITE;

		colors[COMBOBOX_TEXT] = LIGHTGREY;
		colors[COMBOBOX_SELECTED] = CColor(40, 40, 40, 255);
		colors[COMBOBOX_SELECTED_TEXT] = WHITE;
		colors[COMBOBOX_ITEM] = CColor(20, 20, 20, 255);
		colors[COMBOBOX_ITEM_TEXT] = LIGHTGREY;

		colors[SLIDER_BODY] = CColor(40, 40, 40, 255);
		colors[SLIDER_VALUE] = YELLOW;
		colors[SLIDER_TEXT] = WHITE;

		colors[TAB_BODY] = CColor(30, 30, 30, 170);
		colors[TAB_TEXT] = CColor(255, 255, 255, 170);
		colors[TAB_BODY_SELECTED] = CColor(30, 30, 30, 170);
		colors[TAB_TEXT_SELECTED] = YELLOW;

		colors[VERTICAL_TAB_BODY] = CColor(30, 30, 30, 170);
		colors[VERTICAL_TAB_TEXT] = CColor(255, 255, 255, 170);
		colors[VERTICAL_TAB_OUTLINE] = CColor(30, 30, 30, 0);
		colors[VERTICAL_TAB_BODY_SELECTED] = CColor(30, 30, 30, 170);
		colors[VERTICAL_TAB_TEXT_SELECTED] = YELLOW;

		colors[COLOR_PICKER_BODY] = CColor(50, 50, 50, 0);
		colors[COLOR_PICKER_TEXT] = WHITE;
		colors[COLOR_PICKER_OUTLINE] = CColor(0, 0, 0, 0);
	}

	void Do()
	{

	

		if (UTILS::INPUT::input_handler.GetKeyState(VK_INSERT) & 1)
		{
			menu_open = !menu_open;
			INTERFACES::InputSystem->EnableInput(!menu_open);
		}

		if (SETTINGS::settings.autoconfigb > 0)
		{
			if (GetAsyncKeyState(UTILS::INPUT::input_handler.keyBindings(SETTINGS::settings.autoconfigb)) & 1)
			{
				std::string config = "auto_hvh";
				SETTINGS::settings.Load(config);

				INTERFACES::cvar->ConsoleColorPrintf(CColor(200, 255, 0, 255), "[Elevate.win] ");
				GLOBAL::Msg("Configuration loaded.    \n");
			}
		}
		if (SETTINGS::settings.scoutconfigb > 0)
		{
			if (GetAsyncKeyState(UTILS::INPUT::input_handler.keyBindings(SETTINGS::settings.scoutconfigb)) & 1)
			{
				std::string config = "scout_hvh";
				SETTINGS::settings.Load(config);

				INTERFACES::cvar->ConsoleColorPrintf(CColor(200, 255, 0, 255), "[Elevate.win] ");
				GLOBAL::Msg("Configuration loaded.    \n");
			}
		}
		if (SETTINGS::settings.pistolconfigb > 0)
		{
			if (GetAsyncKeyState(UTILS::INPUT::input_handler.keyBindings(SETTINGS::settings.pistolconfigb)) & 1)
			{
				std::string config = "pistol_hvh";
				SETTINGS::settings.Load(config);

				INTERFACES::cvar->ConsoleColorPrintf(CColor(200, 255, 0, 255), "[Elevate.win] ");
				GLOBAL::Msg("Configuration loaded.    \n");
			}
		}

		if (GetAsyncKeyState(UTILS::INPUT::input_handler.keyBindings(SETTINGS::settings.flip_int)) & 1)
			SETTINGS::settings.flip_bool = !SETTINGS::settings.flip_bool;

		if (GetAsyncKeyState(UTILS::INPUT::input_handler.keyBindings(SETTINGS::settings.quickstopkey)) & 1)
			SETTINGS::settings.stop_flip = !SETTINGS::settings.stop_flip;

		if (GetKeyState(UTILS::INPUT::input_handler.keyBindings(SETTINGS::settings.overridekey)) & 1)
			SETTINGS::settings.overridething = !SETTINGS::settings.overridething;


		int w1, h1;
		INTERFACES::Engine->GetScreenSize(w1, h1);
		RENDER::DrawFilledRect(w1 - 242, 4, 242 + w1 - 247, 22, SETTINGS::settings.refixsign);
		RENDER::DrawF(w1 - 242 + 7, 5, FONTS::menu_window_font, false, false, CColor(255, 255, 255, 255), "Elevate");
		RENDER::DrawF(w1 - 242 + 70, 5, FONTS::menu_window_font, false, false, CColor(255, 255, 255, 255), currentDateTime());
		
		InitColors();
		if (menu_hide)
		{

		}
		else
		{
			if (menu_open)
			{

				//RENDER::DrawF(w1 / 2 - 250, h1 - 90, FONTS::bigtext, false, false, SETTINGS::settings.refixsign, "Elevate.win recode");
				using namespace PPGUI_PP_GUI;
				if (!menu_open) return;

				DrawMouse();

				SetFont(FONTS::menu_window_font);
				WindowBegin("OOF", Vector2D(200, 200), Vector2D(620, 500));

				std::vector<std::string> tabs = { "aimbot", "visuals", "misc", "antiaim", "config","colors" };
				std::vector<std::string> aim_mode = { "rage" };
				std::vector<std::string> positionadjustment = { "normal", "history" };
				std::vector<std::string> acc_mode = { "Head-Aim", "Only Body", "Low" ,"High" ,"Veri-high" ,"Adaptive" };
				std::vector<std::string> chams_mode = { "none", "visible", "invisible" };
				std::vector<std::string> hs_when = { "Hs if in air", "Body if in air" };
				std::vector<std::string> chams_type2 = { "invisible","golden","glass","glass 2","crystal","shitty blue","golden 2","crystal blue", "music meme", "platinum", "dark chrome", "plastic", "glow" };
				std::vector<std::string> chams_type3 = { "invisible","golden","glass","glass 2","crystal","shitty blue","golden 2","crystal blue", "music meme", "platinum", "dark chrome", "plastic", "glow" };
				std::vector<std::string> aa_pitch = { "none", "emotion", "fake down", "fake up", "fake zero", "half-pitch" };
				std::vector<std::string> aa_mode = { "none", "backwards", "sideways", "backjitter", "lowerbody", "legit troll", "rotational", "autodirectional", "180z", "freestanding" };
				std::vector<std::string> aa_fake = { "none", "backjitter", "random", "local view", "opposite", "rotational", "180z" };
				std::vector<std::string> configs = { "default", "legit", "autos", "scouts", "pistols", "awps", "nospread" };
				std::vector<std::string> box_style = { "none", "full", "debug" };
				std::vector<std::string> delay_shot = { "off", "lag compensation" };
				std::vector<std::string> fakelag_mode = { "factor", "adaptive" };
				std::vector<std::string> key_binds = { "none", "mouse1", "mouse2", "mouse3", "mouse4", "mouse5", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "f10", "f11", "f12" };
				std::vector<std::string> hitmarker = { "none", "gamesense", "bameware", "custom" };
				std::vector<std::string> antimode = { "Freestanding", "Manual", "Default" };
				std::vector<std::string> antiaimmode = { "standing", "moving", "jumping" };
				std::vector<std::string> glow_styles = { "regular", "pulsing", "outline" };
				std::vector<std::string> local_chams = { "none","sim fakelag: normal", "non-sim fakelag", "sim fakelag: color" };
				std::vector<std::string> chams_type = { "metallic", "texture" };
				std::vector<std::string> team_select = { "enemy", "team" };
				std::vector<std::string> crosshair_select = { "none", "static", "recoil" };
				std::vector<std::string> autostop_method = { "head", "hitscan" };
				std::vector<std::string> override_method = { "set", "key-press" };
				std::vector<std::string> pistol = { "disabled", "dealge | revolver", "elites", "p250" };
				std::vector<std::string> snipers = { "disabled", "scar20 | g3sg1", "ssg08", "awp", "ak47 | m4a1" };
				std::vector<std::string> armor = { "disabled", "kevlar", "Helmet & kevlar" };
				std::vector<std::string> weap_icon = { "off", "icons", "text" };
				std::string config;

				std::vector<std::string> KnifeModel = { "Bayonet",
				"Flip Knife",
				"Gut Knife",
				"Karambit",
				"M9 Bayonet",
				"Huntsman Knife",
				"Butterfly Knife",
				"Falchion Knife",
				"Shadow Daggers",
				"Bowie Knife",
				"Navaja Knife",
				"Stiletto Knife",
				"Ursus Knife",
				"Talon Knife" };
				std::vector<std::string> knifeskins = { "None",
					"Crimson Web",
					"Bone Mask",
					"Fade",
					"Night",
					"Blue Steel",
					"Stained",
					"Case Hardened",
					"Slaughter",
					"Safari Mesh",
					"Boreal Forest",
					"Ultraviolet",
					"Urban Masked",
					"Scorched",
					"Rust Coat",
					"Tiger Tooth",
					"Damascus Steel",
					"Damascus Steel",
					"Marble Fade",
					"Rust Coat",
					"Doppler Ruby",
					"Doppler Sapphire",
					"Doppler Blackpearl",
					"Doppler Phase 1",
					"Doppler Phase 2",
					"Doppler Phase 3",
					"Doppler Phase 4",
					"Gamma Doppler Phase 1",
					"Gamma Doppler Phase 2",
					"Gamma Doppler Phase 3",
					"Gamma Doppler Phase 4",
					"Gamma Doppler Emerald",
					"Lore",
					"Black Laminate",
					"Autotronic",
					"Freehand" };
				std::vector<std::string> GloveModel = { "None",
					"Bloodhound",
					"Sport",
					"Driver",
					"Wraps",
					"Moto",
					"Specialist" };
				std::vector<std::string> xdshit = { "1",
					"2",
					"3",
					"4" };


				SplitWindow(5);
				GroupboxBegin2("", 580, 80, 0);

				switch (Tab("OOF", tabs, OBJECT_FLAGS::FLAG_NONE))
				{
				case 0:
				{
					menutab = 0;
					break;
				}
				case 1:
				{
					menutab = 1;
					break;
				}
				case 2:
				{
					menutab = 2;
					break;
				}
				case 3:
				{
					menutab = 3;
					break;
				}
				case 4:
				{
					menutab = 4;
					break;
				}
				case 5:
				{
					menutab = 5;
					break;
				}
				}
				GroupboxEnd();


				GroupboxBegin2("", 580, 355, 0);
				if (menutab == 0)// RAGE
				{
					if (Checkbox("enable aimbot", SETTINGS::settings.aim_bool)) {
						Combobox("aimbot type", aim_mode, SETTINGS::settings.aim_type);
						Combobox("Hitscan mode", acc_mode, SETTINGS::settings.acc_type);
						Slider("minimum hit-chance", 0, 100, SETTINGS::settings.chance_val);
						Slider("minimum damage", 1, 100, SETTINGS::settings.damage_val);
						Checkbox("Resolver enable", SETTINGS::settings.resolve_bool);

						//Combobox("Resolver mode", resolver__type, SETTINGS::settings.resolver__type);
						if (Checkbox("override enable", SETTINGS::settings.overrideenable))
						{
							Combobox("override key", key_binds, SETTINGS::settings.overridekey);
							Combobox("override method", override_method, SETTINGS::settings.overridemethod);
						}
						if (Checkbox("more aimpoints", SETTINGS::settings.multi_bool))
						{
							Slider("head scale", 0, 1, SETTINGS::settings.point_val);
							Slider("body scale", 0, 1, SETTINGS::settings.body_val);
						}
						Separator("Extra");
						Checkbox("Backtrack", SETTINGS::settings.backtrack_bool);
						Checkbox("auto revolver // crash sometimes", SETTINGS::settings.auto_revolver_enabled);
						Checkbox("auto zeus", SETTINGS::settings.auto_zeus);
						Checkbox("quick stop", SETTINGS::settings.stop_bool);
						Checkbox("auto knife", SETTINGS::settings.autoknife_bool);
						//Checkbox("BAIM if awp", SETTINGS::settings.baimifawp);
						if (Checkbox("Baim if hp < X", SETTINGS::settings.baimifhpbool))
							Slider("Body aim if hp < X", 0, 100, SETTINGS::settings.baimifhp);
						//if (Checkbox("BAIM on key", SETTINGS::settings.baimonkey))
						//	Combobox("key bind", key_binds, SETTINGS::settings.baimkey);
						Checkbox("velocity-prediction", SETTINGS::settings.vecvelocityprediction);
						Checkbox("fakewalk-fix", SETTINGS::settings.fakefix_bool);
						Checkbox("prediciton", SETTINGS::settings.prediction);
						Combobox("if in air...", hs_when, SETTINGS::settings.hsinair);
						Combobox("delay shot", delay_shot, SETTINGS::settings.delay_shot);
					}
				}

				else if (menutab == 1)//VISUALS
				{

					if (Checkbox("enable visuals", SETTINGS::settings.esp_bool))
					{
						if (Checkbox("Main part"))
						{
							Checkbox("draw enemy box", SETTINGS::settings.box_bool);
							Checkbox("draw enemy name", SETTINGS::settings.name_bool);
							Combobox("draw enemy weapon", weap_icon, SETTINGS::settings.weap_bool);
							Checkbox("draw enemy flags", SETTINGS::settings.info_bool);
							Checkbox("draw enemy health", SETTINGS::settings.health_bool);
							Checkbox("draw enemy ammo", SETTINGS::settings.ammo_bool);
							Checkbox("draw enemy fov arrows", SETTINGS::settings.fov_bool);
							Checkbox("lby indicator", SETTINGS::settings.lbyenable);
							Checkbox("fix zoom sensitivity", SETTINGS::settings.fixscopesens);
							Checkbox("render spread", SETTINGS::settings.spread_bool);
							Checkbox("Draw sexy zeus range", SETTINGS::settings.zeusrange);
							Checkbox("Freestanding indicator", SETTINGS::settings.freestandind);
							Separator("");
						}
						if (Checkbox("Remove particles"))
						{
							Checkbox("remove smoke", SETTINGS::settings.smoke_bool);
							Checkbox("remove scope", SETTINGS::settings.scope_bool);
							Checkbox("remove zoom", SETTINGS::settings.removescoping);
							Checkbox("remove visual recoil", SETTINGS::settings.norecoil);
							Separator("");
						}
						if (Checkbox("Bullet settings"))
						{
							if (Checkbox("bullet tracers", SETTINGS::settings.beam_bool))
								Checkbox("Gradient", SETTINGS::settings.gradientbullet);
							Checkbox("bullet impacts", SETTINGS::settings.impacts);
							Separator("");
						}
						if (Checkbox("Glow"))
						{
							Combobox("glow team selection", team_select, SETTINGS::settings.glowteamselection);
							if (SETTINGS::settings.glowteamselection == 0)
								Checkbox("enemy glow enable", SETTINGS::settings.glowenable);
							else if (SETTINGS::settings.glowteamselection == 1)
								Checkbox("team glow enable", SETTINGS::settings.glowteam);
							Combobox("glow style", glow_styles, SETTINGS::settings.glowstyle);
							Checkbox("local glow", SETTINGS::settings.glowlocal);
							Combobox("local glow style", glow_styles, SETTINGS::settings.glowstylelocal);
							if (SETTINGS::settings.glowlocal)
							{
								Checkbox("Gradient local glow", SETTINGS::settings.gradientglow);
							}
							Separator("");
						}
						if (Checkbox("thirdperson", SETTINGS::settings.tp_bool))
						{
							Combobox("thirdperson key", key_binds, SETTINGS::settings.thirdperson_int);
							Slider("Scope trans", 0, 100, SETTINGS::settings.transparency_when_scoped, 50);
							Separator("");
						}
						if (Checkbox("Chams"))
						{
							Combobox("model team selection", team_select, SETTINGS::settings.chamsteamselection);
							Combobox("model type", chams_type, SETTINGS::settings.chamstype);
							Combobox("enemy coloured models", chams_mode, SETTINGS::settings.chams_type);
							if (Checkbox("local chams", SETTINGS::settings.localchams))
							{
								Checkbox("Gradient", SETTINGS::settings.gradient);
							}
							Checkbox("chams on hands", SETTINGS::settings.wirehands);
							if (SETTINGS::settings.wirehands)
							{
								Combobox("hands material", chams_type2, SETTINGS::settings.metallic);
							}
						}
						if (Checkbox("Crosshair"))
						{
							Checkbox("force crosshair", SETTINGS::settings.forcehair);
							Combobox("crosshair", crosshair_select, SETTINGS::settings.xhair_type);
							Separator("");
						}
							Checkbox("Disable Postprocessing", SETTINGS::settings.matpostprocessenable);
						if (Checkbox("Hitmarkers"))
						{
							Combobox("hitmarker sound", hitmarker, SETTINGS::settings.hitmarker_val);
							Checkbox("player hitmarkers", SETTINGS::settings.hitmarker);
							Checkbox("screen hitmarkers", SETTINGS::settings.hitmarker_screen);
							Separator("");
						}
						if (Checkbox("Fov Changers"))
						{
							Slider("viewmodel fov", 0, 179, SETTINGS::settings.viewfov_val, 68);
							Slider("render fov", 0, 179, SETTINGS::settings.fov_val, 90);
							Separator("");
						}
						if (Checkbox("Sky and world"))
						{
							Checkbox("sky color changer", SETTINGS::settings.sky_enabled);
							Checkbox("world color changer", SETTINGS::settings.wolrd_enabled);
							/*Checkbox("world blend", SETTINGS::settings.asuswalls);
							if (SETTINGS::settings.asuswalls)
							{
								Slider("value", 0, 100, SETTINGS::settings.asuswallsvalue);
							}*/
							Separator("");
						}
						if (Checkbox("night mode", SETTINGS::settings.night_bool))
							Slider("night value", 0, 100, SETTINGS::settings.daytimevalue, 50);
						Slider("Gradient speed", 0.001, 0.01, SETTINGS::settings.gradientspeed, 3);
					}

				}

				else if (menutab == 2)//MISC
				{
					Checkbox("enable misc", SETTINGS::settings.misc_bool);	
					Checkbox("auto bunnyhop", SETTINGS::settings.bhop_bool);
					Checkbox("auto strafer", SETTINGS::settings.strafe_bool);
					Checkbox("meme walk", SETTINGS::settings.astro);
					Checkbox("fakewalk", SETTINGS::settings.fakewalk);
					//Slider("Preserve Killfeed", 0, 180, SETTINGS::settings.PreserveKillfeed);
					Slider("fakewalk speed", 3, 8, SETTINGS::settings.fakewalkspeed, 1);
					Checkbox("Logs", SETTINGS::settings.logs);
					Checkbox("Clantag", SETTINGS::settings.Clantag);
					Checkbox("buybot", SETTINGS::settings.buybot_enabled);
					if (SETTINGS::settings.buybot_enabled)
					{
						{
							Combobox("Pistols", pistol, SETTINGS::settings.buybot_pistol);
							Combobox("Snipers", snipers, SETTINGS::settings.buybot_rifle);
							Combobox("Armor", armor, SETTINGS::settings.buybot_armor);
							Checkbox("Zeus", SETTINGS::settings.buybot_zeus);
							Checkbox("Grenades", SETTINGS::settings.buybot_grenade);
						}
					}
					Checkbox("enable fakelag", SETTINGS::settings.lag_bool);
					Combobox("fakelag type", fakelag_mode, SETTINGS::settings.lag_type);
					Slider("moving lag", 1, 14, SETTINGS::settings.move_lag);
					Slider("jumping lag", 1, 14, SETTINGS::settings.jump_lag);

				}

				else if (menutab == 3)//ANTIAIM
				{
					if (Checkbox("enable aa", SETTINGS::settings.aa_bool))
					{
						Separator("Pitch");
						Combobox("antiaim pitch standing", aa_pitch, SETTINGS::settings.aa_pitch_type);
						Combobox("antiaim pitch moving", aa_pitch, SETTINGS::settings.aa_pitch1_type);
						Combobox("antiaim pitch jumping", aa_pitch, SETTINGS::settings.aa_pitch2_type);
						Separator("");
						Combobox("Mode", antimode, SETTINGS::settings.a_mode);
						if (SETTINGS::settings.a_mode == 0)
						{
							Separator("Freestanding");
							Combobox("antiaim fake", aa_fake, SETTINGS::settings.aa_fake_type1);
							
							Slider("fake add stainding", -180, 180, SETTINGS::settings.aa_fakeadditive1_val);
							
							Slider("fake add moving", -180, 180, SETTINGS::settings.aa_fakeadditive_val);
							
							Slider("fake add jumping", -180, 180, SETTINGS::settings.aa_fakeadditive2_val);
							
							if (SETTINGS::settings.aa_fake_type1 == 5)
							{
								Slider("rotate fake °", 0, 180, SETTINGS::settings.spinanglefake);
								Slider("rotate fake %", 0, 100, SETTINGS::settings.spinspeedfake);
							}
						}
						else if (SETTINGS::settings.a_mode == 1)
						{
							Separator("Manual");
							Combobox("flip key", key_binds, SETTINGS::settings.flip_int);


							//Combobox("antiaim real - standing", aa_mode, SETTINGS::settings.aa_real_type);
							//Combobox("antiaim real - moving", aa_mode, SETTINGS::settings.aa_real1_type);
							//Combobox("antiaim real - jumping", aa_mode, SETTINGS::settings.aa_real2_type);
							Separator("Fakes");

							Combobox("antiaim fake - standing", aa_fake, SETTINGS::settings.aa_fake_type);
							Combobox("antiaim fake - moving", aa_fake, SETTINGS::settings.aa_fake1_type);
							Combobox("antiaim fake - jumping", aa_fake, SETTINGS::settings.aa_fake2_type);

							if (Checkbox("Extra"))
							{
								Separator("Extra");		
								
								Slider("delta standing", -180, 180, SETTINGS::settings.delta_val);
								Slider("delta moving", -180, 180, SETTINGS::settings.delta1_val);
								Slider("delta jumping", -180, 180, SETTINGS::settings.delta2_val);
								Slider("real add stainding", -180, 180, SETTINGS::settings.aa_realadditive_val);
								Slider("real add moving", -180, 180, SETTINGS::settings.aa_realadditive1_val);
								Slider("real add jumping", -180, 180, SETTINGS::settings.aa_realadditive2_val);
								Slider("fake add stainding", -180, 180, SETTINGS::settings.aa_fakeadditive1_val);
								Slider("fake add moving", -180, 180, SETTINGS::settings.aa_fakeadditive_val);
								Slider("fake add jumping", -180, 180, SETTINGS::settings.aa_fakeadditive2_val);
								
								
									
								
							}

							if (SETTINGS::settings.aa_real_type == 6)
							{
								Slider("rotate standing °", 0, 180, SETTINGS::settings.spinangle);
								Slider("rotate standing %", 0, 100, SETTINGS::settings.spinspeed);
							}
							else if (SETTINGS::settings.aa_real1_type == 6)
							{
								Slider("rotate moving °", 0, 180, SETTINGS::settings.spinangle1);
								Slider("rotate moving %", 0, 100, SETTINGS::settings.spinspeed1);
							}
							else if (SETTINGS::settings.aa_real2_type == 6)
							{
								Slider("rotate jumping °", 0, 180, SETTINGS::settings.spinangle2);
								Slider("rotate jumping %", 0, 100, SETTINGS::settings.spinspeed2);
							}
							if (SETTINGS::settings.aa_fake_type == 5 || SETTINGS::settings.aa_fake1_type == 5 || SETTINGS::settings.aa_fake2_type == 5)
							{
								Slider("rotate fake °", 0, 180, SETTINGS::settings.spinanglefake);
								Slider("rotate fake %", 0, 100, SETTINGS::settings.spinspeedfake);
							}
						}
						else if (SETTINGS::settings.a_mode == 2)
						{
							Combobox("antiaim real - standing", aa_mode, SETTINGS::settings.aa_real_type);
							Combobox("antiaim real - jumping", aa_mode, SETTINGS::settings.aa_real2_type);
							Combobox("antiaim real - moving", aa_mode, SETTINGS::settings.aa_real1_type);
							Separator("Fakes");
							Combobox("antiaim fake - standing", aa_fake, SETTINGS::settings.aa_fake_type2);
							Combobox("antiaim fake - moving", aa_fake, SETTINGS::settings.aa_fake1_type2);
							Combobox("antiaim fake - jumping", aa_fake, SETTINGS::settings.aa_fake2_type2);
							if (SETTINGS::settings.aa_real_type == 4 || SETTINGS::settings.aa_real1_type == 4 || SETTINGS::settings.aa_real2_type == 4)
							{
								Separator("Flip key");
								Combobox("flip key", key_binds, SETTINGS::settings.flip_int1);
								Separator("");
							}

							if (Checkbox("Extra"))
							{
								Separator("Additives");

								Slider("delta standing", -180, 180, SETTINGS::settings.delta_val);
								Slider("delta moving", -180, 180, SETTINGS::settings.delta1_val);
								Slider("delta jumping", -180, 180, SETTINGS::settings.delta2_val);
								Slider("real add stainding", -180, 180, SETTINGS::settings.aa_realadditive_val);
								Slider("real add moving", -180, 180, SETTINGS::settings.aa_realadditive1_val);
								Slider("real add jumping", -180, 180, SETTINGS::settings.aa_realadditive2_val);
								Slider("fake add stainding", -180, 180, SETTINGS::settings.aa_fakeadditive1_val);
								Slider("fake add moving", -180, 180, SETTINGS::settings.aa_fakeadditive_val);
								Slider("fake add jumping", -180, 180, SETTINGS::settings.aa_fakeadditive2_val);
							}

							if (SETTINGS::settings.aa_real_type == 6)
							{
								Slider("rotate standing °", 0, 180, SETTINGS::settings.spinangle);
								Slider("rotate standing %", 0, 100, SETTINGS::settings.spinspeed);
							}
							else if (SETTINGS::settings.aa_real1_type == 6)
							{
								Slider("rotate moving °", 0, 180, SETTINGS::settings.spinangle1);
								Slider("rotate moving %", 0, 100, SETTINGS::settings.spinspeed1);
							}
							else if (SETTINGS::settings.aa_real2_type == 6)
							{
								Slider("rotate jumping °", 0, 180, SETTINGS::settings.spinangle2);
								Slider("rotate jumping %", 0, 100, SETTINGS::settings.spinspeed2);
							}
							if (SETTINGS::settings.aa_fake_type == 5 || SETTINGS::settings.aa_fake1_type == 5 || SETTINGS::settings.aa_fake2_type == 5)
							{
								Slider("rotate fake °", 0, 180, SETTINGS::settings.spinanglefake);
								Slider("rotate fake %", 0, 100, SETTINGS::settings.spinspeedfake);
							}
						}
						/*Combobox("antiaim mode", antiaimmode, SETTINGS::settings.aa_mode);
						switch (SETTINGS::settings.aa_mode)
						{
						case 0:
							Combobox("antiaim pitch - standing", aa_pitch, SETTINGS::settings.aa_pitch_type);
							Combobox("antiaim real - standing", aa_mode, SETTINGS::settings.aa_real_type);
							Combobox("antiaim fake - standing", aa_fake, SETTINGS::settings.aa_fake_type);
							break;
						case 1:
							Combobox("antiaim pitch - moving", aa_pitch, SETTINGS::settings.aa_pitch1_type);
							Combobox("antiaim real - moving", aa_mode, SETTINGS::settings.aa_real1_type);
							Combobox("antiaim fake - moving", aa_fake, SETTINGS::settings.aa_fake1_type);
							break;
						case 2:
							Combobox("antiaim pitch - jumping", aa_pitch, SETTINGS::settings.aa_pitch2_type);
							Combobox("antiaim real - jumping", aa_mode, SETTINGS::settings.aa_real2_type);
							Combobox("antiaim fake - jumping", aa_fake, SETTINGS::settings.aa_fake2_type);
							break;
						}*/




						//Checkbox("fake angle chams", SETTINGS::settings.aa_fakeangchams_bool);
						Separator("");
						Checkbox("anti-aim arrows", SETTINGS::settings.rifk_arrow);
						Checkbox("draw fake chams", SETTINGS::settings.aa_fakeangchams_bool);
						/*switch (SETTINGS::settings.aa_mode)
						{
						case 0:
							Slider("real additive", -180, 180, SETTINGS::settings.aa_realadditive_val);
							Slider("fake additive", -180, 180, SETTINGS::settings.aa_fakeadditive_val);
							Slider("lowerbodyyaw delta", -180, 180, SETTINGS::settings.delta_val);
							break;
						case 1:
							Slider("real additive ", -180, 180, SETTINGS::settings.aa_realadditive1_val);
							Slider("fake additive", -180, 180, SETTINGS::settings.aa_fakeadditive1_val);
							Slider("lowerbodyyaw delta", -180, 180, SETTINGS::settings.delta1_val);
							break;
						case 2:
							Slider("real additive", -180, 180, SETTINGS::settings.aa_realadditive2_val);
							Slider("fake additive", -180, 180, SETTINGS::settings.aa_fakeadditive2_val);
							Slider("lowerbodyyaw delta", -180, 180, SETTINGS::settings.delta2_val);
							break;
						}*/





						/*switch (SETTINGS::settings.aa_mode)
						{
						case 0:
							Slider("rotate standing °", 0, 180, SETTINGS::settings.spinangle);
							Slider("rotate standing %", 0, 100, SETTINGS::settings.spinspeed);
							break;
						case 1:
							Slider("rotate moving °", 0, 180, SETTINGS::settings.spinangle1);
							Slider("rotate moving %", 0, 100, SETTINGS::settings.spinspeed1);
							break;
						case 2:
							Slider("rotate jumping °", 0, 180, SETTINGS::settings.spinangle2);
							Slider("rotate jumping %", 0, 100, SETTINGS::settings.spinspeed2);
							break;
						}*/

					}


				}
			
				
				else if (menutab == 4)//CONFIG
				{
					Combobox("Auto config bind", key_binds, SETTINGS::settings.autoconfigb);
					Combobox("Scout config bind", key_binds, SETTINGS::settings.scoutconfigb);
					Combobox("Pistol config bind", key_binds, SETTINGS::settings.pistolconfigb);
					Separator("");
					switch (Combobox("config", configs, SETTINGS::settings.config_sel))
					{
					case 0: config = "default"; break;
					case 1: config = "legit"; break;
					case 2: config = "auto_hvh"; break;
					case 3: config = "scout_hvh"; break;
					case 4: config = "pistol_hvh"; break;
					case 5: config = "awp_hvh"; break;
					case 6: config = "nospread_hvh"; break;
					}

					if (Button("Load Config"))
					{
						SETTINGS::settings.Load(config);

						INTERFACES::cvar->ConsoleColorPrintf(CColor(200, 255, 0, 255), "[Elevate.win] ");
						GLOBAL::Msg("Configuration loaded.    \n");
					}

					if (Button("Save Config"))
					{
						SETTINGS::settings.Save(config);

						INTERFACES::cvar->ConsoleColorPrintf(CColor(200, 255, 0, 255), "[Elevate.win] ");
						GLOBAL::Msg("Configuration saved.    \n");
					}

				}
				else if (menutab == 5)// Colors
				{
					Combobox("ESP Colour Selection", team_select, SETTINGS::settings.espteamcolourselection);
					if (SETTINGS::settings.espteamcolourselection == 0)
					{
						ColorPicker("Enemy Box", SETTINGS::settings.box_col);
						ColorPicker("Enemy Name", SETTINGS::settings.name_col);
						ColorPicker("Enemy Weapon", SETTINGS::settings.weapon_col);
						ColorPicker("Enemy Fov Arrows", SETTINGS::settings.fov_col);
						ColorPicker("Enemy Visible", SETTINGS::settings.vmodel_col);
						ColorPicker("Enemy Invisible", SETTINGS::settings.imodel_col);
						ColorPicker("Glow", SETTINGS::settings.glow_col);
						ColorPicker("Bullet Tracer", SETTINGS::settings.bulletenemy_col);
					}
					else if (SETTINGS::settings.espteamcolourselection == 1)
					{
						ColorPicker("Team Box", SETTINGS::settings.boxteam_col);
						ColorPicker("Team Name", SETTINGS::settings.nameteam_col);
						ColorPicker("Team Weapon", SETTINGS::settings.weaponteam_col);
						ColorPicker("Team Fov Arrows", SETTINGS::settings.arrowteam_col);
						ColorPicker("Team Visible", SETTINGS::settings.teamvis_color);
						ColorPicker("Team Invisible", SETTINGS::settings.teaminvis_color);
						ColorPicker("Glow", SETTINGS::settings.teamglow_color);
						ColorPicker("Bullet Tracer", SETTINGS::settings.bulletteam_col);
					}
					ColorPicker("Chams - Local", SETTINGS::settings.localchams_col);
					ColorPicker("Glow - Local", SETTINGS::settings.glowlocal_col);
					ColorPicker("Bullet Tracer - Local", SETTINGS::settings.bulletlocal_col);
					ColorPicker("Grenade - Prediction", SETTINGS::settings.grenadepredline_col);
					ColorPicker("Spread Corsshair", SETTINGS::settings.spread_Col);
					ColorPicker("sky color", SETTINGS::settings.skycolor);
					ColorPicker("world color", SETTINGS::settings.night_col);
					ColorPicker("Hitmarkers", SETTINGS::settings.awcolor);
					Separator("Menu");
					ColorPicker("Refix signature", SETTINGS::settings.refixsign);

				}
				GroupboxEnd();

				WindowEnd();

				int w, h;
				static int x, y;

				INTERFACES::Engine->GetScreenSize(w, h);
				static bool init = false;
				if (init == false) {
					x = w / 2 - (400 / 2);
					y = h / 2 - (200 / 2);
					init = true;
				}
			}
		}
	}
}