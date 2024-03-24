#pragma once

#include <cstdint>
#include "il2cpp.h"

namespace CombatMaster
{
	extern uintptr_t GameAssembly;

	template <typename T>
	T ReadMemory(uintptr_t address);

	/* CombatMaster.Battle.Gameplay.Player.PlayerRoot */
	namespace PlayerRoot
	{
		auto GetStaticFields() -> uintptr_t;
		auto GetMyPlayer() -> uintptr_t;
		auto GetAllPlayers() -> List*;
		
		/* Members */
		auto GetActiveMobView(uintptr_t playerRoot) -> uintptr_t;
		auto GetMainCamera(uintptr_t playerRoot) -> uintptr_t;
		auto IsRealPlayer(uintptr_t playerRoot) -> bool;
		auto GetHealth(uintptr_t playerRoot) -> uintptr_t;
		auto GetController(uintptr_t playerRoot) -> uintptr_t;
		auto GetTeamId(uintptr_t playerRoot) -> int32_t;
	}

	namespace Offsets::PlayerRoot
	{
		// constexpr auto Instance = 0x6BEBC20; 
		constexpr auto Instance = 0x4CD0100; // updated

		constexpr auto StaticFields = 0xB8;
		constexpr auto MyPlayer = 0x8;
		constexpr auto AllPlayers = 0x18;

		/* Members */
		//constexpr auto GetActiveMobView = 0x449DFA0;
		constexpr auto GetActiveMobView = 0x3872870; // updated
		constexpr auto CameraManager = 0x1A644C0;
		//constexpr auto GetMainCamera = 0x449E3C0;
		constexpr auto GetMainCamera = 0x38755E0; // updated

		// constexpr auto IsRealPlayer = 0x448DDF0; // byte oldver:0xFA
		constexpr auto IsRealPlayer = 0x3E35F20; // updated 0xFE or 0x122
		
		// 48 8B 81 ? ? ? ? C3 CC CC CC CC CC CC CC CC 48 81 C1 ? ? ? ? 48 89 11 E9 ? ? ? ? CC 48 8B C4
		constexpr auto PlayerController = 0x88; // newver:0x14E3270  oldver:0x1B20EE0 
		
		// E8 ? ? ? ? 33 DB 48 89 44 24 ? 48 8B C8
		// constexpr auto PlayerHealth = 0xAC32D0; // oldver:0xA0
		// constexpr auto PlayerHealth = 0x680DD0; // updated 0xa0
		constexpr auto PlayerHealth = 0xB0; 
		
		// constexpr auto GetTeamId = 0x449E590;
		constexpr auto GetTeamId = 0x3875830; // updated
	}

	/* CombatMaster.Battle.Gameplay.Player.PlayerMobView */
	namespace PlayerMobView
	{
		auto GetHeadTransform(uintptr_t playerMobView) -> uintptr_t;
	}

	namespace Offsets::PlayerMobView
	{
		// E8 ? ? ? ? 48 8B E8 8B D3
		// constexpr auto GetHeadTransform = 0x660840; // oldver 0x30
		constexpr auto GetHeadTransform = 0x599840; // updated 0x30
	}

	/* CombatMaster.Battle.Gameplay.Player.PlayerHealth*/
	namespace PlayerHealth
	{
		auto GetPercent(uintptr_t playerHealth) -> float;
	}

	namespace Offsets::PlayerHealth
	{
		// constexpr auto GetHealthPercent = 0x440B370;
		constexpr auto GetHealthPercent = 0x3823E00; // updated
	}

	/* UnityEngine.Transform */
	namespace Transform
	{
		auto GetPosition(uintptr_t transform) -> UnityVector3;
	}

	namespace Offsets::Transform
	{
		// constexpr auto GetPosition = 0x3A34270; 
		constexpr auto GetPosition = 0x34300D0; // updated
	}

	/* UnityEngine.Camera */
	namespace Camera
	{	

		auto WorldToScreenPoint(uintptr_t camera, const UnityVector3& world, UnityVector3& screen) -> bool;
	}

	namespace Offsets::Camera
	{
		// constexpr auto WorldToScreenPoint = 0x39D7210;
		constexpr auto WorldToScreenPoint = 0x33D1340; // updated
	}

	/* UnityEngine.CoreModule */
	namespace Component
	{
		auto GetTransform(uintptr_t component) -> uintptr_t;

	}

	namespace Offsets::Component
	{
		// constexpr auto GetTransform = 0x3A1B370;
		constexpr auto GetTransform = 0x3417150; // updated
	}

	/* */
	namespace Vector3
	{
		auto GetDistance(const UnityVector3& a, const UnityVector3& b) -> float;
	}
	
	namespace Offsets::Vector3
	{
		// constexpr auto GetDistance = 0x3A17D30;
		constexpr auto GetDistance = 0x3413B20; // updated
	}
}
