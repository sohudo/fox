#include <Windows.h>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <map>
#include <vector>
#include "./OpenAL/OpenAL.h"
#include "Sndloader.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////
// OggVorbis 初始化
//////////////////////////////////////////////////////////////////////////
/*
 * Copyright (c) 2006, Creative Labs Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided
 * that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions and
 * 	     the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 * 	     and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *     * Neither the name of Creative Labs Inc. nor the names of its contributors may be used to endorse or
 * 	     promote products derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
// Ogg Vorbis

// Ogg Voribis DLL Handle
HINSTANCE g_hVorbisFileDLL = NULL;

// Functions
void InitVorbisFile();
void ShutdownVorbisFile();

size_t ov_read_func(void *ptr, size_t size, size_t nmemb, void *datasource);
int ov_seek_func(void *datasource, ogg_int64_t offset, int whence);
int ov_close_func(void *datasource);
long ov_tell_func(void *datasource);

// Function pointers
typedef int (*LPOVCLEAR)(OggVorbis_File *vf);
typedef long (*LPOVREAD)(OggVorbis_File *vf,char *buffer,int length,int bigendianp,int word,int sgned,int *bitstream);
typedef ogg_int64_t (*LPOVPCMTOTAL)(OggVorbis_File *vf,int i);
typedef vorbis_info * (*LPOVINFO)(OggVorbis_File *vf,int link);
typedef vorbis_comment * (*LPOVCOMMENT)(OggVorbis_File *vf,int link);
typedef int (*LPOVOPENCALLBACKS)(void *datasource, OggVorbis_File *vf,char *initial, long ibytes, ov_callbacks callbacks);

// Variables
LPOVCLEAR			fn_ov_clear = NULL;
LPOVREAD			fn_ov_read = NULL;
LPOVPCMTOTAL		fn_ov_pcm_total = NULL;
LPOVINFO			fn_ov_info = NULL;
LPOVCOMMENT			fn_ov_comment = NULL;
LPOVOPENCALLBACKS	fn_ov_open_callbacks = NULL;

bool g_bVorbisInit = false;

void InitVorbisFile()
{	
	if (g_bVorbisInit)
		return;

	// Try and load Vorbis DLLs (VorbisFile.dll will load ogg.dll and vorbis.dll)
	g_hVorbisFileDLL = LoadLibraryA("vorbisfile.dll");
	if (g_hVorbisFileDLL)
	{
		fn_ov_clear = (LPOVCLEAR)GetProcAddress(g_hVorbisFileDLL, "ov_clear");
		fn_ov_read = (LPOVREAD)GetProcAddress(g_hVorbisFileDLL, "ov_read");
		fn_ov_pcm_total = (LPOVPCMTOTAL)GetProcAddress(g_hVorbisFileDLL, "ov_pcm_total");
		fn_ov_info = (LPOVINFO)GetProcAddress(g_hVorbisFileDLL, "ov_info");
		fn_ov_comment = (LPOVCOMMENT)GetProcAddress(g_hVorbisFileDLL, "ov_comment");
		fn_ov_open_callbacks = (LPOVOPENCALLBACKS)GetProcAddress(g_hVorbisFileDLL, "ov_open_callbacks");

		if (fn_ov_clear && fn_ov_read && fn_ov_pcm_total && fn_ov_info &&
			fn_ov_comment && fn_ov_open_callbacks)
		{
			g_bVorbisInit = true;
		}
	}
}

void ShutdownVorbisFile()
{
	if (g_hVorbisFileDLL)
	{
		FreeLibrary(g_hVorbisFileDLL);
		g_hVorbisFileDLL = NULL;
	}

	fn_ov_clear          = NULL;
	fn_ov_read           = NULL;
	fn_ov_pcm_total      = NULL;
	fn_ov_info           = NULL;
	fn_ov_comment        = NULL;
	fn_ov_open_callbacks = NULL;
	g_bVorbisInit        = false;
}

size_t ov_read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	return fread(ptr, size, nmemb, (FILE*)datasource);
}

int ov_seek_func(void *datasource, ogg_int64_t offset, int whence)
{
	return fseek((FILE*)datasource, (long)offset, whence);
}

int ov_close_func(void *datasource)
{
   return fclose((FILE*)datasource);
}

long ov_tell_func(void *datasource)
{
	return ftell((FILE*)datasource);
}


class OggVorbisInit
{
public:
	OggVorbisInit()
	{
		InitVorbisFile();
	}

	~OggVorbisInit()
	{
		ShutdownVorbisFile();
	}
};
//////////////////////////////////////////////////////////////////////////
char* CaseWord(char *str, int(*fun)(int) = toupper)
{
	if (str == 0 || fun == 0)
	{
		return str;
	}

	int i = 0;

	while (str[i])
	{
		str[i] = fun(str[i]);
		++i;
	}

	return str;
} // CaseWord
//////////////////////////////////////////////////////////////////////////

CSndLoader::CSndLoader(void)
{
}

CSndLoader::~CSndLoader(void)
{
}

unsigned int CSndLoader::Load(const char* filename)
{
	try
	{
		char szDrv[10]   = {0};
		char szDir[256]  = {0};
		char szFile[256] = {0};
		char szExt[10]   = {0};
		_splitpath(filename, szDrv, szDir, szFile, szExt);
		CaseWord(szExt);

		if (strcmp(szExt, ".WAV") == 0)
		{
			return LoadWav(filename);
		}
		else if(strcmp(szExt, ".OGG") == 0)
		{
			return LoadOgg(filename);
		}
		else
		{
			throw filename;
		}
	}
	catch (...)
	{
		printf("<SndCreateBuffer> 不支持声音文件: %s\n", filename);
	}

	return 0;
} // Load

unsigned int CSndLoader::LoadWav(const char* filename)
{
	ALuint buffer = alutCreateBufferFromFile(filename);
	AL_ERR_RETURN_ZERO("创建声音缓存失败");
	return buffer;
} // LoadWav

unsigned int CSndLoader::LoadOgg(const char* filename)
{
	static OggVorbisInit s_init;
	FILE *f = 0;

	try
	{
		f = fopen(filename, "rb");

		if (f == 0)
		{
			throw "打开文件失败";
		}

		OggVorbis_File oggFile;
		ov_callbacks   sCallbacks;

		sCallbacks.read_func = ov_read_func;
		sCallbacks.seek_func = ov_seek_func;
		sCallbacks.close_func = ov_close_func;
		sCallbacks.tell_func = ov_tell_func;

		if (fn_ov_open_callbacks(f, &oggFile, NULL, 0, sCallbacks) != 0)
		{
			throw "读取ogg文件失败,格式不对";
		}

		vorbis_info* pInfo = fn_ov_info(&oggFile, -1);
		ALenum format      = pInfo->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
		ALsizei freq       = pInfo->rate;

		const int BUFFER_SIZE   = 32768; // 每次多钱32kb
		int endian              = 0; // 0:Little-Endian 1:Big-Endian
		char array[BUFFER_SIZE] = {0};
		int bitStream; 
		long bytes;
		vector<char> datas;

		do 
		{
			bytes = fn_ov_read(&oggFile, array, BUFFER_SIZE, endian, 2, 1, &bitStream);

			if (bytes < 0)
			{
				fn_ov_clear(&oggFile);
				throw "解码失败";
			}

			datas.insert(datas.end(), array, array + bytes);
		} while(bytes > 0);
		fn_ov_clear(&oggFile);
		fclose(f);

		// 生成al声音缓冲
		ALuint buffer = 0;
		alGenBuffers(1, &buffer);
		AL_ERR_RETURN_ZERO("创建声音缓存失败");
		alBufferData(buffer, format, &datas[0], (ALsizei)datas.size(), freq);
		AL_ERR_RETURN_ZERO("绑定声音缓存失败");
		return buffer;
	}
	catch (const char* err)
	{
		if (f != 0)
		{
			fclose(f);
		}

		printf("<CSndLoader::LoadOgg> %s File:%s\n", err, filename);
	}

	return 0;
} // LoadOgg

//
//unsigned int CSndLoader::LoadOgg(const char* filename)
//{
//	FILE *f = 0;
//
//	try
//	{
//		f = fopen(filename, "rb");
//
//		if (f == 0)
//		{
//			throw "打开文件失败";
//		}
//		
//		OggVorbis_File oggFile;
//
//		if (ov_open(f, &oggFile, NULL, 0) != 0)
//		{
//			throw "读取ogg文件失败";
//		}
//
//		vorbis_info* pInfo = ov_info(&oggFile, 1);
//		ALenum format      = pInfo->channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
//		ALsizei freq       = pInfo->rate;
//
//		const int BUFFER_SIZE   = 32768; // 每次多钱32kb
//		int endian              = 0; // 0:Little-Endian 1:Big-Endian
//		char array[BUFFER_SIZE] = {0};
//		int bitStream; 
//		long bytes;
//		vector<char> datas;
//	
//		do 
//		{
//			bytes = ov_read(&oggFile, array, BUFFER_SIZE, endian, 2, 1, &bitStream);
//
//			if (bytes < 0)
//			{
//				ov_clear(&oggFile);
//				throw "解码失败";
//			}
//
//			datas.insert(datas.end(), array, array + bytes);
//		} while(bytes > 0);
//		ov_clear(&oggFile);
//		fclose(f);
//
//		// 生成al声音缓冲
//		ALuint buffer = 0;
//		alGenBuffers(1, &buffer);
//		AL_ERR_RETURN_ZERO("创建声音缓存失败");
//		alBufferData(buffer, format, &datas[0], (ALsizei)datas.size(), freq);
//		AL_ERR_RETURN_ZERO("绑定声音缓存失败");
//		return buffer;
//	}
//	catch (const char* err)
//	{
//		if (f != 0)
//		{
//			fclose(f);
//		}
//
//		printf("<CSndLoader::LoadOgg> %s File:%s\n", err, filename);
//	}
//	
//	return 0;
//} // LoadOgg