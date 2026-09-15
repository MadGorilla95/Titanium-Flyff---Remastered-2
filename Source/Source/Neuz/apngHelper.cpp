#include "stdafx.h"
#include <apngHelper.h>
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>

#ifdef __AEGON_APNG
void info_fn(png_structp png_ptr, png_infop info_ptr)
{
    png_set_expand(png_ptr);
    png_set_strip_16(png_ptr);
    png_set_gray_to_rgb(png_ptr);
    png_set_add_alpha(png_ptr, 0xff, PNG_FILLER_AFTER);
    (void)png_set_interlace_handling(png_ptr);
    png_read_update_info(png_ptr, info_ptr);
}

void row_fn(png_structp png_ptr, png_bytep new_row, png_uint_32 row_num, int pass)
{
    APNGFrame* frame = (APNGFrame*)png_get_progressive_ptr(png_ptr);
    png_progressive_combine_row(png_ptr, frame->rows[row_num], new_row);
}

void compose_frame(unsigned char** rows_dst, unsigned char** rows_src, unsigned char bop, unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
    unsigned int  i, j;
    int u, v, al;

    for (j = 0; j < h; j++)
    {
        unsigned char* sp = rows_src[j];
        unsigned char* dp = rows_dst[j + y] + x * 4;

        if (bop == 0)
            memcpy(dp, sp, w * 4);
        else
            for (i = 0; i < w; i++, sp += 4, dp += 4)
            {
                if (sp[3] == 255)
                    memcpy(dp, sp, 4);
                else
                    if (sp[3] != 0)
                    {
                        if (dp[3] != 0)
                        {
                            u = sp[3] * 255;
                            v = (255 - sp[3]) * dp[3];
                            al = u + v;
                            dp[0] = (sp[0] * u + dp[0] * v) / al;
                            dp[1] = (sp[1] * u + dp[1] * v) / al;
                            dp[2] = (sp[2] * u + dp[2] * v) / al;
                            dp[3] = al / 255;
                        }
                        else
                            memcpy(dp, sp, 4);
                    }
            }
    }
}

unsigned int read_chunk(FILE* f, CHUNK* pChunk)
{
    unsigned char len[4];
    if (fread(&len, 4, 1, f) == 1)
    {
        pChunk->size = png_get_uint_32(len) + 12;
        pChunk->p = new unsigned char[pChunk->size];
        memcpy(pChunk->p, len, 4);
        if (fread(pChunk->p + 4, pChunk->size - 4, 1, f) == 1)
            return *(unsigned int*)(pChunk->p + 4);
    }
    return 0;
}

unsigned int read_chunk_buff(int* nPos, char* buff, CHUNK* pChunk, int buffSize)
{
    unsigned char len[4];
    if (*nPos + 4 < buffSize) 
    {
        memcpy(&len, &buff[*nPos], 4);
        *nPos += 4;
        {
            pChunk->size = png_get_uint_32(len) + 12;
            pChunk->p = new unsigned char[pChunk->size];
            memcpy(pChunk->p, len, 4);

            if (*nPos + pChunk->size - 4 <= buffSize) 
            {
                memcpy(pChunk->p + 4, &buff[*nPos], pChunk->size - 4);
                *nPos += pChunk->size - 4;
                return *(unsigned int*)(pChunk->p + 4);
            }
        }
    }
   
    return 0;
}

void processing_start(png_structp& png_ptr, png_infop& info_ptr, void* frame_ptr, bool hasInfo, CHUNK& chunkIHDR, std::vector<CHUNK>& chunksInfo)
{
    unsigned char header[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info_ptr = png_create_info_struct(png_ptr);
    if (!png_ptr || !info_ptr)
        return;

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        return;
    }

    png_set_crc_action(png_ptr, PNG_CRC_QUIET_USE, PNG_CRC_QUIET_USE);
    png_set_progressive_read_fn(png_ptr, frame_ptr, info_fn, row_fn, NULL);

    png_process_data(png_ptr, info_ptr, header, 8);
    png_process_data(png_ptr, info_ptr, chunkIHDR.p, chunkIHDR.size);

    if (hasInfo)
        for (unsigned int i = 0; i < chunksInfo.size(); i++)
            png_process_data(png_ptr, info_ptr, chunksInfo[i].p, chunksInfo[i].size);
}

