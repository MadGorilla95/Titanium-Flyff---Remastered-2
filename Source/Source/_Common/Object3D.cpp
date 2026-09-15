#include "stdafx.h"

#include <stdio.h>
#include <dxerr9.h>
#include "Object3D.h"
#include "Material.h"
//#include "..\_DirectX\dxutil.h"
#include "xUtil3D.h"
#include "ModelObject.h"
#include "..\_UnhandledException\ExceptionHandler.h"

#ifdef __ENVIRONMENT_EFFECT
#include "Environment.h"
#endif // __ENVIRONMENT_EFFECT

#ifdef __BETTER_MODEL_CACHE
#include "BetterCache.h"
#endif //__BETTER_MODEL_CACHE

LPDIRECT3DTEXTURE9	g_pReflectMap = NULL;

LPDIRECT3DVERTEXDECLARATION9 g_pSkinVertexDeclaration;
LPDIRECT3DVERTEXDECLARATION9 g_pShadowVertexDeclaration;
LPDIRECT3DVERTEXSHADER9 g_pSkiningVS;
LPDIRECT3DVERTEXSHADER9 g_pShadowVS;
//DWORD	g_dwSkiningVS = 0xFFFFFFFF;

D3DXMATRIX	g_mReflect = D3DXMATRIX(  0.50f, 0.00f, 0.00f, 0.00f,
									  0.00f,-0.50f, 0.00f, 0.00f,
									  0.00f, 0.00f, 1.00f, 0.00f,
									  0.50f, 0.50f, 0.00f, 1.00f );

extern float s_fDiffuse[];
extern float s_fAmbient[];
extern D3DXVECTOR4 s_vLight;		// ¶óÀÌÆ® º¤ÅÍ
extern D3DXVECTOR4 s_vLightPos;		// ¶óÀÌÆ® Æ÷Áö¼Ç
extern D3DXMATRIX s_mViewProj;		// 
extern D3DXMATRIX s_mView;		// 
extern D3DXMATRIX s_mProj;		// 
extern BOOL	s_bLight;
extern BOOL	s_bNight;
extern BOOL s_bFog;
extern float s_fFogStart;
extern float s_fFogEnd;


#ifndef	__WORLDSERVER
HRESULT		CreateSkinningVS( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName )
{
	HRESULT hr;
    LPD3DXBUFFER pCode;

    // Setup the vertex declaration
    D3DVERTEXELEMENT9 decl[] =
    {
        { 0,   0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION     , 0}, 
        { 0,  12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BLENDWEIGHT  , 0}, 
        { 0,  20, D3DDECLTYPE_SHORT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BLENDINDICES , 0}, 
        { 0,  24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL       , 0}, 
        { 0,  36, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD     , 0}, 
        D3DDECL_END()
    };
	
#ifdef	_DEBUG
	// skinning shader
    if( FAILED( hr = pd3dDevice->CreateVertexDeclaration( decl, &g_pSkinVertexDeclaration ) ) )
        return hr;
    // Create vertex shader from a file
    // if( FAILED( hr = D3DXAssembleShaderFromFile( szFileName, NULL, NULL, 0, &pCode, NULL ) ) )
	if( FAILED( hr = D3DXAssembleShaderFromResource(NULL, MAKEINTRESOURCE(IDR_DATA1), NULL, NULL, 0, &pCode, NULL ) ) )
        return hr;
    if( FAILED( hr = pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(), &g_pSkiningVS ) ) )
    {
		SAFE_RELEASE( pCode );
        return hr;
    }
	SAFE_RELEASE( pCode );
#else
    if( FAILED( hr = pd3dDevice->CreateVertexDeclaration( decl, &g_pSkinVertexDeclaration ) ) )
        return hr;

	// Assemble the vertex shader file
	if( FAILED( hr = D3DXAssembleShaderFromResource(NULL, MAKEINTRESOURCE(IDR_DATA1), NULL, NULL, 0, &pCode, NULL ) ) )
		return hr;
	// Create the vertex shader
	if( FAILED( hr = pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(),
													&g_pSkiningVS ) ) ) 
	{
		SAFE_RELEASE( pCode );
		return hr;
	}
	SAFE_RELEASE( pCode );
#endif
	return S_OK;
}

HRESULT		CreateShadowVS( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName )
{
	HRESULT hr;
    LPD3DXBUFFER pCode;
	
    // Setup the vertex declaration
    D3DVERTEXELEMENT9 decl[] =
    {
        { 0,   0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION     , 0}, 
        { 0,  12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL       , 0}, 
        { 0,  24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD     , 0}, 
        D3DDECL_END()
    };
	
#ifdef	_DEBUG
	// Shadow shader
    if( FAILED( hr = pd3dDevice->CreateVertexDeclaration( decl, &g_pShadowVertexDeclaration ) ) )
        return hr;
    // Create vertex shader from a file
    if( FAILED( hr = D3DXAssembleShaderFromFile( szFileName, NULL, NULL, 0, &pCode, NULL ) ) )
        return hr;
    if( FAILED( hr = pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(), &g_pShadowVS ) ) )
    {
		SAFE_RELEASE( pCode );
        return hr;
    }
	SAFE_RELEASE( pCode );
#else
    if( FAILED( hr = pd3dDevice->CreateVertexDeclaration( decl, &g_pShadowVertexDeclaration ) ) )
        return hr;
	
	// Assemble the vertex shader file
	if( FAILED( hr = D3DXAssembleShaderFromResource(NULL, MAKEINTRESOURCE(IDR_DATA1), NULL, NULL, 0, &pCode, NULL ) ) )
		return hr;
	// Create the vertex shader
	if( FAILED( hr = pd3dDevice->CreateVertexShader( (DWORD*)pCode->GetBufferPointer(),
		&g_pShadowVS ) ) ) 
	{
		SAFE_RELEASE( pCode );
		return hr;
	}
	SAFE_RELEASE( pCode );
#endif
	return S_OK;
}


void	DeleteVertexShader( LPDIRECT3DDEVICE9 pd3dDevice )
{
    if( g_pSkiningVS )
        SAFE_RELEASE( g_pSkiningVS );
    if( g_pShadowVS )
        SAFE_RELEASE( g_pShadowVS );
	if( g_pSkinVertexDeclaration )
		SAFE_RELEASE( g_pSkinVertexDeclaration );
	if( g_pShadowVertexDeclaration )
		SAFE_RELEASE( g_pShadowVertexDeclaration );
}

LPDIRECT3DVERTEXBUFFER9		g_pd3d_ShadowVB;

HRESULT		CreateShadowMask( LPDIRECT3DDEVICE9 pd3dDevice, int nWidth, int nHeight )
{
	HRESULT	hr;

	hr = pd3dDevice->CreateVertexBuffer( 4 * sizeof(SHADOWMASK_VERTEX),
										 D3DUSAGE_WRITEONLY, D3DFVF_SHADOWMASKVERTEX,
										 D3DPOOL_MANAGED, &g_pd3d_ShadowVB, NULL );
	if( FAILED(hr) )
		Error( "CreateShadowMask : cannot cerate Vertex Buffer" );

	SHADOWMASK_VERTEX *pVertex;
	hr = g_pd3d_ShadowVB->Lock(0, 4 * sizeof(SHADOWMASK_VERTEX), (void **)&pVertex, 0);
	if( FAILED(hr) )
		return hr;
	memset( pVertex, 0x00, 4 * sizeof(SHADOWMASK_VERTEX) );
	pVertex[0].x = 0.0f;	pVertex[0].y = 0.0f;
	pVertex[1].x = (float)nWidth; pVertex[1].y = 0.0f;
	pVertex[2].x = 0.0f; pVertex[2].y = (float)nHeight;
	pVertex[3].x = (float)nWidth; pVertex[3].y = (float)nHeight;
	
	pVertex[0].color = pVertex[1].color = pVertex[2].color = pVertex[3].color = 0x44000000;
	
	g_pd3d_ShadowVB->Unlock();
	
	return S_OK;
}

void	RenderShadowMask( LPDIRECT3DDEVICE9 pd3dDevice )
{
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	pd3dDevice->SetRenderState(D3DRS_STENCILFUNC,   D3DCMP_LESSEQUAL);
    pd3dDevice->SetRenderState(D3DRS_STENCILPASS,   D3DSTENCILOP_KEEP);
    pd3dDevice->SetRenderState(D3DRS_STENCILREF,    1);
	
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    pd3dDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );
	pd3dDevice->SetFVF( D3DFVF_SHADOWMASKVERTEX );
	pd3dDevice->SetStreamSource(0, g_pd3d_ShadowVB, 0, sizeof(SHADOWMASK_VERTEX));

	pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// reset
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
    pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
    pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void	DeleteShadowMask( void )
{
	SAFE_RELEASE( g_pd3d_ShadowVB );
}

#endif // !__WORLDSERVER
////////////////////////////////////////////////////////////////////////////////////
///////////////
///////////////
///////////////
///////////////
///////////////
///////////////
////////////////////////////////////////////////////////////////////////////////////
CObject3DMng		g_Object3DMng;

CObject3DMng :: CObject3DMng()
{
	Init();
}

CObject3DMng :: ~CObject3DMng()
{
	Destroy();
}

void	CObject3DMng :: Init( void )
{
#ifdef __JEFF_11_5
#ifdef __BETTER_MODEL_CACHE
	CBetterCache::GetInstance()->ClearCacheEntities();
	CBetterCache::GetInstance()->ClearCacheObjects();
	CBetterCache::GetInstance()->ClearCacheOthers();
#else //__BETTER_MODEL_CACHE
	m_mapObject3D.clear();
#endif //__BETTER_MODEL_CACHE
#else	// __JEFF_11_5
	int		i;
	m_nSize = 0;
	m_nMax = 0;
	m_nCachePos = 0;

	for( i = 0; i < MAX_OBJECT3D; i ++ )	m_pObject3DAry[i] = NULL;
	for( i = 0; i < MAX_OBJECT3D_CACHE; i ++ )	m_pCache[i] = NULL;
//	m_tmTimer	= timeGetTime();
#endif	// __JEFF_11_5
}

// µð¹ÙÀÌ½º ÀÚ¿ø°ú ¸Þ¸ð¸® ¸ðµÎ¸¦ ³¯¸².
void	CObject3DMng :: Destroy( void )
{
#ifdef __JEFF_11_5
#ifdef __BETTER_MODEL_CACHE
	CBetterCache::GetInstance()->Clear();
#else //__BETTER_MODEL_CACHE
	for (map<string, CObject3D*>::iterator i = m_mapObject3D.begin(); i != m_mapObject3D.end(); ++i)
	{
		i->second->DeleteDeviceObjects();
		SAFE_DELETE(i->second);
	}
#endif //__BETTER_MODEL_CACHE
#else	// __JEFF_11_5
	int		i;
	CObject3D		**pAry = m_pObject3DAry;
	CObject3D		*pObject3D;

	for( i = 0; i < MAX_OBJECT3D; i ++ )
	{
		pObject3D = *pAry++;
		if( pObject3D )
		{
			pObject3D->DeleteDeviceObjects();
			SAFE_DELETE( pObject3D );
			//TRACE( "Delete Object 3D%p\n", pObject3D);
		}
	}
#endif	// __JEFF_11_5

	Init();
}

HRESULT CObject3DMng :: InvalidateDeviceObjects()
{
	DeleteDeviceObjects();
	return  S_OK;
}

// µð¹ÙÀÌ½º ÀÚ¿ø¸¸ ³¯¸².
HRESULT CObject3DMng :: DeleteDeviceObjects()
{
#ifdef __JEFF_11_5
#ifdef __BETTER_MODEL_CACHE
	CBetterCache::GetInstance()->DeleteDeviceObjects();
#else //__BETTER_MODEL_CACHE
	for (map<string, CObject3D*>::iterator i = m_mapObject3D.begin(); i != m_mapObject3D.end(); ++i)
		i->second->DeleteDeviceObjects();
#endif //__BETTER_MODEL_CACHE
#else	// __JEFF_11_5
	int		i;
	CObject3D		**pAry = m_pObject3DAry;
	CObject3D		*pObject3D;
	
	for( i = 0; i < MAX_OBJECT3D; i ++ )
	{
		pObject3D = *pAry++;
		if( pObject3D )
		{
			pObject3D->DeleteDeviceObjects();
		}
	}
#endif	// __JEFF_11_5
	return S_OK;
}

