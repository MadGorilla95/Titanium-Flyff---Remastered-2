#pragma once
#ifndef __AEGBOSSMECH__H
#define __AEGBOSSMECH__H
#ifdef __AEGON_BOSS_MECHANICS

typedef struct __AEGON_MOVER_ATTACKS
{
	DWORD m_dwMinWaitTime;
	vector<DWORD> m_vdwAttacks;

	__AEGON_MOVER_ATTACKS()
	{
		m_dwMinWaitTime = 1;
		m_vdwAttacks.clear();
	}
}__AEGON_MOVER_ATTACKS;

typedef struct __AEGON_SPECIAL_ATK
{
	DWORD m_dwAttackAbilityID;
	double m_fChance;

	__AEGON_SPECIAL_ATK()
	{
		m_dwAttackAbilityID = 0;
		m_fChance = 0.0;
	}
}__AEGON_SPECIAL_ATK;


class CAegonBossMechanicsMng
{
public:

	LPCSTR m_scriptFile = "AegonBossMechanics.inc";
	BOOL m_bScriptLoaded;

	map<DWORD, __AEGON_MOVER_ATTACKS> m_mapMoverAttacks;

	map<DWORD, __AEGON_SPECIAL_ATK> m_mapAttacks;

	CAegonBossMechanicsMng();
	virtual ~CAegonBossMechanicsMng();

	static CAegonBossMechanicsMng* GetInstance(void);

	BOOL LoadScript();

	void OnActEndSpecialAtk(CMover* pMover);
	__AEGON_MOVER_ATTACKS* GetMoverSpecialAttacks(DWORD dwMoverId);
	__AEGON_SPECIAL_ATK* GetSpecialAttack(DWORD dwAttackId);


#ifdef __WORLDSERVER
	BOOL OnChanceSpecialAtk(CMover* pMover, CMover* pTarget);
#endif
};
#endif // __AEGON_BOSS_MECHANICS

#endif //__AEGBOSSMECH__H