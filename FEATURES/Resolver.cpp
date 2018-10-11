#include "../includes.h"
#include "../UTILS/interfaces.h"
#include "../SDK/IEngine.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../UTILS/render.h"
#include "../SDK/CTrace.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/ConVar.h"
#include "../SDK/AnimLayer.h"
#include "../UTILS/qangle.h"
#include "../FEATURES/Aimbot.h"
#include "../FEATURES/Resolver.h"

int CResolver::GetResolveTypeIndex(unsigned short resolve_type)
{
	/// gonna have to use lorge if statements cuz fuck you
	if (resolve_type & RESOLVE_TYPE_OVERRIDE)
		return 0;
	else if (resolve_type & RESOLVE_TYPE_NO_FAKE)
		return 1;
	else if (resolve_type & RESOLVE_TYPE_LBY)
		return 2;
	else if (resolve_type & RESOLVE_TYPE_LBY_UPDATE)
		return 3;
	else if (resolve_type & RESOLVE_TYPE_PREDICTED_LBY_UPDATE)
		return 4;
	else if (resolve_type & RESOLVE_TYPE_LAST_MOVING_LBY)
		return 5;
	else if (resolve_type & RESOLVE_TYPE_NOT_BREAKING_LBY)
		return 6;
	else if (resolve_type & RESOLVE_TYPE_BRUTEFORCE)
		return 7;
	else if (resolve_type & RESOLVE_TYPE_LAST_MOVING_LBY_DELTA)
		return 8;
	else if (resolve_type & RESOLVE_TYPE_ANTI_FREESTANDING)
		return 9;

	return 0;
}
#define enc_str(s) std::string(s)
std::string CResolver::TranslateResolveRecord(unsigned short resolve_type)
{
	std::vector<std::string> resolve_strings;

	/// gonna have to use lorge if statements cuz fuck you
	if (resolve_type & RESOLVE_TYPE_OVERRIDE)
		resolve_strings.push_back(enc_str("OVERRIDE"));
	if (resolve_type & RESOLVE_TYPE_NO_FAKE)
		resolve_strings.push_back(enc_str("NO FAKE"));
	if (resolve_type & RESOLVE_TYPE_LBY)
		resolve_strings.push_back(enc_str("LBY"));
	if (resolve_type & RESOLVE_TYPE_LBY_UPDATE)
		resolve_strings.push_back(enc_str("LBY UPDATE"));
	if (resolve_type & RESOLVE_TYPE_PREDICTED_LBY_UPDATE)
		resolve_strings.push_back(enc_str("PREDICTED LBY UPDATE"));
	if (resolve_type & RESOLVE_TYPE_NOT_BREAKING_LBY)
		resolve_strings.push_back(enc_str("NOT BREAKING LBY"));
	if (resolve_type & RESOLVE_TYPE_BRUTEFORCE)
		resolve_strings.push_back(enc_str("BRUTEFORCE"));
	if (resolve_type & RESOLVE_TYPE_LAST_MOVING_LBY_DELTA)
		resolve_strings.push_back(enc_str("LAST MOVING LBY DELTA"));
	if (resolve_type & RESOLVE_TYPE_LAST_MOVING_LBY)
		resolve_strings.push_back(enc_str("LAST MOVING LBY"));
	if (resolve_type & RESOLVE_TYPE_ANTI_FREESTANDING)
		resolve_strings.push_back(enc_str("ANTI FREESTAND"));

	if (resolve_strings.size() <= 0)
		return "ERROR";

	std::string buffer;
	for (const auto& str : resolve_strings)
		buffer.size() ? (buffer += " / " + str) : (buffer = str);

	return buffer;
}

