#include "../includes.h"
#include "../UTILS/interfaces.h"
#include "../SDK/IEngine.h"
#include "../SDK/CTrace.h"
#include "../SDK/CBaseWeapon.h"
#include "../SDK/CUserCmd.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/ConVar.h"
#include "../FEATURES/AutoWall.h"
#include "../FEATURES/Aimbot.h"
#include "../FEATURES/Movement.h"

#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))
#define CheckIfNonValidNumber(x) (fpclassify(x) == FP_INFINITE || fpclassify(x) == FP_NAN || fpclassify(x) == FP_SUBNORMAL)

void CMovement::bunnyhop(SDK::CUserCmd* cmd)
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player) return;

	if (cmd->buttons & IN_JUMP)
	{
		int local_flags = local_player->GetFlags();
		if (!(local_flags & FL_ONGROUND))
			cmd->buttons &= ~IN_JUMP;
	}
}

float get_move_angle(float speed)
{
	auto move_angle = RAD2DEG(asin(15.f / speed));

	if (!isfinite(move_angle) || move_angle > 90.f)
		move_angle = 90.f;
	else if (move_angle < 0.f)
		move_angle = 0.f;

	return move_angle;
}

bool will_hit_obstacle_in_future(float predict_time, float step)
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player) return false;

	static auto sv_gravity = INTERFACES::cvar->FindVar("sv_gravity");
	static auto sv_jump_impulse = INTERFACES::cvar->FindVar("sv_jump_impulse");

	bool ground = local_player->GetFlags() & FL_ONGROUND;
	auto gravity_per_tick = sv_gravity->GetFloat() * INTERFACES::Globals->interval_per_tick;

	auto start = local_player->GetVecOrigin(), end = start;
	auto velocity = local_player->GetVelocity();

	auto min = local_player->GetVecMins(), maxs = local_player->GetVecMaxs();

	SDK::trace_t trace;
	SDK::CTraceWorldOnly filter;
	SDK::Ray_t ray;

	auto predicted_ticks_needed = TIME_TO_TICKS(predict_time);
	auto velocity_rotation_angle = RAD2DEG(atan2(velocity.y, velocity.x));
	auto ticks_done = 0;

	if (predicted_ticks_needed <= 0)return false;

	while (true)
	{
		auto rotation_angle = velocity_rotation_angle + step;

		velocity.x = cos(DEG2RAD(rotation_angle)) * velocity.Length2D();
		velocity.y = sin(DEG2RAD(rotation_angle)) * velocity.Length2D();
		velocity.z = ground ? sv_jump_impulse->GetFloat() : velocity.z - gravity_per_tick;

		end += velocity * INTERFACES::Globals->interval_per_tick;

		ray.Init(start, end, min, maxs);
		INTERFACES::Trace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

		if (trace.flFraction != 1.f && trace.plane.normal.z <= 0.9f || trace.startSolid || trace.allsolid)
			break;

		end = trace.end;
		end.z -= 2.f;

		ray.Init(trace.end, end, min, maxs);
		INTERFACES::Trace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &trace);

		ground = (trace.flFraction < 1.f || trace.allsolid || trace.startSolid) && trace.plane.normal.z >= 0.7f;

		if (++ticks_done >= predicted_ticks_needed)
			return false;

		velocity_rotation_angle = rotation_angle;
	}
	return true;
}

void CMovement::quick_stop(SDK::CBaseEntity* entity, SDK::CUserCmd* cmd)
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer()); //initialize local player

	if (!local_player) //make sure this nigga aint null
		return;

	Vector local_position = local_player->GetVecOrigin() + local_player->GetViewOffset(); //get eye position

	if (entity->GetImmunity()) //make sure u dont stop when they in spawn protect
		return;

	if (entity->GetIsDormant()) //fuck dormant niggas
		return;

	auto weapon = reinterpret_cast<SDK::CBaseWeapon*>(INTERFACES::ClientEntityList->GetClientEntity(local_player->GetActiveWeaponIndex())); //initialize weapon

	if (!weapon) //make sure u aint holdin aiR niGGa
		return;

	if (weapon->is_knife() || weapon->is_grenade() || weapon->GetItemDefenitionIndex() == SDK::WEAPON_TASER) //we dont wanna stop if we holdin a knife, grenade or zeus
		return;

	if (!aimbot->can_shoot(cmd)) //so it doesn't just fully freeze us while reloading or cocking
		return;

	if (autowall->CalculateDamage(local_position, aimbot->point(entity, SETTINGS::settings.acc_type), local_player, entity).damage > SETTINGS::settings.damage_val) //autowall to the enemies position and see if they meet minimum damage
	{
		static int choked = 0;
		choked = choked > 7 ? 0 : choked + 1;
		GLOBAL::originalCMD.move.x = choked < 2 || choked > 3 ? 0 : GLOBAL::originalCMD.move.x;
		GLOBAL::originalCMD.move.y = choked < 2 || choked > 3 ? 0 : GLOBAL::originalCMD.move.y;
		GLOBAL::should_send_packet = choked < 1;

	}
}

void CMovement::autostrafer(SDK::CUserCmd* cmd)
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player) return;

	if (!SETTINGS::settings.strafe_bool)
		return;

	static auto switch_key = 1.f;
	static auto circle_yaw = 0.f;
	static auto old_yaw = 0.f;

	auto velocity = local_player->GetVelocity();
	velocity.z = 0.f;

	auto speed = velocity.Length2D();
	auto ideal_strafe = get_move_angle(speed);

	switch_key *= -1.f;

	if (!(local_player->GetFlags() & FL_ONGROUND))
	{
		auto yaw_delta = MATH::NormalizeAngle(GLOBAL::originalCMD.viewangles.y - old_yaw);
		auto absolute_yaw_delta = abs(yaw_delta);

		circle_yaw = old_yaw = GLOBAL::originalCMD.viewangles.y;

		if (yaw_delta > 0.f)
			GLOBAL::originalCMD.move.y = -450.f;
		else if (yaw_delta < 0.f)
			GLOBAL::originalCMD.move.y = 450.f;

		if (absolute_yaw_delta <= ideal_strafe || absolute_yaw_delta >= 30.f)
		{
			Vector velocity_angles;
			MATH::VectorAngles2(velocity, velocity_angles);

			auto velocity_delta = MATH::NormalizeAngle(GLOBAL::originalCMD.viewangles.y - velocity_angles.y);
			auto retrack = 2.f * (ideal_strafe * 2.f);

			if (velocity_delta <= retrack || speed <= 15.f)
			{
				if (-retrack <= velocity_delta || speed <= 15.f)
				{
					GLOBAL::originalCMD.viewangles.y += ideal_strafe * switch_key;
					GLOBAL::originalCMD.move.y = switch_key * 450.f;
				}
				else
				{
					GLOBAL::originalCMD.viewangles.y = velocity_angles.y - retrack;
					GLOBAL::originalCMD.move.y = 450.f;
				}
			}
			else
			{
				GLOBAL::originalCMD.viewangles.y = velocity_angles.y + retrack;
				GLOBAL::originalCMD.move.y = -450.f;
			}
		}
	}
}



void CMovement::duckinair(SDK::CUserCmd* cmd)
{
	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player) return;

	int local_flags = local_player->GetFlags();
	if (!(local_flags & FL_ONGROUND))
		cmd->buttons |= IN_DUCK;
}

CMovement* movement = new CMovement();