#include "StdAfx.h"
#include "Piercing.h"


CPiercing::CPiercing()
{
}

CPiercing::~CPiercing()
{
}

CPiercing& CPiercing::operator=( const CPiercing & piercing )
{
	m_vPiercing.assign( piercing.m_vPiercing.begin(), piercing.m_vPiercing.end() );
#if __VER >= 12 // __EXT_PIERCING
	m_vUltimatePiercing.assign( piercing.m_vUltimatePiercing.begin(), piercing.m_vUltimatePiercing.end() );
#endif // __EXT_PIERCING
#if __VER >= 15 // __PETVIS
	m_vtmPetVis.assign( piercing.m_vtmPetVis.begin(), piercing.m_vtmPetVis.end() );
#endif // __PETVIS
#ifdef __WEAPON_MERGE
	m_vMergedPiercing.assign(piercing.m_vMergedPiercing.begin(), piercing.m_vMergedPiercing.end());
#endif //__WEAPON_MERGE
	return *this;
}

void CPiercing::Clear( void )
{
	m_vPiercing.clear();
#if __VER >= 12 // __EXT_PIERCING
	m_vUltimatePiercing.clear();
#endif // __EXT_PIERCING
#if __VER >= 15 // __PETVIS
	m_vtmPetVis.clear();
#endif // __PETVIS
#ifdef __WEAPON_MERGE
	m_vMergedPiercing.clear();
#endif //__WEAPON_MERGE
}

void CPiercing::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << static_cast<unsigned long>(GetPiercingSize());

		for( int i = 0; i < GetPiercingSize(); i++ )
			ar << GetPiercingItem( i );
#if __VER >= 12 // __EXT_PIERCING

		ar << static_cast<unsigned long>(GetUltimatePiercingSize());

		for( int i = 0; i < GetUltimatePiercingSize(); i++ )
			ar << GetUltimatePiercingItem( i );
#endif // __EXT_PIERCING
#if __VER >= 15 // __PETVIS

		ar << static_cast<unsigned long>(m_vtmPetVis.size());

		for( int i=0; i<(int)( m_vtmPetVis.size() ); i++ )
			ar << GetVisKeepTime( i ) - time_null();
#endif // __PETVIS
#ifdef __WEAPON_MERGE
		ar << static_cast<unsigned long>(GetMergedPiercingSize());
		for (int i = 0; i < GetMergedPiercingSize(); i++)
			ar << GetMergedPiercingItem(i);
#endif //__WEAPON_MERGE
	}
	else
	{
		m_vPiercing.clear();
		unsigned long nSize;

		ar >> nSize;
		SetPiercingSize( nSize );
		for( int i = 0; i < nSize; i++ )
		{
			DWORD dwItem;
			ar >> dwItem;
			SetPiercingItem( i, dwItem );
		}
#if __VER >= 12 // __EXT_PIERCING
		m_vUltimatePiercing.clear();
		ar >> nSize;
		SetUltimatePiercingSize( nSize );
		for( int i = 0; i < nSize; i++ )
		{
			DWORD dwItem;
			ar >> dwItem;
			SetUltimatePiercingItem( i, dwItem );
		}
#endif // __EXT_PIERCING
#if __VER >= 15 // __PETVIS
		ar >> nSize;
		SetVisKeepTimeSize( nSize );
		for( int i=0; i<nSize; i++ )
		{
			time_t tmTemp;
			ar >> tmTemp;
			SetVisKeepTime( i, tmTemp + time_null() );
		}
#endif // __PETVIS
#ifdef __WEAPON_MERGE
		m_vMergedPiercing.clear();
		ar >> nSize;
		SetMergedPiercingSize(nSize);
		for (int i = 0; i < nSize; i++)
		{
			DWORD dwItem;
			ar >> dwItem;
			SetMergedPiercingItem(i, dwItem);
		}
#endif //__WEAPON_MERGE
	}
}

void CPiercing::SetPiercingSize( int nSize )
{
#ifdef __GAMEGUARD
	if (nSize < 0)
		nSize = 0;

	if (nSize > MAX_PIERCING_WEAPON)
		nSize = MAX_PIERCING_WEAPON;

	if (nSize == 0)
	{
		m_vPiercing.clear();
		return;
	}

	m_vPiercing.resize(nSize, 0);
#else //__GAMEGUARD
	if (nSize > MAX_PIERCING_WEAPON)
		nSize = MAX_PIERCING_WEAPON;

	m_vPiercing.resize(nSize, 0);
#endif //__GAMEGUARD
}