// pTexture¸¦ »ç¿ëÇÏ´Â ¸ÅÅÍ¸®¾óÀ» Ã£¾Æ »èÁ¦ÇÑ´Ù.
// °øÀ¯µÇ¾î ÀÖ´Â ÅØ½ºÃÄ¶ó¸é »ç¿ëÄ«¿îÅÍ¸¦ º¸°í 1ÀÎ°Í¸¸ »èÁ¦ÇÑ´Ù..
#ifdef __BETTER_MODEL_CACHE
int CObject3DMng::DeleteObject3D(CObject3D* pObject3D, int nType)
#else //__BETTER_MODEL_CACHE
int CObject3DMng::DeleteObject3D(CObject3D* pObject3D)
#endif //__BETTER_MODEL_CACHE
{
#ifdef __JEFF_11_5
#ifdef __BETTER_MODEL_CACHE
	CBetterCache::GetInstance()->DecreaseObjectCount(pObject3D, nType);
#else //__BETTER_MODEL_CACHE
	if (m_mapObject3D.size() == 0)
		return FALSE;
	if (pObject3D->m_nUseCnt > 1)
		pObject3D->m_nUseCnt--;
	else
	{
		m_mapObject3D.erase(pObject3D->m_szFileName);
		SAFE_DELETE(pObject3D);
		return TRUE;
	}
#endif //__BETTER_MODEL_CACHE
	return FALSE;
#else	// __JEFF_11_5
	if( pObject3D == NULL )	return FALSE;
	if( m_nMax == 0 )	return FALSE;
	
	int	i, j;

	for( i = 0; i < MAX_OBJECT3D; i ++ )
	{
		if( m_pObject3DAry[i] )
		{
			if( m_pObject3DAry[i] == pObject3D )		// pObject3D¸¦ Ã£¾Ò´Ù.
			{
				if( m_pObject3DAry[i]->m_nUseCnt == 1 )			// °øÀ¯µÈ°Ô ¾Æ´Ï´Ù(usecnt == 1)
				{
					for( j = 0; j < MAX_OBJECT3D_CACHE; j ++ )
					{
						if( m_pCache[j] == pObject3D )
						{
							m_pCache[j] = NULL;			// Ä³½¬¿¡ ÀÖ¾ú´ÂÁö ¸ÕÀú °Ë»çÇØº¸°í Ä³½¬¸¦ ¸ÕÀú Ä¡¿ò.
							break;
						}
					}
					SAFE_DELETE( m_pObject3DAry[i] );			// ¿ÀºêÁ§Æ® °ü¸®ÀÚ¿¡¼­µµ »èÁ¦ÇÏ°í ½ÇÁ¦ °´Ã¼µµ ³¯·Á¹ö¸².
					m_nMax --;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
#endif	// __JEFF_11_5
}

#ifdef __BETTER_MODEL_CACHE
CObject3D* CObject3DMng::LoadObject3D(LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName, int nType)
#else //__BETTER_MODEL_CACHE
CObject3D* CObject3DMng::LoadObject3D(LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName)
#endif //__BETTER_MODEL_CACHE
{
#ifdef __JEFF_11_5
	char sFile[MAX_PATH]	= { 0,};
	strcpy( sFile, szFileName );
	strlwr( sFile );

#ifdef __BETTER_MODEL_CACHE
	CObject3D* bResult = CBetterCache::GetInstance()->GetCacheModel(sFile, nType);
	if (bResult != NULL)
		return bResult;
#else //__BETTER_MODEL_CACHE
	map<string, CObject3D*>::iterator i = m_mapObject3D.find(sFile);
	if (i != m_mapObject3D.end())
	{
		i->second->m_nUseCnt++;
		return i->second;
	}
#endif //__BETTER_MODEL_CACHE

	CObject3D* pObject3D	= new CObject3D;
	pObject3D->InitDeviceObjects( pd3dDevice );
	if( pObject3D->LoadObject( szFileName ) == FAIL )
	{
		SAFE_DELETE( pObject3D );
		return NULL;
	}
	pObject3D->m_nUseCnt	= 1;

#ifdef __BETTER_MODEL_CACHE
	CBetterCache::GetInstance()->InsertCacheModel(sFile, pObject3D, nType);
#else //__BETTER_MODEL_CACHE
	bool bResult = m_mapObject3D.insert(map<string, CObject3D*>::value_type(sFile, pObject3D)).second;
#endif //__BETTER_MODEL_CACHE

	return pObject3D;
#else	// __JEFF_11_5
	int			i;
	CObject3D	**pAry;
	CObject3D	*pObject3D;
	int			nIdx = -1;

	// ÀÌ¹Ì ¸Þ¸ð¸®¿¡ ÀûÀç µÇ¾ú´ÂÁö °Ë»ö, µ¿½Ã¿¡ ºó°÷µµ °Ë»ö
	// ÀÏ´Ü Ä³½¬¸¦ °Ë»ö
	pAry = m_pCache;
	for( i = 0; i < MAX_OBJECT3D_CACHE; i ++ )
	{
		pObject3D = *pAry++;
		if( pObject3D )
		{
			if( strcmpi(pObject3D->m_szFileName, szFileName) == 0 && pObject3D->m_pd3dDevice == pd3dDevice )		// °°Àº°É Ã£¾ÒÀ¸¸é ±×°É ¸®ÅÏ
			{
				pObject3D->m_nUseCnt ++;	// Áßº¹µÇ¾î »ç¿ëµÇ¾îÁö¸é Ä«¿îÆ® ¿Ã¸².
				return pObject3D;
			}
		}
	}
	// ÀÌ¹Ì ¸Þ¸ð¸®¿¡ ÀûÀç µÇ¾ú´ÂÁö °Ë»ö, µ¿½Ã¿¡ ºó°÷µµ °Ë»ö
	pAry = m_pObject3DAry;
	for( i = 0; i < MAX_OBJECT3D; i ++ )
	{
		pObject3D = *pAry++;
		if( pObject3D )				// ³ÎÀÌ ¾Æ´Ñ°ÍÀº ¹«½¼ ¸ð¼ÇÀÌµç ·ÎµùµÇ¾î ÀÖ´Ù´Â ¶æ
		{
			if( strcmpi(pObject3D->m_szFileName, szFileName) == 0 && pObject3D->m_pd3dDevice == pd3dDevice )		// °°Àº°É Ã£¾ÒÀ¸¸é ±×°É ¸®ÅÏ
			{
				pObject3D->m_nUseCnt ++;	// Áßº¹µÇ¾î »ç¿ëµÇ¾îÁö¸é Ä«¿îÆ® ¿Ã¸².
				return pObject3D;
			}
		} else
		{
			if( nIdx == -1 )		nIdx = i;			// ºó°÷ÀÌ ÀÖÀ¸¸é °¡ÀåÃ³À½ ºó°÷ÀÌ ³ª¿Â°÷À» ±â¾ïÇØµÒ
		}
	}
	if( nIdx == -1 )	
	{
		Error( "%s : no ID given", szFileName );
	}

	pObject3D = new CObject3D;
//	pObject3D->m_tmCreate = timeGetTime();		// °´Ã¼°¡ »ý¼ºµÈ ´ç½Ã½Ã°£À» ±â·Ï
	pObject3D->InitDeviceObjects( pd3dDevice );

	// ·ÎµùµÈ°Ô ¾Æ´Ï¾ú´Ù¸é.  ½ÇÁ¦·Î µ¥ÀÌÅ¸ ÀÐÀ½.
	if( pObject3D->LoadObject( szFileName ) == FAIL )
	{
		SAFE_DELETE( pObject3D );
		return NULL;
	}

	pObject3D->m_nUseCnt = 1;	// Ã³À½ ·ÎµùµÇ¾úÀ¸¸é 1ºÎÅÍ
	// ÀÐÀº ¸Þ½¬Æ÷ÀÎÅÍ¸¦ ¸®½ºÆ®¿¡ µî·Ï
	m_pObject3DAry[ nIdx ] = pObject3D;
	
	// Ä³½Ã¿¡µµ ³ÖÀ½
	m_pCache[ m_nCachePos++ ] = pObject3D;
	if( m_nCachePos >= MAX_OBJECT3D_CACHE )		m_nCachePos = 0;

	m_nMax ++;			// °¡Áø ¸ð¼Ç °¹¼ö Áõ°¡

	return pObject3D;		// ÀÐÀº ¸ð¼Ç Æ÷ÀÎÅÍ ¸®ÅÏ
#endif	// __JEFF_11_5
}

// ÁÖ±âÀûÀ¸·Î °Ë»çÇØ¼­ »ç¿ëÇÑÁö ¿À·¡µÈ³ðÀº ¸Þ¸ð¸®¿¡¼­ ³¯¸².
// ¼­¹ö¿¡¼­´Â »ç¿ëÇÏÁö ¸»ÀÚ.
void CObject3DMng :: Process( void )
{
}




////////////////////////////////////////////////////////////////////////////////////
///////////////
///////////////
///////////////
///////////////
///////////////
///////////////
////////////////////////////////////////////////////////////////////////////////////


CObject3D :: CObject3D()
{
	Init();
}

CObject3D :: ~CObject3D()
{
	Destroy();
}

void	CObject3D :: Init( void )
{
	m_pGroup	  = &m_Group[0];
	memset( m_Group, 0, sizeof(LOD_GROUP) * MAX_GROUP );
//	m_pObject	  = NULL;
//	_mUpdate	  = NULL;
	m_pmExternBone = m_pmExternBoneInv = NULL;
	m_pBaseBone	   = NULL;
	m_pBaseBoneInv = NULL;
	m_pMotion = NULL;
	memset( &m_CollObject, 0, sizeof(GMOBJECT) );
	m_CollObject.m_Type = GMT_ERROR;
	m_nID = 0;
	m_nHavePhysique = FALSE;
//	m_nMaxObject = 0;
	m_nMaxBone = 0;
	m_bSendVS = 0;
	m_bLOD = 0;
	memset( m_szFileName, 0, sizeof(m_szFileName) );

	m_vForce1.x = m_vForce1.y = m_vForce1.z = 0;
	m_vForce2.x = m_vForce2.y = m_vForce2.z = 0;
#if __VER >= 9 // __CSC_VER9_5
	m_vForce3.x = m_vForce3.y = m_vForce3.z = 0;
	m_vForce4.x = m_vForce4.y = m_vForce4.z = 0;
#endif //__CSC_VER9_5
	m_vBBMin.x = m_vBBMin.y = m_vBBMin.z = 65535.0;
	m_vBBMax.x = m_vBBMax.y = m_vBBMax.z = -65535.0;
	m_nMaxFace = 0;
	m_nNoTexture = 0;
	m_nNoEffect = 0;

	m_nMaxEvent = 0;
	memset( m_vEvent, 0, sizeof(m_vEvent) );

	m_fScrlU = m_fScrlV = 0.0f;

	m_nUseCnt = 0;	// ÀÏ´Ü ÃÊ±âÈ­´Â 0
	
	m_fAmbient[0] = 1.0f;
	m_fAmbient[1] = 1.0f;
	m_fAmbient[2] = 1.0f;

	m_nTextureEx = 0;
	m_pAttr = NULL;
#ifdef __YENV
	m_pNormalDecl	= NULL;
#endif //__YENV	

#ifdef __BS_EFFECT_LUA
	m_dwEffect_EX = 0;

	m_pMteData = NULL;
#endif //__BS_EFFECT_LUA
}

void	CObject3D :: Destroy( void )
{
	int		i, j, k;

	for( j = 0; j < MAX_GROUP; j ++ )
	{
		GMOBJECT *pObject = m_Group[j].m_pObject;
		for( i = 0; i < m_Group[j].m_nMaxObject; i ++ )
		{
			SAFE_DELETE_ARRAY( pObject[i].m_pVertexList );
			SAFE_DELETE_ARRAY( pObject[i].m_pPhysiqueVertex );
			for( k = 0; k < pObject[i].m_nMaxMtrlBlk; k ++ )
			{
			#if !defined(__WORLDSERVER) 
				g_TextureMng.DeleteMaterial( pObject[i].m_pMtrlBlkTexture[k] );	// »ç¿ëÇÏ´ø ÅØ½ºÃÄ´Â »èÁ¦ÇÑ´Ù. °øÀ¯ÇÏ´Â°ÍÀÌ¸é »èÁ¦ ¾ÈÇÔ.
			#endif // !__WORLDSERVER

#if defined(__YENV) && defined(__CLIENT)
				if( g_Option.m_bSpecBump )
				{
					SAFE_RELEASE( pObject[i].m_pNormalTexture[k] );
					SAFE_RELEASE( pObject[i].m_pNoSpecTexture[k] );
				}
#endif //__YENV
			}
			SAFE_DELETE_ARRAY( pObject[i].m_pMtrlBlk );
			SAFE_DELETE_ARRAY( pObject[i].m_pMtrlBlkTexture );
			SAFE_DELETE_ARRAY( pObject[i].m_pFrame );

#ifdef __YENV
			SAFE_DELETE_ARRAY( pObject[i].m_pNormalTexture );		
			SAFE_DELETE_ARRAY( pObject[i].m_pNoSpecTexture );		
#endif //__YENV

			SAFE_DELETE_ARRAY( pObject[i].m_pVB );
			SAFE_DELETE_ARRAY( pObject[i].m_pIB );
			pObject[i].m_pIIB = NULL;		// IIB´Â IB¿Í °°Àº ºí·°À» »ç¿ëÇÏ¹Ç·Î deleteÇÏ¸é ¾ÈµÈ´Ù.
		}
	}
	
	SAFE_DELETE_ARRAY( m_CollObject.m_pVertexList );
	SAFE_DELETE_ARRAY( m_CollObject.m_pPhysiqueVertex );
	SAFE_DELETE_ARRAY( m_CollObject.m_pMtrlBlk );
	SAFE_DELETE_ARRAY( m_CollObject.m_pMtrlBlkTexture );
	SAFE_DELETE_ARRAY( m_CollObject.m_pFrame );

	
	SAFE_DELETE_ARRAY( m_pAttr );
	
	SAFE_DELETE_ARRAY( m_CollObject.m_pVB );
	SAFE_DELETE_ARRAY( m_CollObject.m_pIB );
	m_CollObject.m_pIIB = NULL;		// IIB´Â IB¿Í °°Àº ºí·°À» »ç¿ëÇÏ¹Ç·Î deleteÇÏ¸é ¾ÈµÈ´Ù.

	DeleteDeviceObjects();	// m_pObject[].m_pd3d_VB
	// m_pObject¸¦ »èÁ¦ÇÏ·Á¸é DeleteDeviceObjets()¸¦ ¸ÕÀúÇÏ°í ÇØ¾ßÇÑ´Ù.

#ifdef __YENV
	SAFE_DELETE_ARRAY( m_CollObject.m_pNormalTexture );		
	SAFE_DELETE_ARRAY( m_CollObject.m_pNoSpecTexture );		
#endif //__YENV
	
	
	SAFE_DELETE_ARRAY( m_Group[0].m_pObject );		// ÅëÂ¥ ¸Þ¸ð¸® Ç®ÀÌ¹Ç·Î [0]¸¸ Áö¿ì¸é µÈ´Ù.
	m_Group[1].m_pObject = NULL;
	m_Group[2].m_pObject = NULL;
	SAFE_DELETE_ARRAY( m_Group[0]._mUpdate );		// ÅëÂ¥ ¸Þ¸ð¸® Ç®ÀÌ¹Ç·Î [0]¸¸ Áö¿ì¸é µÈ´Ù.
	m_Group[1]._mUpdate = NULL;
	m_Group[2]._mUpdate = NULL;
	// Ãæµ¹¿ë ¸Þ½Ã´Â d3d_VB¸¦ »ý¼ºÇÏÁö ¾Ê±â¶§¹®¿¡ DeleteDevice¸¦ ¾ÈÇØµµ µÈ´Ù.
	SAFE_DELETE_ARRAY( m_pBaseBone );
	SAFE_DELETE( m_pMotion );

#ifdef __BS_EFFECT_LUA
	m_dwEffect_EX = 0;

	SAFE_DELETE( m_pMteData );
#endif;

	Init();
}

int		CObject3D :: GetMaxVertex( void )
{
	int		i, nMax = 0;
	for( i = 0; i < m_pGroup->m_nMaxObject; i ++ )
		nMax += m_pGroup->m_pObject[i].m_nMaxVertexList;
	return nMax;
}

int		CObject3D :: GetMaxFace( void )
{
	int		i, nMax = 0;
	for( i = 0; i < m_pGroup->m_nMaxObject; i ++ )
		nMax += m_pGroup->m_pObject[i].m_nMaxFaceList;
	return nMax;
}

int		CObject3D :: GetMaxMtrlBlk( void )
{
	int		i, nMax = 0;
	for( i = 0; i < m_pGroup->m_nMaxObject; i ++ )
		nMax += m_pGroup->m_pObject[i].m_nMaxMtrlBlk;
	return nMax;
}


//
//	¹öÅØ½º ¹öÆÛ¸¦ »ý¼º/Àç»ý¼º.
//
HRESULT CObject3D::RestoreDeviceObjects( LPDIRECT3DVERTEXBUFFER9 *ppd3d_VB, D3DPOOL pool )
{
	int		i, j;
	GMOBJECT	*pObj;

	for( j = 0; j < MAX_GROUP; j ++ )
	{
		GMOBJECT	*pObject = m_Group[j].m_pObject;
		int			nMaxObject = m_Group[j].m_nMaxObject;
		for( i = 0; i < nMaxObject; i ++ )
		{
			pObj = &pObject[i];
			if( pObj->m_Type == GMT_SKIN )	
			{

				#if defined(__YENV) && defined(__CLIENT)
				if( pObj->m_bBump && g_Option.m_bSpecBump )
				{
					ExtractBuffers( GMT_SKIN, &ppd3d_VB[i], pObj );
				}
				#endif //__YENV

				// ½ºÅ²Àº ¹öÅØ½º¹öÆÛ¸¦ µû·Î ½á¾ß ÇÏ¹Ç·Î ÁöÁ¤µÈ ¿ÜºÎÆ÷ÀÎÅÍ¿¡´Ù ¹öÅØ½º ¹öÆÛ¸¦ ¹ÞÀ½.
				CreateDeviceBuffer( pObj, &ppd3d_VB[i], pool );

			} else
			{
				if( pObj->m_pd3d_VB )	
					continue;

				#if defined(__YENV) && defined(__CLIENT)
				if( pObj->m_bBump && g_Option.m_bSpecBump )
					ExtractBuffers( GMT_NORMAL, NULL, pObj );
				#endif //__YENV

				CreateDeviceBuffer( pObj, NULL, pool );

			}
		}
		ppd3d_VB += m_Group[j].m_nMaxObject;
	}


	return S_OK;
}	

HRESULT CObject3D::InvalidateDeviceObjects()
{
	DeleteDeviceObjects();
	return  S_OK;
}	

#if _WIN64
#include <DxErr.h>
#pragma comment(lib, "dxerr.lib")
#endif

HRESULT CObject3D::DeleteDeviceObjects()
{
	int		i, j;
	GMOBJECT	*pObj;
	
	for( j = 0; j < MAX_GROUP; j ++ )
	{
		GMOBJECT	*pObject = m_Group[j].m_pObject;
		int			nMaxObject = m_Group[j].m_nMaxObject;
		for( i = 0; i < nMaxObject; i ++ )
		{
			pObj = &pObject[i];
			SAFE_RELEASE( pObj->m_pd3d_VB );
			SAFE_RELEASE( pObj->m_pd3d_IB );
			
#if defined(__YENV) && defined(__CLIENT)
			if( g_Option.m_bSpecBump )
			{
				for( int k=0; k < pObj->m_nMaxMtrlBlk; k++ )
				{
					if( pObj->m_pNormalTexture )
						SAFE_RELEASE( pObj->m_pNormalTexture[k] );
					
					if( pObj->m_pNoSpecTexture )
						SAFE_RELEASE( pObj->m_pNoSpecTexture[k] );
				}
			}
#endif //__YENV
			
			
			// MtrlBlkÀÇ m_pTexture´Â ¿©±â¼­ Áö¿ìÁö ¾Ê´Â´Ù.
		}
	}

#ifdef __YENV	
	SAFE_RELEASE( m_pNormalDecl );		// Á¤Á¡¼±¾ð
#endif //__YENV

	return  S_OK;
}	

int		g_MaxVB = 0;
//
//	¹öÅØ½º ¹öÆÛ & ÀÎµ¦½º ¹öÆÛ»ý¼º.
//
HRESULT	CObject3D :: CreateDeviceBuffer( GMOBJECT *pObject, LPDIRECT3DVERTEXBUFFER9 *ppd3d_VB, D3DPOOL pool )
{
#ifdef	__WORLDSERVER
	return S_OK;
#else
//	pool = D3DPOOL_MANAGED;
	
	HRESULT		hr;
	int			nVertexSize;
	DWORD		dwFVF;
	DWORD		dwUsage = D3DUSAGE_WRITEONLY;

	if( g_bUsableVS == FALSE && pObject->m_pPhysiqueVertex )	// ¹öÅØ½º½¦ÀÌ´õ Áö¿ø¾ÈµÇ°í ½ºÅ°´×À» ÇØ¾ßÇÏ´Â ¿ÀºêÁ§Æ®¸é
		dwUsage |= D3DUSAGE_SOFTWAREPROCESSING;		// ¼ÒÇÁÆ®¿þ¾î ¹öÅØ½º¹öÆÛ·Î »ý¼º.

	nVertexSize = pObject->m_nVertexSize;
	dwFVF		= pObject->m_dwFVF;

	if( ppd3d_VB == NULL )		// ppd3d_VB°¡ ³ÎÀÌ¸é ³»ºÎ... this->m_pd3d_VB¸¦ ½á¾ß ÇÑ´Ù.
	{
		// create vertexbuffer
		if( pObject->m_pd3d_VB == NULL )
		{
			// for Indexed Primitive
			g_MaxVB += (pObject->m_nMaxVB * nVertexSize);

	#ifdef __YENV_WITHOUT_BUMP
			hr = m_pd3dDevice->CreateVertexBuffer(  pObject->m_nMaxVB * nVertexSize,
													dwUsage, dwFVF,
													pool, &pObject->m_pd3d_VB, NULL );
	#else //__YENV_WITHOUT_BUMP
			hr = m_pd3dDevice->CreateVertexBuffer(  pObject->m_nMaxVB * nVertexSize,
				dwUsage, dwFVF,
				pool, &pObject->m_pd3d_VB, NULL );
	#endif //__YENV_WITHOUT_BUMP

			if( FAILED(hr) )
			{
				LPCTSTR szError = Error( "1 %s Object3D Create VertexBuffer(%s) : m_nMaxVB=%d, nVertexSize=%d, dwFVF=%d, dwUsage=%d, pool=%d %08x", 
#ifdef _WIN64
					m_szFileName, DXGetErrorString(hr), pObject->m_nMaxVB, nVertexSize, dwFVF, dwUsage, static_cast<int>(pool), reinterpret_cast<INT_PTR>(m_pd3dDevice));
#else
																		  m_szFileName, DXGetErrorString9(hr), pObject->m_nMaxVB, nVertexSize, dwFVF, dwUsage, (int)pool, (int)m_pd3dDevice );
#endif
				ADDERRORMSG( szError );
				// Object3D°ü¸®ÀÚ¸¦ ÅëÇØ ¸Þ½¬¸¦ ·ÎµùÇÑ ÈÄ ±× Æ÷ÀÎÅÍ¸¦ ¹Þ¾Æ¿Â´Ù.
				if( FAILED( hr = m_pd3dDevice->TestCooperativeLevel() ) )		// µð¹ÙÀÌ½º°¡ ÇãÁ¢ÇÏ¸é ¿¡·¯³²±è.
				{
					LPCTSTR szErr = Error( "%s CObject3D cannot TestCooperativeLevel() %08x", m_szFileName, (int)hr );
					ADDERRORMSG( szErr );
				}
#ifndef __GAMEGUARD
				int* p = NULL;
				*p = 1;
#endif // __GAMEGUARD
				return E_FAIL;
			}
			SendVertexBuffer( pObject, pObject->m_pd3d_VB );
		}
	} else
	{
		if( *ppd3d_VB == NULL )		// ¾ÆÁ÷ ÇÒ´ç µÇÁö ¾Ê¾ÒÀ½
		{
			// for Indexed Primitive
			g_MaxVB += (pObject->m_nMaxVB * nVertexSize);

	#ifdef __YENV_WITHOUT_BUMP
			hr = m_pd3dDevice->CreateVertexBuffer(  pObject->m_nMaxVB * nVertexSize,
													dwUsage, dwFVF,
													pool, ppd3d_VB, NULL );
	#else //__YENV_WITHOUT_BUMP
			hr = m_pd3dDevice->CreateVertexBuffer(  pObject->m_nMaxVB * nVertexSize,
				dwUsage, dwFVF,
				pool, ppd3d_VB, NULL );			
	#endif //__YENV_WITHOUT_BUMP

			if( FAILED(hr) )
			{
				LPCTSTR szError = Error( "2 %s Object3D Create VertexBuffer(%s) : m_nMaxVB=%d, nVertexSize=%d, dwFVF=%d, dwUsage=%d, pool=%d %08x", 
#ifdef _WIN64
					m_szFileName, DXGetErrorString(hr), pObject->m_nMaxVB, nVertexSize, dwFVF, dwUsage, static_cast<int>(pool), reinterpret_cast<INT_PTR>(m_pd3dDevice));
#else
																		  m_szFileName, DXGetErrorString9(hr), pObject->m_nMaxVB, 
																		  nVertexSize, dwFVF, dwUsage, (int)pool, (int)m_pd3dDevice );
#endif
				ADDERRORMSG( szError );
				if( FAILED( hr = m_pd3dDevice->TestCooperativeLevel() ) )		// µð¹ÙÀÌ½º°¡ ÇãÁ¢ÇÏ¸é ¿¡·¯³²±è.
				{
					LPCTSTR szErr = Error( "%s CObject3D cannot TestCooperativeLevel() %08x", m_szFileName, (int)hr );
					ADDERRORMSG( szErr );
				}
#ifndef __GAMEGUARD
				int* p = NULL;
				*p = 1;
#endif // __GAMEGUARD
				return E_FAIL;
			}
			SendVertexBuffer( pObject, *ppd3d_VB );
		}
	}
	
	// create indexbuffer
	if( pObject->m_pd3d_IB == NULL )
	{
		g_MaxVB += (pObject->m_nMaxIB * sizeof(WORD));

		hr = m_pd3dDevice->CreateIndexBuffer(	pObject->m_nMaxIB * sizeof(WORD),
												dwUsage, D3DFMT_INDEX16,
												D3DPOOL_MANAGED, &pObject->m_pd3d_IB, NULL );
		if( FAILED(hr) )
		{
			Error( "%s Object3D Create IndexBUffer(%s) : m_nMaxIB=%d", 
#if _WIN64
				m_szFileName, DXGetErrorString(hr), pObject->m_nMaxIB);
#else
													m_szFileName, DXGetErrorString9(hr), pObject->m_nMaxIB );
#endif
#ifndef __GAMEGUARD
			int* p = NULL;
			*p = 1;
#endif // __GAMEGUARD
			return E_FAIL;
		}
		SendIndexBuffer( pObject );
	}
	
	
	return S_OK;
#endif // !__WORLDSERVER
}

// szFileNameÀÌ ¾Ö´Ï¸ÞÀÌ¼Ç ÆÄÀÏÀÎ°¡?
int CObject3D::IsAnimateFile( LPCTSTR szFileName )
{
	CResFile resFp;

	BOOL bRet = resFp.Open( szFileName, "rb" );
	if( bRet == FALSE )	
	{
		Error( "IsAnimateFile : %s canot Open File", szFileName );
		return FAIL;
	}
	char c0;
	int d0;
	char buff[64];
	D3DXVECTOR3 v;
	float f0;

	resFp.Read( &c0, 1, 1 );		// ÆÄÀÏ¸í ½ºÆ®¸µ ±æÀÌ ÀÏÀ½.
	resFp.Read( buff, c0, 1 );	// ÆÄÀÏ¸í ÀÐÀ½.
	resFp.Read( &d0, 4, 1 );		// ¹öÀü
	
	resFp.Read( &d0, 4, 1 );		// Serial ID
	resFp.Read( &v, sizeof(D3DXVECTOR3), 1 );		// °Ë±¤1,2ÀÇ ÁÂÇ¥ÀÎµ¥ ÀÏ´Ü ÀÌ·¸°Ô ÇÏÀÚ.
	resFp.Read( &v, sizeof(D3DXVECTOR3), 1 );
	resFp.Read( &f0, sizeof(float), 1 );
	resFp.Read( &f0, sizeof(float), 1 );
	resFp.Seek( 16, SEEK_CUR );		// reserved
	
	resFp.Read( &v, sizeof(D3DXVECTOR3), 1 );		// ´ëÇ¥ ¹Ù¿îµù ¹Ú½º
	resFp.Read( &v, sizeof(D3DXVECTOR3), 1 );
	resFp.Read( &f0, sizeof(float), 1 );		// per slerp
	resFp.Read( &d0, 4, 1 );					// ani frame ¼ö.  ¾Ö´Ï°¡ ¾øÀ¸¸é 0ÀÌµÇµµ·Ï ÀúÀåÇÒ°Í.

	BOOL bAnimate = FALSE;
	if( d0 > 0 )			// MaxFrameÀÌ ÀÖ³Ä?
		bAnimate = TRUE;
	else
		bAnimate = FALSE;

	resFp.Close();

	return bAnimate;
}

//
//		Load Object
//
int		CObject3D :: LoadObject( LPCTSTR szFileName )
{
	int		j, nTemp;
	int		nVer;
	D3DXVECTOR3	vTemp;
	BOOL	bNormalObj = FALSE;

	CResFile resFp;

	BOOL bRet = resFp.Open( MakePath( DIR_MODEL, szFileName ), "rb" );
	if( bRet == FALSE )	
	{
		//Error( "LoadObject : %s ÀÐ±â ½ÇÆÐ", szFileName );
		return FAIL;
	}
	Init();

	// ÆÄÀÏ¸í Ä«ÇÇ
	char szName[MAX_PATH];
	strcpy( m_szFileName, szFileName );
	strlwr( m_szFileName );
//	strcpy( szName, GetFileName(szFileName) );
	GetFileName( szFileName, szName );

	char buff[MAX_PATH];
	char cLen;
	resFp.Read( &cLen, 1, 1 );		// ÆÄÀÏ¸í ½ºÆ®¸µ ±æÀÌ ÀÏÀ½.
	resFp.Read( buff, cLen, 1 );	// ÆÄÀÏ¸í ÀÐÀ½.
	for( j = 0; j < cLen; j ++ )
		buff[j] = buff[j] ^ (char)0xcd;	// ¾ÏÈ£È­ ÇØÁ¦

	if( cLen >= 64 )
	{
		Error( "LoadObject3D : %s ObjectName length is over 64", szFileName, (int)cLen );
		return 0;
	}

	buff[cLen] = 0;	// ³¡¿¡ ³Î ºÙÀÓ.
	if( strcmpi( szName, buff ) != 0 )	// Çì´õÀÇ ÆÄÀÏ¸í°ú ºñ±³ÇØº¸°í Æ²¸®¸é ¿¡·¯.
	{
		Error( "ObjectName is wrong! : %s, %s, %s", szFileName, szName, buff );
		return 0;
	}

	//--- °øÅë Çì´õºÎ
	resFp.Read( &nVer, 4, 1 );		// ¹öÀü
	if( nVer < VER_MESH )
	{
		Error( "%s ObjectName is wrong! %d", szFileName, nVer, VER_MESH );
		resFp.Close();
		return FAIL;
	}
		
	resFp.Read( &m_nID, 4, 1 );		// Serial ID
	resFp.Read( &m_vForce1, sizeof(D3DXVECTOR3), 1 );		// °Ë±¤1,2ÀÇ ÁÂÇ¥ÀÎµ¥ ÀÏ´Ü ÀÌ·¸°Ô ÇÏÀÚ.
	resFp.Read( &m_vForce2, sizeof(D3DXVECTOR3), 1 );
#if __VER >= 9 // __CSC_VER9_5
	if(nVer >= 22)
	{
		resFp.Read( &m_vForce3, sizeof(D3DXVECTOR3), 1 );		// °Ë±¤3,4ÀÇ ÁÂÇ¥ÀÎµ¥ ÀÏ´Ü ÀÌ·¸°Ô ÇÏÀÚ.
		resFp.Read( &m_vForce4, sizeof(D3DXVECTOR3), 1 );
	}
#endif //__CSC_VER9_5
	resFp.Read( &m_fScrlU, sizeof(float), 1 );
	resFp.Read( &m_fScrlV, sizeof(float), 1 );
	resFp.Seek( 16, SEEK_CUR );		// reserved

	resFp.Read( &m_vBBMin, sizeof(D3DXVECTOR3), 1 );		// ´ëÇ¥ ¹Ù¿îµù ¹Ú½º
	resFp.Read( &m_vBBMax, sizeof(D3DXVECTOR3), 1 );
	resFp.Read( &m_fPerSlerp, sizeof(float), 1 );		// per slerp
	resFp.Read( &m_nMaxFrame, 4, 1 );					// ani frame ¼ö.  ¾Ö´Ï°¡ ¾øÀ¸¸é 0ÀÌµÇµµ·Ï ÀúÀåÇÒ°Í.

	resFp.Read( &m_nMaxEvent, 4, 1 );	// ÀÌº¥Æ® ÁÂÇ¥
	if( m_nMaxEvent > 0 )
		resFp.Read( m_vEvent, sizeof(D3DXVECTOR3) * m_nMaxEvent, 1 );

	resFp.Read( &nTemp, 4, 1 );
	if( nTemp )
	{
		m_CollObject.m_Type = GMT_NORMAL;
		LoadGMObject( &resFp, &m_CollObject );		// Ãæµ¹¿ë ¸Þ½Ã
	}
	resFp.Read( &m_bLOD, 4, 1 );					// LOD°¡ ÀÖ´Â°¡?

	//--- ÀÚÃ¼³»Àå º» ¾Ö´Ï¸ÞÀÌ¼ÇÀÌ ÀÖ´Ù¸é º» °³¼ö°¡ ÀÖÀ»°ÍÀÌ´Ù.  ex) Obj_Ç³¼±.o3d    parts_female.o3d, mvr_Å¸Á¶.o3d´Â º»ÆÄÀÏÀÌ µû·Î ÀÖÀ½.
	resFp.Read( &m_nMaxBone, 4, 1 );

	if( m_nMaxBone > 0 )
	{
		m_pBaseBone = new D3DXMATRIX[ m_nMaxBone * 2 ];
		m_pBaseBoneInv = m_pBaseBone + m_nMaxBone;		// InverseTMÀº µÚÂÊ¿¡ ºÙ´Â´Ù.
		resFp.Read( m_pBaseBone,	   sizeof(D3DXMATRIX) * m_nMaxBone, 1 );		// µðÆúÆ® »À´ë ¼ÂÆ® 
		resFp.Read( m_pBaseBoneInv, sizeof(D3DXMATRIX) * m_nMaxBone, 1 );		// InverseTM ¼¼Æ®
		if( m_nMaxFrame > 0 )	// º»ÀÌ ÀÖ°í MaxFrameÀÌ ÀÖÀ¸¸é ¾Ö´Ï¸ÞÀÌ¼ÇÀÌ ÀÖ´Ù´Â°É·Î °£ÁÖ.
		{
			m_pMotion = new CMotion;
			m_pMotion->ReadTM( &resFp, m_nMaxBone, m_nMaxFrame );		// º» ¾Ö´Ï¸ÞÀÌ¼Ç ÀÐÀ½.
		}
		resFp.Read( &m_bSendVS, 4, 1 );		// º»°³¼ö°¡ MAX_VS_BONEº¸´Ù Àû¾î VS·Î ÇÑ¹ø¿¡ Àü¼Û°¡´ÉÇÑ °ÍÀÎ°¡?
	}

	int nMaxGroup = (m_bLOD) ? MAX_GROUP : 1;	// LOD°¡ ÀÖÀ¸¸é 3°³´Ù ÀÐÀ½. ¾øÀ¸¸é 1°³¸¸ ÀÐÀ½.
	LOD_GROUP *pGroup;
	int nPoolSize, nDebugSize = 0;
	resFp.Read( &nPoolSize, 4, 1 );		// ¸Þ¸ð¸® Ç® »çÀÌÁî.
	GMOBJECT *pPool = new GMOBJECT[ nPoolSize ];	// ¸Þ¸ð¸® Ç®.
	if( pPool == NULL )
		Error( "CObject3D::LoadObject( %s ) %d", m_szFileName, nPoolSize );
	memset( pPool, 0, sizeof(GMOBJECT) * nPoolSize );
	for( int i = 0; i < nMaxGroup; i ++ )
	{
		pGroup = &m_Group[i];
		resFp.Read( &pGroup->m_nMaxObject, 4, 1 );		// geometry °¹¼ö

		pGroup->m_pObject = pPool;		// ¸Þ¸ð¸® Æ÷ÀÎÅÍ ÇÒ´ç.
		pPool += pGroup->m_nMaxObject;

		nDebugSize += pGroup->m_nMaxObject;
		if( nDebugSize > nPoolSize )
			Error( "LoadObject : Error while loading Model %d, %d", nPoolSize, nDebugSize );

		memset( pGroup->m_pObject, 0, sizeof(GMOBJECT) * pGroup->m_nMaxObject );
		for( j = 0; j < pGroup->m_nMaxObject; j ++ )		pGroup->m_pObject[j].m_nID = -1;
		
		GMOBJECT	*pObject;
		int		nParentIdx;
		BOOL	bAnimate = FALSE;
		int		nType;
		//------  Geometry Object
		for( j = 0; j < pGroup->m_nMaxObject; j ++ )
		{
			pObject = &pGroup->m_pObject[j];

			resFp.Read( &nType, 4, 1 );	// Type
			pObject->m_Type = (GMTYPE)(nType & 0xffff);
			if( nType & 0x80000000 )
				pObject->m_bLight = TRUE;

			resFp.Read( &pObject->m_nMaxUseBone, 4, 1 );
			if( pObject->m_nMaxUseBone > 0 )
				resFp.Read( pObject->m_UseBone, sizeof(int) * pObject->m_nMaxUseBone, 1 );	// ¿ÀºêÁ§Æ®°¡ »ç¿ëÇÏ´Â º»¸®½ºÆ®
			
#if !defined(__YENV)
			if( pObject->m_Type == GMT_SKIN )
			{
				pObject->m_nVertexSize = sizeof(SKINVERTEX);
				pObject->m_dwFVF	   = D3DFVF_SKINVERTEX;
			} else
			{
				pObject->m_nVertexSize = sizeof(NORMALVERTEX);
				pObject->m_dwFVF	   = D3DFVF_NORMALVERTEX;
			}
#endif //__YENV

			resFp.Read( &pObject->m_nID, 4, 1 );	// Object ID
			resFp.Read( &nParentIdx, 4, 1 );			// parent idx
			pObject->m_nParentIdx = nParentIdx;		// save¸¦ À§ÇØ ¹é¾÷ ¹Þ¾ÆµÒ
			
			// ºÎ¸ð°¡ ÀÖ´Ù¸é ºÎ¸ð Æ÷ÀÎÅÍ ÁöÁ¤
			if( nParentIdx != -1 )
			{
				resFp.Read( &pObject->m_ParentType, 4, 1 );		// ºÎ¸ðÀÇ Å¸ÀÔ ÀÐÀ½.
				// ºÎ¸ðÀÇ Æ÷ÀÎÅÍ¸¦ ¼¼ÆÃ
				switch( pObject->m_ParentType )
				{
				case GMT_BONE:		pObject->m_pParent = NULL;	break;		// ºÎ¸ð°¡ º»ÀÌ¸é m_pParent¸¦ »ç¿ëÇÏÁö ¾ÊÀ½.
				case GMT_NORMAL:	pObject->m_pParent = &pGroup->m_pObject[ nParentIdx ];		break;	// ºÎ¸ð°¡ ÀÏ¹Ý¿ÀºêÁ§ÀÌ¶ó¸é ±× Æ÷ÀÎÅÍ ÁöÁ¤.
					break;
				}
			}

			// ¿øÁ¡±âÁØÀÇ LocalTM.  ºÎ¸ð°¡ ÀÖ´Ù¸é ¿øÁ¡Àº ºÎ¸ð°¡ µÈ´Ù.
			resFp.Read( &pObject->m_mLocalTM, sizeof(D3DXMATRIX), 1 );

			// load geometry
			LoadGMObject( &resFp, pObject );			// MeshºÎ ÀÐÀ½

#if defined(__YENV) && defined(__CLIENT)
			// ¹öÅÃ½º ¹öÆÛ Å¸ÀÔÀ» ÀÏ¹ÝÀ¸·Î..
			// ¹üÇÁ¿ë ¹öÆÛ´Â ExtractBuffersÇÔ¼ö È£ÃâÇÔÀ¸·Î½á »çÀÌÁî°¡ ¹Ù²ï´Ù
			if( g_Option.m_bSpecBump )
				pObject->m_VBType = VBT_NORMAL;
#endif //__YENV
			
			if( pObject->m_Type == GMT_SKIN )
			{
#if defined(__YENV) && defined(__CLIENT)
				if( pObject->m_bBump && g_Option.m_bSpecBump )
				{
	#ifdef __YENV_WITHOUT_BUMP
					pObject->m_nVertexSize = sizeof(SKINVERTEX);
					pObject->m_dwFVF	   = D3DFVF_SKINVERTEX;
	#else //__YENV_WITHOUT_BUMP
					pObject->m_nVertexSize = sizeof(SKINVERTEX_BUMP);
					pObject->m_dwFVF	   = D3DFVF_SKINVERTEX_BUMP;
	#endif //__YENV_WITHOUT_BUMP
				}
				else
#endif //__YENV
				{
					pObject->m_nVertexSize = sizeof(SKINVERTEX);
					pObject->m_dwFVF	   = D3DFVF_SKINVERTEX;
				}
			} else
			{
#if defined(__YENV) && defined(__CLIENT)
				if( pObject->m_bBump && g_Option.m_bSpecBump )
				{
	#ifdef __YENV_WITHOUT_BUMP
					pObject->m_nVertexSize = sizeof(NORMALVERTEX);
					pObject->m_dwFVF	   = D3DFVF_NORMALVERTEX;
	#else //__YENV_WITHOUT_BUMP
					pObject->m_nVertexSize = sizeof(NORMALVERTEX_BUMP);
					pObject->m_dwFVF	   = D3DFVF_NORMALVERTEX_BUMP;
	#endif //__YENV_WITHOUT_BUMP
				}
				else
#endif //__YENV
				{
					pObject->m_nVertexSize = sizeof(NORMALVERTEX);
					pObject->m_dwFVF	   = D3DFVF_NORMALVERTEX;
				}
			}
			
			if( pObject->m_pPhysiqueVertex == NULL )	bNormalObj = TRUE;	// ÀÏ¹ÝÇü¿ÀºêÁ§Æ®°¡ ÇÏ³ª¶óµµ ÀÖÀ»¶§.
			else										m_nHavePhysique = TRUE;		// ÇÇÁöÅ© ¿ÀºêÁ§Æ®°¡ ÇÏ³ª¶óµµ ÀÖÀ¸¸é TRUE°¡ µÈ´Ù.

			// load TM animation - 
			if( pObject->m_Type == GMT_NORMAL )	// ½ºÅ²¿¡´Â m_pFrameÀÌ ¾Æ¿¹ ¾ø´Ù.
			{
				if( m_nMaxFrame > 0 )	LoadTMAni( &resFp, pObject );			// TM Animation µ¥ÀÌÅ¸ ºÎºÐ.  maxframeÀÌ0ÀÌ¸é ¾Æ¿¹ ÀÐÁö ¾ÊÀ½.
			}

		}
	} // LOD_GROUP
	// boudbox vMin, vMax°ªÀ» ÀÌ¿ëÇØ 8°³ÀÇ º¤ÅÍ·Î Ç®¾î³¿
//	SetBB( m_vBBVList, m_vBBMin, m_vBBMax );

	// °»½Å¿ë ¸ÅÆ®¸¯½º ¸®½ºÆ® »ý¼º. - ÀÏ¹ÝÇü¿ÀºêÁ§Æ®°¡ ÇÏ³ª¶óµµ ÀÕÀ¸¸é »ý¼º.
	if( bNormalObj == TRUE )
	{
		D3DXMATRIX *pmPool = new D3DXMATRIX[ nPoolSize ];
		for( int i = 0; i < nMaxGroup; i ++ )
		{
			m_Group[i]._mUpdate = pmPool;
			pmPool += m_Group[i].m_nMaxObject;
			for( j = 0; j < m_Group[i].m_nMaxObject; j ++ )
				D3DXMatrixIdentity( &m_Group[i]._mUpdate[j] );
		}
	}

	// version 21ÀÌ»óºÎÅÍ´Â ÇÁ·¹ÀÓ¼Ó¼º ÀúÀåµÊ.
	if( nVer >= 21 )
	{
		int nAttr = 0;
		
		resFp.Read( &nAttr, 4, 1 );
		if( nAttr == m_nMaxFrame )		// ÇÁ·¹ÀÓ ¼Ó¼º ÀÖ´ÂÁö °Ë»ç
		{
			// ÇÁ·¹ÀÓ ¼Ó¼º ÀÐÀ½.
			if(m_nMaxFrame > 0)
				m_pAttr	= new MOTION_ATTR[ m_nMaxFrame ];
			else
				m_pAttr = NULL;

			resFp.Read( m_pAttr, sizeof(MOTION_ATTR) * m_nMaxFrame, 1 );
		}
	}
	

	resFp.Close();

	return SUCCESS;
}


//
//	Save
//
int		CObject3D::SaveObject( LPCTSTR szFileName )
{
/*
	FILE	*fp = fopen( szFileName, "wb" );
	int		nTemp;
	char	reserved[32];
	int		i;

	if( fp == NULL )
	{
		Error( "CObject3D::SaveMesh() : %¸¦ Ã£À» ¼ö ¾ø´Ù.\r\nÈ¤Àº ÆÄÀÏÀ» Check OutÇß´ÂÁö È®ÀÎÇÒ °Í!.", szFileName );
		return FAIL;
	}
	// common header
	memset( reserved, 0, 32 );
	nTemp = VER_MESH;
	fwrite( &nTemp, 4, 1, fp );		// version
	fwrite( &m_nID, 4, 1, fp );		// Serial ID
	fwrite( &m_fScrlU, sizeof(float), 1, fp );
	fwrite( &m_fScrlV, sizeof(float), 1, fp );
	fwrite( reserved, 16, 1, fp );			// reserved

	fwrite( &m_vBBMin, sizeof(D3DXVECTOR3), 1, fp );	// bounding box
	fwrite( &m_vBBMax, sizeof(D3DXVECTOR3), 1, fp );
	fwrite( &m_fPerSlerp, sizeof(float), 1, fp );	// 

	fwrite( &m_nMaxFrame,	4, 1, fp );					// ani frame ¼ö ¾Ö´Ï°¡ ¾øÀ¸¸é 0

	fwrite( &m_nMaxObject, 4, 1, fp );	// geometry °³¼ö

	GMOBJECT		*pObject;
	//------  Geometry Object
	for( i = 0; i < m_nMaxObject; i ++ )
	{
		pObject = &m_pObject[i];
		
		fwrite( &pObject->m_Type,	 4, 1, fp );	
		fwrite( &pObject->m_nID, 4, 1, fp );			// Object ID
		fwrite( &pObject->m_nParentIdx, 4, 1, fp );	// parent Idx

		fwrite( &pObject->m_mLocalTM, sizeof(D3DXMATRIX), 1, fp );	// LocalTM

		// save geometry
		SaveGMObject( fp, pObject );

		// save TM animation - ÀÏ¹ÝÇü¸Þ½¬¸¸ TMAni¸¦ ÀúÀå
		SaveTMAni( fp, pObject );
	}

	fclose(fp);
*/
	return SUCCESS;
}

//
//  GMOBJECT¸®½ºÆ®¿¡¼­ nID¸¦ Ã£¾Æ ±×³ðÀÇ ¹è¿­ÀÎµ¦½º¸¦ ¸®ÅÏ
/*int		CObject3D::Find( int nID )
{
	int		i;
	GMOBJECT	*pObject;
	for( i = 0; i < m_nMaxObject; i ++ )
	{
		pObject = &m_pObject[i];
		if( pObject->m_nID >= 0 && pObject->m_nID == nID )
			return i;
	}

	return -1;	// ¸øÃ£À¸¸é -1;
}*/


//
//		GMOBJECT ºÎ ÀÐ°¡
//
int		CObject3D::LoadGMObject( CResFile *file, GMOBJECT *pObject )
{
	DWORD	dwTemp;
	int		i;
	
	file->Read( &pObject->m_vBBMin, sizeof(D3DXVECTOR3), 1 );
	file->Read( &pObject->m_vBBMax, sizeof(D3DXVECTOR3), 1 );
	
	file->Read( &pObject->m_bOpacity,    4, 1 );
	file->Read( &pObject->m_bBump, 4, 1 );
	file->Read( &pObject->m_bRigid,		4, 1 );
	file->Seek( 28, SEEK_CUR );		// reserved
	
#ifdef __YENV_WITHOUT_BUMP
	pObject->m_bBump = TRUE;
#endif //__YENV_WITHOUT_BUMP

	// size of list
	file->Read( &pObject->m_nMaxVertexList,	4, 1 );		// ¹öÅØ½º °³¼ö
	file->Read( &pObject->m_nMaxVB,			4, 1 );		// ¹öÅØ½º ¹öÆÛ Å©±â
	file->Read( &pObject->m_nMaxFaceList,	4, 1 );		// ÆäÀÌ½º °³¼ö
	file->Read( &pObject->m_nMaxIB,		4, 1 );		// indexed ÀÎµ¦½º ¹öÆÛ°³¼ö

	m_nMaxFace += pObject->m_nMaxFaceList;

	// ¹öÅØ½º ¸®½ºÆ®¸¦ ÇÒ´çÇÏ°í ¹öÅØ½º°³¼ö¸¸Å­ ÅëÂ°·Î ÀÐÀ½
	// Vertex list
	if( pObject->m_Type == GMT_SKIN )	// ÀÌ·± LoadObject()¿¡¼­ ÀÌ¹Ì ÀÐ¾î¼­ ¿Â°ÍÀÌ´Ù.
	{
		pObject->m_pVertexList	= new D3DXVECTOR3[ pObject->m_nMaxVertexList ];
		SKINVERTEX *pVB			= new SKINVERTEX[ pObject->m_nMaxVB ];	// Indexed Vertex buffer -
		pObject->m_pVB = pVB;
//		pObject->_pVB			= new SKINVERTEX[ pObject->m_nMaxVB ];
	} else
	{
		pObject->m_pVertexList	= new D3DXVECTOR3[ pObject->m_nMaxVertexList ];
		NORMALVERTEX *pVB		= new NORMALVERTEX[ pObject->m_nMaxVB ];	// Indexed Vertex buffer

		pObject->m_pVB = pVB;
	}
	pObject->m_pIB  = new WORD[ pObject->m_nMaxIB + pObject->m_nMaxVB ];		// m_pIIBµµ WORDÇüÀ»  ¾²¹Ç·Î °°ÀÌ ÇÒ´çÇØ¼­ ¾¸.
	pObject->m_pIIB = pObject->m_pIB + pObject->m_nMaxIB;

	file->Read( pObject->m_pVertexList, sizeof(D3DXVECTOR3) * pObject->m_nMaxVertexList, 1 );
	if( pObject->m_Type == GMT_SKIN )
		file->Read( pObject->m_pVB,  sizeof(SKINVERTEX) * pObject->m_nMaxVB, 1 );		// ¹öÅØ½º ¹öÆÛ ÀÐÀ½
	else
		file->Read( pObject->m_pVB,  sizeof(NORMALVERTEX) * pObject->m_nMaxVB, 1 );		// ¹öÅØ½º ¹öÆÛ ÀÐÀ½


	file->Read( pObject->m_pIB,  sizeof(WORD) * pObject->m_nMaxIB, 1 );			// ÀÎµ¦½º ¹öÆÛ ÀÐÀ½
	file->Read( pObject->m_pIIB, sizeof(WORD) * pObject->m_nMaxVB, 1 );

	file->Read( &dwTemp, 4, 1 );			// ÇÇÁöÅ©°¡ ÀÖ´Â°¡ ¾ø´Â°¡
	if( dwTemp )
	{
		// Physique
		pObject->m_pPhysiqueVertex = new int[ pObject->m_nMaxVertexList ];
		file->Read( pObject->m_pPhysiqueVertex, sizeof(int) * pObject->m_nMaxVertexList, 1 );	// ¹öÅØ½º°³¼ö¿Í °°Àº ÇÇÁöÅ©µ¥ÀÌÅ¸ ÀÐÀ½

		// m_pVB¿¡´Â world·Î º¯È¯µÈ ¹öÅØ½ºµéÀÌ µé¾î°¡ÀÖ°Ô µÇ°í
		// _pVB¿¡´Â »À´ë ±âÁØ ·ÎÄÃ·Î µé¾îÀÖ°Ô µÈ´Ù.
		// ½ºÅ°´× ¿ÀºêÁ§Æ®ÀÇ intersectµîÀ» Ã³¸®ÇÏ·Á¸é m_pVB¸¦ ½á¾ß ÇÑ´Ù.
	}


	// Material
	D3DMATERIAL9	mMaterial;
	char			szBitmap[256];
	int				nLen;
	MATERIAL		*mMaterialAry[16];
//nt				nIdx = 0;
	int				bIsMaterial;

	file->Read( &bIsMaterial, 4, 1 );		// ASEÀÇ Main MaxMaterialÀ» ÀúÀåÇß´Ù.  ÀÌ°Ô 0ÀÌ¸é ¸ÅÅÍ¸®¾óÀÌ ¾ø´Ù´Â °Í.
	pObject->m_bMaterial = bIsMaterial;				// ³ªÁß¿¡ ÀúÀåÀ» À§ÇØ¼­ ¹é¾÷¹Þ¾ÆµÐ´Ù.
	if( bIsMaterial )
	{
		memset( mMaterialAry, 0, sizeof(mMaterialAry) );
//		for( i = 0; i < 16; i ++ )	mMaterialAry[i] = NULL;
		
		file->Read( &pObject->m_nMaxMaterial, 4, 1 );				// »ç¿ëÇÏ´Â ¸ÅÆ®¸®¾ó °³¼ö ÀÐÀ½

		if (pObject->m_nMaxMaterial == 0)
		{
			pObject->m_nMaxMaterial = 1;	// CASEMeshÀÇ SaveºÎºÐÀ» Âü°íÇÒ°Í.
		}

		for( i = 0; i < pObject->m_nMaxMaterial; i ++ )
		{
			file->Read( &mMaterial, sizeof(D3DMATERIAL9), 1 );
			file->Read( &nLen, 4, 1 );		// bitmap filename length;  null Æ÷ÇÔ
			if( nLen > sizeof(szBitmap) )		
				Error( "CObject3D::LoadGMObject : %s ÅØ½ºÃÄ ÆÄÀÏ¸íÀÌ ³Ê¹«±æ´Ù : ±æÀÌ = %d", m_szFileName, nLen );
			file->Read( szBitmap, nLen, 1 );
			strlwr( szBitmap );		// ¼Ò¹®ÀÚ·Î º¯È¯
			
			pObject->m_MaterialAry[i].m_Material = mMaterial;

			if( strlen(szBitmap)+1 > sizeof(pObject->m_MaterialAry[i].strBitMapFileName) )
				Error( "CObject3D::LoadGeoMesh() : %sÀÇ ±æÀÌ°¡ ³Ê¹« ±æ´Ù", szBitmap );

			strcpy( pObject->m_MaterialAry[i].strBitMapFileName, szBitmap );
		#if	!defined(__WORLDSERVER)
			if( IsEmpty(szBitmap) == FALSE )
				mMaterialAry[i] = g_TextureMng.AddMaterial( m_pd3dDevice, &mMaterial, szBitmap );
		#endif
		}
	}

	// vertex buffer Material block
	file->Read( &pObject->m_nMaxMtrlBlk, 4, 1 );

	if( pObject->m_nMaxMtrlBlk >= 32 )
	{
		Error( "¸ÅÅÍ¸®¾ó °¹¼ö°¡ 32°³¸¦ ³Ñ¾ú´Ù. %s", m_szFileName );
		return FAIL;
	}
	if( pObject->m_nMaxMtrlBlk > 0 )
	{
		pObject->m_pMtrlBlk = new MATERIAL_BLOCK[ pObject->m_nMaxMtrlBlk ];		// ¸ÅÅÍ¸®¾ó ºí·°À» ÇÒ´çÇÏ°í ÀÐÀ½
		pObject->m_pMtrlBlkTexture = new LPDIRECT3DTEXTURE9[ pObject->m_nMaxMtrlBlk * 8 ];	// È®ÀåÅØ½ºÃÄ(ÃÖ´ë8°³)¶û °°ÀÌ ¾´´Ù. ¾øÀ¸¸é °Á ³ÎÀÌ´Ù.
		memset( pObject->m_pMtrlBlkTexture, 0, sizeof(LPDIRECT3DTEXTURE9) * (pObject->m_nMaxMtrlBlk * 8) );

		file->Read( pObject->m_pMtrlBlk, sizeof(MATERIAL_BLOCK) * pObject->m_nMaxMtrlBlk, 1 );
#ifndef __WORLDSERVER

#ifdef __YENV
		if( g_Option.m_bSpecBump )
		{
			// ³ë¸»¸Ê	
			pObject->m_pNormalTexture = new LPDIRECT3DTEXTURE9[ pObject->m_nMaxMtrlBlk ];
			memset( pObject->m_pNormalTexture, 0, sizeof(LPDIRECT3DTEXTURE9) * (pObject->m_nMaxMtrlBlk) );

			// ³ë½ºÆÑÅ§·¯¸Ê	
			pObject->m_pNoSpecTexture = new LPDIRECT3DTEXTURE9[ pObject->m_nMaxMtrlBlk ];
			memset( pObject->m_pNoSpecTexture, 0, sizeof(LPDIRECT3DTEXTURE9) * (pObject->m_nMaxMtrlBlk) );
		}
#endif //__YENV
		
		
		for( i = 0; i < pObject->m_nMaxMtrlBlk; i ++ )
		{
			if( bIsMaterial )
			{
				if( mMaterialAry[ pObject->m_pMtrlBlk[i].m_nTextureID ] )
				{
					pObject->m_pMtrlBlkTexture[i] = mMaterialAry[ pObject->m_pMtrlBlk[i].m_nTextureID ]->m_pTexture;
						
			#ifdef __YENV
				#ifdef __YENV_WITHOUT_BUMP
					if( g_Option.m_bSpecBump )
				#else //__YENV_WITHOUT_BUMP
					if( pObject->m_pMtrlBlk[i].m_dwEffect & XE_BUMP && g_Option.m_bSpecBump )
				#endif //__YENV_WITHOUT_BUMP
					{
						char	szTexture[MAX_PATH], szFileExt[MAX_PATH];
						char*	strFileName = mMaterialAry[ pObject->m_pMtrlBlk[i].m_nTextureID ]->strBitMapFileName;

						::GetFileTitle( strFileName, szTexture );
						lstrcat( szTexture, "-n." );
						::GetFileExt( strFileName, szFileExt );
						lstrcat( szTexture, szFileExt );
							
						if( IsEmpty(szTexture) == FALSE )
						{
							 CreateNormalMap( pObject->m_Type, m_pd3dDevice, &(pObject->m_pNormalTexture[i]), szTexture );
						}
					}

				#ifdef __YENV_WITHOUT_BUMP
					if( g_Option.m_bSpecBump )
				#else //__YENV_WITHOUT_BUMP
					// ³ë½ºÆÑÅ§·¯¸Ê
					if( pObject->m_pMtrlBlk[i].m_dwEffect & XE_BUMP && g_Option.m_bSpecBump )
				#endif //__YENV_WITHOUT_BUMP
					{
						char	szTexture[MAX_PATH], szFileExt[MAX_PATH];
						char*	strFileName		= mMaterialAry[ pObject->m_pMtrlBlk[i].m_nTextureID ]->strBitMapFileName;
						
						::GetFileTitle( strFileName, szTexture );
						lstrcat( szTexture, "-s." );
						::GetFileExt( strFileName, szFileExt );
						lstrcat( szTexture, szFileExt );
	
						if( IsEmpty(szTexture) == FALSE )
						{
					#ifdef _DEBUG
							if( (_access( MakePath( DIR_MODELTEX, szTexture ), 0 )) != -1 )
								LoadTextureFromRes( m_pd3dDevice, MakePath( DIR_MODELTEX, szTexture ), &(pObject->m_pNoSpecTexture[i] ) );
					#else //_DEBUG
							char szSerchPath[MAX_PATH];
							GetCurrentDirectory( sizeof( szSerchPath ), szSerchPath );
							TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
							_splitpath( szTexture, drive, dir, name, ext );
							
							TCHAR szFileName[ _MAX_PATH ];
							RESOURCE* lpRes;
							strcpy( szFileName, MakePath( DIR_MODELTEX, szTexture ) );
							strlwr( szFileName );
							
							if( CResFile::m_mapResource.Lookup( szFileName, (void*&) lpRes ) )
							{								
								LoadTextureFromRes( m_pd3dDevice, MakePath( DIR_MODELTEX, szTexture ), &(pObject->m_pNoSpecTexture[i] ) );
							}							
					#endif //_DEBUG
						}
					}
					
			#endif //__YENV
				}
//				if( (pObject->m_pMtrlBlk[i].m_nReflect & 0xfffffffe) == 0 )	// ¸¶Áö¸· 1ºñÆ®¸¦ »« ³ª¸ÓÁö¿¡ ¾Æ¹«°ªµµ ¾øÀ¸¸é ¿¾³¯¹öÀüÀÏ °¡´É¼ºÀÌÀÖ´Ù.
//				{
//					// ÀÌ·²¶© ÄÁ¹öÆ®.
//					if( pObject->m_pMtrlBlk[i].m_n2Side )			pObject->m_pMtrlBlk[i].m_nReflect |= XE_2SIDE;
//					if( pObject->m_pMtrlBlk[i].m_nOpacity )			pObject->m_pMtrlBlk[i].m_nReflect |= XE_OPACITY;
//					// XE_REFLECT´Â m_nReflect¿¡ °ªÀÌ µé¾îÀÖ´Â »óÅÂÀÌ¹Ç·Î µû·Î ³Ö¾îÁÙÇÊ¿ä ¾ø´Ù.
//				}


			}
		}
#endif	// __WORLDSERVER
	}

	return SUCCESS;
}

//
//	TM Animation data load
//
int		CObject3D::LoadTMAni( CResFile *file, GMOBJECT *pObject )
{
	int	bFrame;
	file->Read( &bFrame, 4, 1 );
	if( bFrame == 0 )	return FAIL;	// ÇÁ·¹ÀÓ ¾øÀ¸¸é ÀÐÁö ¾ÊÀ½.

	pObject->m_pFrame = new TM_ANIMATION[ m_nMaxFrame ];
	file->Read( pObject->m_pFrame, sizeof(TM_ANIMATION) * m_nMaxFrame, 1 );

	return SUCCESS;
} 

//
// ¸Þ½¬³»¿¡¼­ szSrcÅØ½ºÃÄ¸¦ szDestÅØ½ºÃÄ·Î ¹Ù²Û´Ù.
//
//
void	CObject3D::ChangeTexture( LPCTSTR szSrc, LPCTSTR szDest )
{
#ifdef __WORLDSERVER
	return;
#else
	int		i, j, k;
	GMOBJECT	*pObject;
	char	szBuff[128];

	strcpy( szBuff, szSrc );
	strlwr( szBuff );
	TCHAR szBitMapFileName[ 16 ][ 128 ];
	for( k = 0; k < MAX_GROUP; k++ )
	{
		for( i = 0; i < m_Group[k].m_nMaxObject; i ++ )
		{
			pObject = &m_Group[k].m_pObject[i];

			// ÆÄÀÏ¸í ºñ±³¸¦ À§ÇØ¼­ º¹»çÇØ ³õ´Â´Ù. ±³Ã¼ Ã³¸®ÈÄ 
			// pObject->m_MaterialAry[ nID ].strBitMapFileNameÀÇ ³»¿ëÀÌ º¯°æµÇ±â ¶§¹®¿¡ ¿À¸®Áö³¯ ½ºÆ®¸µÀ»
			// ºñ±³¸¦ À§ÇØ ÀÓ½Ã ÀúÀåÇØµÑ ÇÊ¿ä°¡ ÀÖ´Ù.
			for( j = 0; j < pObject->m_nMaxMtrlBlk; j ++ )		// ¸ÅÅÍ¸®¾ó ºí·°À» µ¹¸é¼­
			{
				int nID = pObject->m_pMtrlBlk[j].m_nTextureID;
				_tcscpy( szBitMapFileName[ j ], pObject->m_MaterialAry[ nID ].strBitMapFileName );
			}
			for( j = 0; j < pObject->m_nMaxMtrlBlk; j ++ )		// ¸ÅÅÍ¸®¾ó ºí·°À» µ¹¸é¼­
			{
				int nID = pObject->m_pMtrlBlk[j].m_nTextureID;
				if( strcmp( szBitMapFileName[j], szBuff ) == 0 )	// szSrc¶û °°Àº ÆÄÀÏ¸íÀÌ ÀÖÀ¸¸é
				{
					MATERIAL	*pMtrl;
					D3DMATERIAL9	mMtrl;
					pMtrl = g_TextureMng.AddMaterial( m_pd3dDevice, &mMtrl, szDest );		// szDest·Î ÀÐ¾î¼­
					pObject->m_pMtrlBlkTexture[j] = pMtrl->m_pTexture;	// ±×³ðÀ¸·Î ´ëÃ¼½ÃÅ°°í
					strcpy( pObject->m_MaterialAry[ nID ].strBitMapFileName, szDest );	// ÆÄÀÏ¸í ¹Ù²ã³õ´Â´Ù.
				}
			}
		}
	} // LOD_GROUP
#endif //!__WORLDSERVER
}

//
//	GMOBJECTºÎ ÀúÀå
//
int		CObject3D::SaveGMObject( FILE *fp, GMOBJECT *pObject )
{
	return SUCCESS;
}



//
//
//
int		CObject3D::SaveTMAni( FILE *fp, GMOBJECT *pObject )
{
	int		bFrame;
	if( pObject->m_pFrame )	bFrame = 1;
	else					bFrame = 0;
	fwrite( &bFrame, 4, 1, fp );
	if( bFrame == 0 )	return FAIL;	// ÇÁ·¹ÀÓ ¾øÀ¸¸é ¾²Áö ¾ÊÀ½.

	if( m_nMaxFrame > 0 )
	{
		fwrite( pObject->m_pFrame, sizeof(TM_ANIMATION) * m_nMaxFrame, 1, fp );
	}

	return SUCCESS;
}

//static int	_nSlideCnt = 0;

//
// Line°ú ±³Â÷ÇÏ´Â »ï°¢ÇüÀ» Ã£¾Æ Slideº¤ÅÍ¸¦ °è»ê.
// ÁÖÀÇ : SlideVectorXZ´Â ½ºÅ°´× ¿ÀºêÁ§Æ®¿¡´Â »ç¿ë±ÝÁö´Ù.
// XZ¹æÇâÀ¸·Î ÀÌµ¿ÇÏ´Â º¤ÅÍ¿¡¸¸ »ç¿ëÇÏ´Â °ÍÀ¸·Î ÀÏ¹ÝÀûÀÎ »óÈ²¿£ SlideVector()¸¦ ½á¾ß ÇÑ´Ù.
// bCollObj : Ãæµ¹¸Þ½Ã·Î °Ë»çÇÏ´Â°¡? (µðÆúÆ®)
//
int		CObject3D::SlideVectorXZ( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd , const D3DXMATRIX &mWorld, BOOL bCollObj, int nSlideCnt )
{
	D3DXVECTOR3	*v1, *v2, *v3;
	D3DXVECTOR3	vInvPos, vInvEnd, vInvDir, vDir, vIntersect;
	D3DXVECTOR3 vA, vB, vN, vTemp;
	D3DXMATRIX	mTM, mInv;
	int		nMaxFace;
	GMOBJECT* pObject = &m_CollObject;		// Ãæµ¹¸Þ½Ã·Î Ãæµ¹°Ë»ç.
//	GMOBJECT* pObject = m_Group[0].m_pObject;
	NORMALVERTEX *pVB;
	WORD		*pIB;
	int		j;
	FLOAT	fDist;
	BOOL	bRet;

	int i;
	LOD_GROUP *pGroup = &m_Group[0];
	int		nMaxObj = 1;
	if( bCollObj == FALSE )		// Ãæµ¹¸Þ½Ã·Î Ãæµ¹°Ë»ç ÇÏÁö¸¶¶ó.
	{
		pObject = pGroup->m_pObject;
		nMaxObj = pGroup->m_nMaxObject;
		if( pGroup->_mUpdate == NULL )		
			return FAIL;
	}
	
	if( pObject->m_Type == GMT_ERROR )		return 0;

	for( i = 0; i < nMaxObj; i++ )
	{
		if( pObject->m_Type == GMT_SKIN )	
			Error( "SlideVectorXZ:has Skin %s", m_szFileName ); 
		nMaxFace = pObject->m_nMaxFaceList;
		pIB = pObject->m_pIB;
		pVB = (NORMALVERTEX*)(pObject->m_pVB);

		if( bCollObj == FALSE )
			mTM = pGroup->_mUpdate[i] * mWorld;
		else
			mTM = mWorld;

		pObject++;
		D3DXMatrixInverse( &mInv, NULL, &mTM );
		D3DXVec3TransformCoord( &vInvPos, &vPos, &mInv );	// Line½ÃÀÛÁ¡À» Geometry±âÁØÀ¸·Î º¯È¯
		D3DXVec3TransformCoord( &vInvEnd, &vEnd, &mInv );	// Line³¡Á¡À» ¿ª½Ã º¯È¯
		vInvDir = vInvEnd - vInvPos;
		for( j = 0; j < nMaxFace; j++ )
		{
			v1 = &pVB[ *pIB++ ].position;
			v2 = &pVB[ *pIB++ ].position;
			v3 = &pVB[ *pIB++ ].position;

//			FLOAT fU, fV;
//			bRet = D3DXIntersectTri( v1, v2, v3, &vInvPos, &vInvDir, &fU, &fV, &fDist );
			bRet = IsTouchRayTri( v1, v2, v3, &vInvPos, &vInvDir, &fDist );

			if( bRet && fDist >= 0.0f )	// ¹Ý´ë¹æÇâ ¸éÀº °Ë»çÇÏÁö ¾ÊÀ½.
			{
				// Á¤È®ÇÏ°Ô ÇÏ·Á¸é ¸ðµç ¸éÀ» ´Ù °Ë»çÇØ¼­ °¡Àå °¡±î¿î°ÍÀ» °ñ¶ó¾ß ÇÏ³ª
				// SÀÚ ÇüÅÂ·Î ±¸ºÎ·¯Áø ¸éÀÌ ¾ø´Ù´Â °¡Á¤ÇÏ¿¡ °£·«È­ ½ÃÅ²´Ù.
				if( fDist < 1.0f )
				{
					// Line°ú ´êÀº »ï°¢ÇüÀ» Ã£¾Æ³Â´Ù.
					vDir = vEnd - vPos;
					fDist *= 0.5f;
					vIntersect = vPos + fDist * vDir;	// ±³Â÷Á¡À» °è»ê.

					vA = *v2 - *v1;
					vB = *v3 - *v1;
					D3DXVec3Cross( &vN, &vA, &vB );		// Ãæµ¹ÇÑ ¸éÀÇ ³ë¸» ±¸ÇÔ. ÀÌ°Ç ³ªÁß¿¡ ¹Ì¸® °è»êÇØµÎÀÚ.
					mInv = mTM;
					mInv._41 = mInv._42 = mInv._43 = 0;
					D3DXVec3TransformCoord( &vN, &vN, &mInv );	// Ãæµ¹ÇÑ¸éÀÇ ³ë¸»À» ¿ø·¡´ë·Î(mWorld)µ¹¸².

					if( vN.x == 0 && vN.z == 0 )
						Error( "CActionMover::ProcessCollisionGround : Error" );

					vN.y = 0;	// y¼ººÐÀ» ¾ø¾Ö¼­ ¼öÁ÷¸éÀÇ ¹ý¼±ÀÎ°ÍÃ³·³ º¯È¯
					D3DXVec3Normalize( &vN, &vN );		// ÃÖÁ¾ ´ÜÀ§º¤ÅÍ·Î º¯È¯
					
					// ÀÚ, ÀÌÁ¦ vIntersect¿Í vNÀº ¿ùµåÁÂÇ¥°è·Î ÁØºñ°¡ µÇ¾ú´Ù.
					vTemp = vEnd - vIntersect;		// ±³Â÷Á¡ - ¶óÀÎ³¡ vector V¶ó°í ÄªÇÔ
					CalcSlideVec( &vTemp, vTemp, vN );
//					vTemp += vIntersect;
//					*pOut = vTemp - vPos;
					*pOut = vTemp;

					vTemp += vPos;
					
					if( ++nSlideCnt < 3 )  // ¹«ÇÑ ¸®Ä¿Àü ¹æÁö
						SlideVectorXZ( pOut, vPos, vTemp, mWorld, bCollObj, nSlideCnt );		// ÀÌ°Å¿Ö ½ÃÀÛÁ¡ÀÌ vPosÀÏ±î³ª -.-;;;
					
//					nSlideCnt = 0;
					return 1;
				}
			}
		}
	}
//	_nSlideCnt = 0;
	return 0;
}

//
// ÀÌ ÇÔ¼ö ÀÚÃ¼·Î ¸®Ä¿½ÃºêÄÝÀ» ÇÏÁö ¾Ê´Â ¹öÀü.
//
int		CObject3D::SlideVectorXZ2( D3DXVECTOR3 *pOut, D3DXVECTOR3 *pIntersect, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd , const D3DXMATRIX &mWorld, BOOL bCollObj )
{
	D3DXVECTOR3	*v1, *v2, *v3;
	D3DXVECTOR3	vInvPos, vInvEnd, vInvDir, vDir, vIntersect;
	D3DXVECTOR3 vA, vB, vN, vTemp;
	D3DXMATRIX	mTM, mInv;
	int		nMaxFace;
	GMOBJECT* pObject = &m_CollObject;		// Ãæµ¹¸Þ½Ã·Î Ãæµ¹°Ë»ç.
	NORMALVERTEX *pVB;
	WORD		*pIB;
	int		j;
	FLOAT	fDist;
	BOOL	bRet;

	int i;
	LOD_GROUP *pGroup = &m_Group[0];
	int		nMaxObj = 1;
	if( bCollObj == FALSE )		// Ãæµ¹¸Þ½Ã·Î Ãæµ¹°Ë»ç ÇÏÁö¸¶¶ó.
	{
		pObject = pGroup->m_pObject;
		nMaxObj = pGroup->m_nMaxObject;
		if( pGroup->_mUpdate == NULL )		return FAIL;
	}

	
	if( pObject->m_Type == GMT_ERROR )		return 0;

	for( i = 0; i < nMaxObj; i++ )
	{
		if( pObject->m_Type == GMT_SKIN )	
			Error( "SlideVectorXZ:has Skin %s", m_szFileName ); 

		nMaxFace = pObject->m_nMaxFaceList;
		pIB = pObject->m_pIB;
		pVB = (NORMALVERTEX*)(pObject->m_pVB);
		if( bCollObj == FALSE )
			mTM = pGroup->_mUpdate[i] * mWorld;
		else
			mTM = mWorld;

		pObject++;
		D3DXMatrixInverse( &mInv, NULL, &mTM );
		D3DXVec3TransformCoord( &vInvPos, &vPos, &mInv );	// Line½ÃÀÛÁ¡À» Geometry±âÁØÀ¸·Î º¯È¯
		D3DXVec3TransformCoord( &vInvEnd, &vEnd, &mInv );	// Line³¡Á¡À» ¿ª½Ã º¯È¯
		vInvDir = vInvEnd - vInvPos;
		for( j = 0; j < nMaxFace; j++ )
		{
			v1 = &pVB[ *pIB++ ].position;
			v2 = &pVB[ *pIB++ ].position;
			v3 = &pVB[ *pIB++ ].position;

			bRet = IsTouchRayTri( v1, v2, v3, &vInvPos, &vInvDir, &fDist );
#if __VER >= 11 //  __FIX_COLLISION
			if(bRet)
			{
				vDir = vEnd - vPos;
				float fDirLength = D3DXVec3Length(&vDir);
				// Á¤È®ÇÏ°Ô ÇÏ·Á¸é ¸ðµç ¸éÀ» ´Ù °Ë»çÇØ¼­ °¡Àå °¡±î¿î°ÍÀ» °ñ¶ó¾ß ÇÏ³ª
				// SÀÚ ÇüÅÂ·Î ±¸ºÎ·¯Áø ¸éÀÌ ¾ø´Ù´Â °¡Á¤ÇÏ¿¡ °£·«È­ ½ÃÅ²´Ù.
				if(fDirLength > 3.0f)
				{
					if( fabs(fDist) < fDirLength)
					{
						// Line°ú ´êÀº »ï°¢ÇüÀ» Ã£¾Æ³Â´Ù.
						
	//					fDist *= 0.5f;
						if(fDist < 0.0f)
							vIntersect = vPos - fDist * vDir;	// ±³Â÷Á¡À» °è»ê.
						else 
							vIntersect = vPos + fDist * vDir;	// ±³Â÷Á¡À» °è»ê.
						vA = *v2 - *v1;
						vB = *v3 - *v1;
						D3DXVec3Cross( &vN, &vA, &vB );		// Ãæµ¹ÇÑ ¸éÀÇ ³ë¸» ±¸ÇÔ. ÀÌ°Ç ³ªÁß¿¡ ¹Ì¸® °è»êÇØµÎÀÚ.
						mInv = mTM;
						mInv._41 = mInv._42 = mInv._43 = 0;
						D3DXVec3TransformCoord( &vN, &vN, &mInv );	// Ãæµ¹ÇÑ¸éÀÇ ³ë¸»À» ¿ø·¡´ë·Î(mWorld)µ¹¸².

						if( vN.x == 0 && vN.z == 0 )
							Error( "CActionMover::ProcessCollisionGround : Error" );

						vN.y = 0;	// y¼ººÐÀ» ¾ø¾Ö¼­ ¼öÁ÷¸éÀÇ ¹ý¼±ÀÎ°ÍÃ³·³ º¯È¯
						D3DXVec3Normalize( &vN, &vN );		// ÃÖÁ¾ ´ÜÀ§º¤ÅÍ·Î º¯È¯
						
						// ÀÚ, ÀÌÁ¦ vIntersect¿Í vNÀº ¿ùµåÁÂÇ¥°è·Î ÁØºñ°¡ µÇ¾ú´Ù.
						vTemp = vEnd - vIntersect;		// ±³Â÷Á¡ - ¶óÀÎ³¡ vector V¶ó°í ÄªÇÔ
						*pIntersect = vIntersect;		// ±³Â÷Á¡À» ¹Þ¾ÆµÒ
					
						vInvDir.x = -vDir.x;	vInvDir.y = -vDir.y;	vInvDir.z = -vDir.z;	// -D
						FLOAT fLenN = D3DXVec3Dot( &vInvDir, &vN );		// -D dot N
						vN *= (fLenN * 2.0f);							// (-D dot N) * 2  ÀÌ¶§ N´Â ´ÜÀ§º¤ÅÍ »óÅÂ¿©¾ßÇÑ´Ù.
						vTemp = vDir + vN;								// ÃÖÁ¾ ¹Ý»çº¤ÅÍ.
						D3DXVec3Scale(&vTemp, &vTemp, 0.2f);					
						
						*pOut = vTemp;		// ¹Ì²ô·¯Áø º¤ÅÍ¸¦ °á°ú·Î ¹Þµµ·Ï ¹Ù²ãº¸ÀÚ.
						
						return 1;
					}
				}
				else
				{
					if(fDist >= 0.0f)
					{
						if( fDist < 1.0f )
						{
							// Line°ú ´êÀº »ï°¢ÇüÀ» Ã£¾Æ³Â´Ù.
							vDir = vEnd - vPos;
							vIntersect = vPos + fDist * vDir;	// ±³Â÷Á¡À» °è»ê.

							vA = *v2 - *v1;
							vB = *v3 - *v1;
							D3DXVec3Cross( &vN, &vA, &vB );		// Ãæµ¹ÇÑ ¸éÀÇ ³ë¸» ±¸ÇÔ. ÀÌ°Ç ³ªÁß¿¡ ¹Ì¸® °è»êÇØµÎÀÚ.
							mInv = mTM;
							mInv._41 = mInv._42 = mInv._43 = 0;
							D3DXVec3TransformCoord( &vN, &vN, &mInv );	// Ãæµ¹ÇÑ¸éÀÇ ³ë¸»À» ¿ø·¡´ë·Î(mWorld)µ¹¸².

							if( vN.x == 0 && vN.z == 0 )
								Error( "CActionMover::ProcessCollisionGround : Error" );

							vN.y = 0;	// y¼ººÐÀ» ¾ø¾Ö¼­ ¼öÁ÷¸éÀÇ ¹ý¼±ÀÎ°ÍÃ³·³ º¯È¯
							D3DXVec3Normalize( &vN, &vN );		// ÃÖÁ¾ ´ÜÀ§º¤ÅÍ·Î º¯È¯
							
							// ÀÚ, ÀÌÁ¦ vIntersect¿Í vNÀº ¿ùµåÁÂÇ¥°è·Î ÁØºñ°¡ µÇ¾ú´Ù.
							vTemp = vEnd - vIntersect;		// ±³Â÷Á¡ - ¶óÀÎ³¡ vector V¶ó°í ÄªÇÔ
							*pIntersect = vIntersect;		// ±³Â÷Á¡À» ¹Þ¾ÆµÒ
							CalcSlideVec( &vTemp, vTemp, vN );
							
							*pOut = vTemp;		// ¹Ì²ô·¯Áø º¤ÅÍ¸¦ °á°ú·Î ¹Þµµ·Ï ¹Ù²ãº¸ÀÚ.
							
							return 1;
						}
					}
				}
				
			}
		}
	}
	return 0;
#else
			if( bRet && fDist >= 0.0f )	// ¹Ý´ë¹æÇâ ¸éÀº °Ë»çÇÏÁö ¾ÊÀ½.
			{
				// Á¤È®ÇÏ°Ô ÇÏ·Á¸é ¸ðµç ¸éÀ» ´Ù °Ë»çÇØ¼­ °¡Àå °¡±î¿î°ÍÀ» °ñ¶ó¾ß ÇÏ³ª
				// SÀÚ ÇüÅÂ·Î ±¸ºÎ·¯Áø ¸éÀÌ ¾ø´Ù´Â °¡Á¤ÇÏ¿¡ °£·«È­ ½ÃÅ²´Ù.
				if( fDist < 1.0f )
				{
					// Line°ú ´êÀº »ï°¢ÇüÀ» Ã£¾Æ³Â´Ù.
					vDir = vEnd - vPos;
//					fDist *= 0.5f;
					vIntersect = vPos + fDist * vDir;	// ±³Â÷Á¡À» °è»ê.
					vA = *v2 - *v1;
					vB = *v3 - *v1;
					D3DXVec3Cross( &vN, &vA, &vB );		// Ãæµ¹ÇÑ ¸éÀÇ ³ë¸» ±¸ÇÔ. ÀÌ°Ç ³ªÁß¿¡ ¹Ì¸® °è»êÇØµÎÀÚ.
					mInv = mTM;
					mInv._41 = mInv._42 = mInv._43 = 0;
					D3DXVec3TransformCoord( &vN, &vN, &mInv );	// Ãæµ¹ÇÑ¸éÀÇ ³ë¸»À» ¿ø·¡´ë·Î(mWorld)µ¹¸².

					if( vN.x == 0 && vN.z == 0 )
						Error( "CActionMover::ProcessCollisionGround : Ãæµ¹ÇÑ ¸éÀÇ ³ë¸»ÀÌ ¿ÏÀü ¼öÁ÷ÀÌ´Ù" );

					vN.y = 0;	// y¼ººÐÀ» ¾ø¾Ö¼­ ¼öÁ÷¸éÀÇ ¹ý¼±ÀÎ°ÍÃ³·³ º¯È¯
					D3DXVec3Normalize( &vN, &vN );		// ÃÖÁ¾ ´ÜÀ§º¤ÅÍ·Î º¯È¯
					
					// ÀÚ, ÀÌÁ¦ vIntersect¿Í vNÀº ¿ùµåÁÂÇ¥°è·Î ÁØºñ°¡ µÇ¾ú´Ù.
					vTemp = vEnd - vIntersect;		// ±³Â÷Á¡ - ¶óÀÎ³¡ vector V¶ó°í ÄªÇÔ
					*pIntersect = vIntersect;		// ±³Â÷Á¡À» ¹Þ¾ÆµÒ
					CalcSlideVec( &vTemp, vTemp, vN );
					
					*pOut = vTemp;		// ¹Ì²ô·¯Áø º¤ÅÍ¸¦ °á°ú·Î ¹Þµµ·Ï ¹Ù²ãº¸ÀÚ.
//					*pOut = vTemp;
					
					return 1;
				}
			}
		}
	}
	return 0;