void processing_data(png_structp png_ptr, png_infop info_ptr, unsigned char* p, unsigned int size)
{
    if (!png_ptr || !info_ptr)
        return;

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        return;
    }

    png_process_data(png_ptr, info_ptr, p, size);
}

int processing_finish(png_structp png_ptr, png_infop info_ptr)
{
    unsigned char footer[12] = { 0, 0, 0, 0, 73, 69, 78, 68, 174, 66, 96, 130 };

    if (!png_ptr || !info_ptr)
        return 1;

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        return 1;
    }

    png_process_data(png_ptr, info_ptr, footer, 12);
    png_destroy_read_struct(&png_ptr, &info_ptr, 0);

    return 0;
}

int load_apng(LPCSTR szIn, std::vector<APNGFrame>& frames)
{
    CResFile file;
    unsigned int id, i, j, w, h, w0, h0, x0, y0;
    unsigned int delay_num, delay_den, dop, bop, rowbytes, imagesize;
    unsigned char sig[8];
    png_structp png_ptr;
    png_infop info_ptr;
    CHUNK chunk;
    CHUNK chunkIHDR;
    std::vector<CHUNK> chunksInfo;
    bool isAnimated = false;
    bool skipFirst = false;
    bool hasInfo = false;
    APNGFrame frameRaw = { 0 };
    APNGFrame frameCur = { 0 };
    APNGFrame frameNext = { 0 };
    int res = -1;
    int nPos = 0;

    if (file.Open(szIn, "rb") != 0) 
    {
        int nSrcDataSize = file.GetLength();
        char* pSrcData = new char[nSrcDataSize];


        if (file.Read(pSrcData, nSrcDataSize) >= 1)
        {
            file.Close();

            if (nPos + 8 <= nSrcDataSize) 
            {
                memcpy(sig, &pSrcData[nPos], 8);
                nPos += 8;
                if (png_sig_cmp(sig, 0, 8) == 0)
                {
                    id = read_chunk_buff(&nPos, pSrcData, &chunkIHDR, nSrcDataSize);

                    if (id == id_IHDR && chunkIHDR.size == 25)
                    {
                        w0 = w = png_get_uint_32(chunkIHDR.p + 8);
                        h0 = h = png_get_uint_32(chunkIHDR.p + 12);
                        x0 = 0;
                        y0 = 0;
                        delay_num = 1;
                        delay_den = 10;
                        dop = 0;
                        bop = 0;
                        rowbytes = w * 4;
                        imagesize = h * rowbytes;

                        frameRaw.p = new unsigned char[imagesize];
                        frameRaw.rows = new png_bytep[h * sizeof(png_bytep)];
                        for (j = 0; j < h; j++)
                            frameRaw.rows[j] = frameRaw.p + j * rowbytes;

                        frameCur.w = w;
                        frameCur.h = h;
                        frameCur.p = new unsigned char[imagesize];
                        frameCur.rows = new png_bytep[h * sizeof(png_bytep)];
                        for (j = 0; j < h; j++)
                            frameCur.rows[j] = frameCur.p + j * rowbytes;

                        processing_start(png_ptr, info_ptr, (void*)&frameRaw, hasInfo, chunkIHDR, chunksInfo);

                        while (nPos < nSrcDataSize)
                        {
                            id = read_chunk_buff(&nPos, pSrcData, &chunk, nSrcDataSize);

                            if (id == id_acTL && !hasInfo && !isAnimated)
                            {
                                isAnimated = true;
                                skipFirst = true;
                            }
                            else
                                if (id == id_fcTL && (!hasInfo || isAnimated))
                                {
                                    if (hasInfo)
                                    {
                                        if (!processing_finish(png_ptr, info_ptr))
                                        {
                                            frameNext.p = new unsigned char[imagesize];
                                            frameNext.rows = new png_bytep[h * sizeof(png_bytep)];
                                            for (j = 0; j < h; j++)
                                                frameNext.rows[j] = frameNext.p + j * rowbytes;

                                            if (dop == 2)
                                                memcpy(frameNext.p, frameCur.p, imagesize);

                                            compose_frame(frameCur.rows, frameRaw.rows, bop, x0, y0, w0, h0);
                                            frameCur.delay_num = delay_num;
                                            frameCur.delay_den = delay_den;

                                            frames.push_back(frameCur);

                                            if (dop != 2)
                                            {
                                                memcpy(frameNext.p, frameCur.p, imagesize);
                                                if (dop == 1)
                                                    for (j = 0; j < h0; j++)
                                                        memset(frameNext.rows[y0 + j] + x0 * 4, 0, w0 * 4);
                                            }
                                            frameCur.p = frameNext.p;
                                            frameCur.rows = frameNext.rows;
                                        }
                                        else
                                        {
                                            delete[] frameCur.rows;
                                            delete[] frameCur.p;
                                            delete[] chunk.p;
                                            break;
                                        }
                                    }

                                    // At this point the old frame is done. Let's start a new one.
                                    w0 = png_get_uint_32(chunk.p + 12);
                                    h0 = png_get_uint_32(chunk.p + 16);
                                    x0 = png_get_uint_32(chunk.p + 20);
                                    y0 = png_get_uint_32(chunk.p + 24);
                                    delay_num = png_get_uint_16(chunk.p + 28);
                                    delay_den = png_get_uint_16(chunk.p + 30);
                                    dop = chunk.p[32];
                                    bop = chunk.p[33];

                                    if (hasInfo)
                                    {
                                        memcpy(chunkIHDR.p + 8, chunk.p + 12, 8);
                                        processing_start(png_ptr, info_ptr, (void*)&frameRaw, hasInfo, chunkIHDR, chunksInfo);
                                    }
                                    else
                                        skipFirst = false;

                                    if (frames.size() == (skipFirst ? 1 : 0))
                                    {
                                        bop = 0;
                                        if (dop == 2)
                                            dop = 1;
                                    }
                                }
                                else
                                    if (id == id_IDAT)
                                    {
                                        hasInfo = true;
                                        processing_data(png_ptr, info_ptr, chunk.p, chunk.size);
                                    }
                                    else
                                        if (id == id_fdAT && isAnimated)
                                        {
                                            png_save_uint_32(chunk.p + 4, chunk.size - 16);
                                            memcpy(chunk.p + 8, "IDAT", 4);
                                            processing_data(png_ptr, info_ptr, chunk.p + 4, chunk.size - 4);
                                        }
                                        else
                                            if (id == id_IEND)
                                            {
                                                if (hasInfo && !processing_finish(png_ptr, info_ptr))
                                                {
                                                    compose_frame(frameCur.rows, frameRaw.rows, bop, x0, y0, w0, h0);
                                                    frameCur.delay_num = delay_num;
                                                    frameCur.delay_den = delay_den;
                                                    frames.push_back(frameCur);
                                                }
                                                else
                                                {
                                                    delete[] frameCur.rows;
                                                    delete[] frameCur.p;
                                                }
                                                delete[] chunk.p;
                                                break;
                                            }
                                            else
                                                if (notabc(chunk.p[4]) || notabc(chunk.p[5]) || notabc(chunk.p[6]) || notabc(chunk.p[7]))
                                                {
                                                    delete[] chunk.p;
                                                    break;
                                                }
                                                else
                                                    if (!hasInfo)
                                                    {
                                                        processing_data(png_ptr, info_ptr, chunk.p, chunk.size);
                                                        chunksInfo.push_back(chunk);
                                                        continue;
                                                    }
                            delete[] chunk.p;
                        }
                        delete[] frameRaw.rows;
                        delete[] frameRaw.p;

                        if (!frames.empty())
                            res = (skipFirst) ? 0 : 1;
                    }
                }
            }

            //fclose(f);

            for (i = 0; i < chunksInfo.size(); i++)
                delete[] chunksInfo[i].p;

            chunksInfo.clear();
            delete[] chunkIHDR.p;
        }
    }

    return res;
}
int load_apng_with_tmpfile(LPCSTR szIn, std::vector<APNGFrame>& frames)
{
    CResFile file;
    FILE* f;
    unsigned int id, i, j, w, h, w0, h0, x0, y0;
    unsigned int delay_num, delay_den, dop, bop, rowbytes, imagesize;
    unsigned char sig[8];
    png_structp png_ptr;
    png_infop info_ptr;
    CHUNK chunk;
    CHUNK chunkIHDR;
    std::vector<CHUNK> chunksInfo;
    bool isAnimated = false;
    bool skipFirst = false;
    bool hasInfo = false;
    APNGFrame frameRaw = { 0 };
    APNGFrame frameCur = { 0 };
    APNGFrame frameNext = { 0 };
    int res = -1;

    f = tmpfile();

    if (file.Open(szIn, "rb") != 0)
    {
        int nSrcDataSize = file.GetLength();
        char* pSrcData = new char[nSrcDataSize];


        if (file.Read(pSrcData, nSrcDataSize) >= 1)
        {
            file.Close();
            fwrite(pSrcData, sizeof(char), nSrcDataSize, f);
            rewind(f);

            if (fread(sig, 1, 8, f) == 8 && png_sig_cmp(sig, 0, 8) == 0)
            {
                id = read_chunk(f, &chunkIHDR);

                if (id == id_IHDR && chunkIHDR.size == 25)
                {
                    w0 = w = png_get_uint_32(chunkIHDR.p + 8);
                    h0 = h = png_get_uint_32(chunkIHDR.p + 12);
                    x0 = 0;
                    y0 = 0;
                    delay_num = 1;
                    delay_den = 10;
                    dop = 0;
                    bop = 0;
                    rowbytes = w * 4;
                    imagesize = h * rowbytes;

                    frameRaw.p = new unsigned char[imagesize];
                    frameRaw.rows = new png_bytep[h * sizeof(png_bytep)];
                    for (j = 0; j < h; j++)
                        frameRaw.rows[j] = frameRaw.p + j * rowbytes;

                    frameCur.w = w;
                    frameCur.h = h;
                    frameCur.p = new unsigned char[imagesize];
                    frameCur.rows = new png_bytep[h * sizeof(png_bytep)];
                    for (j = 0; j < h; j++)
                        frameCur.rows[j] = frameCur.p + j * rowbytes;

                    processing_start(png_ptr, info_ptr, (void*)&frameRaw, hasInfo, chunkIHDR, chunksInfo);

                    while (!feof(f))
                    {
                        id = read_chunk(f, &chunk);

                        if (id == id_acTL && !hasInfo && !isAnimated)
                        {
                            isAnimated = true;
                            skipFirst = true;
                        }
                        else
                            if (id == id_fcTL && (!hasInfo || isAnimated))
                            {
                                if (hasInfo)
                                {
                                    if (!processing_finish(png_ptr, info_ptr))
                                    {
                                        frameNext.p = new unsigned char[imagesize];
                                        frameNext.rows = new png_bytep[h * sizeof(png_bytep)];
                                        for (j = 0; j < h; j++)
                                            frameNext.rows[j] = frameNext.p + j * rowbytes;

                                        if (dop == 2)
                                            memcpy(frameNext.p, frameCur.p, imagesize);

                                        compose_frame(frameCur.rows, frameRaw.rows, bop, x0, y0, w0, h0);
                                        frameCur.delay_num = delay_num;
                                        frameCur.delay_den = delay_den;

                                        frames.push_back(frameCur);

                                        if (dop != 2)
                                        {
                                            memcpy(frameNext.p, frameCur.p, imagesize);
                                            if (dop == 1)
                                                for (j = 0; j < h0; j++)
                                                    memset(frameNext.rows[y0 + j] + x0 * 4, 0, w0 * 4);
                                        }
                                        frameCur.p = frameNext.p;
                                        frameCur.rows = frameNext.rows;
                                    }
                                    else
                                    {
                                        delete[] frameCur.rows;
                                        delete[] frameCur.p;
                                        delete[] chunk.p;
                                        break;
                                    }
                                }

                                // At this point the old frame is done. Let's start a new one.
                                w0 = png_get_uint_32(chunk.p + 12);
                                h0 = png_get_uint_32(chunk.p + 16);
                                x0 = png_get_uint_32(chunk.p + 20);
                                y0 = png_get_uint_32(chunk.p + 24);
                                delay_num = png_get_uint_16(chunk.p + 28);
                                delay_den = png_get_uint_16(chunk.p + 30);
                                dop = chunk.p[32];
                                bop = chunk.p[33];

                                if (hasInfo)
                                {
                                    memcpy(chunkIHDR.p + 8, chunk.p + 12, 8);
                                    processing_start(png_ptr, info_ptr, (void*)&frameRaw, hasInfo, chunkIHDR, chunksInfo);
                                }
                                else
                                    skipFirst = false;

                                if (frames.size() == (skipFirst ? 1 : 0))
                                {
                                    bop = 0;
                                    if (dop == 2)
                                        dop = 1;
                                }
                            }
                            else
                                if (id == id_IDAT)
                                {
                                    hasInfo = true;
                                    processing_data(png_ptr, info_ptr, chunk.p, chunk.size);
                                }
                                else
                                    if (id == id_fdAT && isAnimated)
                                    {
                                        png_save_uint_32(chunk.p + 4, chunk.size - 16);
                                        memcpy(chunk.p + 8, "IDAT", 4);
                                        processing_data(png_ptr, info_ptr, chunk.p + 4, chunk.size - 4);
                                    }
                                    else
                                        if (id == id_IEND)
                                        {
                                            if (hasInfo && !processing_finish(png_ptr, info_ptr))
                                            {
                                                compose_frame(frameCur.rows, frameRaw.rows, bop, x0, y0, w0, h0);
                                                frameCur.delay_num = delay_num;
                                                frameCur.delay_den = delay_den;
                                                frames.push_back(frameCur);
                                            }
                                            else
                                            {
                                                delete[] frameCur.rows;
                                                delete[] frameCur.p;
                                            }
                                            delete[] chunk.p;
                                            break;
                                        }
                                        else
                                            if (notabc(chunk.p[4]) || notabc(chunk.p[5]) || notabc(chunk.p[6]) || notabc(chunk.p[7]))
                                            {
                                                delete[] chunk.p;
                                                break;
                                            }
                                            else
                                                if (!hasInfo)
                                                {
                                                    processing_data(png_ptr, info_ptr, chunk.p, chunk.size);
                                                    chunksInfo.push_back(chunk);
                                                    continue;
                                                }
                        delete[] chunk.p;
                    }
                    delete[] frameRaw.rows;
                    delete[] frameRaw.p;

                    if (!frames.empty())
                        res = (skipFirst) ? 0 : 1;
                }
            }
            fclose(f);

            for (i = 0; i < chunksInfo.size(); i++)
                delete[] chunksInfo[i].p;

            chunksInfo.clear();
            delete[] chunkIHDR.p;
        }
    }

    return res;
}
/*void save_png(char* szOut, APNGFrame* frame)
{
    FILE* f;
    png_structp  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop    info_ptr = png_create_info_struct(png_ptr);

    if (!png_ptr || !info_ptr)
        return;

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        return;
    }

    if ((f = fopen(szOut, "wb")) != 0)
    {
        png_init_io(png_ptr, f);
        png_set_compression_level(png_ptr, 9);
        png_set_IHDR(png_ptr, info_ptr, frame->w, frame->h, 8, 6, 0, 0, 0);
        png_write_info(png_ptr, info_ptr);
        png_write_image(png_ptr, frame->rows);
        png_write_end(png_ptr, info_ptr);
        fclose(f);
    }
    png_destroy_write_struct(&png_ptr, &info_ptr);
    printf("  [libpng");
#ifdef PNG_APNG_SUPPORTED
    printf("+apng");
#endif
    printf(" %s]:  ", PNG_LIBPNG_VER_STRING);
    printf("%s : %dx%d\n", szOut, frame->w, frame->h);
}*/


