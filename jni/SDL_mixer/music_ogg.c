/*
  SDL_mixer:  An audio mixer library based on the SDL library
  Copyright (C) 1997-2012 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/* $Id$ */

#ifdef OGG_MUSIC

/* This file supports Ogg Vorbis music streams */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL_thread.h"
#include "SDL_mutex.h"

#include "SDL_mixer.h"
#include "dynamic_ogg.h"
#include "music_ogg.h"

#ifdef WIN32 
#include <malloc.h>
#define alloca _malloca
#endif

#if defined(ANDROID)
#include <android/log.h>
#endif

/* This is the format of the audio mixer data */
static SDL_AudioSpec mixer;
#define OGG_MUSIC_BUFFER 2048

static char sharedData[OGG_MUSIC_BUFFER];
static int charedDataLen=0;
static int sharedDataRead = 1;
static int boolNewData = 0;
static int sharedPlaying = 0;


static SDL_mutex  *sharedLock;

/* Initialize the Ogg Vorbis player, with the given mixer settings
   This function returns 0, or -1 if there was an error.
 */
int OGG_init(SDL_AudioSpec *mixerfmt)
{
        mixer = *mixerfmt;
        return(0);
}

/* Set the volume for an OGG stream */
void OGG_setvolume(OGG_music *music, int volume)
{
        music->volume = volume;
}

static size_t sdl_read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{
    return SDL_RWread((SDL_RWops*)datasource, ptr, size, nmemb);
}

static int sdl_seek_func(void *datasource, ogg_int64_t offset, int whence)
{
    return SDL_RWseek((SDL_RWops*)datasource, (int)offset, whence);
}

static long sdl_tell_func(void *datasource)
{
    return SDL_RWtell((SDL_RWops*)datasource);
}

/* Load an OGG stream from an SDL_RWops object */
OGG_music *OGG_new_RW(SDL_RWops *rw, int freerw)
{
        OGG_music *music;
        ov_callbacks callbacks;
        
        sharedLock = SDL_CreateMutex();
        


        if ( !Mix_Init(MIX_INIT_OGG) ) {
                if ( freerw ) {
                        SDL_RWclose(rw);
                }
                return(NULL);
        }

        SDL_memset(&callbacks, 0, sizeof(callbacks));
        callbacks.read_func = sdl_read_func;
        callbacks.seek_func = sdl_seek_func;
        callbacks.tell_func = sdl_tell_func;

        music = (OGG_music *)SDL_malloc(sizeof *music);
        if ( music ) {
                /* Initialize the music structure */
                memset(music, 0, (sizeof *music));
                music->rw = rw;
                music->freerw = freerw;
                OGG_stop(music);
                OGG_setvolume(music, MIX_MAX_VOLUME);
                music->section = -1;

                if ( ov_open_callbacks(rw, &music->vf, NULL, 0, callbacks) < 0 ) {
                        SDL_free(music);
                        if ( freerw ) {
                                SDL_RWclose(rw);
                        }
                        SDL_SetError("Not an Ogg Vorbis audio stream");
                        return(NULL);
                }
        } else {
                if ( freerw ) {
                        SDL_RWclose(rw);
                }
                SDL_OutOfMemory();
                return(NULL);
        }
        return(music);
}



/* Return non-zero if a stream is currently playing */
int OGG_playing(OGG_music *music)
{
        return(music->playing);
}