#endif		
			
					

					
}


//
// Line°ú ±³Â÷ÇÏ´Â »ï°¢ÇüÀ» Ã£¾Æ Slideº¤ÅÍ¸¦ °è»ê.
// ÁÖÀÇ : SlideVector´Â ½ºÅ°´× ¿ÀºêÁ§Æ®¿¡´Â »ç¿ë±ÝÁö´Ù.
// Áß·Âº¤ÅÍÀÇ ¹Ù´Ú ¹Ì²ô·¯Áü°è»ê½Ã »ç¿ëÇÏ´Â Àü¿ëÇÔ¼ö.
// ¸®Ä¿ÀüÀ» ÇÏÁö ¾Ê´Â´Ù.  - ¼Óµµ¶«¿¡ -
//
#if defined( __SLIDE_060502 )

extern BOOL CanSlide( const D3DXVECTOR3& v0, const D3DXVECTOR3& v1, const D3DXVECTOR3& v2,
				 float fCosine, D3DXVECTOR3* pOut );

int	CObject3D::SlideVectorUnder( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd , const D3DXMATRIX &mWorld, D3DXVECTOR3 *pIntersect )
{
	D3DXVECTOR3	*v1, *v2, *v3;
	D3DXVECTOR3	vInvPos, vInvEnd, vInvDir;
	D3DXMATRIX	mInv;
	FLOAT	    fDist;
	BOOL	    bRet;
	NORMALVERTEX *pVB;
	WORD		 *pIB;

	GMOBJECT* pObject = &m_CollObject;
	if( pObject->m_Type != GMT_NORMAL )
		return 0;

	pIB = pObject->m_pIB;
	pVB = (NORMALVERTEX*)(pObject->m_pVB);

	D3DXMatrixInverse( &mInv, NULL, &mWorld );
	D3DXVec3TransformCoord( &vInvPos, &vPos, &mInv );	// Line½ÃÀÛÁ¡À» Geometry±âÁØÀ¸·Î º¯È¯
	D3DXVec3TransformCoord( &vInvEnd, &vEnd, &mInv );	// Line³¡Á¡À» ¿ª½Ã º¯È¯
	vInvDir = vInvEnd - vInvPos;

	for( int j = 0; j < pObject->m_nMaxFaceList; ++j )
	{
		v1 = &pVB[ *pIB++ ].position;
		v2 = &pVB[ *pIB++ ].position;
		v3 = &pVB[ *pIB++ ].position;

		bRet = IsTouchRayTri( v1, v2, v3, &vInvPos, &vInvDir, &fDist );
		if( bRet && fDist >= 0.0f )	// ¹Ý´ë¹æÇâ ¸éÀº °Ë»çÇÏÁö ¾ÊÀ½.
		{
			// Á¤È®ÇÏ°Ô ÇÏ·Á¸é ¸ðµç ¸éÀ» ´Ù °Ë»çÇØ¼­ °¡Àå °¡±î¿î°ÍÀ» °ñ¶ó¾ß ÇÏ³ª
			// SÀÚ ÇüÅÂ·Î ±¸ºÎ·¯Áø ¸éÀÌ ¾ø´Ù´Â °¡Á¤ÇÏ¿¡ °£·«È­ ½ÃÅ²´Ù.
			if( fDist < 1.0f )
			{
				// Line°ú ´êÀº »ï°¢ÇüÀ» Ã£¾Æ³Â´Ù.
				D3DXVECTOR3 vDir = vEnd - vPos;
				*pIntersect = vPos + fDist * vDir;	// ±³Â÷Á¡À» °è»ê.

				// 101µµ ÀÌ»óÀÇ °æ»ç¸é¸¸ ¹Ì²ô·¯Áø´Ù.
				D3DXVECTOR3 vSlide;
				if( CanSlide( *v1, *v2, *v3, -0.19080f, &vSlide ) == FALSE )	// -0.19080f = cos(101)
				{
					*pOut = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
					return 1;
				}
				
				// ¹Ì²ô·¯Áø ÁÂÇ¥¸¦ ¾ò´Â´Ù.
				D3DXVec3TransformNormal( &vSlide, &vSlide, &mWorld );
				*pOut = *pIntersect + vSlide;
				return 1;
			}
		}
	}
	return 0;
}
#else // __SLIDE_060502