CColor CResolver::GetResolveColor(unsigned short resolve_type)
{
	/// gonna have to use lorge if statements cuz fuck you
	if (resolve_type & RESOLVE_TYPE_OVERRIDE)
		return PURPLE;
	else if (resolve_type & RESOLVE_TYPE_NO_FAKE)
		return BLACK;
	else if (resolve_type & RESOLVE_TYPE_LBY)
		return RED;
	else if (resolve_type & RESOLVE_TYPE_LBY_UPDATE)
		return LIGHTRED;
	else if (resolve_type & RESOLVE_TYPE_PREDICTED_LBY_UPDATE)
		return DARKGREY;

	else if (resolve_type & RESOLVE_TYPE_NOT_BREAKING_LBY)
		return DARKRED;
	else if (resolve_type & RESOLVE_TYPE_BRUTEFORCE)
		return GREEN;
	else if (resolve_type & RESOLVE_TYPE_LAST_MOVING_LBY_DELTA)
		return ORANGE;
	else if (resolve_type & RESOLVE_TYPE_LAST_MOVING_LBY)
		return BLUE;
	else if (resolve_type & RESOLVE_TYPE_ANTI_FREESTANDING)
		return LIGHTBLUE;

	return WHITE;
}




void CResolver::record(SDK::CBaseEntity* entity, float new_yaw)
{
	if (entity->GetVelocity().Length2D() > 36)
		return;

	auto c_baseweapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(entity->GetActiveWeaponIndex()));

	if (!c_baseweapon)
		return;

	auto &info = player_info[entity->GetIndex()];

	if (entity->GetActiveWeaponIndex() && info.last_ammo < c_baseweapon->GetLoadedAmmo()) {
		//ignore the yaw when it is from shooting (will be looking at you/other player)
		info.last_ammo = c_baseweapon->GetLoadedAmmo();
		return;
	}

	info.unresolved_yaw.insert(info.unresolved_yaw.begin(), new_yaw);
	if (info.unresolved_yaw.size() > 20) {
		info.unresolved_yaw.pop_back();
	}

	if (info.unresolved_yaw.size() < 2)
		return;

	auto average_unresolved_yaw = 0;
	for (auto val : info.unresolved_yaw)
		average_unresolved_yaw += val;
	average_unresolved_yaw /= info.unresolved_yaw.size();

	int delta = average_unresolved_yaw - entity->GetLowerBodyYaw();
	auto big_math_delta = abs((((delta + 180) % 360 + 360) % 360 - 180));

	info.lby_deltas.insert(info.lby_deltas.begin(), big_math_delta);
	if (info.lby_deltas.size() > 10) {
		info.lby_deltas.pop_back();
	}
}

static void nospread_resolve(SDK::CBaseEntity* player, int entID) //gay but very effective reversed aw air resolver
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	Vector local_position = local_player->GetVecOrigin() + local_player->GetViewOffset();
	float atTargetAngle = UTILS::CalcAngle(local_player->GetHealth() <= 0 ? local_player->GetVecOrigin() : local_position, player->GetVecOrigin()).y;
	Vector velocityAngle;
	MATH::VectorAngles(player->GetVelocity(), velocityAngle);

	float primaryBaseAngle = player->GetLowerBodyYaw();
	float secondaryBaseAngle = velocityAngle.y;

	switch ((shots_missed[entID]) % 1)
	{
	case 0:
		player->EasyEyeAngles()->yaw = atTargetAngle + 180.f;
		break;
	case 1:
		player->EasyEyeAngles()->yaw = velocityAngle.y + 180.f;
		break;
	case 2:
		player->EasyEyeAngles()->yaw = primaryBaseAngle;
		break;
	case 3:
		player->EasyEyeAngles()->yaw = primaryBaseAngle - 45.f;
		break;
	case 4:
		player->EasyEyeAngles()->yaw = primaryBaseAngle + 90.f;
		break;
	case 5:
		player->EasyEyeAngles()->yaw = primaryBaseAngle - 130.f;
		break;
	case 6:
		player->EasyEyeAngles()->yaw = primaryBaseAngle - 180.f;
		break;
	case 7:
		player->EasyEyeAngles()->yaw = secondaryBaseAngle;
		break;
	case 8:
		player->EasyEyeAngles()->yaw = secondaryBaseAngle - 40.f;
		break;
	case 9:
		player->EasyEyeAngles()->yaw = secondaryBaseAngle - 90.f;
		break;
	case 10:
		player->EasyEyeAngles()->yaw = secondaryBaseAngle - 130.f;
		break;
	case 11:
		player->EasyEyeAngles()->yaw = secondaryBaseAngle - 70.f;
		break;
	case 12:
		player->EasyEyeAngles()->yaw = primaryBaseAngle + 45.f;
		break;
	case 13:
		player->EasyEyeAngles()->yaw = primaryBaseAngle + 135.f;
		break;
	case 14:
		player->EasyEyeAngles()->yaw = primaryBaseAngle - 90.f;
		break;
	case 15:
		player->EasyEyeAngles()->yaw = primaryBaseAngle + 90.f;
		break;
	case 16:
		player->EasyEyeAngles()->yaw = primaryBaseAngle - 167.f;
		break;
	case 17:
		player->EasyEyeAngles()->yaw = primaryBaseAngle - 149.f;
		break;
	case 18:
		player->EasyEyeAngles()->yaw = primaryBaseAngle + 42.f;
		break;
	case 19:
		player->EasyEyeAngles()->yaw = primaryBaseAngle - 67.f;
		break;
	case 20:
		player->EasyEyeAngles()->yaw = primaryBaseAngle - 89.f;
		break;
	}
}