void my_png_write_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
    /* with libpng15 next line causes pointer deference error; use libpng12 */
    struct mem_encode* p = (struct mem_encode*)png_get_io_ptr(png_ptr); /* was png_ptr->io_ptr */
    size_t nsize = p->size + length;

    /* allocate or grow buffer */
    if (p->buffer)
        p->buffer = (char*)realloc(p->buffer, nsize);
    else
        p->buffer = (char*)malloc(nsize);

    if (!p->buffer)
        png_error(png_ptr, "Write Error");

    /* copy new bytes to end of buffer */
    memcpy(p->buffer + p->size, data, length);
    p->size += length;
}
/* This is optional but included to show how png_set_write_fn() is called */
void my_png_flush(png_structp png_ptr)
{
    return;
}

HRESULT LoadAnimatedTextureFromFrame(LPDIRECT3DDEVICE9 pd3dDevice, APNGFrame* frame, CAnimatedTexture* animatedTexture)
{
    //FILE* f;
    png_structp  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop    info_ptr = png_create_info_struct(png_ptr);

    if (!png_ptr || !info_ptr)
        return FALSE;

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, 0);
        return FALSE;
    }

    struct mem_encode state;

    /* initialise - put this before png_write_png() call */
    state.buffer = NULL;
    state.size = 0;

    /* if my_png_flush() is not needed, change the arg to NULL */
    png_set_write_fn(png_ptr, &state, my_png_write_data, my_png_flush);

    png_set_compression_level(png_ptr, 9);
    png_set_IHDR(png_ptr, info_ptr, frame->w, frame->h, 8, 6, 0, 0, 0);
    png_write_info(png_ptr, info_ptr);
    png_write_image(png_ptr, frame->rows);
    png_write_end(png_ptr, info_ptr);


    png_destroy_write_struct(&png_ptr, &info_ptr);
    /* now state.buffer contains the PNG image of size s.size bytes */


    CTexture* pTexture = new CTexture;
    
    D3DXIMAGE_INFO imageInfo;
    HRESULT hr = CreateTextureFromBuffer(pd3dDevice, &state, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR, D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR, 0, &imageInfo, nullptr, &pTexture->m_pTexture);
    if (FAILED(hr)) 
    {
        safe_delete(pTexture);
    }
    else 
    {
        SIZE size;
        size.cx = imageInfo.Width;
        size.cy = imageInfo.Height;
        pTexture->m_size = size;

        pTexture->m_fuLT = 0.0f;
        pTexture->m_fvLT = 0.0f;

        pTexture->m_fuRT = (FLOAT)size.cx / pTexture->m_size.cx;
        pTexture->m_fvRT = 0.0f;

        pTexture->m_fuLB = 0.0f;
        pTexture->m_fvLB = (FLOAT)size.cy / pTexture->m_size.cy;

        pTexture->m_fuRB = (FLOAT)size.cx / pTexture->m_size.cx;
        pTexture->m_fvRB = (FLOAT)size.cy / pTexture->m_size.cy;

        pTexture->m_ptCenter.x = 0;
        pTexture->m_ptCenter.y = 0;

        pTexture->m_sizePitch = pTexture->m_size;
        pTexture->m_size = size;

        pTexture->m_bAutoFree = TRUE;

        animatedTexture->m_vecImages.push_back(pTexture);
    }
    

    /* cleanup */
    if (state.buffer)
        free(state.buffer);

    return hr;
}

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
    PALETTEENTRY* pPalette, LPDIRECT3DTEXTURE9* ppTexture)
{
    HRESULT hr;
    D3DXIMAGE_INFO imageInfo;
    hr = D3DXGetImageInfoFromFileInMemory(state->buffer, state->size, &imageInfo);

    if (hr == D3D_OK && imageInfo.Height > 0 && imageInfo.Width > 0)
    {
        hr = D3DXCreateTextureFromFileInMemoryEx(pDevice,
            state->buffer, state->size, imageInfo.Width, imageInfo.Height, imageInfo.MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture);
    }
    else
    {
        hr = D3DXCreateTextureFromFileInMemoryEx(pDevice,
            state->buffer, state->size, Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pSrcInfo, pPalette, ppTexture);
    }
    return hr;
}