int OGG_DecodeThread (void* threadData)
{       
        
        
		//#ifdef WIN32
		//char *dateReady = (char*)alloca(OGG_MUSIC_BUFFER);
		//#else
		char dateReady[OGG_MUSIC_BUFFER];	
		//#endif

        
        int lenReady = 0;
        int newData = 0;
        int section = 0;
        int lockCounter =0;

        OggVorbis_File* sharedVF = (OggVorbis_File*)threadData;
        
		#if defined(ANDROID)
			__android_log_print(ANDROID_LOG_INFO, "music_ogg.c", "begining OGG_DecodeThread");
		#endif



        while(sharedPlaying)
        {

                if(sharedDataRead && newData)
                {
                        //SDL_mutexP(sharedLock);
                        ///len = vorbis.ov_read(sharedVF, data, sizeof(data), 0, 2, 1, &section);                               
                        SDL_mutexP(sharedLock);
                        memcpy(sharedData, dateReady, lenReady);
                        charedDataLen = lenReady;
                        SDL_mutexV(sharedLock);

                        //SDL_mutexV(sharedLock);
                        sharedDataRead = 0;
                        newData = 0;
                        lockCounter = 0;
                }
                else
                {
                        SDL_mutexP(sharedLock);
                        
						#ifdef OGG_USE_TREMOR
				        lenReady = ov_read(sharedVF, dateReady, sizeof(dateReady),&section);                   
						#else
						lenReady = ov_read(sharedVF, dateReady, sizeof(dateReady), 0, 2, 1, &section);                   
						#endif

                        SDL_mutexV(sharedLock);

						

                        newData = 1;

                        if(lenReady > 0)
                        {
                                while(sharedDataRead == 0 && sharedPlaying )
                                {
                                        SDL_Delay(1);
                                        lockCounter++;
                                }
                        }
                        else
                        {
                                //if(sharedDataRead)
                                {
                                        sharedDataRead = 0;
                                        charedDataLen = 0;                              
                                }
                                break;
                        
                        }

                        lockCounter++;
                        
                }
        
        }
         
		#ifdef WIN32
		//_freea(dateReady);
		#endif

		#if defined(ANDROID)
			__android_log_print(ANDROID_LOG_INFO, "music_ogg.c", "Ending OGG_DecodeThread");
		#endif


		return 0;
}       

void OGG_StartDecodeThread (OggVorbis_File* vf)
{
	char name[64];
    sharedPlaying = 1;
	sharedDataRead = 1;
	

    SDL_snprintf(name, sizeof (name), "OGGTHREAD");


		#if defined(__WIN32__) && !defined(HAVE_LIBC)
			#undef SDL_CreateThread
			SDL_CreateThread(OGG_DecodeThread,name,(void*) vf,NULL,NULL);
		#else
			SDL_CreateThread(OGG_DecodeThread, name, (void*) vf);
		#endif
		


}


/* Start playback of a given OGG stream */
void OGG_play(OGG_music *music)
{
        if(music->playing == 0)
		{

			#if defined(ANDROID)
			__android_log_print(ANDROID_LOG_INFO, "music_ogg.c", "starting music thread");
			#endif

		#if defined(WIN32)
          OGG_StartDecodeThread(&music->vf);
		#endif

		}

        music->playing = 1;

        


}


/* Read some Ogg stream data and convert it for output */

#if defined(ANDROID) 

	static void OGG_getsome(void  *msc)
	{
		
		int section=0;
		int len=0;
		char data[OGG_MUSIC_BUFFER];
	
		SDL_AudioCVT *cvt;
		OGG_music *music = (OGG_music *)msc;


	#ifdef OGG_USE_TREMOR
		len = ov_read(&music->vf, data, sizeof(data), &section);
	#else
		len = ov_read(&music->vf, data, sizeof(data), 0, 2, 1, &section);
	#endif

		if ( len <= 0 ) {
			if ( len == 0 ) {
				music->playing = 0;
			}
			return;
		}
		cvt = &music->cvt;
		if ( section != music->section ) {
			vorbis_info *vi;

			vi = ov_info(&music->vf, -1);
			SDL_BuildAudioCVT(cvt, AUDIO_S16, vi->channels, vi->rate,
								   mixer.format,mixer.channels,mixer.freq);
			if ( cvt->buf ) {
				SDL_free(cvt->buf);
			}
			cvt->buf = (Uint8 *)SDL_malloc(sizeof(data)*cvt->len_mult);
			music->section = section;
		}
		if ( cvt->buf ) {
			memcpy(cvt->buf, data, len);
			if ( cvt->needed ) {
				cvt->len = len;
				SDL_ConvertAudio(cvt);
			} else {
				cvt->len_cvt = len;
			}
			music->len_available = music->cvt.len_cvt;
			music->snd_available = music->cvt.buf;
		} else {
			SDL_SetError("Out of memory");
			music->playing = 0;
		}
	
		
	}