void CResolver::Nospread1(SDK::CBaseEntity * player, int entID)
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	Vector local_position = local_player->GetVecOrigin() + local_player->GetViewOffset();

	float atTargetAngle = UTILS::CalcAngle(local_player->GetHealth() <= 0 ? local_player->GetVecOrigin() : local_position, player->GetVecOrigin()).y;
	Vector velocityAngle;
	MATH::VectorAngles(player->GetVelocity(), velocityAngle);

	float primaryBaseAngle = player->GetLowerBodyYaw();
	float secondaryBaseAngle = velocityAngle.y;

	switch ((shots_missed[entID]) % 15)
	{
	case 0:
		player->EasyEyeAngles()->yaw = atTargetAngle + 180.f;
		break;
	case 1:
		player->EasyEyeAngles()->yaw = velocityAngle.y + 180.f;
		break;
	case 2:
		player->EasyEyeAngles()->yaw = primaryBaseAngle;
		break;
	case 3:
		player->EasyEyeAngles()->yaw = primaryBaseAngle - 45.f;
		break;
	case 4:
		player->EasyEyeAngles()->yaw = primaryBaseAngle + 90.f;
		break;
	case 5:
		player->EasyEyeAngles()->yaw = primaryBaseAngle - 130.f;
		break;
	case 6:
		player->EasyEyeAngles()->yaw = primaryBaseAngle - 180.f;
		break;
	case 7:
		player->EasyEyeAngles()->yaw = secondaryBaseAngle;
		break;
	case 8:
		player->EasyEyeAngles()->yaw = secondaryBaseAngle - 40.f;
		break;
	case 9:
		player->EasyEyeAngles()->yaw = secondaryBaseAngle - 90.f;
		break;
	case 10:
		player->EasyEyeAngles()->yaw = secondaryBaseAngle - 130.f;
		break;
	case 11:
		player->EasyEyeAngles()->yaw = secondaryBaseAngle - 70.f;
		break;
	case 12:
		player->EasyEyeAngles()->yaw = primaryBaseAngle + 45.f;
		break;
	case 13:
		player->EasyEyeAngles()->yaw = primaryBaseAngle + 135.f;
		break;
	case 14:
		player->EasyEyeAngles()->yaw = primaryBaseAngle - 90.f;
		break;
	case 15:
		player->EasyEyeAngles()->yaw = primaryBaseAngle / 1.1;
		break;
	case 16:
		player->EasyEyeAngles()->yaw = primaryBaseAngle * 1.1;
		break;
	case 17:
		player->EasyEyeAngles()->yaw = secondaryBaseAngle / 1.13;
		break;
	case 18:
		player->EasyEyeAngles()->yaw = secondaryBaseAngle * 1.13;
		break;
	case 19:
		player->EasyEyeAngles()->yaw = atTargetAngle / 1.12;
		break;
	case 20:
		player->EasyEyeAngles()->yaw = atTargetAngle * 1.12;
		break;
	case 21:
		player->EasyEyeAngles()->yaw = atTargetAngle / 1.5;
		break;
	case 22:
		player->EasyEyeAngles()->yaw = atTargetAngle * 1.5;
		break;
	case 23:
		player->EasyEyeAngles()->roll = atTargetAngle * 1.12;
		break;
	}
}

