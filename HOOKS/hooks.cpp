#include "..\includes.h"
#include "../xdxdxd.h"
#include "hooks.h"
#include "../UTILS/interfaces.h"
#include "../UTILS/offsets.h"
#include "../UTILS/NetvarHookManager.h"
#include "../UTILS/render.h"
#include "../SDK/CInput.h"
#include "../SDK/IClient.h"
#include "../SDK/CPanel.h"
#include "../SDK/ConVar.h"
#include "../SDK/CGlowObjectManager.h"
#include "../SDK/IEngine.h"
#include "../SDK/CTrace.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/RecvData.h"
#include "../SDK/CBaseAnimState.h"
#include "../SDK/ModelInfo.h"
#include "../SDK/ModelRender.h"
#include "../SDK/RenderView.h"
#include "../SDK/CTrace.h"
#include "../SDK/CViewSetup.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/CPrediction.h"

#include "../FEATURES/Movement.h"
#include "../FEATURES/EventListener.h"
#include "../FEATURES/Visuals.h"
#include "../FEATURES/Chams.h"
#include "../FEATURES/AntiAim.h"
#include "../FEATURES/Aimbot.h"
#include "../FEATURES/Resolver.h"
#include "../FEATURES/Backtracking.h"
#include "../FEATURES/FakeLag.h"
#include "../FEATURES/EnginePred.h"
#include "../FEATURES/EventListener.h"
#include "../FEATURES/GrenadePrediction.h"
#include "../ImGUI/imgui.h"
#include "../ImGUI/imgui_internal.h"
#include "../MENU/menu_framework.h"

#include <intrin.h>
typedef unsigned long CBaseHandle;
//--- Other Globally Used Variables ---///
static bool tick = false;
static int ground_tick;
Vector vecAimPunch, vecViewPunch;
Vector* pAimPunch = nullptr;
Vector* pViewPunch = nullptr;

//--- Declare Signatures and Patterns Here ---///
static auto CAM_THINK = UTILS::FindSignature("client_panorama.dll", "85 C0 75 30 38 86");
static auto linegoesthrusmoke = UTILS::FindPattern("client_panorama.dll", (PBYTE)"\x55\x8B\xEC\x83\xEC\x08\x8B\x15\x00\x00\x00\x00\x0F\x57\xC0", "xxxxxxxx????xxx");

//--- Tick Counting ---//
void ground_ticks()
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	if (local_player->GetFlags() & FL_ONGROUND)
		ground_tick++;
	else
		ground_tick = 0;
}

namespace HOOKS
{
	CreateMoveFn original_create_move;
	PaintTraverseFn original_paint_traverse;
	PaintFn original_paint;
	FrameStageNotifyFn original_frame_stage_notify;
	DrawModelExecuteFn original_draw_model_execute;
	SceneEndFn original_scene_end;
	TraceRayFn original_trace_ray;
	OverrideViewFn original_override_view;
	RenderViewFn original_render_view;
	SvCheatsGetBoolFn original_get_bool;
	GetViewmodelFOVFn original_viewmodel_fov;
	SendDatagramFn original_send_datagram = 0;

	VMT::VMTHookManager iclient_hook_manager;
	VMT::VMTHookManager panel_hook_manager;
	VMT::VMTHookManager paint_hook_manager;
	VMT::VMTHookManager model_render_hook_manager;
	VMT::VMTHookManager scene_end_hook_manager;
	VMT::VMTHookManager render_view_hook_manager;
	VMT::VMTHookManager trace_hook_manager;
	VMT::VMTHookManager net_channel_hook_manager;
	VMT::VMTHookManager override_view_hook_manager;
	VMT::VMTHookManager input_table_manager;
	VMT::VMTHookManager get_bool_manager;
	VMT::VMTHookManager client_mode_hook_manager;
	VMT::VMTHookManager setup_bones_hook_manager;
	CSX::Hook::VTable SurfaceTable;