#if _WIN64
#include <DxErr.h>
#pragma comment(lib, "dxerr.lib")
#endif
CAnimatedTexture* CreateAnimatedTextureFrom_apng(LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFile)
{
    unsigned int i;
    int res;
    std::vector<APNGFrame> frames;
    CAnimatedTexture* animatedTexture = NULL;
    HRESULT hr;

    BOOL bLoadedFrames = FALSE;

    res = load_apng(szFile, frames);
    if (res >= 0) 
    {
        animatedTexture = new CAnimatedTexture;

       
        for (i = 0; i < frames.size(); i++)
        {
            hr = LoadAnimatedTextureFromFrame(pd3dDevice, &frames[i], animatedTexture);
            if (FAILED(hr)) 
            {
#if _WIN64
                LPCTSTR szErr = Error("Function '%s' failed in line %d - %s %s", __FUNCTION__, __LINE__, szFile, DXGetErrorString(hr));
#else
                LPCTSTR szErr = Error("Function '%s' failed in line %d - %s %s", __FUNCTION__, __LINE__, szFile, DXGetErrorString9(hr));
#endif
                ADDERRORMSG(szErr);
            }
            else 
            {
                bLoadedFrames = TRUE;
            }
            delete[] frames[i].rows;
            delete[] frames[i].p;
        }       
    }
    frames.clear();

    if (!bLoadedFrames) 
    {
        safe_delete(animatedTexture);
        animatedTexture = NULL;
    }

    return animatedTexture;
}



