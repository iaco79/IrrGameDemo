#include <irrlicht.h>
#include "stdio.h"
#include "SDL.h"
#include "SDL_mixer.h"
#include "AudioManager.h"


#if defined(ANDROID)
#include <android/log.h>
#endif


AudioManager* AudioManager::gInstance= NULL;
bool AudioManager::gInstanceFlag = false;
int AudioManager::gChunkIndexes[AudioItem::CHUNK_COUNT];

unsigned int soundIds = 0;

AudioManager* AudioManager::getInstance()
{
   if(!gInstanceFlag)
    {
        gInstance = new AudioManager();
        gInstanceFlag = true;
        return gInstance;
    }
    else
    {
        return gInstance;
    }
}


bool AudioManager::initialize()
{
	int audio_rate= MIX_DEFAULT_FREQUENCY;
	Uint16 audio_format = MIX_DEFAULT_FORMAT;
	int audio_channels = 2;

#ifdef WIN32
	int audio_buffers = 512;
#else
	int audio_buffers = 1536;
#endif
	int audio_volume = MIX_MAX_VOLUME;

	
	//initialize SDL audio only
	
#ifdef WIN32
	if ( SDL_Init(SDL_INIT_AUDIO) < 0 ) { 

		fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
		return false;
	}
#endif

	//initialize sdl mixer
	
	if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) < 0) {
		fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
		return false;
	} 

	Mix_VolumeMusic(audio_volume);


#if defined(_IRR_ANDROID_PLATEFORM_)
	gChunkIndexes[AudioItem::CHUNK_BLAST] = loadChunk("/mnt/sdcard/GameDemo/blas4.wav");
	gChunkIndexes[AudioItem::CHUNK_PULL] = loadChunk("/mnt/sdcard/GameDemo/pull2.wav");
	gChunkIndexes[AudioItem::CHUNK_EXTCOLLIDE] = loadChunk("/mnt/sdcard/GameDemo/pool1.wav");
	gChunkIndexes[AudioItem::CHUNK_CREATE] = loadChunk("/mnt/sdcard/GameDemo/blas2.wav");
	gChunkIndexes[AudioItem::CHUNK_LEVELUP] = loadChunk("/mnt/sdcard/GameDemo/levelUp.wav");
	gChunkIndexes[AudioItem::CHUNK_LOST] = loadChunk("/mnt/sdcard/GameDemo/lifelost.wav");
	
	
#else
	gChunkIndexes[AudioItem::CHUNK_BLAST] = loadChunk("IrrlichtSdcard/blas4.wav");
	gChunkIndexes[AudioItem::CHUNK_PULL] = loadChunk("IrrlichtSdcard/pull2.wav");
	gChunkIndexes[AudioItem::CHUNK_EXTCOLLIDE] = loadChunk("IrrlichtSdcard/pool1.wav");
	gChunkIndexes[AudioItem::CHUNK_CREATE] = loadChunk("IrrlichtSdcard/blas2.wav");
	gChunkIndexes[AudioItem::CHUNK_LEVELUP] = loadChunk("IrrlichtSdcard/levelUp.wav");
	gChunkIndexes[AudioItem::CHUNK_LOST] = loadChunk("IrrlichtSdcard/lifelost.wav");
#endif


}

void AudioManager::shutDown()
{


	clearChunks();

	if ( mMusic ) {
		Mix_FreeMusic((Mix_Music*)mMusic);
		mMusic = NULL;
	}

	Mix_CloseAudio();

	SDL_Quit();

}

