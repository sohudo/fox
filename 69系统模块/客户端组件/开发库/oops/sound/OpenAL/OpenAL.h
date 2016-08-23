#ifndef _OpenAL_H_
#define _OpenAL_H_

#define OPEN_AL_LIBPATH(p,f) p##".dir/../"##f

// al
#include "./OpenAL 1.1 SDK/include/al.h"
#include "./OpenAL 1.1 SDK/include/alc.h"
#include "./freealut-1.1.0-bin/include/AL/alut.h"

#pragma comment(lib,OPEN_AL_LIBPATH(__FILE__,   "freealut-1.1.0-bin/lib/alut.lib")) 
#pragma comment(lib,OPEN_AL_LIBPATH(__FILE__,   "OpenAL 1.1 SDK/libs/Win32/OpenAL32.lib")) 


// ogg
#include "./Ogg/Ogg/ogg.h"
#include "./Ogg/Vorbis/vorbisfile.h"


#endif _OpenAL_H_