////////////////////////////////////
//////Animated Texture Class///////
///////////////////////////////////

CAnimatedTexture::CAnimatedTexture() 
{
    m_nCurrentImage = 0;
    m_dwLastRender = 0;
    m_dwTimeBetweenFrames = 50;
    m_bRunAnimation = TRUE;
    m_bLoopAnimation = TRUE;
}
CAnimatedTexture::~CAnimatedTexture()
{
    
}
BOOL CAnimatedTexture::DeleteDeviceObjects()
{
    BOOL bOut = TRUE;
    for (int i = 0; i < m_vecImages.size(); i++)
    {
        if (!m_vecImages[i]->DeleteDeviceObjects())
            bOut = FALSE;
        
    }
    return bOut;
}
void CAnimatedTexture::UpdateCurrentImage(int* nCurrentImage, DWORD* dwLastRender)
{
    if (!nCurrentImage)
        nCurrentImage = &m_nCurrentImage;

    if (!dwLastRender)
        dwLastRender = &m_dwLastRender;


    if (!m_bRunAnimation)
        return;

    if (*dwLastRender == 0)
    {
        *dwLastRender = GetTickCount();
    }

    if(GetTickCount() > *dwLastRender + m_dwTimeBetweenFrames)
    {
        *nCurrentImage = *nCurrentImage + 1;
        if (*nCurrentImage >= m_vecImages.size())
        {
            if (m_bLoopAnimation) 
            {
                *nCurrentImage = 0;
            }
            else 
            {
                *nCurrentImage = m_vecImages.size() - 1;
            }
        }
        *dwLastRender = GetTickCount();
    }   
}
void CAnimatedTexture::Render(C2DRender* p2DRender, CPoint pt, BOOL bUpdateImage, DWORD dwBlendFactorAlhpa)
{
    if(bUpdateImage)
        UpdateCurrentImage();
    p2DRender->RenderTexture(pt, m_vecImages[m_nCurrentImage], dwBlendFactorAlhpa);
}
void CAnimatedTexture::Render(C2DRender* p2DRender, CPoint pt, CPoint pt2, DWORD dwBlendFactorAlhpa, FLOAT fscalX, FLOAT fscalY)
{
    UpdateCurrentImage();
    p2DRender->RenderTextureEx(pt, pt2, m_vecImages[m_nCurrentImage], dwBlendFactorAlhpa, fscalX, fscalY);
}
void CAnimatedTexture::SetTimeBetweenFrames(DWORD dwMilliTime) 
{
    m_dwTimeBetweenFrames = dwMilliTime;
}
void CAnimatedTexture::SetAnimate(BOOL bRunAnimation)
{
    m_bRunAnimation = bRunAnimation;
}
SIZE CAnimatedTexture::GetSize()
{
    if (m_vecImages.size() > 0)
        return m_vecImages[0]->m_size;

    SIZE out;
    out.cx = 0;
    out.cy = 0;
    return out;
}
void CAnimatedTexture::SetSize(SIZE newSize) 
{
    for (int i = 0; i < m_vecImages.size(); i++) 
    {
        m_vecImages[i]->m_size = newSize;
    }
}
void CAnimatedTexture::SetLoop(BOOL bLoopAnimation) 
{
    m_bLoopAnimation = bLoopAnimation;
}
void CAnimatedTexture::Restart()
{
    m_nCurrentImage = 0;
}
BOOL CAnimatedTexture::HasEnded(int* nCurrentImage)
{
    if (!nCurrentImage)
        nCurrentImage = &m_nCurrentImage;

    if (!m_bLoopAnimation && *nCurrentImage == m_vecImages.size() - 1)
        return TRUE;

    return FALSE;
}
BOOL CAnimatedTexture::IsLastFrame(int* nCurrentImage)
{
    if (!nCurrentImage)
        nCurrentImage = &m_nCurrentImage;

    if (*nCurrentImage == m_vecImages.size() - 1)
        return TRUE;

    return FALSE;
}
BOOL CAnimatedTexture::SetCurrentImage(int nImageIndex)
{
    if (nImageIndex >= 0 && nImageIndex < m_vecImages.size()) 
    {
        m_nCurrentImage = nImageIndex;
        return TRUE;
    }
    return FALSE;
}