#else
	static void OGG_getsome(OGG_music *music)
	{
			int section;
			int len;
	        
			//#ifdef WIN32
			//char *data= (char*)alloca(OGG_MUSIC_BUFFER);
			//#else
			char data[OGG_MUSIC_BUFFER];
			//#endif


			SDL_AudioCVT *cvt;
	/*
	#ifdef OGG_USE_TREMOR
			len = ov_read(&music->vf, data, sizeof(data), &section);
	#else
			len = ov_read(&music->vf, data, sizeof(data), 0, 2, 1, &section);
	#endif
	*/

			if (!sharedDataRead && charedDataLen)
			{

					SDL_mutexP(sharedLock);
					memcpy(data, sharedData, charedDataLen);
					len = charedDataLen;
					sharedDataRead = 1;
					SDL_mutexV(sharedLock);
	                
					if ( len <= 0 ) {
							if ( len == 0 ) {
									music->playing = 0;
							}

							#ifdef WIN32
							//_freea(data);
							#endif

							return;
					}
					cvt = &music->cvt;
					if ( section != music->section ) {
							vorbis_info *vi;

							SDL_mutexP(sharedLock);
							vi = ov_info(&music->vf, -1);
							SDL_mutexV(sharedLock);

							SDL_BuildAudioCVT(cvt, AUDIO_S16, vi->channels, vi->rate,
																	   mixer.format,mixer.channels,mixer.freq);
							if ( cvt->buf ) {
									SDL_free(cvt->buf);
							}
							cvt->buf = (Uint8 *)SDL_malloc(sizeof(data)*cvt->len_mult);
							music->section = section;
					}
					if ( cvt->buf ) {
							memcpy(cvt->buf, data, len);
							if ( cvt->needed ) {
									cvt->len = len;
									SDL_ConvertAudio(cvt);
							} else {
									cvt->len_cvt = len;
							}
							music->len_available = music->cvt.len_cvt;
							music->snd_available = music->cvt.buf;
					} else {
							SDL_SetError("Out of memory");
							music->playing = 0;
					}

			}
			else
			{
					if (!sharedDataRead && charedDataLen == 0)
					{
							music->playing = 0;                     
							#ifdef WIN32
							//_freea(data);
							#endif

							return;
					}

			}

		#ifdef WIN32
		//_freea(data);
		#endif


	}
#endif

/* Play some of a stream previously started with OGG_play() */
int OGG_playAudio(OGG_music *music, Uint8 *snd, int len)
{
        int mixable;

        while ( (len > 0) && music->playing ) {
                if ( ! music->len_available ) {
                        OGG_getsome(music);
                }
                mixable = len;
                if ( mixable > music->len_available ) {
                        mixable = music->len_available;
                }
                if ( music->volume == MIX_MAX_VOLUME ) {
                        memcpy(snd, music->snd_available, mixable);
                } else {
                        SDL_MixAudio(snd, music->snd_available, mixable,
                                                      music->volume);
                }
                music->len_available -= mixable;
                music->snd_available += mixable;
                len -= mixable;
                snd += mixable;
        }
        
        return len;
}

/* Stop playback of a stream previously started with OGG_play() */
void OGG_stop(OGG_music *music)
{
        music->playing = 0;
        sharedPlaying = 0;

}

/* Close the given OGG stream */
void OGG_delete(OGG_music *music)
{
        if ( music ) {
                if ( music->cvt.buf ) {
                        SDL_free(music->cvt.buf);
                }
                if ( music->freerw ) {
                        SDL_RWclose(music->rw);
                }
                ov_clear(&music->vf);
                SDL_free(music);
        }
}

/* Jump (seek) to a given position (time is in seconds) */
void OGG_jump_to_time(OGG_music *music, double time)
{

        SDL_mutexP(sharedLock);
#ifdef OGG_USE_TREMOR
       ov_time_seek( &music->vf, (ogg_int64_t)time );
#else
       ov_time_seek( &music->vf, time );
#endif
        SDL_mutexV(sharedLock);
}

#endif /* OGG_MUSIC */
