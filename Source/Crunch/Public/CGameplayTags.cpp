
#include "CGameplayTags.h"
#include "GameplayTagContainer.h"

namespace CGameplayTags
{

	// Character
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Dead, "Status.Dead", "Given to when character is dead");
	
	// Abilities
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(BasicAttack, "Ability.BasicAttack", "BasicAttack");

	// Combo
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Change, "Ability.Combo.Change", "Change group");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Combo01, "Ability.Combo.Change.Combo01", "Combo01");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Combo02, "Ability.Combo.Change.Combo02", "Combo02");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Combo03, "Ability.Combo.Change.Combo03", "Combo03");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Combo04, "Ability.Combo.Change.Combo04", "Combo04");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ComboEnd, "Ability.Combo.Change.ComboEnd", "ComboEnd");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(ComboDamage, "Ability.Combo.ComboDamage", "Combo Damage");
};