	bool __stdcall HookedCreateMove(float sample_input_frametime, SDK::CUserCmd* cmd)
	{ 
		if (!cmd || cmd->command_number == 0)
			return false;


		backtracking->UpdateIncomingSequences();

		uintptr_t* FPointer; __asm { MOV FPointer, EBP }
		byte* SendPacket = (byte*)(*FPointer - 0x1C);
		if (!SendPacket) return false;

		auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
		if (!local_player) return false;

		GLOBAL::should_send_packet = *SendPacket;
		GLOBAL::originalCMD = *cmd;
		if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
		{
			GrenadePrediction::instance().Tick(cmd->buttons);

			if (SETTINGS::settings.fakewalk)
			{
				if (GetAsyncKeyState(VK_SHIFT))
				{
					static int choked = 0;
					choked = choked > 7 ? 0 : choked + 1;
					GLOBAL::originalCMD.move.x = choked < 2 || choked > SETTINGS::settings.fakewalkspeed ? 0 : GLOBAL::originalCMD.move.x;
					GLOBAL::originalCMD.move.y = choked < 2 || choked > SETTINGS::settings.fakewalkspeed ? 0 : GLOBAL::originalCMD.move.y;
					GLOBAL::should_send_packet = choked < 1;
				}
			}

			if (!GetAsyncKeyState(0x56))
				fakelag->do_fakelag(cmd);

			static SDK::ConVar* impacts = INTERFACES::cvar->FindVar("sv_showimpacts");
			if (SETTINGS::settings.impacts)
			{
				impacts->SetValue(1);
			}
			else {
				impacts->SetValue(0);
			}

			if (SETTINGS::settings.astro)
			{
				if (GLOBAL::originalCMD.move.x > 0)
				{
					cmd->buttons |= IN_BACK;
					cmd->buttons &= ~IN_FORWARD;
				}

				if (GLOBAL::originalCMD.move.x < 0)
				{
					cmd->buttons |= IN_FORWARD;
					cmd->buttons &= ~IN_BACK;
				}

				if (GLOBAL::originalCMD.move.y < 0)
				{
					cmd->buttons |= IN_MOVERIGHT;
					cmd->buttons &= ~IN_MOVELEFT;
				}

				if (GLOBAL::originalCMD.move.y > 0)
				{
					cmd->buttons |= IN_MOVELEFT;
					cmd->buttons &= ~IN_MOVERIGHT;
				}
			}


			GLOBAL::lineFakeAngle = cmd->viewangles.y;


			if (SETTINGS::settings.bhop_bool) movement->bunnyhop(cmd);
			if (SETTINGS::settings.duck_bool) movement->duckinair(cmd);
			if (SETTINGS::settings.Clantag) visuals->GameSense();

			if (SETTINGS::settings.autoknife_bool) aimbot->autoknife(cmd);
			if (SETTINGS::settings.auto_zeus) aimbot->autozeus(cmd);
			if (SETTINGS::settings.auto_revolver_enabled) aimbot->auto_revolver(cmd);

			if (SETTINGS::settings.prediction)
			{
				prediction->run_prediction(cmd);
				prediction->end_prediction(cmd);
			}

			for (int i = 1; i <= 65; i++)
			{
				auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);

				if (!entity)
					continue;

				auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

				if (!local_player)
					return;

				bool is_local_player = entity == local_player;
				bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

				if (is_local_player)
					continue;

				if (is_teammate)
					continue;

				if (entity->GetHealth() <= 0)
					continue;

				if (entity->GetIsDormant())
					continue;

				if (SETTINGS::settings.stop_bool)
					movement->quick_stop(entity, cmd);
			}
			
			if (SETTINGS::settings.strafe_bool) movement->autostrafer(cmd);

			if (SETTINGS::settings.aim_type == 0 && SETTINGS::settings.aim_bool)
			{
				aimbot->run_aimbot(cmd);
			}

			if (SETTINGS::settings.backtrack_bool)
			{
				backtracking->backtrack_player(cmd);

			}

			if (SETTINGS::settings.aa_bool)
			{
				antiaim->do_antiaim(cmd);
				antiaim->fix_movement(cmd);
			}
			

			if (!GLOBAL::should_send_packet)
				GLOBAL::real_angles = cmd->viewangles;
			else
			{
				GLOBAL::FakePosition = local_player->GetAbsOrigin();
				GLOBAL::fake_angles = cmd->viewangles;
			}

		}
		