CTexture* CAnimatedTexture::GetFrame(int nFrame)
{
    if (m_vecImages.size() > nFrame)
    {
        return m_vecImages[nFrame];
    }
    return NULL;
}
int CAnimatedTexture::GetFrameCount()
{
    return (int)m_vecImages.size();
}
///////////////////////////////////////
//////Animated Texture Mng Class///////
//////////////////////////////////////

CAnimatedTextureMng::CAnimatedTextureMng()
{
}
CAnimatedTextureMng::~CAnimatedTextureMng()
{
    DeleteDeviceObjects();
}

BOOL CAnimatedTextureMng::DeleteDeviceObjects()
{
    if (m_mapAnimatedTexture.size() <= 0) 
        return TRUE;

    for (map<string, CAnimatedTexture*>::iterator i = m_mapAnimatedTexture.begin(); i != m_mapAnimatedTexture.end(); ++i) 
    {
        if (i->second) 
        {
            if (i->second->m_vecImages.size() > 0) 
            {
                for (int j = i->second->m_vecImages.size() - 1; j >= 0; j--)
                {
                    i->second->m_vecImages.erase(i->second->m_vecImages.begin() + j);
                }
            }
            SAFE_DELETE(i->second);
        }
       
    }

    m_mapAnimatedTexture.clear();
    return TRUE;
}
BOOL CAnimatedTextureMng::RemoveAnimatedTexture(LPCTSTR pKey)
{
    if (m_mapAnimatedTexture.empty())
        return TRUE;

    map<string, CAnimatedTexture*>::iterator mapTexItor = m_mapAnimatedTexture.find(pKey);
    if (mapTexItor != m_mapAnimatedTexture.end())
    {
        for (int i = 0; i < (*mapTexItor).second->m_vecImages.size(); i++) 
        {
            SAFE_DELETE((*mapTexItor).second->m_vecImages[i]);
        }
        SAFE_DELETE((*mapTexItor).second);
        m_mapAnimatedTexture.erase(pKey);
    }
    return TRUE;
}
CAnimatedTexture* CAnimatedTextureMng::AddAnimatedTexture(LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR pFileName)
{
    CAnimatedTexture* pAnimTexture = NULL;
    map<string, CAnimatedTexture*>::iterator mapTexItor;

    mapTexItor = m_mapAnimatedTexture.find(pFileName);
    if (mapTexItor != m_mapAnimatedTexture.end())
        return (*mapTexItor).second;

    pAnimTexture = CreateAnimatedTextureFrom_apng(pd3dDevice, pFileName);
    if (pAnimTexture)
    {
        m_mapAnimatedTexture.insert(make_pair(pFileName, pAnimTexture));
        return pAnimTexture;
    }
    safe_delete(pAnimTexture);

#ifdef __GAMEGUARD
    m_mapAnimatedTexture.insert(make_pair(pFileName, nullptr));
#endif //__GAMEGUARD

    return NULL;
}

CAnimatedTexture* CAnimatedTextureMng::GetAt(LPCTSTR pFileName)
{
    map<string, CAnimatedTexture*>::iterator mapTexItor = m_mapAnimatedTexture.find(pFileName);
    if (mapTexItor != m_mapAnimatedTexture.end())
        return (CAnimatedTexture*)(*mapTexItor).second;
    return NULL;
}

CAnimatedTexture* CAnimatedTextureMng::AddAnimatedTexture(LPCTSTR pKey, CAnimatedTexture* pAniTexture)
{
    map<string, CAnimatedTexture*>::iterator mapTexItor;
    mapTexItor = m_mapAnimatedTexture.find(pKey);
    if (mapTexItor != m_mapAnimatedTexture.end())
        return (*mapTexItor).second;
    m_mapAnimatedTexture.insert(make_pair(pKey, pAniTexture));
    return pAniTexture;
}
#endif // __AEGON_APNG