void AudioManager::update()
{
	irr::core::array<int> toRemove;


	int toProcess = (int)mAudioList.size();

	for(int i=0;i<(int)mAudioList.size();i++)
	{
		switch(mAudioList[i].mState )
		{
			case AudioItem::TOPLAY:
			{				
				mAudioList[i].mChannel =  Mix_PlayChannel(-1,
													(Mix_Chunk*)mChunkList[mAudioList[i].mChunkId],
													mAudioList[i].mLoop);
				
				mAudioList[i].mState = AudioItem::PLAYING;
				if( (mAudioList[i].mFlags & AudioItem::PLAYONCE) > 0)
				{
					toRemove.push_back(mAudioList[i].mId);
				}
			}
			break;			
			case AudioItem::TOSTOP:
			{
				if (mAudioList[i].mChannel>=0)
				{						
					if( Mix_Playing(mAudioList[i].mChannel))
					{
						Mix_HaltChannel(mAudioList[i].mChannel);
						mAudioList[i].mChannel= -1;
						
					}
					
				}
				mAudioList[i].mState = AudioItem::STOPPED;
			}
			break;
			case AudioItem::RESET:
			{
				if (mAudioList[i].mChannel>=0)
				{						

					if( Mix_Playing(mAudioList[i].mChannel))
					{
						Mix_HaltChannel(mAudioList[i].mChannel);
						mAudioList[i].mChannel=  Mix_PlayChannel(
							-1,
							(Mix_Chunk*)mChunkList[mAudioList[i].mChunkId],
							mAudioList[i].mLoop);
					}					
					mAudioList[i].mState = AudioItem::PLAYING;
				}	

				
			}
			break;

		
		}	
	}


	int toRemoveCount = (int)toRemove.size();

	for (irr::u32 j = 0; j < (int)toRemove.size(); j++)
		{
			int id = toRemove[j];
			
			for (irr::u32 i = 0; i < mAudioList.size(); i++)
			{	
				if (mAudioList[i].mId == id)
				{
					mAudioList.erase(i);
					break;
				}
			}
		}	



}

void AudioManager::stopSound(AudioItem audio)
{
	if(audio.mId  > 0)
	{
		int index = getSoundIndexById(audio.mId);

		if(index>=0)
		{	
			mAudioList[index].mState= AudioItem::TOSTOP;		
		}
	}	
}


int AudioManager::playSoundNow(AudioItem audio)
{	
	
	return Mix_PlayChannel(-1,(Mix_Chunk*)mChunkList[audio.mChunkId],0);
}

int AudioManager::playSound(AudioItem audio)
{

	if(audio.mChunkId >=0)
	{
		if(audio.mId == -1)
		{
			soundIds++;		
			audio.mId = soundIds;

			audio.mState = AudioItem::TOPLAY; 
			mAudioList.push_back(audio);

			return audio.mId;
		}
		else
		{
			int index = getSoundIndexById(audio.mId);

			if(index>=0)
			{
				if(mAudioList[index].mChunkId != audio.mChunkId)
				{
					mAudioList[index].mLastChunkId = mAudioList[index].mChunkId;
					mAudioList[index].mChunkId = audio.mId;
					mAudioList[index].mState= AudioItem::RESET;
				}
				else
				{
					if(mAudioList[index].mState == AudioItem::STOPPED)
						mAudioList[index].mState= AudioItem::TOPLAY;

				}

			}

			return audio.mId;
		}
	}
	
	return -1;
}



void AudioManager::playMusic(irr::core::stringc fileName)
{

	mMusic = (void *)Mix_LoadMUS(fileName.c_str());


	if(mMusic)
	{
		Mix_FadeInMusic((Mix_Music*)mMusic,-1,2000);
		Mix_VolumeMusic(40);
	}
	else
	{
	
		#if defined(ANDROID)
			__android_log_print(ANDROID_LOG_INFO, "AudioManager.cpp", "cannot open music");
		#endif

	
	}

}
void AudioManager::stopMusic()
{

}

int AudioManager::loadChunk(irr::core::stringc fileName)
{
	void *chunk = NULL;

	chunk = (void *)Mix_LoadWAV(fileName.c_str());
	
	if(chunk)
	{
		mChunkList.push_back(chunk);
		return mChunkList.size()-1;
	}

	return -1;	

}

void AudioManager::clearChunks()
{

	for(int i=0;i<(int)mChunkList.size();i++)
	{
		if(mChunkList[i]!=NULL)
			Mix_FreeChunk((Mix_Chunk*)mChunkList[i]);
	} 

	mChunkList.clear();

}




int AudioManager::getSoundIndexById(int id)
{

	int index=-1;

	for(int i=0;i<(int)mAudioList.size();i++)
	{
		if(mAudioList[i].mId == id)
		{	
			index = i;
		}	
	}

	return index;
}


