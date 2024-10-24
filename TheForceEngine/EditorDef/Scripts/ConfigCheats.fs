/////////////////////////////////////////////////////////////
// 
/////////////////////////////////////////////////////////////

void main()
{
	bool invincible = darkForces.cheat_getInvincible();
	if (imgui.Checkbox("Invinciblity (LAIMLAME)", invincible))
	{
		darkForces.cheat_toggleInvincible();
	}
	
	bool aiDisabled = darkForces.cheat_getAiDisabled();
	if (imgui.Checkbox("Disable AI (LAREDLITE)", aiDisabled))
	{
		darkForces.cheat_toggleAi();
	}
	
	bool heightCheckDisabled = darkForces.cheat_getHeightCheckDisabled();
	if (imgui.Checkbox("Disable height check (LAPOGO)", heightCheckDisabled))
	{
		darkForces.cheat_toggleHeightCheck();
	}
	
	bool smallMode = darkForces.cheat_getSmallMode();
	if (imgui.Checkbox("Small mode (LABUG)", smallMode))
	{
		darkForces.cheat_toggleSmallMode();
	}
	frontEndUi.Tooltip("Allows the player to fit through narrow gaps.");
	
	bool showData = darkForces.cheat_getData();
	if (imgui.Checkbox("Show coordinates (LADATA)", showData))
	{
		darkForces.cheat_toggleData();
	}
	
	bool fly = darkForces.cheat_getFly();
	if (imgui.Checkbox("Fly (LAFLY)", fly))
	{
		darkForces.cheat_toggleFly();
	}
	
	bool fullBright = darkForces.cheat_getFullBright();
	if (imgui.Checkbox("Full-Bright (LABRIGHT)", fullBright))
	{
		darkForces.cheat_toggleFullBright();
	}
	
	bool noClip = darkForces.cheat_getNoClip();
	if (imgui.Checkbox("Noclip (LANOCLIP)", noClip))
	{
		darkForces.cheat_toggleNoClip();
	}
	frontEndUi.Tooltip("Allows the player to move through walls.");
	
	if (imgui.Checkbox("One-hit kill (LAIMDEATH)", cheat_oneHitKill))
	{
		cheat_toggleOneHitKill();
	}
	frontEndUi.Tooltip("Direct attacks kill any enemy in one hit (does not apply to explosions).");
	
	if (imgui.Checkbox("Instant death (LAHARDCORE)", cheat_instantDeath))
	{
		cheat_toggleInstantDeath();
	}
	frontEndUi.Tooltip("Player dies in one hit from projectiles and explosions.");
	
	if (imgui.Button("Reveal full map (LACDS)"))
	{
		cheat_revealMap();
	}	
	if (imgui.Button("Grant supercharge (LARANDY)"))
	{
		cheat_supercharge();
	}		
	if (imgui.Button("Grant all weapons/max health (LAPOSTAL)"))
	{
		cheat_postal();
	}	
}