bool find_layer(SDK::CBaseEntity* entity, int act, SDK::CAnimationLayer *set)
{
	for (int i = 0; i < 13; i++)
	{
		SDK::CAnimationLayer layer = entity->GetAnimOverlay(i);
		const int activity = entity->GetSequenceActivity(layer.m_nSequence);
		if (activity == act) {
			*set = layer;
			return true;
		}
	}
	return false;
}




void CResolver::ProcessSnapShots()
{
	if (shot_snapshots.size() <= 0)
		return;

	const auto snapshot = shot_snapshots.front();
	if (fabs(UTILS::GetCurtime() - snapshot.time) > 1.f)
	{

		shot_snapshots.erase(shot_snapshots.begin());
		return;
	}

	const int player_index = snapshot.entity->GetIndex();
	if (snapshot.hitgroup_hit != -1)
	{
		for (int i = 0; i < RESOLVE_TYPE_NUM; i++)
		{
			if (snapshot.resolve_record.resolve_type & (1 << i))
			{
				player_resolve_records[player_index].shots_fired[i]++;
				player_resolve_records[player_index].shots_hit[i]++;
			}
		}


	}
	else if (snapshot.first_processed_time != 0.f && fabs(UTILS::GetCurtime() - snapshot.first_processed_time) > 0.1f)
	{
		for (int i = 0; i < RESOLVE_TYPE_NUM; i++)
		{
			if (snapshot.resolve_record.resolve_type & (1 << i))
				player_resolve_records[player_index].shots_fired[i]++;
		}

		if (snapshot.resolve_record.resolve_type & RESOLVE_TYPE_LAST_MOVING_LBY)
			player_resolve_records[player_index].shots_missed_moving_lby++;

		if (snapshot.resolve_record.resolve_type & RESOLVE_TYPE_LAST_MOVING_LBY_DELTA)
			player_resolve_records[player_index].shots_missed_moving_lby_delta++;

	}
	else
		return;

	shot_snapshots.erase(shot_snapshots.begin());
}

bool CResolver::AntiFreestanding(SDK::CBaseEntity* entity, float& yaw)
{
	const auto freestanding_record = player_resolve_records[entity->GetIndex()].anti_freestanding_record;

	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player)
		return false;

	if (freestanding_record.left_damage >= 20 && freestanding_record.right_damage >= 20)
		return false;

	const float at_target_yaw = UTILS::CalcAngle(local_player->GetVecOrigin(), entity->GetVecOrigin()).y;
	if (freestanding_record.left_damage <= 0 && freestanding_record.right_damage <= 0)
	{
		if (freestanding_record.right_fraction < freestanding_record.left_fraction)
			yaw = at_target_yaw + 125.f;
		else
			yaw = at_target_yaw - 73.f;
	}
	else
	{
		if (freestanding_record.left_damage > freestanding_record.right_damage)
			yaw = at_target_yaw + 130.f;
		else
			yaw = at_target_yaw - 49.f;
	}

	return true;
}