int		CObject3D::SlideVectorUnder( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd , const D3DXMATRIX &mWorld, D3DXVECTOR3 *pIntersect )
{
	D3DXVECTOR3	*v1, *v2, *v3;
	D3DXVECTOR3	vInvPos, vInvEnd, vInvDir, vDir, vIntersect;
	D3DXVECTOR3 vA, vB, vN, vTemp;
	D3DXMATRIX	mTM, mInv;
	int		nMaxFace;
	GMOBJECT* pObject = &m_CollObject;
//	GMOBJECT* pObject = m_Group[0].m_pObject;
	NORMALVERTEX *pVB;
	WORD		*pIB;
	int		j;
	FLOAT	fDist;
	BOOL	bRet;

//	if( _mUpdate == NULL )		return FAIL;

	if( pObject->m_Type == GMT_ERROR )
		return 0;

//	for( i = 0; i < nMaxObj; i++ )
	{
		if( pObject->m_Type == GMT_SKIN )	
			Error( "SlideVectorUnder:has Skin %s", m_szFileName ); 

		nMaxFace = pObject->m_nMaxFaceList;
		pIB = pObject->m_pIB;
		pVB = (NORMALVERTEX*)(pObject->m_pVB);
		pObject++;
		mTM = mWorld;		// Åø¿¡¼± LocalTMÀ» ¾²Áö ¾Ê´Â´Ù.
//		mTM = m_Group[0].m_pObject[0].m_mLocalTM * mWorld;

		D3DXMatrixInverse( &mInv, NULL, &mTM );
		D3DXVec3TransformCoord( &vInvPos, &vPos, &mInv );	// Line½ÃÀÛÁ¡À» Geometry±âÁØÀ¸·Î º¯È¯
		D3DXVec3TransformCoord( &vInvEnd, &vEnd, &mInv );	// Line³¡Á¡À» ¿ª½Ã º¯È¯
		vInvDir = vInvEnd - vInvPos;
		for( j = 0; j < nMaxFace; j++ )
		{
			v1 = &pVB[ *pIB++ ].position;
			v2 = &pVB[ *pIB++ ].position;
			v3 = &pVB[ *pIB++ ].position;

			bRet = IsTouchRayTri( v1, v2, v3, &vInvPos, &vInvDir, &fDist );

			if( bRet && fDist >= 0.0f )	// ¹Ý´ë¹æÇâ ¸éÀº °Ë»çÇÏÁö ¾ÊÀ½.
			{
				// Á¤È®ÇÏ°Ô ÇÏ·Á¸é ¸ðµç ¸éÀ» ´Ù °Ë»çÇØ¼­ °¡Àå °¡±î¿î°ÍÀ» °ñ¶ó¾ß ÇÏ³ª
				// SÀÚ ÇüÅÂ·Î ±¸ºÎ·¯Áø ¸éÀÌ ¾ø´Ù´Â °¡Á¤ÇÏ¿¡ °£·«È­ ½ÃÅ²´Ù.
				if( fDist < 1.0f )
				{
					// Line°ú ´êÀº »ï°¢ÇüÀ» Ã£¾Æ³Â´Ù.
					vDir = vEnd - vPos;
					*pIntersect = vPos + fDist * vDir;	// ±³Â÷Á¡À» °è»ê.
	
					vA = *v2 - *v1;		// ³ë¸» °è»ê ½ÃÀÛ.
					vB = *v3 - *v1;
					D3DXVec3Cross( &vTemp, &vA, &vB );		// Ãæµ¹ÇÑ ¸éÀÇ ³ë¸» ±¸ÇÔ. ÀÌ°Ç ³ªÁß¿¡ ¹Ì¸® °è»êÇØµÎÀÚ.
					D3DXVec3Normalize( &vN, &vTemp );		// ´ÜÀ§º¤ÅÍ·Î º¯È¯

					mInv = mTM;
					mInv._41 = mInv._42 = mInv._43 = 0;
					D3DXVec3TransformCoord( &vTemp, &vN, &mInv );	// Ãæµ¹ÇÑ¸éÀÇ ³ë¸»À» ¿ø·¡´ë·Î(mWorld)µ¹¸².
					vN = vTemp;		// Ãæµ¹¸é ³ë¸».

					vDir.x = vDir.z = 0;	vDir.y = -1.0f;
					
					// ÀÏÁ¤°¢ ÀÌ»ó ±â¿ï¾îÁø ¸é¸¸ ½½¶óÀÌµå¸¦ Àû¿ë½ÃÅ²´Ù. µÎº¤ÅÍ´Ù ³ë¸»ÀÌ¾î¾ß ÇÑ´Ù.
					if( D3DXVec3Dot( &vN, &vDir ) < -0.19080f )		// -0.19080f = cos(101)
						return 1;
					vDir.y = -0.01f;

					// ÀÚ, ÀÌÁ¦ vIntersect¿Í vNÀº ¿ùµåÁÂÇ¥°è·Î ÁØºñ°¡ µÇ¾ú´Ù.
					CalcSlideVec( &vTemp, vDir, vN );	// Ãæµ¹Á¡¿¡¼­ ¹Ì²ô·¯ÁøºÎºÐ±îÁöÀÇ º¤ÅÍ
					*pOut = *pIntersect + vTemp;
					return 1;
				}
			}
		}
	}
	return 0;
}
#endif // not __SLIDE_060502

