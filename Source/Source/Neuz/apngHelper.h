#pragma once
#ifndef __APNG_HELPER_H
#define __APNG_HELPER_H

#include <png.h>


#define notabc(c) ((c) < 65 || (c) > 122 || ((c) > 90 && (c) < 97))

#define id_IHDR 0x52444849
#define id_acTL 0x4C546361
#define id_fcTL 0x4C546366
#define id_IDAT 0x54414449
#define id_fdAT 0x54416466
#define id_IEND 0x444E4549
struct CHUNK { unsigned char* p; unsigned int size; };
struct APNGFrame { unsigned char* p, ** rows; unsigned int w, h, delay_num, delay_den; };
class CAnimatedTexture;

struct mem_encode
{
	char* buffer;
	size_t size;
};

unsigned int read_chunk(FILE* f, CHUNK* pChunk);
void info_fn(png_structp png_ptr, png_infop info_ptr);
void row_fn(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass);
void processing_start(png_structp& png_ptr, png_infop& info_ptr, void* frame_ptr, bool hasInfo, CHUNK& chunkIHDR, std::vector<CHUNK>& chunksInfo);
int	processing_finish(png_structp png_ptr, png_infop info_ptr);
void compose_frame(unsigned char** rows_dst, unsigned char** rows_src, unsigned char bop, unsigned int x, unsigned int y, unsigned int w, unsigned int h);
void processing_data(png_structp png_ptr, png_infop info_ptr, unsigned char* p, unsigned int size);
int load_apng(LPCSTR szIn, std::vector<APNGFrame>& frames);
void save_png(char* szOut, APNGFrame* frame);
void my_png_write_data(png_structp png_ptr, png_bytep data, png_size_t length);
void my_png_flush(png_structp png_ptr);

HRESULT LoadAnimatedTextureFromFrame(LPDIRECT3DDEVICE9 pd3dDevice, APNGFrame* frame, CAnimatedTexture* animatedTexture);
HRESULT CreateTextureFromBuffer(LPDIRECT3DDEVICE9 pDevice,
	mem_encode* state,
	UINT Width,
	UINT Height,
	UINT MipLevels,
	DWORD Usage,
	D3DFORMAT Format,
	D3DPOOL Pool,
	DWORD Filter,
	DWORD MipFilter,
	D3DCOLOR ColorKey,
	D3DXIMAGE_INFO* pSrcInfo,
	PALETTEENTRY* pPalette, LPDIRECT3DTEXTURE9* ppTexture);


CAnimatedTexture* CreateAnimatedTextureFrom_apng(LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFile);

class CAnimatedTexture
{
public:
	vector<CTexture*> m_vecImages;
	int m_nCurrentImage;
	DWORD m_dwLastRender;
	DWORD m_dwTimeBetweenFrames;
	BOOL m_bRunAnimation;
	BOOL m_bLoopAnimation;


	CAnimatedTexture();
	~CAnimatedTexture();

	BOOL DeleteDeviceObjects();

	void UpdateCurrentImage(int* nCurrentImage = NULL, DWORD* dwLastRender = NULL);

	void Render(C2DRender* p2DRender, CPoint pt, BOOL bUpdateImage = TRUE, DWORD dwBlendFactorAlhpa = 255);
	void Render(C2DRender* p2DRender, CPoint pt, CPoint pt2, DWORD dwBlendFactorAlhpa = 255, FLOAT fscalX = 1.0, FLOAT fscalY = 1.0);

	void SetTimeBetweenFrames(DWORD dwMilliTime);
	DWORD GetTimeBetweenFrames() { return m_dwTimeBetweenFrames; };
	void SetAnimate(BOOL bRunAnimation);
	SIZE GetSize();
	void SetSize(SIZE newSize);
	void SetLoop(BOOL bLoopAnimation);
	void Restart();
	BOOL HasEnded(int* nCurrentImage = NULL);
	BOOL IsLastFrame(int* nCurrentImage = NULL);
	BOOL SetCurrentImage(int nImageIndex);
	CTexture* GetFrame(int nFrame);
	int CAnimatedTexture::GetFrameCount();
};

class CAnimatedTextureMng
{
public:
	map<string, CAnimatedTexture*> m_mapAnimatedTexture;
	CAnimatedTextureMng();
	~CAnimatedTextureMng();
	BOOL RemoveAnimatedTexture(LPCTSTR pKey);
	BOOL DeleteDeviceObjects();
	CAnimatedTexture* AddAnimatedTexture(LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR pFileName);
	CAnimatedTexture* GetAt(LPCTSTR pFileName);
	CAnimatedTexture* AddAnimatedTexture(LPCTSTR pKey, CAnimatedTexture* pAniTexture);
};


#endif // __APNG_HELPER_H