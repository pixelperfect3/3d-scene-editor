#pragma once

#include <AL/alut.h>
#include <map>
#include <vector>
#include <conio.h>

using std::map;
using std::pair;
using std::vector;

struct AudioObject{
	ALfloat SourcePos[3];// = { 0.0, 0.0, 0.0 };
	ALfloat SourceVel[3];// = { 0.0f, 0.0f, 0.1f };
	ALuint source;
	ALuint buffer;
	String name;
};

class AudioManager
{
public:

	AudioManager(void)
	{
		alutInit(NULL,0);
		

		if(alGetError() != AL_NO_ERROR)
			exit(1);

//		atexit(KillALData);
		ListenerVel[0] = 0;
		ListenerVel[1] = 0;
		ListenerVel[2] = 0;

		ListenerOri[0] = 0;
		ListenerOri[1] = 0;
		ListenerOri[2] = 0;//dir.z;
		ListenerOri[3] = 0;
		ListenerOri[4] = 1;
		ListenerOri[5] = 0;

		ListenerPos[0] = 0;//pos.x;
		ListenerPos[1] = 0;//pos.y;
		ListenerPos[2] = 0;//pos.z;

		update();
		cleaned = false;
	}

	~AudioManager(void)
	{
		if(!cleaned)
			KillALData();
	}

	ALboolean addSound(String name,char* soundFile){
		// Variables to load into.

		ALuint Buffer;
		ALuint Source;

		ALenum  format;
		ALsizei   size;
		ALvoid*   data;
		ALsizei   freq;
		ALboolean loop;

		// Load wav data into a buffer.

		alGenBuffers(1, &Buffer);

		if(alGetError() != AL_NO_ERROR)
			return AL_FALSE;

		alutLoadWAVFile(soundFile, &format, &data, &size, &freq, &loop);
		alBufferData(Buffer, format, data, size, freq);
		alutUnloadWAV(format, data, size, freq);

		// Bind the buffer with the source.

		alGenSources(1, &Source);

		if(alGetError() != AL_NO_ERROR)
			return AL_FALSE;


		AudioObject* sObj = new AudioObject();
		sObj->source = Source;
		sObj->buffer = Buffer;


		alSourcei (sObj->source, AL_BUFFER,   sObj->buffer   );
		alSourcef (sObj->source, AL_PITCH,    0.5f     );
		alSourcef (sObj->source, AL_GAIN,     1.0f     );
		alSourcei (sObj->source, AL_LOOPING,  AL_TRUE  );

	//	alSourcei(sObj->source,AL_SOURCE_RELATIVE,AL_TRUE);

		audioTable.insert(pair<String,AudioObject*>(name,sObj));

		return AL_TRUE;
	}

	ALboolean addSound(String name,char* soundFile,ALfloat SourcePos[3],ALfloat SourceVel[3]){
		// Variables to load into.

		ALuint Buffer;
		ALuint Source;

		ALenum  format;
		ALsizei   size;
		ALvoid*   data;
		ALsizei   freq;
		ALboolean loop;

		// Load wav data into a buffer.

		alGenBuffers(1, &Buffer);

		if(alGetError() != AL_NO_ERROR)
			return AL_FALSE;

		alutLoadWAVFile(soundFile, &format, &data, &size, &freq, &loop);
		alBufferData(Buffer, format, data, size, freq);
		alutUnloadWAV(format, data, size, freq);

		// Bind the buffer with the source.

		alGenSources(1, &Source);

		if(alGetError() != AL_NO_ERROR)
			return AL_FALSE;


		AudioObject* sObj = new AudioObject();
		sObj->source = Source;
		sObj->buffer = Buffer;
		for(int i = 0;i<3;i++){
			sObj->SourcePos[i] = SourcePos[i];
			sObj->SourceVel[i] = SourceVel[i];
		}

		alSourcei (sObj->source, AL_BUFFER,   sObj->buffer   );
		alSourcef (sObj->source, AL_PITCH,    1.0f     );
		alSourcef (sObj->source, AL_GAIN,     1.0f     );
		alSourcefv(sObj->source, AL_POSITION, sObj->SourcePos);
		alSourcefv(sObj->source, AL_VELOCITY, sObj->SourceVel);
		alSourcei (sObj->source, AL_LOOPING,  AL_TRUE  );

	//	alSourcei(sObj->source,AL_SOURCE_RELATIVE,AL_TRUE);

		audioTable.insert(pair<String,AudioObject*>(name,sObj));

		return AL_TRUE;
	}