//
// Line°ú ±³Â÷ÇÏ´Â »ï°¢ÇüÀ» Ã£¾Æ ¸®ÅÏ
// pTriOut : D3DXVECTOR3 *pTri[3]; ÀÇ ½ÃÀÛ Æ÷ÀÎÅÍ.
// ÁÖÀÇ : ½ºÅ°´× ¿ÀºêÁ§Æ®¿¡´Â »ç¿ë ±ÝÁö.
//
void CObject3D::FindTouchTriLine( D3DXVECTOR3 **pTriOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd , const D3DXMATRIX &mWorld, FLOAT *pDist, BOOL bCollObj )
{
	D3DXVECTOR3	*v1, *v2, *v3;
	D3DXVECTOR3	vInvPos, vInvEnd, vInvDir, vDir;
	D3DXVECTOR3 vA, vB, vN, vTemp;
	D3DXMATRIX	mTM, mInv;
	int		nMaxFace;
	GMOBJECT* pObject = &m_CollObject;
	NORMALVERTEX *pVB;

	int i;
	LOD_GROUP *pGroup = &m_Group[0];
	int		nMaxObj = 1;
	if( bCollObj == FALSE )		// Ãæµ¹¸Þ½Ã·Î Ãæµ¹°Ë»ç ÇÏÁö¸¶¶ó.
	{
		pObject = pGroup->m_pObject;
		nMaxObj = pGroup->m_nMaxObject;
		if( pGroup->_mUpdate == NULL )		
			return;
	}
	
	if( pObject->m_Type == GMT_ERROR )
	{
		*pTriOut = NULL;
		return;
	}

	WORD	*pIB;
	int		j;
	BOOL	bRet;

	for( i = 0; i < nMaxObj; i++ )
	{
		if( pObject->m_Type == GMT_SKIN )	
			Error( "FindTouchTriLine:has Skin %s", m_szFileName );

		nMaxFace = pObject->m_nMaxFaceList;
		pVB = (NORMALVERTEX*)(pObject->m_pVB);
		pIB	= pObject->m_pIB;
		pObject++;
		if( bCollObj == FALSE )
			mTM = pGroup->_mUpdate[i] * mWorld;
		else
			mTM = mWorld;		// Ãæµ¹¿ë ¸Þ½Ã¿¡´Â LocalTMÀÌ ¾ø´Ù.

		D3DXMatrixInverse( &mInv, NULL, &mTM );
		D3DXVec3TransformCoord( &vInvPos, &vPos, &mInv );	// Line½ÃÀÛÁ¡À» Geometry±âÁØÀ¸·Î º¯È¯
		D3DXVec3TransformCoord( &vInvEnd, &vEnd, &mInv );	// Line³¡Á¡À» ¿ª½Ã º¯È¯
		vInvDir = vInvEnd - vInvPos;
		for( j = 0; j < nMaxFace; j++ )
		{
			v1 = &pVB[ *pIB++ ].position;
			v2 = &pVB[ *pIB++ ].position;
			v3 = &pVB[ *pIB++ ].position;

			bRet = IsTouchRayTri( v1, v2, v3, &vInvPos, &vInvDir, pDist );

			if( bRet && *pDist >= 0.0f )	// ¹Ý´ë¹æÇâ ¸éÀº °Ë»çÇÏÁö ¾ÊÀ½.
			{
				// Á¤È®ÇÏ°Ô ÇÏ·Á¸é ¸ðµç ¸éÀ» ´Ù °Ë»çÇØ¼­ °¡Àå °¡±î¿î°ÍÀ» °ñ¶ó¾ß ÇÏ³ª
				// SÀÚ ÇüÅÂ·Î ±¸ºÎ·¯Áø ¸éÀÌ ¾ø´Ù´Â °¡Á¤ÇÏ¿¡ °£·«È­ ½ÃÅ²´Ù.
				if( *pDist < 1.0f )
				{
					// Line°ú ´êÀº »ï°¢ÇüÀ» Ã£¾Æ³Â´Ù.
					// ÀÌ ÁÂÇ¥µé mTMÀ¸·Î °öÇØ¼­ ³Ñ°Ü¾ß ÇÏ´Â°Å ¾Æ´Ñ°¡?.
					pTriOut[0] = v1;
					pTriOut[1] = v2;
					pTriOut[2] = v3;
					return;
				}
			}
		}
	}

	*pTriOut = NULL;
}


#ifdef __CLIENT
// intersectRayTri¿¡¼­ ±³Â÷ÇÑ »ï°¢Çü µ¥ÀÌÅ¸°¡ ÀÌ°÷À¸·Î ÀÓ½Ã·Î µé¾î¿Â´Ù.
static D3DXVECTOR3	s_vLastPickTri[3];
D3DXVECTOR3 *GetLastPickTri( void )
{
	return s_vLastPickTri;
}
#endif // __CLIENT

// ¿ë·®ÀÌ Å« ÀÌÀ¯´Â ½ºÅ²¿ÀºêÁ§Æ®´Â »À´ëº¯È¯µÈ ¹öÅØ½º¸¦ ´Ù½Ã °è»ê ÇØ¾ßÇÏ¹Ç·Î ±× º¯È¯µÈ ¹öÅØ½º°¡ vPool·Î µé¾î¿Â´Ù.
#ifdef __GAMEGUARD
static D3DXVECTOR3	_vPool[MAX_SF_SPLINE];
#else // __GAMEGUARD
static D3DXVECTOR3	_vPool[4696];
#endif // __GAMEGUARD

#if __VER >= 13 // __HOUSING

void	CObject3D::ComputeInterval(float fVV0,float fVV1,float fVV2,float fD0,float fD1,float fD2,float fD0D1,float fD0D2,float &fA,float &fB,float &fC,float &fX0,float &fX1)
{
	if(fD0D1 > 0.0f) 
	{ 
		fA = fVV2; fB = (fVV0 - fVV2) * fD2; fC = (fVV1 - fVV2) * fD2; fX0 = fD2 - fD0; fX1 = fD2 - fD1; 
	}
	else if(fD0D2 > 0.0f)
	{
		fA = fVV1; fB = (fVV0 - fVV1) * fD1; fC = (fVV2 - fVV1) * fD1; fX0 = fD1 - fD0; fX1 = fD1 - fD2;
	}
	else if(fD1 * fD2 > 0.0f || fD0 != 0.0f)
	{
		fA = fVV0; fB = (fVV1 - fVV0) * fD0; fC = (fVV2 - fVV0) * fD0; fX0 = fD0 - fD1; fX1 = fD0 - fD2;
	}
	else if(fD1 != 0.0f) 
	{
		fA = fVV1; fB = (fVV0 - fVV1) * fD1; fC = ( fVV2 - fVV1) * fD1; fX0 = fD1 - fD0; fX1 = fD1 - fD2;
	}
	else if(fD2 != 0.0f)
	{
		fA = fVV2; fB = (fVV0 - fVV2) * fD2; fC = (fVV1 - fVV2) * fD2; fX0 = fD2 - fD0; fX1 = fD2 - fD1;
	}
	else
	{
		return;
	}

}

BOOL	CObject3D::SimpleTriIntersect(D3DXMATRIX mWorld, GMOBJECT* pTargetObj, D3DXMATRIX mTargetWorld)
{

	D3DXVECTOR3	v1, v2, v3, tv1, tv2, tv3;
	D3DXMATRIX	mTM;
	int			nMax, nMax2;
	int			nMaxObj = m_Group[0].m_nMaxObject;
	GMOBJECT*	pObj = &m_Group[0].m_pObject[0];
	WORD		*pIB, *pIB2;
	int			i, j, k;
	NORMALVERTEX *pVB, *pVB2;
	
	if( m_CollObject.m_Type != GMT_ERROR )	// Ãæµ¹¸Þ½Ã°¡ ¾øÀ¸¸é °Á ÇÔ.
	{
		pObj = &m_CollObject;
		nMaxObj = 1;	// Ãæµ¹¸Þ½Ã´Â ¹«Á¶°Ç 1°³´Ù.
	}

	// ½ºÅ² ¿ÀºêÁ§Æ®´Â ¾øÀ»Å×´Ï±î Á¦³¢°í ÇÏÀÚ...½Ã°£»ó
	for( i = 0; i < nMaxObj; i++ )
	{
		
		mTM = m_Group[0]._mUpdate[i] * mWorld;
		
		nMax = pObj->m_nMaxFaceList;
		pVB	= (NORMALVERTEX*)(pObj->m_pVB);
		pIB	= pObj->m_pIB;
		for( j = 0; j < nMax; j++ )
		{
			// ·ÎÄÃÁÂÇ¥°è¿¡¼­ ¿ùµåÁÂÇ¥·Î º¯È¯..
			D3DXVec3TransformCoord( &v1, &pVB[ *pIB++ ].position, &mTM );
			D3DXVec3TransformCoord( &v2, &pVB[ *pIB++ ].position, &mTM );
			D3DXVec3TransformCoord( &v3, &pVB[ *pIB++ ].position, &mTM );
			
			D3DXVECTOR3	vw1, vw2, vNormal;
			float		fD;
			// Ã¹¹øÂ° Æú¸®°ïÀÇ ³ë¸ÖÀ» ±¸ÇÑ´Ù
			D3DXVec3Subtract(&vw1, &v2, &v1);
			D3DXVec3Subtract(&vw2, &v3, &v1);
			D3DXVec3Cross(&vNormal, &vw1, &vw2);
			// Æò¸éÀÇ ¹æÁ¤½Ä 1: N1.X+d1=0 
			fD = -D3DXVec3Dot(&vNormal, &v1);	// Æò¸éÀÇ ¹æÁ¤½Ä D°ª ±¸ÇÔ
			
			// Å¸°Ù¿ÀºêÁ§Æ®µµ Æú¸®°ï »ÌÀÚ..
			nMax2 = pTargetObj->m_nMaxFaceList;
			pVB2  = (NORMALVERTEX*)(pTargetObj->m_pVB);
			pIB2  = pTargetObj->m_pIB;
			for( k = 0; k < nMax2; k++ )
			{
				// ÁÖÀÇ! ¿ÀºêÁ§Æ®°¡ ÇÏ³ªÀÎ °æ¿ì·Î °¡Á¤ÇÏ°í Çß´Ù..¾Æ´Ï¶ó¸é ´Ù½Ã ¿ÀºêÁ§Æ®¼ö¸¸Å­ ·çÇÁµ¹¸®°í ¸Å°³º¯¼ö ¹Ù²ã¼­ ´Ù½Ã Â¥¾ßµÈ´Ù
				// ·ÎÄÃÁÂÇ¥°è¿¡¼­ ¿ùµåÁÂÇ¥·Î º¯È¯..
				float fdu0, fdu1, fdu2, ftdu0, ftdu1, ftdu2, fdu01, fdu02, ftdu01, ftdu02; 
				D3DXVECTOR3	vtw1, vtw2, vtNormal, vCross;
				float		fD2;
				// Ãà°ü·Ã
				float		fXLen, fYLen, fZLen, fCood1, fCood2, fCood3, ftCood1, ftCood2, ftCood3;

				D3DXVec3TransformCoord( &tv1, &pVB2[ *pIB2++ ].position, &mTargetWorld );
				D3DXVec3TransformCoord( &tv2, &pVB2[ *pIB2++ ].position, &mTargetWorld );
				D3DXVec3TransformCoord( &tv3, &pVB2[ *pIB2++ ].position, &mTargetWorld );
				
				// ´Ù¸¥ Æú¸®°ïÀÇ °¢ Á¤Á¡¿¡ ´ëÇØ¼­ °è»êÇØ¼­ ¾î´ÀÂÊ¿¡ ÀÖ´ÂÁö ÆÇ´Ü
				fdu0 = D3DXVec3Dot(&vNormal, &tv1) + fD;
				fdu1 = D3DXVec3Dot(&vNormal, &tv2) + fD;
				fdu2 = D3DXVec3Dot(&vNormal, &tv3) + fD;

				fdu01 = fdu0*fdu1;
				fdu02 = fdu0*fdu2;
				if(fdu01  > 0.0f && fdu02 > 0.0f) 
					continue;                    // ¸ðµÎ ºÎÈ£°¡ °°À¸¸é °°ÀºÂÊ¿¡ ÀÖ´Â°ÍÀÌ¹Ç·Î ±³Â÷¾øÀ½
				
				D3DXVec3Subtract(&vtw1, &tv2, &tv1);
				D3DXVec3Subtract(&vtw2, &tv3, &tv1);
				D3DXVec3Cross(&vtNormal, &vtw1, &vtw2);
				// Æò¸éÀÇ ¹æÁ¤½Ä 1: N1.X+d1=0 
				fD2 = -D3DXVec3Dot(&vtNormal, &tv1);	// Æò¸éÀÇ ¹æÁ¤½Ä D°ª ±¸ÇÔ

				// ´Ù¸¥ Æú¸®°ïÀÇ °¢ Á¤Á¡¿¡ ´ëÇØ¼­ °è»êÇØ¼­ ¾î´ÀÂÊ¿¡ ÀÖ´ÂÁö ÆÇ´Ü
				ftdu0 = D3DXVec3Dot(&vtNormal, &v1) + fD2;
				ftdu1 = D3DXVec3Dot(&vtNormal, &v2) + fD2;
				ftdu2 = D3DXVec3Dot(&vtNormal, &v3) + fD2;

				ftdu01 = ftdu0*ftdu1;
				ftdu02 = ftdu0*ftdu2;
				if(ftdu01  > 0.0f && ftdu02 > 0.0f) 
					continue;                    // ¸ðµÎ ºÎÈ£°¡ °°À¸¸é °°ÀºÂÊ¿¡ ÀÖ´Â°ÍÀÌ¹Ç·Î ±³Â÷¾øÀ½
				
				// ±³Â÷¼±ÀÇ ¹æÇâÀ» ±¸ÇÔ 
				D3DXVec3Cross(&vCross,&vNormal,&vtNormal);

				// ±³Â÷¼±ÀÇ ¼ººÐÁß °¡Àå ±ä ÃàÀ» ±¸ÇÔ
				fXLen = fabs(vCross.x);
				fYLen = fabs(vCross.y);
				fZLen = fabs(vCross.z);

				// ±×Ãà¿¡ µÎ°³ÀÇ »ï°¢ÇüÀ» Åõ¿µÇÔ
				if(fYLen > fXLen && fYLen > fZLen) // y
				{
					fCood1 = v1.y;
					fCood2 = v2.y;
					fCood3 = v3.y;
					ftCood1 = tv1.y;
					ftCood2 = tv2.y;
					ftCood3 = tv3.y;
				}
				if(fZLen > fXLen && fZLen > fYLen) // z
				{
					fCood1 = v1.z;
					fCood2 = v2.z;
					fCood3 = v3.z;
					ftCood1 = tv1.z;
					ftCood2 = tv2.z;
					ftCood3 = tv3.z;
				}
				else								// x
				{
					fCood1 = v1.x;
					fCood2 = v2.x;
					fCood3 = v3.x;
					ftCood1 = tv1.x;
					ftCood2 = tv2.x;
					ftCood3 = tv3.x;
				}

				float a,b,c,x0,x1, d,e,f,y0,y1;
				float xx,yy,xxyy,tmp;
				float isect1[2], isect2[2];

				// Æú¸®°ï1ÀÇ °£°Ý °è»ê
				ComputeInterval(fCood1,fCood2,fCood3,fdu0,fdu1,fdu2,fdu01,fdu02,a,b,c,x0,x1);

				// Æú¸®°ï2ÀÇ °£°Ý °è»ê
				ComputeInterval(ftCood1,ftCood2,ftCood3,ftdu0,ftdu1,ftdu2,ftdu01,ftdu02,d,e,f,y0,y1);

				xx = x0 * x1;
				yy = y0 * y1;
				xxyy = xx * yy;

				tmp = a * xxyy;
				isect1[0] = tmp + b * x1 * yy;
				isect1[1] = tmp + c * x0 * yy;

				tmp = d * xxyy;
				isect2[0] = tmp + e * xx * y1;
				isect2[1] = tmp + f * xx * y0;

				if(isect1[0] > isect1[1])
				 tmp = isect1[0];
				 isect1[0] = isect1[1];
				 isect1[1] = tmp;
               
				if(isect2[0] > isect2[1])
				 tmp = isect2[0];
				 isect2[0] = isect2[1];
				 isect2[1] = tmp;

				if(isect1[1] < isect2[0] || isect2[1] < isect1[0]) continue;
			
				return TRUE;
			}
		}	
		pObj++;
	}
	
	return FALSE;
}
#endif // __HOUSING
//
// ·¹ÀÌ¿Í ±³Â÷ÇÑ »ï°¢ÇüÀÇ ½ÃÀÛÆ÷ÀÎÅÍ¸¦ ¸®ÅÏ.
//
D3DXVECTOR3 *CObject3D::IntersectRayTri( const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir, const D3DXMATRIX &mWorld, 
											  D3DXVECTOR3* pvIntersect, FLOAT* pfDist, BOOL bColl )
{
	D3DXVECTOR3	*v1, *v2, *v3, *v4;
	D3DXVECTOR3	vInvRayOrig, vInvRayDir, vw1, vw2;
	D3DXMATRIX	mTM, mInv, *pBone, *pBoneInv;
	int		nMax;
	int		nMaxObj = m_Group[0].m_nMaxObject;
	GMOBJECT* pObj = &m_Group[0].m_pObject[0];
	WORD		*pIB, *pIIB;
	int		i, j, nVIdx;
	FLOAT	fDist, fMinDist = 65535.0f;
	BOOL	bRet, bFlag = FALSE;

	if( bColl )		// Ãæµ¹¸Þ½Ã·Î °Ë»çÇÏ°Ô ÇÏ´Â ¿É¼Ç
	{
		if( m_CollObject.m_Type != GMT_ERROR )	// Ãæµ¹¸Þ½Ã°¡ ¾øÀ¸¸é °Á ÇÔ.
		{
			pObj = &m_CollObject;
			nMaxObj = 1;	// Ãæµ¹¸Þ½Ã´Â ¹«Á¶°Ç 1°³´Ù.
		}
	}

	for( i = 0; i < nMaxObj; i++ )
	{
		if( pObj->m_Type == GMT_SKIN )	
		{
			SKINVERTEX *pVB;
			mTM = /*_mUpdate[i] * */mWorld;
			D3DXMatrixInverse( &mInv, NULL, &mTM );
			D3DXVec3TransformCoord( &vInvRayOrig, &vRayOrig, &mInv );
			
			mInv._41 = 0;	mInv._42 = 0;	mInv._43 = 0;	// vRayDirÀº ¹æÇâº¤ÅÍ¸¸ ÀÖ±â¶¡½Ã À§Ä¡´Â ¾ø¾ÖÁØ´Ù.
			D3DXVec3TransformCoord( &vInvRayDir, &vRayDir, &mInv );
			int *pPhysique = pObj->m_pPhysiqueVertex;
			D3DXVECTOR3	*pVList = pObj->m_pVertexList;
			pIB	= pObj->m_pIB;
			pIIB = pObj->m_pIIB;
			pVB = (SKINVERTEX *)pObj->m_pVB;
			if( m_pmExternBone )	
			{
				pBone = m_pmExternBone;		// ¿ÜºÎÁöÁ¤ »À´ë°¡ ÀÖ´Ù¸é ±×°É ¾²°í
				pBoneInv = m_pmExternBoneInv;
			}
			else					
			{
				pBone = m_pBaseBone;		// ¾ø´Ù¸é µðÆúÆ®¸¦ ¾´´Ù.
				pBoneInv = m_pBaseBoneInv;
			}
			nMax = pObj->m_nMaxVB;

			if (nMax > sizeof(_vPool) / sizeof(D3DXVECTOR3)) 
			{
				Error("IntersectRayTri : Vertex to High! [Model:%d] [Max:%d] [pObj->m_nID:%d]", nMax, (sizeof(_vPool) / sizeof(D3DXVECTOR3)), pObj->m_nID);
#ifdef __AEGON_FIXES
				nMax = sizeof(_vPool) / sizeof(D3DXVECTOR3);
#endif
			}

			int	matIdx;
			v4 = _vPool;
			for( j = 0; j < nMax; j ++ )
			{
				nVIdx = *pIIB++;
//				D3DXVec3TransformCoord( v4, &pVList[nVIdx], &pBone[ pPhysique[nVIdx] ] );
				matIdx = pPhysique[ nVIdx ];	// 2link´Â ¹«½ÃÇÏ°í ±×Áß ÇÏ³ª¸¸ ¾´´Ù. ±×·¡¼­ °è»êÀÌ Á¤È®ÇÏÁö ¾ÊÀ» ¼ö ÀÖ´Ù,.
				mTM = pBoneInv[ matIdx ] * pBone[ matIdx ];
				D3DXVec3TransformCoord( v4, &pVB[j].position, &mTM );
				v4 ++;
			}
			nMax = pObj->m_nMaxFaceList;
			v4 = _vPool;	// pVB
			for( j = 0; j < nMax; j++ )
			{
				v1 = &v4[ *pIB++ ];
				v2 = &v4[ *pIB++ ];
				v3 = &v4[ *pIB++ ];

				bRet = IsTouchRayTri( v1, v2, v3, &vInvRayOrig, &vInvRayDir, &fDist );
				if( bRet && fDist >= 0.0f )	// ¹Ý´ë¹æÇâ ¸é(-)Àº °Ë»çÇÏÁö ¾ÊÀ½.
				{
					if( fDist < fMinDist )	// ·¹ÀÌ¿øÁ¡À¸·ÎºÎÅÍ °¡Àå °¡±î¿î °Å¸®¸¦ Ã£À½.
					{
						fMinDist = fDist;
						bFlag = TRUE;
					#ifdef __CLIENT
						s_vLastPickTri[0] = *v1;	s_vLastPickTri[1] = *v2;	s_vLastPickTri[2] = *v3;
					#endif
					}
				}
			}
		} else
		{  // non-skin object
			NORMALVERTEX *pVB;
			mTM = m_Group[0]._mUpdate[i] * mWorld;
			D3DXMatrixInverse( &mInv, NULL, &mTM );
			D3DXVec3TransformCoord( &vInvRayOrig, &vRayOrig, &mInv );
			
			mInv._41 = 0;	mInv._42 = 0;	mInv._43 = 0;	// vRayDirÀº ¹æÇâº¤ÅÍ¸¸ ÀÖ±â¶¡½Ã À§Ä¡´Â ¾ø¾ÖÁØ´Ù.
			D3DXVec3TransformCoord( &vInvRayDir, &vRayDir, &mInv );
			nMax = pObj->m_nMaxFaceList;
			pVB	= (NORMALVERTEX*)(pObj->m_pVB);
			pIB	= pObj->m_pIB;
			for( j = 0; j < nMax; j++ )
			{
				v1 = &pVB[ *pIB++ ].position;
				v2 = &pVB[ *pIB++ ].position;
				v3 = &pVB[ *pIB++ ].position;

				bRet = IsTouchRayTri( v1, v2, v3, &vInvRayOrig, &vInvRayDir, &fDist );

				if( bRet && fDist >= 0.0f )	// ¹Ý´ë¹æÇâ ¸é(-)Àº °Ë»çÇÏÁö ¾ÊÀ½.
				{
					if( fDist < fMinDist )	// ·¹ÀÌ¿øÁ¡À¸·ÎºÎÅÍ °¡Àå °¡±î¿î °Å¸®¸¦ Ã£À½.
					{
						fMinDist = fDist;
						bFlag = TRUE;
					#ifdef __CLIENT
						D3DXVec3TransformCoord( &s_vLastPickTri[0], v1, &m_Group[0]._mUpdate[i] );
						D3DXVec3TransformCoord( &s_vLastPickTri[1], v2, &m_Group[0]._mUpdate[i] );
						D3DXVec3TransformCoord( &s_vLastPickTri[2], v3, &m_Group[0]._mUpdate[i] );
					#endif	// __CLIENT
					}
				}
			}
		}
		pObj++;

	}
	if( bFlag )	// ÇÑ¹øÀÌ¶óµµ ±³Â÷µÈ »ï°¢ÇüÀ» Ã£¾Ò´Ù¸é.
	{
		*pvIntersect = vRayOrig + fMinDist * vRayDir;		// ±³Â÷Çß´ø °¡Àå °¡±î¿î°Å¸®·Î Ãæµ¹ÁöÁ¡ °è»ê.
		return _vPool;		// ±× »ï°¢Çü ÁÂÇ¥ ¸®ÅÏ
	}
	
	return NULL;
}

void	CObject3D::SetTexture( LPCTSTR szTexture )
{
#if !defined(__WORLDSERVER)
	MATERIAL	*pMtrl;
	D3DMATERIAL9	mMtrl;

	pMtrl = g_TextureMng.AddMaterial( m_pd3dDevice, &mMtrl, szTexture );
	m_Group[0].m_pObject[0].m_pMtrlBlkTexture[0] = pMtrl->m_pTexture;
#endif
}

void	CObject3D::SetTexture( LPDIRECT3DTEXTURE9 pTexture )
{
#if !defined(__WORLDSERVER)
	m_Group[0].m_pObject[0].m_pMtrlBlkTexture[0] = pTexture;
#endif
}