void CResolver::UpdateResolveRecord(SDK::CBaseEntity* entity)
{
	const auto previous_record = player_resolve_records[entity->GetIndex()];
	auto& record = player_resolve_records[entity->GetIndex()];

	record.resolved_angles = record.networked_angles;
	record.velocity = entity->GetVelocity();
	record.origin = entity->GetVecOrigin();
	record.lower_body_yaw = entity->GetLowerBodyYaw();
	record.is_dormant = entity->GetIsDormant();

	record.resolve_type = 0;

	record.is_balance_adjust_triggered = false, record.is_balance_adjust_playing = false;
	for (int i = 0; i < 15; i++)
	{
		record.anim_layers[i] = entity->GetAnimOverlay(i);

		if (entity->GetSequenceActivity(record.anim_layers[i].m_nSequence) == SDK::CSGO_ACTS::ACT_CSGO_IDLE_TURN_BALANCEADJUST)
		{
			record.is_balance_adjust_playing = true;

			if (record.anim_layers[i].m_flWeight == 1 || record.anim_layers[i].m_flCycle > previous_record.anim_layers[i].m_flCycle)
				record.last_balance_adjust_trigger_time = UTILS::GetCurtime();
			if (fabs(UTILS::GetCurtime() - record.last_balance_adjust_trigger_time) < 0.5f)
				record.is_balance_adjust_triggered = true;
		}
	}

	if (record.is_dormant)
		record.next_predicted_lby_update = FLT_MAX;

	if (record.lower_body_yaw != previous_record.lower_body_yaw && !record.is_dormant && !previous_record.is_dormant)
		record.did_lby_flick = true;

	const bool is_moving_on_ground = record.velocity.Length2D() > 50 && entity->GetFlags() & FL_ONGROUND;
	if (is_moving_on_ground && record.is_balance_adjust_triggered)
		record.is_fakewalking = true;
	else
		record.is_fakewalking = false;

	if (is_moving_on_ground && !record.is_fakewalking && record.velocity.Length2D() > 1.f && !record.is_dormant)
	{
		record.is_last_moving_lby_valid = true;
		record.is_last_moving_lby_delta_valid = false;
		record.shots_missed_moving_lby = 0;
		record.shots_missed_moving_lby_delta = 0;
		record.last_moving_lby = record.lower_body_yaw + 45;
		record.last_time_moving = UTILS::GetCurtime();
	}
	if (!record.is_dormant && previous_record.is_dormant)
	{
		if ((record.origin - previous_record.origin).Length2D() > 16.f)
			record.is_last_moving_lby_valid = false;
	}
	if (!record.is_last_moving_lby_delta_valid && record.is_last_moving_lby_valid && record.velocity.Length2D() < 20 && fabs(UTILS::GetCurtime() - record.last_time_moving) < 1.0)
	{
		if (record.lower_body_yaw != previous_record.lower_body_yaw)
		{
			record.last_moving_lby_delta = MATH::NormalizeYaw(record.last_moving_lby - record.lower_body_yaw);
			record.is_last_moving_lby_delta_valid = true;
		}
	}

	if (MATH::NormalizePitch(record.networked_angles.x) > 5.f)
		record.last_time_down_pitch = UTILS::GetCurtime();

}





bool IsFakeWalking(SDK::CBaseEntity* entity) {
	for (int s = 0; s < 13; s++) {
		auto anim_layer = entity->GetAnimOverlay(s);
		if (!anim_layer.m_pOwner)
			continue;

		auto activity = entity->GetSequenceActivity(anim_layer.m_nSequence);
		GLOBAL::is_fakewalking = false;
		bool stage1 = false,			// stages needed cause we are iterating all layers, otherwise won't work :)
			stage2 = false,
			stage3 = false;

		for (int i = 0; i < 16; i++) {
			if (activity == 26 && anim_layer.m_flWeight < 0.4f)
				stage1 = true;
			if (activity == 7 && anim_layer.m_flWeight > 0.001f)
				stage2 = true;
			if (activity == 2 && anim_layer.m_flWeight == 0)
				stage3 = true;
			entity->EasyEyeAngles()->yaw = entity->GetLowerBodyYaw() / 1.17;
			entity->EasyEyeAngles()->roll = entity->GetEyeAngles().z * 1.03;
		}

		if (stage1 && stage2)
			GLOBAL::is_fakewalking = true;
		else
			GLOBAL::is_fakewalking = false;

		return GLOBAL::is_fakewalking;
	}
}