int CPiercing::GetPiercingSize()
{
	return m_vPiercing.size();
}

void CPiercing::SetPiercingItem( int nth, DWORD dwItem )
{
	if( nth >= GetPiercingSize() )
		return;

#ifdef __AEGON_FIXES
	if (nth < 0)
		return;
#endif
	
	m_vPiercing[nth]	= dwItem;
}

DWORD CPiercing::GetPiercingItem( int nth )
{
	if( nth >= GetPiercingSize()  )
		return 0;

#ifdef __AEGON_FIXES
	if (nth < 0)
		return 0;
#endif
	
	return m_vPiercing[nth];
}

BOOL CPiercing::IsPiercedItem()
{
	if( GetPiercingSize() > 0
#if __VER >= 12 // __EXT_PIERCING
		|| GetUltimatePiercingSize() > 0 
#endif // __EXT_PIERCING
#ifdef __WEAPON_MERGE
		|| GetMergedPiercingSize() > 0
#endif
		)
		return TRUE;

	return FALSE;
}

#if __VER >= 12 // __EXT_PIERCING
void CPiercing::SetUltimatePiercingSize( int nSize )
{
	if( nSize > MAX_PIERCING_ULTIMATE )
		nSize = MAX_PIERCING_ULTIMATE;

#ifdef __AEGON_FIXES
	if (nSize < 0)
		nSize = 0;
#endif

	m_vUltimatePiercing.resize( nSize, 0 );
}

int CPiercing::GetUltimatePiercingSize()
{
	return m_vUltimatePiercing.size();
}

void CPiercing::SetUltimatePiercingItem( int nth, DWORD dwItem )
{
	if( nth >= GetUltimatePiercingSize() )
		return;

#ifdef __AEGON_FIXES
	if (nth < 0)
		return;
#endif

	m_vUltimatePiercing[nth]	= dwItem;
}

DWORD CPiercing::GetUltimatePiercingItem( int nth )
{
	if( nth >= GetUltimatePiercingSize()  )
		return 0;

	return m_vUltimatePiercing[nth];
}
#endif // __EXT_PIERCING

#if __VER >= 15 // __PETVIS
void CPiercing::SetVisKeepTime( int nth, time_t tmKeep )
{
	if( nth >= GetPiercingSize() || nth >= (int)( m_vtmPetVis.size() ) )
		return;

#ifdef __AEGON_FIXES
	if (nth < 0)
		return;
#endif

	m_vtmPetVis[nth] = tmKeep;
}

#ifdef _WIN64
__int64 CPiercing::GetVisKeepTime(int nth)
#else
time_t CPiercing::GetVisKeepTime( int nth )
#endif
{
	if( nth >= GetPiercingSize() || nth >= (int)( m_vtmPetVis.size() ) )
		return 0;

#ifdef __AEGON_FIXES
	if (nth < 0)
		return 0;
#endif

	return m_vtmPetVis[nth];
}
#endif // __PETVIS

#ifdef __WEAPON_MERGE
void CPiercing::SetMergedPiercingSize(int nSize)
{
#ifdef __AEGON_FIXES
	if (nSize < 0)
		nSize = 0;
	if (nSize > MAX_PIERCING_WEAPON)
		nSize = MAX_PIERCING_WEAPON;

	if (nSize == 0)
	{
		m_vMergedPiercing.clear();
		return;
}
#else
	if (nSize >= MAX_PIERCING_WEAPON)
		return;
#endif

	m_vMergedPiercing.resize(nSize, 0);
}
int	CPiercing::GetMergedPiercingSize()
{
	return m_vMergedPiercing.size();
}
void CPiercing::SetMergedPiercingItem(int nth, DWORD dwItem)
{
	if (nth >= GetMergedPiercingSize() || nth < 0)
		return;
	m_vMergedPiercing[nth] = dwItem;
}
DWORD CPiercing::GetMergedPiercingItem(int nth)
{
	if (nth >= GetMergedPiercingSize() || nth < 0)
		return NULL;
	return m_vMergedPiercing[nth];
}
#endif // __WEAPON_MERGE