void	CObject3D::LoadTextureEx( int nNumEx, GMOBJECT *pObj, MATERIAL *pmMaterial[16] )
{
#if !defined(__WORLDSERVER)
	int		i;
	char	szTexture[MAX_PATH];
	char	szFileExt[MAX_PATH];
	char	szNum[16];
	D3DMATERIAL9	mMaterial;

	memset( &mMaterial, 0, sizeof(mMaterial) );

	// »ç¿ëÇÏ´Â ÅØ½ºÃÄ¿¡ -et¸¦ ºÙ¿©¼­ È®Àå ÅØ½ºÃÄ¸¦ ÀÐ¾îµéÀÓ.
	for( i = 0; i < pObj->m_nMaxMaterial; i ++ )
	{
		if( IsEmpty( pObj->m_MaterialAry[i].strBitMapFileName ) )	continue;
		strcpy( szTexture, pObj->m_MaterialAry[i].strBitMapFileName );		// mvr_bang.dds
//		strcpy( szTexture, ::GetFileTitle( pObj->m_MaterialAry[i].strBitMapFileName ) );	// mvr_bang
		GetFileTitle( pObj->m_MaterialAry[i].strBitMapFileName, szTexture );	// mvr_bang
#ifdef __AEGON_FIXES
		CString strFile = szTexture;
#endif
		strcat( szTexture, "-et" );		// -et. ¸¦ ºÙÀÓ.  mvr_bang-et.
		sprintf( szNum, "%02d.", nNumEx );	
#ifdef __AEGON_FIXES
		strFile += ".";
#endif
		strcat( szTexture, szNum );			// -et01 ~ 07
//		strcat( szTexture, ::GetFileExt( pObj->m_MaterialAry[i].strBitMapFileName ) );		// mvr_bang-et.dds
		GetFileExt( pObj->m_MaterialAry[i].strBitMapFileName, szFileExt );	// mvr_bang-et.dds

#ifdef __AEGON_FIXES
		strFile += szFileExt;
#endif

		lstrcat( szTexture, szFileExt );

		if (IsEmpty(szTexture) == FALSE) 
		{
			pmMaterial[i] = g_TextureMng.AddMaterial(m_pd3dDevice, &mMaterial, szTexture);

#ifdef __AEGON_FIXES
			if (pmMaterial[i] == NULL)
			{
				pmMaterial[i] = g_TextureMng.AddMaterial(m_pd3dDevice, &mMaterial, strFile.GetString());

				LPCTSTR szErr = NULL;
				if (pmMaterial[i]) 
				{
					szErr = Error("CObject3D::LoadTextureEx reload successful for material: %s", strFile.GetString());
				}
				else 
				{
					szErr = Error("CObject3D::LoadTextureEx reload failed for material: %s", strFile.GetString());
				}
				ADDERRORMSG(szErr);
			}
#endif
		}
	}
#endif // !__WORLDSERVER
}

// ÅØ½ºÃÄ¸¦ È®ÀåÅØ½ºÃÄ·Î ÁöÁ¤ÇÑ´Ù.
// ±âº»Àº È®ÀÛÅØ½ºÃÄ¸¦ ·ÎµùÇÏÁö ¾ÊÁö¸¸ ¿äÃ»ÀÌ µé¾î¿À¸é
// ÅØ½ºÃÄ¸¦ ·ÎµùÇÏ°í ±×Æ÷ÀÎÆ®·Î ±âº»Æ÷ÀÎÅÍ·Î ¹Ù²Û´Ù.
void	CObject3D::SetTextureEx( GMOBJECT *pObj, int nNumEx )
{
#if !defined(__WORLDSERVER)
	LPDIRECT3DTEXTURE9	*pTextureEx;		// ¸ÅÅÍ¸®¾ó ºí·°³» ÅØ½ºÃÄÆ÷ÀÎÅÍ
	int		i;
	MATERIAL	*mMaterial[16];
	int		nID;
	
#ifdef __MAX_ATEX
	if (nNumEx >= __MAX_ATEX)
#else __MAX_ATEX
	if( nNumEx >= 8 )
#endif // __MAX_ATEX
	{
		Error( "CObject3D::SetTextureEx : nNumEx = %d", nNumEx );
		return;
	}
	pTextureEx = pObj->m_pMtrlBlkTexture + (((UINT_PTR)pObj->m_nMaxMtrlBlk) * nNumEx);		// È®ÀåºÎºÐ Æ÷ÀÎÅÍ.
	
//	if( bUse )
	if( 1 )
	{
		memset( mMaterial, 0, sizeof(mMaterial) );
		
		if( pTextureEx[0] == NULL )		// È®Àå ÅØ½ºÃÄ°¡ ·ÎµùµÈÀûÀÌ ¾ø´Ù.
		{
			LoadTextureEx( nNumEx, pObj, mMaterial );		// È®ÀåÅØ½ºÃÄ¸¦ ·ÎµùÇÔ.
			for( i = 0; i < pObj->m_nMaxMtrlBlk; i ++ )
			{
				nID = pObj->m_pMtrlBlk[i].m_nTextureID;
				if(nID >= 0 && nID < 16 && mMaterial[ nID ] )
					pTextureEx[i] = mMaterial[ nID ]->m_pTexture;		// È®ÀåÅØ½ºÃÄ¸¦ ·ÎµùÇÔ.
			}
		}
	}
	else
	{
		for( i = 0; i < pObj->m_nMaxMtrlBlk; i ++ )
			pTextureEx[i] = NULL;
	}
	
	m_nTextureEx = nNumEx;
#endif
}

void	CObject3D :: ClearNormal( void )
{
/*	int		i;
	D3DXVECTOR3		n = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );

	for( int j = 0; j < m_nMaxObject; j++ )
	{
		GMOBJECT* pObject = &m_pObject[j];
		if( pObject->m_Type == GMT_SKIN )
		{
			SKINVERTEX *pVB = (SKINVERTEX*)(pObject->m_pVB);
			for( i = 0; i < pObject->m_nMaxVB; i ++ )		pVB[i].normal = n;
		}
		else
		{
			NORMALVERTEX *pVB = (NORMALVERTEX*)(pObject->m_pVB);
			for( i = 0; i < pObject->m_nMaxVB; i ++ )		pVB[i].normal = n;
		}

		SendVertexBuffer( pObject, NULL );
	}
	*/
}

void	CObject3D::Animate( FLOAT fFrameCurrent, int nNextFrame )
{
#if !defined(__WORLDSERVER)
	int		i;
	TM_ANIMATION	*pFrame = NULL;					// ÇöÀç ÇÁ·¹ÀÓ
	TM_ANIMATION	*pNext = NULL;					// ´ÙÀ½ ÇÁ·¹ÀÓ
	D3DXQUATERNION	qSlerp;
	D3DXVECTOR3		vSlerp;
	int		nCurrFrame;
	float	fSlp;
	D3DXMATRIX	m1, m2;
	D3DXMATRIX *mUpdate = m_pGroup->_mUpdate;
	D3DXMATRIX *mParent;

	GMOBJECT	*pObj;
	int nMaxObj = m_pGroup->m_nMaxObject;
	for( i = 0; i < nMaxObj; i ++ )
	{
		pObj = &m_pGroup->m_pObject[i];
		if( pObj->m_ParentType == GMT_BONE )
		{
			if( m_pmExternBone )	mParent = m_pmExternBone;
			else					mParent = m_pBaseBone;
		} else
			mParent = m_pGroup->_mUpdate;
		if( m_nMaxFrame > 0 )	// ÇÁ·¹ÀÓÀÌ ÀÖÀ»¶§
		{
			if( pObj->m_pFrame )
			{
				// º¸°£À» À§ÇÑ Slerp °è»ê.
				nCurrFrame = (int)fFrameCurrent;			// ¼Ò¼ýÁ¡ ¶¼³»°í Á¤¼öºÎ¸¸..
				fSlp = fFrameCurrent - (float)nCurrFrame;	// ¼Ò¼ýÁ¡ ºÎºÐ¸¸ °¡Á®¿È

				pFrame = &pObj->m_pFrame[ nCurrFrame ];		// ÇöÀç ÇÁ·¹ÀÓ Æ÷ÀÎÅÍ ¹ÞÀ½
				pNext  = &pObj->m_pFrame[ nNextFrame ];		// ´ÙÀ½ ÇÁ·¹ÀÓ Æ÷ÀÎÅÍ ¹ÞÀ½

				D3DXQuaternionSlerp( &qSlerp, &pFrame->m_qRot, &pNext->m_qRot, fSlp );		// È¸Àü º¸°£
				D3DXVec3Lerp( &vSlerp, &pFrame->m_vPos, &pNext->m_vPos, fSlp );					// º¤ÅÍ º¸°£

				// matAniTM°è»ê
				// ÀÌµ¿Çà·Ä ¸¸µê
				D3DXMatrixTranslation( &m1,	 vSlerp.x,  vSlerp.y,  vSlerp.z );
			
				// ÄõÅÍ´Ï¿Â º¸°£µÈ È¸ÀüÅ°°ªÀ» È¸ÀüÇà·Ä·Î ¸¸µê
			
#ifdef __FIX_ANIMATE_MODEL			
				if (fabs(qSlerp.x) < 0.000001f)
					qSlerp.x = qSlerp.x > 0 ? 0.000001f : -0.000001f;
				if (fabs(qSlerp.y) < 0.000001f)
					qSlerp.y = qSlerp.y > 0 ? 0.000001f : -0.000001f;
				if (fabs(qSlerp.z) < 0.000001f)
					qSlerp.z = qSlerp.z > 0 ? 0.000001f : -0.000001f;
				if (fabs(qSlerp.w) < 0.000001f)
					qSlerp.w = qSlerp.w > 0 ? 0.000001f : -0.000001f;
#endif // __FIX_ANIMATE_MODEL

				D3DXMatrixRotationQuaternion( &m2, &qSlerp );
				mUpdate[i] = m2 * m1;		// ÀÌµ¿Çà·Ä X È¸ÀüÇà·Ä = ¾Ö´ÏÇà·Ä

				if( pObj->m_nParentIdx != -1 )
					mUpdate[i] *= mParent[ pObj->m_nParentIdx ];
//				else
//					mUpdate[i] *= *mCenter;		// ·çÆ®´Â ¼¾ÅÍ¶û ÃÖÃÊ °öÇÔ.
					
			} else
			// ÀÌ ¿ÀºêÁ§Æ®¿¡ ÇÁ·¹ÀÓÀÌ ¾øÀ»¶§
			{
				if( pObj->m_nParentIdx != -1 )	// ºÎ¸ð°¡ ÀÖ´Â °æ¿ì
					mUpdate[i] = pObj->m_mLocalTM * mParent[ pObj->m_nParentIdx ];
				else
					mUpdate[i] = pObj->m_mLocalTM;					// ºÎ¸ð°¡ ¾ø´Ù¸é ·çÆ®´Ù.
//					mUpdate[i] = pObj->m_mLocalTM * *mCenter;		// ºÎ¸ð°¡ ¾ø´Ù¸é ·çÆ®´Ù. ·çÆ®´Â ¼¾ÅÍ¶û ÃÖÃÊ °öÇÔ.
			}
		} else
		// ÇÁ·¹ÀÓÀÌ ¾øÀ»¶§.
		{
			if( pObj->m_nParentIdx != -1 )
				mUpdate[i] = pObj->m_mLocalTM * mParent[ pObj->m_nParentIdx ];
			else
				mUpdate[i] = pObj->m_mLocalTM;		// ºÎ¸ð°¡ ¾ø´Ù¸é ·çÆ®´Ù. 
//				mUpdate[i] = pObj->m_mLocalTM * *mCenter;		// ºÎ¸ð°¡ ¾ø´Ù¸é ·çÆ®´Ù. ·çÆ®´Â ¼¾ÅÍ¶û ÃÖÃÊ °öÇÔ.
		}

		// mUpdate[i]  ÃÖÁ¾ WorldTM
	}
#endif // __WORLDSERVER
}

//
// ½ºÅ°´×.
// º»ÀÇ ¾Ö´Ï¸ÞÀÌ¼ÇÀÌ ³¡³­ÈÄ »À´ë¿Í ·ÎÄÃ¹öÅØ½º¸¦ °öÇÏ¿© ÃÖÁ¾ ¹öÅØ½ºÁÂÇ¥¸¦ °è»êÇÑ´Ù.
// mBones : °è»êÀÌ ³¡³­ »À´ëµéÀÇ ¸ÅÆ®¸¯½º 
//
void	CObject3D::Skining( GMOBJECT *pObj, const D3DXMATRIX *mBones )
{
/*
	D3DXVECTOR3	*vLocal, *vWorld;
	int			*pPhysique;
	int			nMax;

	// ¹öÅØ½º °¹¼ö¸¸Å­ µ·´Ù.
	// ½ºÅ²ÀÇ °¢ ¹öÅØ½ºµéÀº ÀÚ±â°¡ ¼Ò¼ÓµÈ BONEÀÇ ÃÖÁ¾°á°ú ¸ÅÆ®¸¯½º¿Í ÀÚ±â·ÎÄÃ ÁÂÇ¥¸¦ °öÇØ¼­
	// ÃÖÁ¾ ÁÂÇ¥¸¦ »ý¼ºÇØ³½´Ù.
	vLocal	  = pObj->m_pVertexList;
	pPhysique = pObj->m_pPhysiqueVertex;
	nMax	  = pObj->m_nMaxVertexList;
	vWorld	  = pObj->_pVertexUpdate;
	while( nMax-- )
	{
		D3DXVec3TransformCoord( vWorld, vLocal, &mBones[ *pPhysique ] );		// ÀÏ´ÜÀº ¿µÇâ¹Þ´Â boneÀ» ÇÑ°³¸¸ ¾´´Ù.
		// ¿©±â¼­ ³ë¸»µµ °°ÀÌ µ¹·Á¾ß ÇÑ´Ù.
		vLocal ++;
		vWorld ++;
		pPhysique ++;
	}
*/
}

// º»ÀÇ º¯È¯ÀÌ ¸ðµÎ ³¡³­ÈÄ ½ÇÇàµÈ´Ù.
// ÇÇÁöÅ© ¹öÅØ½ºµéÀ» º»¿¡ ¸ÂÃç ´Ù½Ã °è»êÇØ¼­ ¿ùµå ÁÂÇ¥·Î »ý¼º
// ¿ùµåÁÂÇ¥·Î »ý¼ºµÈ ¹öÅØ½ºµéÀ» ¹öÅØ½º ¹öÆÛ¿¡ Ä«ÇÇ
HRESULT		CObject3D::SetVertexBuffer( GMOBJECT *pObj )
{
/*
	CUSTOMVERTEX	*_pVB = pObj->_pVB;			// ½ºÅ°´×À» À§ÇÑ ÀÓ½Ã¹öÆÛ
	WORD			*pIB;
	D3DXVECTOR3		*pVList;
	int				nMax = pObj->m_nMaxVB;		// 

	// WorldPos°ª¸¸ °»½ÅµÇ¸é µÇ¹Ç·Î 
	// Skining()¿¡¼­ °è»êµÈ WorldPos°ª¸¸ Ä«ÇÇ½ÃÄÑ ÁØ´Ù.
	pIB    = pObj->m_pIIB;
	pVList = pObj->_pVertexUpdate;
	while( nMax-- )
	{
		_pVB->position = pVList[ *pIB ];
		_pVB ++;
		pIB ++;
	}
*/
	return S_OK;
}

// ½ºÅ°´×À¸·Î °»½ÅµÈ ¹öÅØ½º¹öÆÛ¸¦ d3d¹öÅØ½º ¹öÆÛ·Î Àü¼Û.
//
HRESULT CObject3D::SendVertexBuffer( GMOBJECT *pObj, LPDIRECT3DVERTEXBUFFER9 pd3d_VB )
{
	LPDIRECT3DDEVICE9 pd3dDevice = m_pd3dDevice;
	HRESULT	hr;
	VOID*	pVertices;
	int		nMax;
	int		nVertexSize;
	
	nVertexSize = pObj->m_nVertexSize;
	
	// ¹öÅØ½º ¹öÆÛ Àü¼Û
	nMax = pObj->m_nMaxVB * nVertexSize;
/*
	char buff[256] = { 0 };
	sprintf( buff, "%s\t%d,%d : TYPE : %d\n", m_szFileName, pObj->m_nMaxVB, pObj->m_nVertexSize, pObj->m_VBType );
	OutputDebugString( buff );
*/				
				
	if( FAILED( hr = pd3d_VB->Lock( 0, nMax, (void**)&pVertices, 0 ) ) )		// send vertex buffer
		return hr;
	memcpy( pVertices, pObj->m_pVB, nMax );	
	
	pd3d_VB->Unlock();
	
	return S_OK;
}

//
//
//
HRESULT CObject3D::SendIndexBuffer( GMOBJECT *pObj )
{
	LPDIRECT3DDEVICE9 pd3dDevice = m_pd3dDevice;
	VOID*	pVertices;
	int		nMax;

	// ÀÎµ¦½º ¹öÆÛ Àü¼Û
	nMax = pObj->m_nMaxIB * sizeof(WORD);
	if( FAILED(	pObj->m_pd3d_IB->Lock( 0, nMax, (void**)&pVertices, 0 ) ) )	// send index buffer
		return FAIL;
	
	memcpy( pVertices, pObj->m_pIB, nMax );	
	pObj->m_pd3d_IB->Unlock();

	return S_OK;
}


#ifndef __WORLDSERVER
void CObject3D::SetState( MATERIAL_BLOCK* pBlock, int nEffect, DWORD dwBlendFactor )
{
	// set
	if( pBlock->m_dwEffect & XE_2SIDE )
		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	//m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );

	if( m_nNoEffect == 1 )	return;
	
	if( dwBlendFactor != 0xff000000 ) // 
	//if( nBlendFactor < 255 ) // 
	{
		if( m_nNoEffect )	return;
		DWORD dwBlendF =  dwBlendFactor;
		//m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
		//m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );		// Æ¯Á¤ ¾ËÆÄÅ°°ª »­.
		//m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		//m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0xb0 );
		//pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA  );
		//pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		//m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( nBlendF, 0, 0, 0) );
		m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, dwBlendF );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
		//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );		// Æ¯Á¤ ¾ËÆÄÅ°°ª »­.
		
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR  );
	}
	else
	if( (pBlock->m_dwEffect & XE_OPACITY)  )	// ¾ËÆÄÃ¤³Î
	{
		m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0xb0 );
		//m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_ALWAYS   );		
		//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		if( m_nNoEffect )	return;
		
		if( pBlock->m_nAmount < 255 )		// 
		{
			m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( pBlock->m_nAmount, 0, 0, 0) );
			m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
			m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0 );
			m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );		// Æ¯Á¤ ¾ËÆÄÅ°°ª »­.
			
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR  );
		}
		
	}	

#ifdef __BS_EFFECT_LUA
	if( nEffect & XE_MTE )
	{
		//animated texcoord ( alpha map - subtract )
   
		//check data
		if( !m_pMteData )
			return;

#ifdef __ANISOTROPIC
		if (g_Option.m_nAnisotropic == TRUE)
		{
			m_pd3dDevice->SetSamplerState(1, D3DSAMP_MAXANISOTROPY, g_pD3dApp->m_d3dCaps.MaxAnisotropy);
			m_pd3dDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
			m_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			m_pd3dDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		}
		else
#endif // __ANISOTROPIC
		{
			m_pd3dDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			m_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		}
		
		//D3DRS_ALPHATESTENABLE²¨Áà¾ß ºí·»µù È¿°ú¸¦ ±â´ëÇÒ¼öÀÖ´Ù.
		m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	    
		m_pd3dDevice->SetTexture( 0, m_pMteData->_pTex[0] );

//		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
//		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
//		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR  );

		m_pd3dDevice->SetTexture( 1, m_pMteData->_pTex[1] );

		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADDSIGNED2X );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SUBTRACT );

		// 20sec : 1.0f
		// ¾Æ·¡´Â tool¿¡¼­ ±¸ÇöµÈ °Í°ú ´Ù¸¥µ¥, ½ÇÁ¦°ÔÀÓ¿¡¼­´Â ½Ã°£À¸·Î ±× ¼öÄ¡¸¦ Á¤ÇÏ°í ÀÖ±â ¶§¹®ÀÌ´Ù. 
		// tool¿¡¼­´Â ¿©·¯ °³Ã¼¼ö¸¦ ¶ç¿ïÇÊ¿ä°¡ ¾ø±â¿¡ ½Ã°£´ë½Å »ó¼ö¸¦ »ç¿ëÇÏ¿´´Ù.
		
		extern DWORD g_timeMTE;	// 20ÃÊ °»½Å¿ë Å¸ÀÌ¸Ó
		float fUV = (float)(g_timeMTE * 0.00005f);
		m_pMteData->_kUV[0].x = fUV;
		m_pMteData->_kUV[0].y = fUV;

		if( m_pMteData->_kUV[0].x > 1.0f )
			m_pMteData->_kUV[0] = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		m_pd3dDevice->SetVertexShaderConstantF( 91, (float*)&m_pMteData->_kUV[0], 1 );		//texture uv ( skinning2.vsh )


		if( g_timeMTE < 11000 ) // ¼­¼­È÷ ¹à¾ÆÁö°Ô ÇÏÀÚ
		{
			float v = (float)(g_timeMTE * 0.0001f);
			m_pMteData->_kUV[1] = D3DXVECTOR3( v, v, v );
		}
		else 
		{
			float v = (float)( ( 22000 - g_timeMTE ) * 0.0001f );
			m_pMteData->_kUV[1] = D3DXVECTOR3( v, v, v );
		}
	
		////light
		m_pd3dDevice->SetVertexShaderConstantF( 93, (float*)&m_pMteData->_kUV[1], 1 );
		m_pd3dDevice->SetVertexShaderConstantF( 94, (float*)&m_pMteData->_kUV[1], 1 );
	}
	else 
	
#endif //__BS_EFFECT_LUA

	// Set Reflection
	if( (pBlock->m_dwEffect & XE_REFLECT) || (nEffect & XE_REFLECT) )
	{
		if( m_nNoEffect )	return;
		m_pd3dDevice->SetTransform( D3DTS_TEXTURE1, &g_mReflect );
		extern LPDIRECT3DTEXTURE9 g_pReflectMap;
		m_pd3dDevice->SetTexture( 1, g_pReflectMap );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_ADDSIGNED2X );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	} else
	if( pBlock->m_dwEffect & XE_SELF_ILLUMINATE )
	{
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	//
	} else
	// Set Specular
	if( (pBlock->m_dwEffect & XE_SPECULAR) || (nEffect & XE_SPECULAR) )
	{
		if( m_nNoEffect )	return;
		int a = 0;
	} else
	if( (pBlock->m_dwEffect & XE_BUMP) || (nEffect & XE_BUMP) )
	{
		if( m_nNoEffect )	return;
		int a = 0;
	}
	else
	if( (pBlock->m_dwEffect & XE_HIGHLIGHT_OBJ ) || (nEffect & XE_HIGHLIGHT_OBJ) ) 
	{
		if( m_nNoEffect )	return;
//		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ); 
		m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR ); 
		m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO ); 
		m_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD); 
	}
	
//	m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
//	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

}

//
//
//
void CObject3D::ResetState( MATERIAL_BLOCK* pBlock,  int nEffect, DWORD dwBlendFactor )
{
	// reset
	if( pBlock->m_dwEffect & XE_2SIDE )
		m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	if( m_nNoEffect == 1 )	return;
	
	//if( nBlendFactor == 255 )
	if( dwBlendFactor == 0xff000000 )
	{
		m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
		if( m_nNoEffect )	return;
	}
	else
	{
		m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
		if( m_nNoEffect )	return;
		m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0xb0 );
		m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255, 0, 0, 0) );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
		
		//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
		//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	}
	
	if( 0 ) //pBlock->m_nOpacity )
	{
		if( m_nNoEffect )	return;
		if( dwBlendFactor == 255 )
		{
			m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
			m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
		}
		else
		{
			m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
			m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,        0xb0 );
			m_pd3dDevice->SetRenderState( D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB( 255, 0, 0, 0) );
			m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
			
			//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR );
			//m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		}
	} else
	if( (pBlock->m_dwEffect & XE_REFLECT) || (nEffect & XE_REFLECT) )
	{
		if( m_nNoEffect )	return;
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );	
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	} else
	if( pBlock->m_dwEffect & XE_SELF_ILLUMINATE )
	{
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, s_bLight );	//
	} else
	// reset Specular
	if( (pBlock->m_dwEffect & XE_SPECULAR) || (nEffect & XE_SPECULAR) )
	{
		if( m_nNoEffect )	return;
	} else
	if( (pBlock->m_dwEffect & XE_BUMP) || (nEffect & XE_BUMP) )
	{
		if( m_nNoEffect )	return;
		int a = 0;
	}
	
#ifdef __BS_EFFECT_LUA
	if( nEffect & XE_MTE )
	{
		if( m_nNoEffect ) 
			return;

		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );	
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
		m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );
	}
#endif //__BS_EFFECT_LUA

	m_pd3dDevice->SetTexture( 0, NULL );
}
//
//	½ºÅ²ÇüÅÂÀÇ ¿ÀºêÁ§Æ® ·»´õ·¯
//  ´Üµ¶À¸·Î´Â µ¿ÀÛÇÏÁö ¸øÇÑ´Ù.
//
void	CObject3D::RenderSkin( LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECT3DVERTEXBUFFER9 pd3d_VB, GMOBJECT *pObj, const D3DXMATRIX *mWorld, int nEffect, DWORD dwBlendFactor )
{
	//---- ±×´ë! ½ºÅ²¿ÀºêÁ§Æ®¸¸ È­¸é¿¡ ¾È³ª¿À´Â°¡! ±×·¯¸é CModelObject::Render()ÀÇ ¼³¸íÀ» ÀÐ¾îº¸°í È®ÀÎÇØº¸¾Æ¶ó!
	if( g_bUsableVS == FALSE )		// ¹öÅØ½º½¦ÀÌ´õ 1.1À» Áö¿ø¸øÇÏ¸é
		pd3dDevice->SetSoftwareVertexProcessing(TRUE);		// ¼ÒÇÁÆ®¿þ¾î ¸ðµå·Î ¹Ù²Þ
	
	MATERIAL_BLOCK	*pBlock;
	int		nMaxMtrl, nMaxVB, nMaxBone;
	D3DXMATRIX *pBone;
	D3DXMATRIX *pmBonesInv;
	int		i, idx;

	if( m_nNoEffect == 0 )
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
	}
	
	D3DXMATRIX	m1;

	if( m_pmExternBone )	
	{
		pBone = m_pmExternBone;		// ¿ÜºÎÁöÁ¤ »À´ë°¡ ÀÖ´Ù¸é ±×°É ¾²°í
		pmBonesInv = m_pmExternBoneInv;
	}
	else					
	{
		pBone = m_pBaseBone;		// ¾ø´Ù¸é µðÆúÆ®¸¦ ¾´´Ù.
		pmBonesInv = m_pBaseBoneInv;
	}

	if( pObj->m_nMaxUseBone )		// »À´ë°³¼ö°¡ ¸¹¾Æ¼­ µû·Î Àü¼ÛÇØ¾ßÇÔ.
	{
		nMaxBone = pObj->m_nMaxUseBone;
		for( i = 0; i < nMaxBone; i ++ )	// 
		{
			idx = pObj->m_UseBone[i];
			m1 = pmBonesInv[idx] * pBone[idx];

#ifdef	__YENV		
#ifdef __dx9c
			HRESULT hr = g_Neuz.m_pEffect->SetMatrix(g_Neuz.m_hvBones[i], &m1);
#else
			CString str;
			str.Format( "mBoneMatrix[%d]", i );
			HRESULT hr = g_Neuz.m_pEffect->SetMatrix( str, &m1 );
#endif
#else //__YENV
			D3DXMatrixTranspose( &m1, &m1 );		// ¸ÅÆ®¸¯½º¸¦ µ¹¸°´ÙÀ½.
			pd3dDevice->SetVertexShaderConstantF( i * 3, (float*)&m1, 3 );		// »ó¼ö·¹Áö½ºÅÍ¿¡ Áý¾î³ÖÀ½.
#endif //__YENV

		}
	}