void CResolver::resolve(SDK::CBaseEntity* entity)
{
	if (SETTINGS::settings.resolve_bool)
	{
		auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

		if (!entity)
			return;

		if (!local_player)
			return;

		bool is_local_player = entity == local_player;
		bool is_teammate = local_player->GetTeam() == entity->GetTeam() && !is_local_player;

		if (is_local_player)
			return;

		if (is_teammate)
			return;

		if (entity->GetHealth() <= 0)
			return;

		if (local_player->GetHealth() <= 0)
			return;

		if ((SETTINGS::settings.overridemethod == 1 && GetAsyncKeyState(UTILS::INPUT::input_handler.keyBindings(SETTINGS::settings.overridekey))) || (SETTINGS::settings.overridemethod == 0 && SETTINGS::settings.overridething))
		{
			Vector viewangles; INTERFACES::Engine->GetViewAngles(viewangles);
			auto at_target_yaw = UTILS::CalcAngle(entity->GetVecOrigin(), local_player->GetVecOrigin()).y;

			auto delta = MATH::NormalizeYaw(viewangles.y - at_target_yaw);
			auto rightDelta = Vector(entity->GetEyeAngles().x, at_target_yaw + 90, entity->GetEyeAngles().z);
			auto leftDelta = Vector(entity->GetEyeAngles().x, at_target_yaw - 90, entity->GetEyeAngles().z);

			if (delta > 0)
				entity->SetEyeAngles(rightDelta);
			else
				entity->SetEyeAngles(leftDelta);
			return;
		}

		//--- Variable Declaration ---//;
		auto &info = player_info[entity->GetIndex()];
		float fl_lby = entity->GetLowerBodyYaw();

		info.lby = Vector(entity->GetEyeAngles().x, entity->GetLowerBodyYaw(), 0.f);
		info.inverse = Vector(entity->GetEyeAngles().x, entity->GetLowerBodyYaw() + 180.f, 0.f);
		info.last_lby = Vector(entity->GetEyeAngles().x, info.last_moving_lby, 0.f);
		info.inverse_left = Vector(entity->GetEyeAngles().x, entity->GetLowerBodyYaw() + 115.f, 0.f);
		info.inverse_right = Vector(entity->GetEyeAngles().x, entity->GetLowerBodyYaw() - 115.f, 0.f);

		info.back = Vector(entity->GetEyeAngles().x, UTILS::CalcAngle(entity->GetVecOrigin(), local_player->GetVecOrigin()).y + 180.f, 0.f);
		info.right = Vector(entity->GetEyeAngles().x, UTILS::CalcAngle(entity->GetVecOrigin(), local_player->GetVecOrigin()).y + 70.f, 0.f);
		info.left = Vector(entity->GetEyeAngles().x, UTILS::CalcAngle(entity->GetVecOrigin(), local_player->GetVecOrigin()).y - 70.f, 0.f);

		info.backtrack = Vector(entity->GetEyeAngles().x, lby_to_back[entity->GetIndex()], 0.f);

		shots_missed[entity->GetIndex()] = shots_fired[entity->GetIndex()] - shots_hit[entity->GetIndex()];

		info.is_moving = entity->GetVelocity().Length2D() > 0.1 && entity->GetFlags() & FL_ONGROUND;

		auto& resolve_record = player_resolve_records[entity->GetIndex()];
		info.is_jumping = !entity->GetFlags() & FL_ONGROUND;
		info.could_be_slowmo = entity->GetVelocity().Length2D() > 6 && entity->GetVelocity().Length2D() < 36 && !info.is_crouching;
		info.is_crouching = entity->GetFlags() & FL_DUCKING;
		info.is_dormant = entity->GetIsDormant();
		update_time[entity->GetIndex()] = info.next_lby_update_time;

	

		static float old_simtime[65];
		if (entity->GetSimTime() != old_simtime[entity->GetIndex()])
		{
			using_fake_angles[entity->GetIndex()] = entity->GetSimTime() - old_simtime[entity->GetIndex()] == INTERFACES::Globals->interval_per_tick;
			old_simtime[entity->GetIndex()] = entity->GetSimTime();
		}

		if (!using_fake_angles[entity->GetIndex()])
		{
			Nospread1(entity, entity->GetIndex()); 
		}
	}
}
CResolver* resolver = new CResolver();