		*SendPacket = GLOBAL::should_send_packet;
		cmd->move = antiaim->fix_movement(cmd, GLOBAL::originalCMD);
		if (SETTINGS::settings.aa_pitch < 2 || SETTINGS::settings.aa_pitch1_type < 2 || SETTINGS::settings.aa_pitch2_type < 2)
			UTILS::ClampLemon(cmd->viewangles);
		return false;
	}
	void __stdcall HookedPaintTraverse(int VGUIPanel, bool ForceRepaint, bool AllowForce)
	{
		std::string panel_name = INTERFACES::Panel->GetName(VGUIPanel);
		if (panel_name == "HudZoom" && SETTINGS::settings.scope_bool) return;
		if (panel_name == "FocusOverlayPanel")
		{
			if (FONTS::ShouldReloadFonts())
				FONTS::InitFonts();
			visuals->DrawMisc();

			if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
			{
				GrenadePrediction::instance().Paint();

				static SDK::ConVar* postprocess = INTERFACES::cvar->FindVar("mat_postprocess_enable");
				if (SETTINGS::settings.matpostprocessenable)
				{
					if (INTERFACES::Engine->IsInGame())
					{
						postprocess->SetValue(0);
					}
					else
					{
						if (INTERFACES::Engine->IsInGame())
						{
							postprocess->SetValue(1);
						}
					}
				}
				else
				{
					if (INTERFACES::Engine->IsInGame())
					{
						postprocess->SetValue(1);
					}
				}

				if (SETTINGS::settings.night_bool) visuals->ModulateWorld();


				if (SETTINGS::settings.esp_bool)
				{
					visuals->Draw();
					visuals->ClientDraw();
				}

				auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
				if (!local_player) return;
				auto GetCorrectDistance = [&local_player](float ideal_distance) -> float
				{
					Vector inverse_angles;
					INTERFACES::Engine->GetViewAngles(inverse_angles);

					inverse_angles.x *= -1.f, inverse_angles.y += 180.f;

					Vector direction;
					MATH::AngleVectors(inverse_angles, &direction);

					SDK::CTraceWorldOnly filter;
					SDK::trace_t trace;
					SDK::Ray_t ray;

					ray.Init(local_player->GetVecOrigin() + local_player->GetViewOffset(), (local_player->GetVecOrigin() + local_player->GetViewOffset()) + (direction * (ideal_distance + 5.f)));
					INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);

					return ideal_distance * trace.flFraction;
				};

				/*static DWORD* deathNotice;
				static void(__thiscall *ClearDeathNotices)(DWORD);

				auto m_local = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
				//killfead
				//ok
				if (m_local) {
					if (!deathNotice) deathNotice = UTILS::Find_Hud_Element<DWORD>("CCSGO_HudDeathNotice");
					if (deathNotice) {
						float* localDeathNotice = (float*)((DWORD)deathNotice + 0x50);
						if (localDeathNotice) *localDeathNotice = SETTINGS::settings.PreserveKillfeed ? FLT_MAX : 1.5f;
						if (GLOBAL::NewRound = true && deathNotice - 20) {
							GLOBAL::NewRound = false;
							if (!ClearDeathNotices) ClearDeathNotices = (void(__thiscall*)(DWORD))UTILS::FindSignature("client_panorama.dll", "B9 ? ? ? ? E8 ? ? ? ? 83 7D F8 ?");
							if (ClearDeathNotices) ClearDeathNotices(((DWORD)deathNotice - 20));
						}
					}
				}*/


			}

			MENU::PPGUI_PP_GUI::Begin();
			MENU::Do();
			MENU::PPGUI_PP_GUI::End();

			UTILS::INPUT::input_handler.Update();

			if (SETTINGS::settings.logs) visuals->LogEvents();
		}
		original_paint_traverse(INTERFACES::Panel, VGUIPanel, ForceRepaint, AllowForce);
	}
	void __fastcall HookedFrameStageNotify(void* ecx, void* edx, ClientFrameStage_t stage)
	{
		auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
		if (!local_player) return;
		Vector vecAimPunch, vecViewPunch;
		Vector* pAimPunch = nullptr; Vector* pViewPunch = nullptr;
		visuals->ModulateSky();
		visuals->DoFSN();
		
		switch (stage)
		{
			case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
				if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
				{
					for (int i = 1; i <= 65; i++)
					{
						auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
						if (!entity) continue;

						bool is_local_player = entity == local_player;
						bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

						if (is_local_player) continue;
						if (is_teammate) continue;
						if (entity->GetHealth() <= 0) continue;
						if (entity->GetIsDormant()) continue;
						if (SETTINGS::settings.aim_type == 0 && SETTINGS::settings.resolve_bool)
							resolver->resolve(entity);
					}
				} 
			    break;
			case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
				break;
				if (stage == FRAME_RENDER_START)
				{
					backtracking->AnimationFix(stage);
					for (int i = 1; i < INTERFACES::Globals->maxclients; i++)
					{
						SDK::CBaseEntity* pEntity = INTERFACES::ClientEntityList->GetClientEntity(i);
						if (pEntity) {
							if (pEntity->GetHealth() > 0 && !pEntity->GetIsDormant()) {

								backtracking->UpdateBacktrackRecords(pEntity);
								backtracking->UpdateExtrapolationRecords(pEntity);
							}
						}
					}
				}
			case FRAME_RENDER_START:
				if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
				{

					if (in_tp)
					{
						*(Vector*)((DWORD)local_player + 0x31C8) = Vector(GLOBAL::real_angles.x, GLOBAL::real_angles.y, 0.f);

						INTERFACES::pPrediction->SetLocalViewAngles(GLOBAL::real_angles);
						local_player->UpdateClientSideAnimation();
						INTERFACES::pPrediction->SetLocalViewAngles(GLOBAL::fake_angles);

					}
					for (int i = 1; i <= 65; i++)
					{
						auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);
						if (!entity) continue;
						if (entity == local_player) continue;

						*(int*)((uintptr_t)entity + 0xA30) = INTERFACES::Globals->framecount;
						*(int*)((uintptr_t)entity + 0xA28) = 0;
					}
				} break;

			case FRAME_NET_UPDATE_START:
				if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
				{
					if (SETTINGS::settings.beam_bool)
						visuals->DrawBulletBeams();
				} break;
			case FRAME_NET_UPDATE_END:
				if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
				{
					for (int i = 1; i < 65; i++)
					{
						auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);

						if (!entity)
							continue;

						if (!local_player)
							continue;

						bool is_local_player = entity == local_player;
						bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

						if (is_local_player)
							continue;

						if (is_teammate)
							continue;

						if (entity->GetHealth() <= 0)
							continue;

							backtracking->DisableInterpolation(entity);
					}
				}
				break;
		}
		original_frame_stage_notify(ecx, stage);
	}
	void __fastcall HookedDrawModelExecute(void* ecx, void* edx, SDK::IMatRenderContext* context, const SDK::DrawModelState_t& state, const SDK::ModelRenderInfo_t& render_info, matrix3x4_t* matrix)
	{
		if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
		{
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			std::string ModelName = INTERFACES::ModelInfo->GetModelName(render_info.pModel);

			if (ModelName.find("v_sleeve") != std::string::npos)
			{
				SDK::IMaterial* material = INTERFACES::MaterialSystem->FindMaterial(ModelName.c_str(), TEXTURE_GROUP_MODEL);
				if (!material) return;
				material->SetMaterialVarFlag(SDK::MATERIAL_VAR_NO_DRAW, true);
				INTERFACES::ModelRender->ForcedMaterialOverride(material);
			}
			std::string strModelName = INTERFACES::ModelInfo->GetModelName(render_info.pModel);
			if (SETTINGS::settings.wirehands)
			{
				
				if (strModelName.find("arms") != std::string::npos && SETTINGS::settings.wirehands)
				{

					static SDK::IMaterial* huesos;
					static SDK::IMaterial* huesos2;
					static SDK::IMaterial* huesos3;
					static SDK::IMaterial* huesos4;
					static SDK::IMaterial* huesos5;
					static SDK::IMaterial* huesos6;
					static SDK::IMaterial* huesos7;
					static SDK::IMaterial* huesos8;
					static SDK::IMaterial* huesos9;
					static SDK::IMaterial* huesos10;
					static SDK::IMaterial* huesos11;
					static SDK::IMaterial* huesos12;

					huesos = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/gold", TEXTURE_GROUP_MODEL);
					huesos2 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass", TEXTURE_GROUP_MODEL);
					huesos3 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/gloss", TEXTURE_GROUP_MODEL);
					huesos4 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", TEXTURE_GROUP_MODEL);
					huesos5 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/velvet", TEXTURE_GROUP_MODEL);
					huesos6 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/wildfire_gold/wildfire_gold_detail", TEXTURE_GROUP_MODEL);
					huesos7 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_blue", TEXTURE_GROUP_MODEL);
					huesos8 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/music_kit/darude_01/mp3_detail", TEXTURE_GROUP_MODEL);
					huesos9 = INTERFACES::MaterialSystem->FindMaterial("models/player/ct_fbi/ct_fbi_glass", TEXTURE_GROUP_MODEL);
					huesos10 = INTERFACES::MaterialSystem->FindMaterial("models/gibs/glass/glass", TEXTURE_GROUP_MODEL);
					huesos11 = INTERFACES::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/gloss", TEXTURE_GROUP_MODEL);
					huesos12 = INTERFACES::MaterialSystem->FindMaterial("vgui/achievements/glow", TEXTURE_GROUP_MODEL);


					SDK::IMaterial* WireHands = INTERFACES::MaterialSystem->FindMaterial(strModelName.c_str(), TEXTURE_GROUP_MODEL);
					WireHands->SetMaterialVarFlag(SDK::MATERIAL_VAR_WIREFRAME, true);
					//WireHands->AlphaModulate(0.7f);
					//WireHands->SetMaterialVarFlag(SDK::MATERIAL_VAR_TRANSLUCENT, true);
					//INTERFACES::ModelRender->ForcedMaterialOverride(WireHands);
					
					if (SETTINGS::settings.metallic == 0)
					{
						INTERFACES::ModelRender->ForcedMaterialOverride(WireHands);
					}
					else if (SETTINGS::settings.metallic == 1)
					{
						INTERFACES::ModelRender->ForcedMaterialOverride(huesos);
					}
					else if (SETTINGS::settings.metallic == 2)
					{
						INTERFACES::ModelRender->ForcedMaterialOverride(huesos2);
					}
					else if (SETTINGS::settings.metallic == 3)
					{
						INTERFACES::ModelRender->ForcedMaterialOverride(huesos3);
					}
					else if (SETTINGS::settings.metallic == 4)
					{
						INTERFACES::ModelRender->ForcedMaterialOverride(huesos4);
					}
					else if (SETTINGS::settings.metallic == 5)
					{
						INTERFACES::ModelRender->ForcedMaterialOverride(huesos5);
					}
					else if (SETTINGS::settings.metallic == 6)
					{
						INTERFACES::ModelRender->ForcedMaterialOverride(huesos6);
					}
					else if (SETTINGS::settings.metallic == 7)
					{
						INTERFACES::ModelRender->ForcedMaterialOverride(huesos7);
					}
					else if (SETTINGS::settings.metallic == 8)
					{
						INTERFACES::ModelRender->ForcedMaterialOverride(huesos8);
					}
					else if (SETTINGS::settings.metallic == 9)
					{
						INTERFACES::ModelRender->ForcedMaterialOverride(huesos9);
					}
					else if (SETTINGS::settings.metallic == 10)
					{
						INTERFACES::ModelRender->ForcedMaterialOverride(huesos10);
					}
					else if (SETTINGS::settings.metallic == 11)
					{
						INTERFACES::ModelRender->ForcedMaterialOverride(huesos11);
					}
					else if (SETTINGS::settings.metallic == 12)
					{
						INTERFACES::ModelRender->ForcedMaterialOverride(huesos12);
					}
				}
				
			}
			
			auto entity = INTERFACES::ClientEntityList->GetClientEntity(render_info.entity_index);
			if (entity == local_player)
			{
				if (local_player && local_player->GetIsScoped())
					INTERFACES::RenderView->SetBlend(SETTINGS::settings.transparency_when_scoped * 0.01f);
			}
			
			
		}
		original_draw_model_execute(ecx, context, state, render_info, matrix);
	}
	void __fastcall HookedSceneEnd(void* ecx, void* edx)
	{
		original_scene_end(ecx);
		static SDK::IMaterial* ignorez = chams->CreateMaterialBasic(true, true, false);
		static SDK::IMaterial* notignorez = chams->CreateMaterialBasic(false, true, false);
		static SDK::IMaterial* ignorez_metallic = chams->CreateMaterialMetallic(true, true, false);
		static SDK::IMaterial* notignorez_metallic = chams->CreateMaterialMetallic(false, true, false);

		if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
		{
			if (SETTINGS::settings.aa_fakeangchams_bool)
			{
				auto pLocal = reinterpret_cast<SDK::CBaseEntity*>(INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer()));
				if (pLocal)
				{

					static SDK::IMaterial* mat = chams->CreateMaterialBasic(false, true, true);
					if (mat)
					{
						typedef Vector Vector3D;
						Vector3D OrigAng;
						OrigAng = pLocal->GetEyeAngles();
						pLocal->SetAngle2(Vector(0, GLOBAL::fake_angles.y, 0)); // paste he  re ur AA.y value or pLocal->GetLby() (for example)
						bool LbyColor = false; // u can make LBY INDICATOR. When LbyColor is true. Color will be Green , if false it will be White
											   /*CColor NormalColor = CColor( 1, 1, 1 );
											   CColor lbyUpdateColor = CColor(0, 1, 0);*/
											   //	SetColorModulation(LbyColor ? lbyUpdateColor : NormalColor);
						mat->ColorModulate(255, 255, 255);
						INTERFACES::ModelRender->ForcedMaterialOverride(mat);
						pLocal->DrawModel(0x1, 150);
						INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
						pLocal->SetAngle2(OrigAng);
					}
				}
			}
			auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
			if (!local_player) return;
			CColor color = SETTINGS::settings.glow_col, colorTeam = SETTINGS::settings.teamglow_color, colorlocal = SETTINGS::settings.glowlocal_col;
			for (int i = 1; i < 65; i++)
			{
				auto entity = INTERFACES::ClientEntityList->GetClientEntity(i);

				if (!entity) continue;
				if (!local_player) continue;

				bool is_local_player = entity == local_player;
				bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;
				auto ignorezmaterial = SETTINGS::settings.chamstype == 0 ? ignorez_metallic : ignorez;
				auto notignorezmaterial = SETTINGS::settings.chamstype == 0 ? notignorez_metallic : notignorez;
				if (is_local_player)
				{
					if (SETTINGS::settings.localchams)
					{
						if (SETTINGS::settings.gradient)
						{
							static float rainbow;
							rainbow += SETTINGS::settings.gradientspeed;
							if (rainbow > 1.f) rainbow = 0.f;
							notignorezmaterial->ColorModulate(CColor::FromHSB(rainbow, 1.f, 1.f));
							INTERFACES::ModelRender->ForcedMaterialOverride(notignorezmaterial);
							local_player->DrawModel(0x1, 255);
							INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
							continue;  break;
						}
						else
						{
							notignorezmaterial->ColorModulate(SETTINGS::settings.localchams_col);
							INTERFACES::ModelRender->ForcedMaterialOverride(notignorezmaterial);
							local_player->DrawModel(0x1, 255);
							INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
							continue;  break;
						}
					}
				}

				if (entity->GetHealth() <= 0) continue;
				if (entity->GetIsDormant())	continue;
				if (entity->GetClientClass()->m_ClassID != 35) continue;

				if (is_teammate)
				{
					if (entity && SETTINGS::settings.chamsteam == 2)
					{
						ignorezmaterial->ColorModulate(SETTINGS::settings.teaminvis_color);
						INTERFACES::ModelRender->ForcedMaterialOverride(ignorezmaterial);
						entity->DrawModel(0x1, 255);

						notignorezmaterial->ColorModulate(SETTINGS::settings.teamvis_color);
						INTERFACES::ModelRender->ForcedMaterialOverride(notignorezmaterial);
						entity->DrawModel(0x1, 255);

						INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
					}
					else if (entity && SETTINGS::settings.chamsteam == 1)
					{
						notignorezmaterial->ColorModulate(SETTINGS::settings.teamvis_color);
						INTERFACES::ModelRender->ForcedMaterialOverride(notignorezmaterial);
						entity->DrawModel(0x1, 255);

						INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
					} continue;
				}
				else if (is_teammate && SETTINGS::settings.chamsteam)
					continue;

				if (entity && SETTINGS::settings.chams_type == 2)
				{
					ignorezmaterial->ColorModulate(SETTINGS::settings.imodel_col);
					INTERFACES::ModelRender->ForcedMaterialOverride(ignorezmaterial);
					entity->DrawModel(0x1, 255);

					notignorezmaterial->ColorModulate(SETTINGS::settings.vmodel_col);
					INTERFACES::ModelRender->ForcedMaterialOverride(notignorezmaterial);
					entity->DrawModel(0x1, 255);

					INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);

					static SDK::IMaterial* wire = chams->CreateMaterialBasic(true, false, true);
					static SDK::IMaterial* materialIGNOREZ = chams->CreateMaterialBasic(true, true, false);
					Vector oldOrigin = entity->GetVecOrigin();
					QAngle oldAngs = entity->GetAngles();
					
				}
				else if (entity && SETTINGS::settings.chams_type == 1)
				{
					notignorezmaterial->ColorModulate(SETTINGS::settings.vmodel_col);
					INTERFACES::ModelRender->ForcedMaterialOverride(notignorezmaterial);
					entity->DrawModel(0x1, 255);

					INTERFACES::ModelRender->ForcedMaterialOverride(nullptr);
				}
			}

			for (auto i = 0; i < INTERFACES::GlowObjManager->GetSize(); i++)
			{
				auto &glowObject = INTERFACES::GlowObjManager->m_GlowObjectDefinitions[i];
				auto entity = reinterpret_cast<SDK::CBaseEntity*>(glowObject.m_pEntity);

				if (!entity) continue;
				if (!local_player) continue;

				if (glowObject.IsUnused()) continue;

				bool is_local_player = entity == local_player;
				bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

				if (is_local_player && in_tp && SETTINGS::settings.glowlocal)
				{
					if (SETTINGS::settings.gradientglow)
					{
						static float rainbow;
						rainbow += SETTINGS::settings.gradientspeed;
						if (rainbow > 1.f) rainbow = 0.f;
						colorlocal = CColor::FromHSB(rainbow, 1.f, 1.f);
					}
					glowObject.m_nGlowStyle = SETTINGS::settings.glowstylelocal;
					glowObject.m_flRed = colorlocal.RGBA[0] / 255.0f;
					glowObject.m_flGreen = colorlocal.RGBA[1] / 255.0f;
					glowObject.m_flBlue = colorlocal.RGBA[2] / 255.0f;
					glowObject.m_flAlpha = colorlocal.RGBA[3] / 255.0f;
					glowObject.m_bRenderWhenOccluded = true;
					glowObject.m_bRenderWhenUnoccluded = false;
					continue;
				}
				else if (!SETTINGS::settings.glowlocal && is_local_player)
					continue;

				if (entity->GetHealth() <= 0) continue;
				if (entity->GetIsDormant())	continue;
				if (entity->GetClientClass()->m_ClassID != 35) continue;

				if (is_teammate && SETTINGS::settings.glowteam)
				{
					glowObject.m_nGlowStyle = SETTINGS::settings.glowstyle; //0;
					glowObject.m_flRed = colorTeam.RGBA[0] / 255.0f;
					glowObject.m_flGreen = colorTeam.RGBA[1] / 255.0f;
					glowObject.m_flBlue = colorTeam.RGBA[2] / 255.0f;
					glowObject.m_flAlpha = colorTeam.RGBA[3] / 255.0f;
					glowObject.m_bRenderWhenOccluded = true;
					glowObject.m_bRenderWhenUnoccluded = false;
					continue;
				}
				else if (is_teammate && !SETTINGS::settings.glowteam)
					continue;

				if (SETTINGS::settings.glowenable)
				{
					glowObject.m_nGlowStyle = SETTINGS::settings.glowstyle;//0;
					glowObject.m_flRed = color.RGBA[0] / 255.0f;
					glowObject.m_flGreen = color.RGBA[1] / 255.0f;
					glowObject.m_flBlue = color.RGBA[2] / 255.0f;
					glowObject.m_flAlpha = color.RGBA[3] / 255.0f;
					glowObject.m_bRenderWhenOccluded = true;
					glowObject.m_bRenderWhenUnoccluded = false;
				}
			}

			if (SETTINGS::settings.smoke_bool)
			{
				std::vector<const char*> vistasmoke_wireframe = { "particle/vistasmokev1/vistasmokev1_smokegrenade" };

				std::vector<const char*> vistasmoke_nodraw =
				{
					"particle/vistasmokev1/vistasmokev1_fire",
					"particle/vistasmokev1/vistasmokev1_emods",
					"particle/vistasmokev1/vistasmokev1_emods_impactdust",
				};

				for (auto mat_s : vistasmoke_wireframe)
				{
					SDK::IMaterial* mat = INTERFACES::MaterialSystem->FindMaterial(mat_s, TEXTURE_GROUP_OTHER);
					mat->SetMaterialVarFlag(SDK::MATERIAL_VAR_WIREFRAME, true);
				}

				for (auto mat_n : vistasmoke_nodraw)
				{
					SDK::IMaterial* mat = INTERFACES::MaterialSystem->FindMaterial(mat_n, TEXTURE_GROUP_OTHER);
					mat->SetMaterialVarFlag(SDK::MATERIAL_VAR_NO_DRAW, true);
				}

				static auto smokecout = *(DWORD*)(linegoesthrusmoke + 0x8);
				*(int*)(smokecout) = 0;
			}
		}
	}
	void __fastcall HookedOverrideView(void* ecx, void* edx, SDK::CViewSetup* pSetup)
	{
		auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
		if (!local_player) return;

		auto animstate = local_player->GetAnimState();
		if (!animstate) return;

		if (GetAsyncKeyState(UTILS::INPUT::input_handler.keyBindings(SETTINGS::settings.thirdperson_int)) & 1)
			in_tp = !in_tp;

		if (SETTINGS::settings.norecoil)
		{
			pSetup->angles -= *local_player->GetAimPunchAngle() * 0.9f + *local_player->GetViewPunchAngle();
		}

		if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
		{
			GrenadePrediction::instance().View(pSetup);
			auto GetCorrectDistance = [&local_player](float ideal_distance) -> float //lambda
			{
				Vector inverse_angles;
				INTERFACES::Engine->GetViewAngles(inverse_angles);

				inverse_angles.x *= -1.f, inverse_angles.y += 180.f;

				Vector direction;
				MATH::AngleVectors(inverse_angles, &direction);

				SDK::CTraceWorldOnly filter;
				SDK::trace_t trace;
				SDK::Ray_t ray;

				ray.Init(local_player->GetVecOrigin() + local_player->GetViewOffset(), (local_player->GetVecOrigin() + local_player->GetViewOffset()) + (direction * (ideal_distance + 5.f)));
				INTERFACES::Trace->TraceRay(ray, MASK_ALL, &filter, &trace);

				return ideal_distance * trace.flFraction;
			};

			if (SETTINGS::settings.tp_bool && in_tp)
			{
				if (local_player->GetHealth() <= 0)
					local_player->SetObserverMode(5);

				if (!INTERFACES::Input->m_fCameraInThirdPerson)
				{
					INTERFACES::Input->m_fCameraInThirdPerson = true;
					INTERFACES::Input->m_vecCameraOffset = Vector(GLOBAL::real_angles.x, GLOBAL::real_angles.y, GetCorrectDistance(100));

					Vector camForward;
					MATH::AngleVectors(Vector(INTERFACES::Input->m_vecCameraOffset.x, INTERFACES::Input->m_vecCameraOffset.y, 0), &camForward);
				}
			}
			else
			{
				INTERFACES::Input->m_fCameraInThirdPerson = false;
				INTERFACES::Input->m_vecCameraOffset = Vector(GLOBAL::real_angles.x, GLOBAL::real_angles.y, 0);
			}
			auto zoomsensration = INTERFACES::cvar->FindVar("zoom_sensitivity_ratio_mouse");
			if (SETTINGS::settings.fixscopesens)
				zoomsensration->SetValue("0");
			else
				zoomsensration->SetValue("1");

			if (SETTINGS::settings.aim_type == 0)
			{
				if (!local_player->GetIsScoped())
					pSetup->fov = SETTINGS::settings.fov_val;
				else if (local_player->GetIsScoped() && SETTINGS::settings.removescoping)
					pSetup->fov = SETTINGS::settings.fov_val;
			}
			else if (!(SETTINGS::settings.aim_type == 0) && !local_player->GetIsScoped())
				pSetup->fov = 90;
		}

		original_override_view(ecx, pSetup);
	}
	void __fastcall HookedTraceRay(void *thisptr, void*, const SDK::Ray_t &ray, unsigned int fMask, SDK::ITraceFilter *pTraceFilter, SDK::trace_t *pTrace)
	{
		original_trace_ray(thisptr, ray, fMask, pTraceFilter, pTrace);
		if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
			pTrace->surface.flags |= SURF_SKY;
	}
	bool __fastcall HookedGetBool(void* pConVar, void* edx)
	{
		if ((uintptr_t)_ReturnAddress() == CAM_THINK)
			return true;

		return original_get_bool(pConVar);
	}
	float __fastcall GetViewmodelFOV()
	{
		if (INTERFACES::Engine->IsConnected() && INTERFACES::Engine->IsInGame())
		{
			float player_fov = original_viewmodel_fov();

			if (SETTINGS::settings.esp_bool)
				player_fov = SETTINGS::settings.viewfov_val;

			return player_fov;
		}
	}
	void Hook_LockCursor(void* xd)
	{
		SurfaceTable.UnHook();
		INTERFACES::Surface->lockcursor();
		SurfaceTable.ReHook();
		if (menu_open)
			INTERFACES::Surface->unlockcursor();
	}
	void InitHooks()
	{
		INTERFACES::Engine->ClientCmd_Unrestricted("clear");
		Sleep(200);
		INTERFACES::cvar->ConsoleColorPrintf(CColor(200, 255, 0, 255), "[Elevate.win] \n");
		INTERFACES::cvar->ConsoleColorPrintf(CColor(200, 100, 0, 255), " A lot of gradient stuff \n");
		INTERFACES::cvar->ConsoleColorPrintf(CColor(200, 100, 0, 255), " Better resolver \n");
		INTERFACES::cvar->ConsoleColorPrintf(CColor(200, 100, 0, 255), " Better backtrack \n");
		INTERFACES::cvar->ConsoleColorPrintf(CColor(200, 100, 0, 255), " Better fakewalk fix \n");

		iclient_hook_manager.Init(INTERFACES::Client);
		original_frame_stage_notify = reinterpret_cast<FrameStageNotifyFn>(iclient_hook_manager.HookFunction<FrameStageNotifyFn>(37, HookedFrameStageNotify));

		panel_hook_manager.Init(INTERFACES::Panel);
		original_paint_traverse = reinterpret_cast<PaintTraverseFn>(panel_hook_manager.HookFunction<PaintTraverseFn>(41, HookedPaintTraverse));

		model_render_hook_manager.Init(INTERFACES::ModelRender);
		original_draw_model_execute = reinterpret_cast<DrawModelExecuteFn>(model_render_hook_manager.HookFunction<DrawModelExecuteFn>(21, HookedDrawModelExecute));

		scene_end_hook_manager.Init(INTERFACES::RenderView);
		original_scene_end = reinterpret_cast<SceneEndFn>(scene_end_hook_manager.HookFunction<SceneEndFn>(9, HookedSceneEnd));

		trace_hook_manager.Init(INTERFACES::Trace);
		original_trace_ray = reinterpret_cast<TraceRayFn>(trace_hook_manager.HookFunction<TraceRayFn>(5, HookedTraceRay));

		override_view_hook_manager.Init(INTERFACES::ClientMode);
		original_override_view = reinterpret_cast<OverrideViewFn>(override_view_hook_manager.HookFunction<OverrideViewFn>(18, HookedOverrideView));
		original_create_move = reinterpret_cast<CreateMoveFn>(override_view_hook_manager.HookFunction<CreateMoveFn>(24, HookedCreateMove));
		original_viewmodel_fov = reinterpret_cast<GetViewmodelFOVFn>(override_view_hook_manager.HookFunction<GetViewmodelFOVFn>(35, GetViewmodelFOV));

		auto sv_cheats = INTERFACES::cvar->FindVar("sv_cheats");
		get_bool_manager = VMT::VMTHookManager(reinterpret_cast<DWORD**>(sv_cheats));
		original_get_bool = reinterpret_cast<SvCheatsGetBoolFn>(get_bool_manager.HookFunction<SvCheatsGetBoolFn>(13, HookedGetBool));

		SurfaceTable.InitTable(INTERFACES::Surface);
		SurfaceTable.HookIndex(67, Hook_LockCursor);
	
	}
	void EyeAnglesPitchHook(const SDK::CRecvProxyData *pData, void *pStruct, void *pOut)
	{
		*reinterpret_cast<float*>(pOut) = pData->m_Value.m_Float;

		auto entity = reinterpret_cast<SDK::CBaseEntity*>(pStruct);
		if (!entity)
			return;

	}
	void EyeAnglesYawHook(const SDK::CRecvProxyData *pData, void *pStruct, void *pOut)
	{
		*reinterpret_cast<float*>(pOut) = pData->m_Value.m_Float;

		auto entity = reinterpret_cast<SDK::CBaseEntity*>(pStruct);
		if (!entity)
			return;
	}

	void InitNetvarHooks()
	{
		UTILS::netvar_hook_manager.Hook("DT_CSPlayer", "m_angEyeAngles[0]", EyeAnglesPitchHook);
		UTILS::netvar_hook_manager.Hook("DT_CSPlayer", "m_angEyeAngles[1]", EyeAnglesYawHook);
	}
}