//	pd3dDevice->SetVertexShader( D3DFVF_SKINVERTEX );
	pd3dDevice->SetVertexDeclaration( g_pSkinVertexDeclaration );
//	return;
#ifndef __YENV	
	HRESULT hr = pd3dDevice->SetVertexShader( g_pSkiningVS );
#endif //__YENV

#ifdef __YENV
	if( pObj->m_bBump && g_Option.m_bSpecBump )
	{
	#ifdef __YENV_WITHOUT_BUMP
		pd3dDevice->SetFVF( D3DFVF_SKINVERTEX );
		pd3dDevice->SetStreamSource( 0, pd3d_VB, 0,sizeof(SKINVERTEX) );
	#else 
		pd3dDevice->SetFVF( D3DFVF_SKINVERTEX_BUMP );
		pd3dDevice->SetStreamSource( 0, pd3d_VB, 0, sizeof(SKINVERTEX_BUMP) );
	#endif 
	}
	else
#endif //__YENV
	{
		pd3dDevice->SetStreamSource( 0, pd3d_VB, 0,sizeof(SKINVERTEX) );
	}


	pd3dDevice->SetIndices( pObj->m_pd3d_IB );

	pd3dDevice->SetTransform( D3DTS_WORLD, mWorld );	

	LPDIRECT3DTEXTURE9	*pBlkTexture = pObj->m_pMtrlBlkTexture;

	if( m_nTextureEx )
		pBlkTexture += (pObj->m_nMaxMtrlBlk * m_nTextureEx);
	pBlock   = pObj->m_pMtrlBlk;
	nMaxMtrl = pObj->m_nMaxMtrlBlk;
	nMaxVB	 = pObj->m_nMaxVB;
	while( nMaxMtrl-- )
	{
		if( pBlock->m_nMaxUseBone )		// »À´ë°³¼ö°¡ ¸¹¾Æ¼­ µû·Î Àü¼ÛÇØ¾ßÇÔ.
		{
			nMaxBone = pBlock->m_nMaxUseBone;
			for( i = 0; i < nMaxBone; i ++ )	// 
			{
				idx = pBlock->m_UseBone[i];
				m1 = pmBonesInv[idx] * pBone[idx];

	#ifdef	__YENV	
#ifdef __dx9c
				HRESULT hr = g_Neuz.m_pEffect->SetMatrix(g_Neuz.m_hvBones[i], &m1);
#else
				CString str;
				str.Format( "mBoneMatrix[%d]", i );
				HRESULT hr = g_Neuz.m_pEffect->SetMatrix( str, &m1 );
#endif
	#else //__YENV
				D3DXMatrixTranspose( &m1, &m1 );		// ¸ÅÆ®¸¯½º¸¦ µ¹¸°´ÙÀ½.
				pd3dDevice->SetVertexShaderConstantF( i * 3, (float*)&m1, 3 );		// »ó¼ö·¹Áö½ºÅÍ¿¡ Áý¾î³ÖÀ½.
	#endif //__YENV

			}
		}
		// »óÅÂ ¼¼ÆÃ 
		SetState( pBlock, nEffect, dwBlendFactor );
#ifdef __BS_EFFECT_LUA
		if( XE_MTE != nEffect)
#endif //__BS_EFFECT_LUA
		{
			LPDIRECT3DTEXTURE9 pCloakTexture = g_ModelGlobal.GetTexture();
			if( pCloakTexture )		// ¿ÜºÎ ÁöÁ¤ ÅØ½ºÃÄ°¡ ÀÖÀ»¶§.
				pd3dDevice->SetTexture( 0, pCloakTexture );
			else
			if( m_nNoTexture && pObj->m_bOpacity == 0 )
				pd3dDevice->SetTexture( 0, NULL );
			else
				pd3dDevice->SetTexture( 0, *pBlkTexture );
		}

#ifdef	__YENV
		LPDIRECT3DTEXTURE9	pNoSpecTexture;
		LPDIRECT3DTEXTURE9	pNormalTexture;
		if( g_Option.m_bSpecBump )
		{
			pNoSpecTexture = (pObj->m_pNoSpecTexture[0]);
			pNormalTexture = (pObj->m_pNormalTexture[0]);
		}
#endif //__YENV
		
#ifdef	__YENV		
		if( g_Option.m_bSpecBump )
		{
			//if( pNoSpecTexture )
				g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexSpecular, pNoSpecTexture );
#ifdef __dx9c
				g_Neuz.m_pEffect->SetTexture(g_Neuz.m_hvTexEnvi, g_Neuz.m_pEnvTex);
#else
				g_Neuz.m_pEffect->SetTexture("Tex_EnvironmentMap", g_Neuz.m_pEnvTex);
#endif

				
				//else
			//	g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexSpecular, *pBlkTexture );
		}
#endif //__YENV			

#ifdef	__YENV	
		if( g_Neuz.m_pEffect != NULL )
		{
			g_Neuz.m_pEffect->SetTechnique(g_Neuz.m_hTechnique);
			g_Neuz.m_pEffect->Begin( NULL, 0 );

	#ifdef __YENV_WITHOUT_BUMP
			if( pNoSpecTexture && g_Option.m_bSpecBump )
	#else //__YENV_WITHOUT_BUMP
			if( pBlock->m_dwEffect & XE_BUMP && g_Option.m_bSpecBump )
	#endif //__YENV_WITHOUT_BUMP
			{
				if (dwBlendFactor != 0xff000000)
#ifdef __dx9c 
					g_Neuz.m_pEffect->BeginPass(5);
#else
					g_Neuz.m_pEffect->Pass(5);
#endif
				else
#ifdef __dx9c
					g_Neuz.m_pEffect->BeginPass(4);
#else
					g_Neuz.m_pEffect->Pass(4);
#endif
					
				D3DXMATRIX mWorld2 = *mWorld;
				D3DXMATRIX m;
				m = mWorld2 * s_mView * s_mProj;
				
				// Åõ¿µ ¼³Á¤...
				g_Neuz.m_pEffect->SetMatrix( g_Neuz.m_hmWVP, &m );
				
				if( m_nNoTexture )
				{
#ifdef __dx9c
					g_Neuz.m_pEffect->BeginPass(3);
#else
					g_Neuz.m_pEffect->Pass(3);
#endif
					g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexDiffuse, NULL );
			#ifndef __YENV_WITHOUT_BUMP			
					g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexBump, NULL );
			#endif //__YENV_WITHOUT_BUMP
				}
				else
				{
					g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexDiffuse, *pBlkTexture );
					
					
					// ¶óÀÌÆ® À§Ä¡ ¼³Á¤
					D3DXVECTOR4 v;
					D3DXVECTOR4 vLight_Dir = s_vLight;//D3DXVECTOR4( 0.0f, 0.0f, -0.05f, 0 );
					D3DXMATRIX mLocal;
					D3DXMatrixInverse( &mLocal, NULL, &mWorld2 );						
					D3DXVec4Transform( &v, &vLight_Dir, &mLocal );						// ·ÎÄÃÁÂÇ¥·Î º¯È¯
					D3DXVec3Normalize( (D3DXVECTOR3*)&v, (D3DXVECTOR3*)&v );			// Á¤±ÔÈ­
					
					// ¶óÀÌÆ® ¹æÇâ ¼³Á¤

//					g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvLightDir, &D3DXVECTOR4( 0.0f, -1.0f, 0.0f, 1.0f) );
					g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvLightDir, &v );
					
					// Æ÷±×
					D3DXVECTOR4 vFog;
					vFog.x = s_fFogEnd/(s_fFogEnd-s_fFogStart);
					vFog.y = -1.0f/(s_fFogEnd-s_fFogStart);
					
					if( g_Neuz.m_hvFog != NULL ) 
						g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vFog );
					
					DWORD dwColor = CWorld::GetDiffuseColor();
					pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,  dwColor );//CWorld::m_dwBgColor ) ;//CWorld::m_dwBgColor );
								
					g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvDiffuse, (D3DXVECTOR4*)&s_fDiffuse[0] );
					g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvAmbient, (D3DXVECTOR4*)&s_fAmbient[0] );
					
					// Ä«¸Þ¶ó À§Ä¡
					m = mWorld2 * s_mView ;
					D3DXMatrixInverse( &m, NULL, &m );						
					v = D3DXVECTOR4( 0, 0, 0, 1 );
					D3DXVec4Transform( &v, &v, &m );
					g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvEyePos, &v );
					
					// ¹ý¼±¸Ê
			#ifndef __YENV_WITHOUT_BUMP			
					g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexBump, pNormalTexture );
			#endif //__YENV_WITHOUT_BUMP
				}
				
				// Á¤Á¡¼±¾ð
				m_pd3dDevice->SetVertexDeclaration( m_pNormalDecl );
			}
			else
			{
#ifdef __dx9c
				g_Neuz.m_pEffect->BeginPass(3);
#else
				g_Neuz.m_pEffect->Pass(3);
#endif
				D3DXMATRIX mWorld2 = *mWorld;
				D3DXMATRIX m;
				m = mWorld2 * s_mView * s_mProj;
				
				// Åõ¿µ ¼³Á¤...
				g_Neuz.m_pEffect->SetMatrix( g_Neuz.m_hmWVP, &m );
#ifdef __dx9c
				g_Neuz.m_pEffect->SetMatrix(g_Neuz.m_hvmatView, &s_mView);
				g_Neuz.m_pEffect->SetMatrix(g_Neuz.m_hvmatProj, &s_mProj);
#else
				g_Neuz.m_pEffect->SetMatrix( "mView", &s_mView );
				g_Neuz.m_pEffect->SetMatrix( "mProj", &s_mProj );
#endif
				
				if( m_nNoTexture && pObj->m_bOpacity == 0 )
					g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexDiffuse, NULL );
				else
					g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexDiffuse, *pBlkTexture );
				
				// ¶óÀÌÆ® À§Ä¡ ¼³Á¤
				D3DXVECTOR4 v;
				D3DXVECTOR4 vLight_Dir = s_vLight;//D3DXVECTOR4( 0.0f, 0.0f, -0.05f, 0 );
				D3DXMATRIX mLocal;
				D3DXMatrixInverse( &mLocal, NULL, &mWorld2 );						
				D3DXVec4Transform( &v, &vLight_Dir, &mLocal );						// ·ÎÄÃÁÂÇ¥·Î º¯È¯
				D3DXVec3Normalize( (D3DXVECTOR3*)&v, (D3DXVECTOR3*)&v );			// Á¤±ÔÈ­
				
				// ¶óÀÌÆ® ¹æÇâ ¼³Á¤
				g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvLightDir, &v );
			
				g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvDiffuse, (D3DXVECTOR4*)&s_fDiffuse[0] );	
				g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvAmbient, (D3DXVECTOR4*)&s_fAmbient[0] );
				
				// Æ÷±×
				D3DXVECTOR4 vFog;
				vFog.x = s_fFogEnd/(s_fFogEnd-s_fFogStart);
				vFog.y = -1.0f/(s_fFogEnd-s_fFogStart);
				
				if( g_Neuz.m_hvFog != NULL ) 
#ifdef __dx9c
					g_Neuz.m_pEffect->SetVector(g_Neuz.m_hvFog, &vFog);
#else
					g_Neuz.m_pEffect->SetVector( "vFog", &vFog );
#endif
				
				DWORD dwColor = CWorld::GetDiffuseColor();
				pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,  dwColor );//CWorld::m_dwBgColor ) ;//CWorld::m_dwBgColor );
				
				m_pd3dDevice->SetVertexDeclaration( g_pSkinVertexDeclaration );
			}

			HRESULT hr;
			hr = pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, nMaxVB, pBlock->m_nStartVertex, pBlock->m_nPrimitiveCount );

/*
	#ifdef __YENV_WITHOUT_BUMP
			g_Neuz.m_pEffect->Pass(6);		
			pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, nMaxVB, pBlock->m_nStartVertex, pBlock->m_nPrimitiveCount );
	#endif //__YENV_WITHOUT_BUMP
/**/

			g_Neuz.m_pEffect->End();
		}
#else //__YENV
		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, nMaxVB, pBlock->m_nStartVertex, pBlock->m_nPrimitiveCount );
#endif //__YENV

		ResetState( pBlock, nEffect, dwBlendFactor );

		g_nMaxTri += pBlock->m_nPrimitiveCount;

		pBlock ++;
		pBlkTexture ++;
	}

	// ¹öÅØ½º½¦ÀÌ´õ ¾²´Â³ÑÀº Æ÷±×¸¦ ²¨¾ß Á¦´ë·Î ³ª¿À´õ¶ó.
//	pd3dDevice->SetRenderState( D3DRS_FOGENABLE, s_bFog );
	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );
	
#ifdef __CLIENT
	if( g_Neuz.m_d3dSettings.GetVertexProcessingType() != SOFTWARE_VP && g_bUsableVS == FALSE )		// ¹öÅØ½º½¦ÀÌ´õ 1.1À» Áö¿ø¸øÇÏ¸é
		pd3dDevice->SetSoftwareVertexProcessing(FALSE);		// ´Ù½Ã ÇÏµå¿þ¾îÃ³¸®·Î µ¹¸².
#endif	
	
}

//
// ´Üµ¶À¸·Î´Â µ¿ÀÛÇÏÁö ¸øÇÑ´Ù. »çÀü¿¡ SetRenderµîÀ» ÇØÁà¾ß ÇÑ´Ù.
// ÀÏ¹ÝÇüÅÂÀÇ ·»´õ·¯
// mUpdate : °¢ ¿ÀºêÁ§Æ®°¡ AnimateµÇ°í³­ ÈÄ, ÃÖÁ¾ ¸ÅÆ®¸¯½ºÀÇ ¸®½ºÆ®.
// mWorld : ±âÁØ ¸ÅÆ®¸¯½º.
//
void	CObject3D::RenderNormal( LPDIRECT3DDEVICE9 pd3dDevice, GMOBJECT *pObj, const D3DXMATRIX *mWorld, int nEffect, int nBlendFactor )
{
	MATERIAL_BLOCK	*pBlock;
	int		nMaxMtrl, nMaxVB;
//	int		i;
//	D3DXMATRIX	m1;
//	D3DXMATRIX *mUpdate = _mUpdate;
	pd3dDevice->SetVertexShader( NULL );

	pd3dDevice->SetVertexDeclaration( NULL );
	
	if( m_nNoEffect == 0 )
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );

#ifdef __ENVIRONMENT_EFFECT

		if( CEnvironment::GetInstance()->GetSeason() == SEASON_SPRING )
		{
			if( stricmp( m_szFileName, "obj_macoprtr16.o3d" ) == 0 || stricmp( m_szFileName, "obj_macoprtr17.o3d" ) == 0 )
			{
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
			}
			else
			{
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			}
		}

#else // __ENVIRONMENT_EFFECT

#ifdef __JAPAN_SAKURA
		if( stricmp( m_szFileName, "obj_macoprtr16.o3d" ) == 0 || stricmp( m_szFileName, "obj_macoprtr17.o3d" ) == 0 )
		{
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE2X );
		}
		else
		{
			pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		}
#else
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
#endif //__JAPAN_SAKURA

#endif // __ENVIRONMENT_EFFECT
		
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	}
	
	LPDIRECT3DTEXTURE9	*pBlkTexture = pObj->m_pMtrlBlkTexture;
	if( m_nTextureEx )
		pBlkTexture += (pObj->m_nMaxMtrlBlk * m_nTextureEx);
	
#ifdef	__YENV
	if( pObj->m_bBump && g_Option.m_bSpecBump )
	{
	#ifdef __YENV_WITHOUT_BUMP
		pd3dDevice->SetFVF( D3DFVF_NORMALVERTEX );
		pd3dDevice->SetStreamSource( 0, pObj->m_pd3d_VB, 0, sizeof(NORMALVERTEX) );
	#else //__YENV_WITHOUT_BUMP
		pd3dDevice->SetFVF( D3DFVF_NORMALVERTEX_BUMP );
		pd3dDevice->SetStreamSource( 0, pObj->m_pd3d_VB, 0, sizeof(NORMALVERTEX_BUMP) );
	#endif //__YENV_WITHOUT_BUMP
	}
	else
#endif //__YENV
	{
		pd3dDevice->SetFVF( D3DFVF_NORMALVERTEX );
		pd3dDevice->SetStreamSource( 0, pObj->m_pd3d_VB, 0, sizeof(NORMALVERTEX) );
	}
	
	pd3dDevice->SetIndices( pObj->m_pd3d_IB );
	
//	m1 = mUpdate[i] * mWorld;		// mUpdate¿¡ ¹Ì¸® mWorld¸¦ °öÇÑ»óÅÂ·Î ¿Íµµ µÉ°Í °°´Ù.
	pd3dDevice->SetTransform( D3DTS_WORLD, mWorld );

	pBlock   = pObj->m_pMtrlBlk;
	nMaxMtrl = pObj->m_nMaxMtrlBlk;
	nMaxVB	 = pObj->m_nMaxVB;
	
	while( nMaxMtrl-- )
	{
		// »óÅÂ ¼¼ÆÃ 
		if( pObj->m_bLight )
		{
			if( m_nNoEffect == 0 )
			{
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
				pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	//
				pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE  );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			}
		} else
			SetState( pBlock, nEffect, nBlendFactor );

		{
			if( m_nNoTexture && pObj->m_bOpacity == 0 )
				pd3dDevice->SetTexture( 0, NULL );
			else
				pd3dDevice->SetTexture( 0, *pBlkTexture );
		}

#ifdef	__YENV
		
		LPDIRECT3DTEXTURE9	pNoSpecTexture;
		LPDIRECT3DTEXTURE9	pNormalTexture;

		if( g_Option.m_bSpecBump )
		{
			pNoSpecTexture = pObj->m_pNoSpecTexture[0];
			pNormalTexture = pObj->m_pNormalTexture[0];
			
			//if( pNoSpecTexture )
			g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexSpecular, pNoSpecTexture );
#ifdef __YENV
#ifdef __dx9c
			g_Neuz.m_pEffect->SetTexture(g_Neuz.m_hvTexEnvi, g_Neuz.m_pEnvTex);
#else
			g_Neuz.m_pEffect->SetTexture( "Tex_EnvironmentMap", g_Neuz.m_pEnvTex );
#endif
#endif //__YENV				
		}			

		if( g_dwUsablePS_Level == 2 && !(nEffect & XE_REFLECT) )
		{
			if( g_Neuz.m_pEffect != nullptr )
			{
				if( !m_nNoTexture )
				{
					g_Neuz.m_pEffect->SetTechnique(g_Neuz.m_hTechnique);
					g_Neuz.m_pEffect->Begin( NULL, 0 );

					D3DXMATRIX mWorld2 = *mWorld;
					D3DXMATRIX m;
					m = mWorld2 * s_mView * s_mProj;
					
					// Åõ¿µ ¼³Á¤...
					g_Neuz.m_pEffect->SetMatrix( g_Neuz.m_hmWVP, &m );
					g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexDiffuse, *pBlkTexture );
					
					// ¶óÀÌÆ® À§Ä¡ ¼³Á¤
					D3DXVECTOR4 v;
					D3DXVECTOR4 vLight_Dir = s_vLight;//D3DXVECTOR4( 0.0f, 0.0f, -0.05f, 0 );
					D3DXMATRIX mLocal;
					D3DXMatrixInverse( &mLocal, NULL, &mWorld2 );						
					D3DXVec4Transform( &v, &vLight_Dir, &mLocal );						// ·ÎÄÃÁÂÇ¥·Î º¯È¯
					D3DXVec3Normalize( (D3DXVECTOR3*)&v, (D3DXVECTOR3*)&v );			// Á¤±ÔÈ­
					v.w = -0.6f;														// È¯°æ±¤ÀÇ ¹à±â(Ambint) Def : -0.3f
					
					// ¶óÀÌÆ® ¹æÇâ ¼³Á¤
					g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvLightDir, &v );
					
					// Æ÷±×
					D3DXVECTOR4 vFog;
					vFog.x = s_fFogEnd/(s_fFogEnd-s_fFogStart);
					vFog.y = -1.0f/(s_fFogEnd-s_fFogStart);
					
					if( g_Neuz.m_hvFog != NULL ) 
						g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvFog, &vFog );
					
					DWORD dwColor = CWorld::GetDiffuseColor();
					pd3dDevice->SetRenderState( D3DRS_FOGCOLOR,  dwColor );//CWorld::m_dwBgColor ) ;//CWorld::m_dwBgColor );
					
					//g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvDiffuse, &D3DXVECTOR4( 1.0f, 0.0f, 0.0f, 0.0f) );	
					g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvDiffuse, (D3DXVECTOR4*)&s_fDiffuse[0] );	
					g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvAmbient, (D3DXVECTOR4*)&s_fAmbient[0] );
					
					// Ä«¸Þ¶ó À§Ä¡
					m = mWorld2 * s_mView ;
					D3DXMatrixInverse( &m, NULL, &m );						
					v = D3DXVECTOR4( 0, 0, 0, 1 );
					D3DXVec4Transform( &v, &v, &m );
					g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvEyePos, &v );
					
					// ¹öÅÃ½º½¦ÀÌ´õ°¡ 1.1 Áö¿øÇÏ°í ÇÈ¼¿½¦ÀÌ´õ°¡ 2.0ÀÌ»óÀÌ¾î¾ß ÇÑ´Ù...
					if( nEffect & XE_NOBUMP )
#ifdef __dx9c
						g_Neuz.m_pEffect->BeginPass(0);
#else
						g_Neuz.m_pEffect->Pass(0);		
#endif
					else				
		#ifdef __YENV_WITHOUT_BUMP
					if( pNoSpecTexture && g_Option.m_bSpecBump  )
					{
						if( nBlendFactor != 0xff000000 )
#ifdef __dx9c
							g_Neuz.m_pEffect->BeginPass(2);
#else
							g_Neuz.m_pEffect->Pass(2);
#endif
						else
#ifdef __dx9c
							g_Neuz.m_pEffect->BeginPass(1);
#else
							g_Neuz.m_pEffect->Pass(1);					
#endif

						m_pd3dDevice->SetVertexDeclaration( m_pNormalDecl );
					}
					else
#ifdef __dx9c
						g_Neuz.m_pEffect->BeginPass(0);
#else
						g_Neuz.m_pEffect->Pass(0);		
#endif
		#else //__YENV_WITHOUT_BUMP
					if( pBlock->m_dwEffect & XE_BUMP && g_Option.m_bSpecBump  )
					{
						if( nBlendFactor != 0xff000000 )
							g_Neuz.m_pEffect->Pass(2);
						else
							g_Neuz.m_pEffect->Pass(1);
						
						// ¹ý¼±¸Ê
						g_Neuz.m_pEffect->SetTexture( g_Neuz.m_hTexBump, pNormalTexture );				
						
						// Á¤Á¡¼±¾ð
						m_pd3dDevice->SetVertexDeclaration( m_pNormalDecl );
					}
					else
						g_Neuz.m_pEffect->Pass(0);		
		#endif //__YENV_WITHOUT_BUMP
				}

				pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, nMaxVB, pBlock->m_nStartVertex, pBlock->m_nPrimitiveCount );			
	/*
		#ifdef __YENV_WITHOUT_BUMP
				g_Neuz.m_pEffect->Pass(6);		
				pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, nMaxVB, pBlock->m_nStartVertex, pBlock->m_nPrimitiveCount );
		#endif //__YENV_WITHOUT_BUMP
	/**/
				if( !m_nNoTexture )
					g_Neuz.m_pEffect->End();
			}
		}
		else
			pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, nMaxVB, pBlock->m_nStartVertex, pBlock->m_nPrimitiveCount );

#else //__YENV
		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, nMaxVB, pBlock->m_nStartVertex, pBlock->m_nPrimitiveCount );
#endif //__YENV

		// »óÅÂ º¹¿ø 
		if( pObj->m_bLight )
		{
			if( m_nNoEffect == 0 )
			{
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
				pd3dDevice->SetRenderState( D3DRS_LIGHTING, s_bLight );	//
				pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			}
		} else
			ResetState( pBlock, nEffect, nBlendFactor );

		g_nMaxTri += pBlock->m_nPrimitiveCount;

		pBlock ++;
		pBlkTexture ++;
	}
}

