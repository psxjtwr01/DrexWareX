#include "sdk.h"
#include "imgui/imgui.h"

uintptr_t CombatMaster::GameAssembly = 0;

template <typename T>
T CombatMaster::ReadMemory(uintptr_t address)
{
	return *(T*)(address);
}

auto CombatMaster::PlayerRoot::GetStaticFields() -> uintptr_t
{
	uintptr_t instance = *(uintptr_t*)(GameAssembly + Offsets::PlayerRoot::Instance);
	if (!instance)
		return NULL;

	uintptr_t staticFields = *(uintptr_t*)(instance + Offsets::PlayerRoot::StaticFields);
	return staticFields;
}

auto CombatMaster::PlayerRoot::GetMyPlayer() -> uintptr_t
{
	uintptr_t staticFields = GetStaticFields();
	if (!staticFields)
		return NULL;

	return ReadMemory<uintptr_t>(staticFields + Offsets::PlayerRoot::MyPlayer);
}

auto CombatMaster::PlayerRoot::GetAllPlayers() -> List*
{
	uintptr_t staticFields = GetStaticFields();
	if (!staticFields)
		return NULL;

	return ReadMemory<List*>(staticFields + Offsets::PlayerRoot::AllPlayers);
}

auto CombatMaster::PlayerRoot::GetActiveMobView(uintptr_t playerRoot) -> uintptr_t
{
	return reinterpret_cast<uintptr_t(__fastcall*)(uintptr_t)>(GameAssembly + Offsets::PlayerRoot::GetActiveMobView)(playerRoot);
}

auto CombatMaster::PlayerRoot::GetMainCamera(uintptr_t playerRoot) -> uintptr_t
{
	return reinterpret_cast<uintptr_t(__fastcall*)(uintptr_t)>(GameAssembly + Offsets::PlayerRoot::GetMainCamera)(playerRoot);
}

auto CombatMaster::PlayerRoot::IsRealPlayer(uintptr_t playerRoot) -> bool
{
	return reinterpret_cast<bool(__fastcall*)(uintptr_t)>(GameAssembly + Offsets::PlayerRoot::IsRealPlayer)(playerRoot);


}

auto CombatMaster::PlayerRoot::GetHealth(uintptr_t playerRoot) -> uintptr_t
{
	return *(uintptr_t*)(playerRoot + Offsets::PlayerRoot::PlayerHealth);
}

auto CombatMaster::PlayerRoot::GetController(uintptr_t playerRoot) -> uintptr_t
{
	return *(uintptr_t*)(playerRoot + Offsets::PlayerRoot::PlayerController);
}

auto CombatMaster::PlayerRoot::GetTeamId(uintptr_t playerRoot) -> int32_t
{
	return reinterpret_cast<int32_t(__fastcall*)(uintptr_t)>(GameAssembly + Offsets::PlayerRoot::GetTeamId)(playerRoot);
}

auto CombatMaster::PlayerMobView::GetHeadTransform(uintptr_t playerMobView) -> uintptr_t
{
	return reinterpret_cast<uintptr_t(__fastcall*)(uintptr_t)>(GameAssembly + Offsets::PlayerMobView::GetHeadTransform)(playerMobView);
	//return *(uintptr_t*)(playerMobView + 0x30);
}

auto CombatMaster::Transform::GetPosition(uintptr_t transform) -> UnityVector3
{
	return reinterpret_cast<UnityVector3(__fastcall*)(uintptr_t)>(GameAssembly + Offsets::Transform::GetPosition)(transform);
}

auto CombatMaster::Camera::WorldToScreenPoint(uintptr_t camera, const UnityVector3& world, UnityVector3& screen) -> bool
{
	UnityVector3 result = reinterpret_cast<UnityVector3(__fastcall*)(uintptr_t, UnityVector3)>(GameAssembly + Offsets::Camera::WorldToScreenPoint)(camera, world);

	ImVec2 screenSize = ImGui::GetIO().DisplaySize;
	screen.x = result.x;
	screen.y = (screenSize.y - result.y);

	if (result.z < 0.1f)
		return false;

	return true;
}

auto CombatMaster::Component::GetTransform(uintptr_t component) -> uintptr_t
{
	return reinterpret_cast<uintptr_t(__fastcall*)(uintptr_t)>(CombatMaster::GameAssembly + Offsets::Component::GetTransform)(component);
}

auto CombatMaster::PlayerHealth::GetPercent(uintptr_t playerHealth) -> float
{
	return reinterpret_cast<float(__fastcall*)(uintptr_t)>(CombatMaster::GameAssembly + Offsets::PlayerHealth::GetHealthPercent)(playerHealth);
}

auto CombatMaster::Vector3::GetDistance(const UnityVector3& a, const UnityVector3& b) -> float
{
	return reinterpret_cast<float(__fastcall*)(const UnityVector3&, const UnityVector3&)>(CombatMaster::GameAssembly + Offsets::Vector3::GetDistance)(a, b);
}