	// changes the position of a sound
	void changePosition(String name, ALfloat SourcePos[]) {
		// get the object first
		AudioObject* sObj = audioTable[name];

		// change it's position
		for(int i = 0;i<3;i++)
			sObj->SourcePos[i] = SourcePos[i];
		
		alSourcefv(sObj->source, AL_POSITION, SourcePos);
	}

	ALboolean addSound(String name,char* soundFile,ALfloat SourcePos[3],ALfloat SourceVel[3],int loop1){
		// Variables to load into.

		ALuint Buffer;
		ALuint Source;

		ALenum  format;
		ALsizei   size;
		ALvoid*   data;
		ALsizei   freq;
		ALboolean loop;

		// Load wav data into a buffer.

		alGenBuffers(1, &Buffer);

		if(alGetError() != AL_NO_ERROR)
			return AL_FALSE;

		alutLoadWAVFile(soundFile, &format, &data, &size, &freq, &loop);
		alBufferData(Buffer, format, data, size, freq);
		alutUnloadWAV(format, data, size, freq);

		// Bind the buffer with the source.

		alGenSources(1, &Source);

		if(alGetError() != AL_NO_ERROR)
			return AL_FALSE;


		AudioObject* sObj = new AudioObject();
		sObj->source = Source;
		sObj->buffer = Buffer;
		for(int i = 0;i<3;i++){
			sObj->SourcePos[i] = SourcePos[i];
			sObj->SourceVel[i] = SourceVel[i];
		}

		alSourcei (sObj->source, AL_BUFFER,   sObj->buffer   );
		alSourcef (sObj->source, AL_PITCH,    1.0f     );
		alSourcef (sObj->source, AL_GAIN,     1.0f     );
		alSourcefv(sObj->source, AL_POSITION, sObj->SourcePos);
		alSourcefv(sObj->source, AL_VELOCITY, sObj->SourceVel);
		alSourcei (sObj->source, AL_LOOPING,  loop1  );

	//	alSourcei(sObj->source,AL_SOURCE_RELATIVE,AL_TRUE);

		audioTable.insert(pair<String,AudioObject*>(name,sObj));

		return AL_TRUE;
	}

	void play(String name){
		
		AudioObject* aObj = audioTable[name];	
	/*	playingTable.push_back(aObj->source);

		ALuint* a = new ALuint [playingTable.size()];
		copy( playingTable.begin(), playingTable.end(), a);
		alSourcePlayv(playingTable.size(),a);
		delete [] a;*/

		alSourcePlay(aObj->source);

	}
	void stop(String name){
		AudioObject* aObj = audioTable[name];	
		::alSourceStop(aObj->source);
	}

	void update(){
		alListenerfv(AL_POSITION,    ListenerPos);
		alListenerfv(AL_VELOCITY,    ListenerVel);
		alListenerfv(AL_ORIENTATION, ListenerOri);
	}

	void KillALData()
	{
		for(map<String,AudioObject*>::iterator it = audioTable.begin();it!=audioTable.end();it++){
			ALuint src = (*it).second->source;
			ALuint buf = (*it).second->buffer;
			alDeleteBuffers(1, &buf);
			alDeleteSources(1, &src);
			delete (*it).second;
		}
		alutExit();
		cleaned = true;
	}

private:
	map<String,AudioObject*> audioTable;
	bool cleaned;
	//vector<ALuint> playingTable;
public:
	// Position of the listener.
	ALfloat ListenerPos[3];
	// Velocity of the listener.
	ALfloat ListenerVel[3];
	// Orientation of the listener. (first 3 elements are "at", second 3 are "up")
	ALfloat ListenerOri[6];
};