#if 0
// Ãæµ¹¸Þ½ÃÀÇ ·»´õ.
void	CObject3D::RenderCollObject( LPDIRECT3DDEVICE9 pd3dDevice, const D3DXMATRIX *mWorld )
{
	GMOBJECT *pObj = &m_CollObject;
	int nEffect = 0;
	int nBlendFactor = 255;
	MATERIAL_BLOCK	*pBlock;
	int		nMaxMtrl, nMaxVB;

	pd3dDevice->SetVertexShader( NULL );
	pd3dDevice->SetVertexDeclaration( NULL );
	pd3dDevice->SetFVF( D3DFVF_NORMALVERTEX );

	if( m_nNoEffect == 0 )
	{
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	}
	
	LPDIRECT3DTEXTURE9	*pBlkTexture = pObj->m_pMtrlBlkTexture;

#ifdef __YENV
	if( pObj->m_bBump && g_Option.m_bSpecBump )
		pd3dDevice->SetStreamSource( 0, pObj->m_pd3d_VB, 0, sizeof(NORMALVERTEX_BUMP) );
	else
#endif //__YENV
		pd3dDevice->SetStreamSource( 0, pObj->m_pd3d_VB, 0, sizeof(NORMALVERTEX) );
	
	
	pd3dDevice->SetIndices( pObj->m_pd3d_IB );
	
	//	m1 = mUpdate[i] * mWorld;		// mUpdate¿¡ ¹Ì¸® mWorld¸¦ °öÇÑ»óÅÂ·Î ¿Íµµ µÉ°Í °°´Ù.
	pd3dDevice->SetTransform( D3DTS_WORLD, mWorld );
	
	pBlock   = pObj->m_pMtrlBlk;
	nMaxMtrl = pObj->m_nMaxMtrlBlk;
	nMaxVB	 = pObj->m_nMaxVB;
	while( nMaxMtrl-- )
	{
		// »óÅÂ ¼¼ÆÃ 
		if( pObj->m_bLight )
		{
			if( m_nNoEffect == 0 )
			{
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
				pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	//
				pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE  );
				pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			}
		} else
			SetState( pBlock, nEffect, nBlendFactor );
		
		{
			if( m_nNoTexture && pObj->m_bOpacity == 0 )
				pd3dDevice->SetTexture( 0, NULL );
			else
				pd3dDevice->SetTexture( 0, *pBlkTexture );
		}
		
		pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, nMaxVB, pBlock->m_nStartVertex, pBlock->m_nPrimitiveCount );

		// »óÅÂ º¹¿ø 
		if( pObj->m_bLight )
		{
			if( m_nNoEffect == 0 )
			{
				pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
				pd3dDevice->SetRenderState( D3DRS_LIGHTING, s_bLight );	//
				pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
				pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			}
		} else
			ResetState( pBlock, nEffect, nBlendFactor );

		g_nMaxTri += pBlock->m_nPrimitiveCount;

		pBlock ++;
		pBlkTexture ++;
	}
}
#endif // 0

//
// ÀÚÃ¼ º»¾Ö´Ï¸ÞÀÌ¼ÇÀ» ¾²´Â ¹è°æ¿ÀºêÁ§Æ®µéÀº ÀÌ ÇÔ¼ö·Î º» ¸ÅÆ®¸¯½º¸¦ ½¦ÀÌ´õ¿¡ º¸³½´Ù.
//
void	CObject3D::SetShader( const D3DXMATRIX *mWorld )
{
	int		i;
	if( m_pBaseBone == NULL )	return;

	D3DXMATRIX *pmBones = m_pBaseBone;		// »À´ë¸ð¼ÇÀÌ ·ÎµùµÈ°Ô ¾øÀ¸¸é µðÆúÆ®¸¦ ¾´´Ù.
	D3DXMATRIX *pmBonesInv = m_pBaseBoneInv ;
	D3DXMATRIX mWorldTranspose;
	
	if( m_bSendVS )	// »À´ë°³¼ö°¡ MAX_VS_BONEÀÌÇÏ¶ó¼­ ÇÑ¹ø¿¡ ´Ù Àü¼ÛÇÑ´Ù.
	{
		int		nMaxBone = m_nMaxBone;
		if( nMaxBone > MAX_VS_BONE )	
			Error( "CModelObject::Render : to Many Bones in a Material Block(28) %d", nMaxBone );

		for( i = 0; i < nMaxBone; i ++ )	// MAX_VS_BONE°³ ÀÌÇÏ	
		{
			mWorldTranspose = pmBonesInv[i] * pmBones[i];

#ifdef	__YENV		
#ifdef __dx9c
			HRESULT hr = g_Neuz.m_pEffect->SetMatrix(g_Neuz.m_hvBones[i], &mWorldTranspose);
#else
			CString str;
			str.Format( "mBoneMatrix[%d]", i );
			HRESULT hr = g_Neuz.m_pEffect->SetMatrix( str, &mWorldTranspose );
#endif
#else //__YENV
			D3DXMatrixTranspose( &mWorldTranspose, &mWorldTranspose );		// ¸ÅÆ®¸¯½º¸¦ µ¹¸°´ÙÀ½.
			m_pd3dDevice->SetVertexShaderConstantF( i * 3, (float*)&mWorldTranspose, 3 );		// »ó¼ö·¹Áö½ºÅÍ¿¡ Áý¾î³ÖÀ½.
#endif //__YENV

		}
	}
	
	D3DXMATRIX	mView, mProj;
	D3DXMATRIX	mViewProj, mViewProjTranspose, mInvWorld;
	
	//		static D3DXVECTOR4 vConst( 1.0f, 1.0f, 1.0f, 100.0f );
	D3DXVECTOR4 vLight = s_vLight;
//	D3DXVECTOR4 vLight( 0.0f, 0.0f, 1.0f, 0.0f );
//	D3DXVECTOR4 vEyePos( 0.0f, 1.0f, -5.0f, 0.0f );
//	D3DXVec4Normalize( &vLight, &vLight );
	//	FLOAT       fDiffuse[] = { 1.00f, 1.00f, 0.00f, 0.00f };
	//	FLOAT       fAmbient[] = { 0.25f, 0.25f, 0.25f, 0.25f };
//	static FLOAT       fDiffuse[] = { 1.00f, 1.00f, 1.00f, 1.00f };
	//		static FLOAT       fDiffuse[] = { 1.00f, 1.00f, 1.00f, 1.00f };
//	static FLOAT       fAmbient[] = { 1.00f, 1.00f, 1.00f, 1.00f };
//	static FLOAT       fSpecular[] ={ 1.00f, 1.00f, 1.00f, 128.00f };
//	static FLOAT       fLightCol[] ={ 1.00f, 1.00f, 1.00f, 1.00f };
//	m_pd3dDevice->GetTransform( D3DTS_VIEW,       &mView );
//	m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &mProj );
//	mViewProj = *mWorld * mView * mProj;
	mViewProj = *mWorld * s_mView * s_mProj;
	D3DXMatrixTranspose( &mViewProjTranspose, &mViewProj );
	D3DXMatrixTranspose( &mWorldTranspose, mWorld );
	
	D3DXMatrixInverse( &mInvWorld, NULL, mWorld );
	D3DXVec4Transform( &vLight, &vLight, &mInvWorld );
//	D3DXVec4Transform( &vEyePos, &vEyePos, &mInvWorld );
	D3DXVec4Normalize( &vLight, &vLight );
	
#ifdef __YENV	
	/*
	// Åõ¿µ ¼³Á¤...
	g_Neuz.m_pEffect->SetMatrix( g_Neuz.m_hmWVP, &mViewProjTranspose );
	
	// ¶óÀÌÆ® À§Ä¡ ¼³Á¤
	D3DXVECTOR4 v;
	D3DXVECTOR4 vLight_Pos = s_vLight;
	D3DXMATRIX mLocal;
	D3DXMatrixInverse( &mLocal, NULL, &mViewProjTranspose );						
	D3DXVec4Transform( &v, &vLight_Pos, &mLocal );						// ·ÎÄÃÁÂÇ¥·Î º¯È¯
	D3DXVec3Normalize( (D3DXVECTOR3*)&v, (D3DXVECTOR3*)&v );			// Á¤±ÔÈ­
	v.w = -0.6f;														// È¯°æ±¤ÀÇ ¹à±â(Ambint) Def : -0.3f
	
	// ¶óÀÌÆ® ¹æÇâ ¼³Á¤
	g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvLightDir, &v );
	
	g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvDiffuse, (D3DXVECTOR4*)&s_fDiffuse[0] );	
	g_Neuz.m_pEffect->SetVector( g_Neuz.m_hvAmbient, (D3DXVECTOR4*)&s_fAmbient[0] );	
	*/
#else //__YENV
	m_pd3dDevice->SetVertexShaderConstantF( 84, (float*)&mViewProjTranspose, 4 );
	//		m_pd3dDevice->SetVertexShaderConstantF( 88, (float*)&mWorldTranspose, 4 );
//	m_pd3dDevice->SetVertexShaderConstantF( 88, (float*)&vEyePos,  1 );
//	m_pd3dDevice->SetVertexShaderConstantF( 89, (float*)&fSpecular, 1 );
//	m_pd3dDevice->SetVertexShaderConstantF( 90, (float*)&fLightCol, 1 );
	m_pd3dDevice->SetVertexShaderConstantF( 92, (float*)&vLight,   1 );
	m_pd3dDevice->SetVertexShaderConstantF( 93, (float*)&s_fDiffuse, 1 );
	m_pd3dDevice->SetVertexShaderConstantF( 94, (float*)&s_fAmbient, 1 );
	//		m_pd3dDevice->SetVertexShaderConstant( 95, &vConst, 1 );
#endif //__YENV

}

//
//
//
void	CObject3D::Render( LPDIRECT3DDEVICE9 pd3dDevice, LPDIRECT3DVERTEXBUFFER9 *ppd3d_VB, FLOAT fFrameCurrent, int nNextFrame, const D3DXMATRIX *mWorld, int nEffect, DWORD dwBlendFactor )
{
	int			i;
	int			nMax = m_pGroup->m_nMaxObject;
	GMOBJECT	*pObj;
	D3DXMATRIX	m1;
	D3DXMATRIX	*pBone = NULL;

	if( m_pGroup->_mUpdate )		// maxframeÀÌ ÀÖ´Ù¸é ¿ÜºÎº»ÀÌ°Ç ³»Àåº»ÀÌ°Ç Animate´Â µ¹¾Æ¾ß ÇÑ´Ù.
		Animate( fFrameCurrent, nNextFrame );	// mUpdate ¾Ö´Ï¸ÞÀÌ¼Ç °è»êÈÄ °»½ÅµÈ ¸ÅÆ®¸¯½º
	if( m_nMaxFrame > 0 )
	{
		if( m_pmExternBone == NULL )
		{
			if( m_pMotion )	
				m_pMotion->AnimateBone( m_pBaseBone, NULL, fFrameCurrent, nNextFrame, 0, FALSE, 0.0f );
		}
	}
	
	if( m_pBaseBone )	// ¸ð¼ÇÀº ¾ø°í º»¸¸ ÀÖ´õ¶óµµ ½¦ÀÌ´õ¿¡ Àü¼ÛÇØ¾ßÇÔ.
		SetShader( mWorld );
	
	for( i = 0; i < nMax; i ++ )
	{
		pObj = &m_pGroup->m_pObject[i];

		if( pObj->m_nVertexSize == 0 || pObj->m_dwFVF == 0 )
			Error( "%s : FVF Zero. VertexSize=%d, FVF=%d", m_szFileName, pObj->m_nVertexSize, pObj->m_dwFVF );

		if( pObj->m_Type == GMT_SKIN )
		{
			if( ppd3d_VB == NULL )
				Error( "CObject3D::Render : %s Polygon zero.", m_szFileName );

			RenderSkin( pd3dDevice, ppd3d_VB[i], pObj, mWorld, nEffect, dwBlendFactor );

#ifdef __BS_EFFECT_LUA
			if( m_dwEffect_EX & XE_MTE )
				RenderSkin( pd3dDevice, ppd3d_VB[i], pObj, mWorld, XE_MTE, dwBlendFactor );
#endif //__BS_EFFECT_LUA

		} else
		{
			m1 = m_pGroup->_mUpdate[i] * *mWorld;
#ifndef __ALWAYS_MODEL_LIGHT
			if (pObj->m_bLight && s_bNight == FALSE)	// ¶óÀÌÆ® ¿ÀºêÁ§Æ®´Â ³·¿£ ·»´õ¸µ µÇÁö ¾ÊÀ½.
				continue;
#endif //__ALWAYS_MODEL_LIGHT
			RenderNormal( pd3dDevice, pObj, &m1, nEffect, dwBlendFactor );
		}
	}
	if( m_nNoTexture == 0 )
	{
		pd3dDevice->SetTexture( 1, NULL );
		pd3dDevice->SetTexture( 0, NULL );
	}

}

//
//
//

#ifdef __YENV
HRESULT CObject3D::ExtractBuffers( int nType, LPDIRECT3DVERTEXBUFFER9 *ppd3d_VB, GMOBJECT *pObject )
{
	#ifdef __YENV_WITHOUT_BUMP
	return S_OK;
	#endif //__YENV_WITHOUT_BUMP

	int		nMax;
	GMOBJECT* pObj = pObject;
	WORD		*pIB;
	int		 j;

	if( pObj->m_VBType == VBT_BUMP )
		return S_OK;

	if( nType == GMT_SKIN )
	{
		SKINVERTEX_BUMP*  pVB_BUMP;

		{
			SKINVERTEX *pVB;
			nMax = pObj->m_nMaxFaceList;

			pVB	= (SKINVERTEX*)(pObj->m_pVB);
			pIB	= pObj->m_pIB;

			pVB_BUMP = new SKINVERTEX_BUMP[ pObj->m_nMaxVB ];

			for( j = 0; j < nMax; j++ )
			{
				int nIndex = *pIB++;
				pVB_BUMP[nIndex].position	= pVB[ nIndex ].position;
				pVB_BUMP[nIndex].normal		= pVB[ nIndex ].normal;
				pVB_BUMP[nIndex].w1			= pVB[ nIndex ].w1;
				pVB_BUMP[nIndex].w2			= pVB[ nIndex ].w2;
				pVB_BUMP[nIndex].matIdx		= pVB[ nIndex ].matIdx;
				pVB_BUMP[nIndex].tu			= pVB[ nIndex ].tu;
				pVB_BUMP[nIndex].tv			= pVB[ nIndex ].tv;
				SKINVERTEX_BUMP *pVertex1	= &pVB_BUMP[nIndex];
				nIndex = *pIB++;
				pVB_BUMP[nIndex].position	= pVB[ nIndex ].position;
				pVB_BUMP[nIndex].normal		= pVB[ nIndex ].normal;
				pVB_BUMP[nIndex].w1			= pVB[ nIndex ].w1;
				pVB_BUMP[nIndex].w2			= pVB[ nIndex ].w2;
				pVB_BUMP[nIndex].matIdx		= pVB[ nIndex ].matIdx;
				pVB_BUMP[nIndex].tu			= pVB[ nIndex ].tu;
				pVB_BUMP[nIndex].tv			= pVB[ nIndex ].tv;
				SKINVERTEX_BUMP *pVertex2	= &pVB_BUMP[nIndex];
				nIndex = *pIB++;
				pVB_BUMP[nIndex].position	= pVB[ nIndex ].position;
				pVB_BUMP[nIndex].normal		= pVB[ nIndex ].normal;
				pVB_BUMP[nIndex].w1			= pVB[ nIndex ].w1;
				pVB_BUMP[nIndex].w2			= pVB[ nIndex ].w2;
				pVB_BUMP[nIndex].matIdx		= pVB[ nIndex ].matIdx;
				pVB_BUMP[nIndex].tu			= pVB[ nIndex ].tu;
				pVB_BUMP[nIndex].tv			= pVB[ nIndex ].tv;
				SKINVERTEX_BUMP *pVertex3	= &pVB_BUMP[nIndex];

				//Compute the U and V in terms of X gradients
				D3DXVECTOR3 Side1 = D3DXVECTOR3(pVertex2->position.x - pVertex1->position.x, 
					pVertex2->tu - pVertex1->tu, 
					pVertex2->tv - pVertex1->tv);
				
				D3DXVECTOR3 Side2 = D3DXVECTOR3(pVertex3->position.x - pVertex1->position.x, 
					pVertex3->tu - pVertex1->tu, 
					pVertex3->tv - pVertex1->tv);
				
				//Get the cross product to find the X component of the U and V vectors
				D3DXVECTOR3 CrossProduct;
				D3DXVec3Cross(&CrossProduct, &Side1, &Side2);
				
				D3DXVECTOR3 U;
				D3DXVECTOR3 V;
				
				//Set the X components
				U.x = -CrossProduct.y / CrossProduct.x;
				V.x = -CrossProduct.z / CrossProduct.x;
				
				//repeat for y
				Side1 = D3DXVECTOR3(pVertex2->position.y - pVertex1->position.y, 
					pVertex2->tu - pVertex1->tu, 
					pVertex2->tv - pVertex1->tv);
				
				Side2 = D3DXVECTOR3(pVertex3->position.y - pVertex1->position.y, 
					pVertex3->tu - pVertex1->tu, 
					pVertex3->tv - pVertex1->tv);
				
				//Get the cross product to find the Y component of the U and V vectors
				D3DXVec3Cross(&CrossProduct, &Side1, &Side2);
				
				//Set the Y components
				U.y = -CrossProduct.y / CrossProduct.x;
				V.y = -CrossProduct.z / CrossProduct.x;
				
				
				//repeat for z
				Side1 = D3DXVECTOR3(pVertex2->position.z - pVertex1->position.z, 
					pVertex2->tu - pVertex1->tu, 
					pVertex2->tv - pVertex1->tv);
				
				Side2 = D3DXVECTOR3(pVertex3->position.z - pVertex1->position.z, 
					pVertex3->tu - pVertex1->tu, 
					pVertex3->tv - pVertex1->tv);
				
				D3DXVec3Cross(&CrossProduct, &Side1, &Side2);
				
				U.z = -CrossProduct.y / CrossProduct.x;
				V.z = -CrossProduct.z / CrossProduct.x;
				
				//Normalize the U and V vectors		
				D3DXVec3Normalize(&U, &U);
				D3DXVec3Normalize(&V, &V);
				
				//Now, compute the UxV vector
				D3DXVECTOR3 UxV;
				D3DXVec3Cross(&UxV, &U, &V);
				
				//Make sure UxT is in the same direction as the normal
				if (D3DXVec3Dot(&UxV, &D3DXVECTOR3(pVertex1->normal.x, pVertex1->normal.y, pVertex1->normal.z)) < 0.0f)
					UxV = -UxV;
				
				D3DXVec3Normalize( &pVertex1->normal, &pVertex1->normal );
				D3DXVec3Normalize( &pVertex2->normal, &pVertex2->normal );
				D3DXVec3Normalize( &pVertex3->normal, &pVertex3->normal );
				
				//Now, set each vertex component to the vectors
				pVertex1->tangent.x = pVertex2->tangent.x = pVertex3->tangent.x = UxV.x;
				pVertex1->tangent.y = pVertex2->tangent.y = pVertex3->tangent.y = UxV.y;
				pVertex1->tangent.z = pVertex2->tangent.z = pVertex3->tangent.z = UxV.z;
			}
			SAFE_DELETE_ARRAY( pObj->m_pVB );

			pObj->m_pVB = new SKINVERTEX_BUMP[ pObj->m_nMaxVB ];
			memcpy( pObj->m_pVB, pVB_BUMP, sizeof(SKINVERTEX_BUMP) * pObj->m_nMaxVB );

			pObj->m_nVertexSize = sizeof(SKINVERTEX_BUMP);		
			//SendVertexBuffer( pObj, *ppd3d_VB );


			SAFE_DELETE_ARRAY( pVB_BUMP );

		}
	}
	else
	if( nType == GMT_NORMAL )
	{
		NORMALVERTEX_BUMP*  pVB_BUMP;

		{
			NORMALVERTEX *pVB;
			nMax = pObj->m_nMaxFaceList;
			pVB	= (NORMALVERTEX*)(pObj->m_pVB);
			pIB	= pObj->m_pIB;

			pVB_BUMP = new NORMALVERTEX_BUMP[ pObj->m_nMaxVB ];

			for( j = 0; j < nMax; j++ )
			{
				int nIndex = *pIB++;
				pVB_BUMP[nIndex].position = pVB[ nIndex ].position;
				pVB_BUMP[nIndex].normal = pVB[ nIndex ].normal;
				pVB_BUMP[nIndex].tu = pVB[ nIndex ].tu;
				pVB_BUMP[nIndex].tv = pVB[ nIndex ].tv;
				NORMALVERTEX_BUMP *pVertex1 = &pVB_BUMP[nIndex];
				nIndex = *pIB++;
				pVB_BUMP[nIndex].position = pVB[ nIndex ].position;
				pVB_BUMP[nIndex].normal = pVB[ nIndex ].normal;
				pVB_BUMP[nIndex].tu = pVB[ nIndex ].tu;
				pVB_BUMP[nIndex].tv = pVB[ nIndex ].tv;
				NORMALVERTEX_BUMP *pVertex2 = &pVB_BUMP[nIndex];
				nIndex = *pIB++;
				pVB_BUMP[nIndex].position = pVB[ nIndex ].position;
				pVB_BUMP[nIndex].normal = pVB[ nIndex ].normal;
				pVB_BUMP[nIndex].tu = pVB[ nIndex ].tu;
				pVB_BUMP[nIndex].tv = pVB[ nIndex ].tv;
				NORMALVERTEX_BUMP *pVertex3 = &pVB_BUMP[nIndex];

				//Compute the U and V in terms of X gradients
				D3DXVECTOR3 Side1 = D3DXVECTOR3(pVertex2->position.x - pVertex1->position.x, 
					pVertex2->tu - pVertex1->tu, 
					pVertex2->tv - pVertex1->tv);
				
				D3DXVECTOR3 Side2 = D3DXVECTOR3(pVertex3->position.x - pVertex1->position.x, 
					pVertex3->tu - pVertex1->tu, 
					pVertex3->tv - pVertex1->tv);
				
				//Get the cross product to find the X component of the U and V vectors
				D3DXVECTOR3 CrossProduct;
				D3DXVec3Cross(&CrossProduct, &Side1, &Side2);
				
				D3DXVECTOR3 U;
				D3DXVECTOR3 V;
				
				//Set the X components
				U.x = -CrossProduct.y / CrossProduct.x;
				V.x = -CrossProduct.z / CrossProduct.x;
				
				//repeat for y
				Side1 = D3DXVECTOR3(pVertex2->position.y - pVertex1->position.y, 
					pVertex2->tu - pVertex1->tu, 
					pVertex2->tv - pVertex1->tv);
				
				Side2 = D3DXVECTOR3(pVertex3->position.y - pVertex1->position.y, 
					pVertex3->tu - pVertex1->tu, 
					pVertex3->tv - pVertex1->tv);
				
				//Get the cross product to find the Y component of the U and V vectors
				D3DXVec3Cross(&CrossProduct, &Side1, &Side2);
				
				//Set the Y components
				U.y = -CrossProduct.y / CrossProduct.x;
				V.y = -CrossProduct.z / CrossProduct.x;
				
				
				//repeat for z
				Side1 = D3DXVECTOR3(pVertex2->position.z - pVertex1->position.z, 
					pVertex2->tu - pVertex1->tu, 
					pVertex2->tv - pVertex1->tv);
				
				Side2 = D3DXVECTOR3(pVertex3->position.z - pVertex1->position.z, 
					pVertex3->tu - pVertex1->tu, 
					pVertex3->tv - pVertex1->tv);
				
				D3DXVec3Cross(&CrossProduct, &Side1, &Side2);
				
				U.z = -CrossProduct.y / CrossProduct.x;
				V.z = -CrossProduct.z / CrossProduct.x;
				
				//Normalize the U and V vectors		
				D3DXVec3Normalize(&U, &U);
				D3DXVec3Normalize(&V, &V);
				
				//Now, compute the UxV vector
				D3DXVECTOR3 UxV;
				D3DXVec3Cross(&UxV, &U, &V);
				
				//Make sure UxT is in the same direction as the normal
				if (D3DXVec3Dot(&UxV, &D3DXVECTOR3(pVertex1->normal.x, pVertex1->normal.y, pVertex1->normal.z)) < 0.0f)
					UxV = -UxV;
				
				D3DXVec3Normalize( &pVertex1->normal, &pVertex1->normal );
				D3DXVec3Normalize( &pVertex2->normal, &pVertex2->normal );
				D3DXVec3Normalize( &pVertex3->normal, &pVertex3->normal );
				
				//Now, set each vertex component to the vectors
				pVertex1->tangent.x = pVertex2->tangent.x = pVertex3->tangent.x = UxV.x;
				pVertex1->tangent.y = pVertex2->tangent.y = pVertex3->tangent.y = UxV.y;
				pVertex1->tangent.z = pVertex2->tangent.z = pVertex3->tangent.z = UxV.z;
			}
			SAFE_DELETE_ARRAY( pObj->m_pVB );

			pObj->m_pVB = new NORMALVERTEX_BUMP[ pObj->m_nMaxVB ];
			memcpy( pObj->m_pVB, pVB_BUMP, sizeof(NORMALVERTEX_BUMP) * pObj->m_nMaxVB );

			pObj->m_nVertexSize = sizeof(NORMALVERTEX_BUMP);		
			//SendVertexBuffer( pObj, pObj->m_pd3d_VB );


			SAFE_DELETE_ARRAY( pVB_BUMP );
		}
	}
			
	pObj->m_VBType = VBT_BUMP;

	return S_OK;
}

LPDIRECT3DTEXTURE9 CObject3D::CreateNormalMap( int nType, LPDIRECT3DDEVICE9 m_pd3dDevice, LPDIRECT3DTEXTURE9* pTexture, LPCTSTR strFileName, LPCTSTR szPath )
{
	HRESULT hr;
	
#ifdef __YENV_WITHOUT_BUMP
	D3DVERTEXELEMENT9 decl1[] =
	{
		{0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	0},
		{0, 24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	
	D3DVERTEXELEMENT9 decl2[] =
	{
		{ 0,   0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION     , 0}, 
		{ 0,  12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BLENDWEIGHT  , 0}, 
		{ 0,  20, D3DDECLTYPE_SHORT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BLENDINDICES , 0}, 
		{ 0,  24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL       , 0}, 
		{ 0,  36, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD     , 0}, 
		
		D3DDECL_END()
	};
#else //__YENV_WITHOUT_BUMP
	D3DVERTEXELEMENT9 decl1[] =
	{
		{0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	0},
		{0, 24, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,	0},
		{0, 36, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	D3DVERTEXELEMENT9 decl2[] =
	{
		{ 0,   0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_POSITION     , 0}, 
		{ 0,  12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BLENDWEIGHT  , 0}, 
		{ 0,  20, D3DDECLTYPE_SHORT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_BLENDINDICES , 0}, 
		{ 0,  24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_NORMAL       , 0}, 
		{ 0,  36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT		, 0},
		{ 0,  48, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT,  D3DDECLUSAGE_TEXCOORD     , 0}, 
		
		D3DDECL_END()
	};
#endif //__YENV_WITHOUT_BUMP	
	
	if( !m_pNormalDecl )
	{
		if( nType == GMT_NORMAL )
		{
			if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration(
				decl1, &m_pNormalDecl )))
				return NULL;
		}
		else
		if( nType == GMT_SKIN )
		{
			if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration(
				decl2, &m_pNormalDecl )))
				return NULL;
		}
	}

#ifndef __YENV_WITHOUT_BUMP
	{
		LoadTextureFromRes( m_pd3dDevice, MakePath( DIR_MODELTEX, strFileName ), pTexture );
	}
#endif //__YENV_WITHOUT_BUMP	

#ifdef __YENV_WITHOUT_BUMP
	if( pTexture == NULL )
		return NULL;
#endif //__YENV_WITHOUT_BUMP

	return *pTexture;
}

#endif //__YENV

#endif // __WORLDSERVER


