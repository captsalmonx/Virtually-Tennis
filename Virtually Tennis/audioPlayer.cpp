#include "audioPlayer.h"

string soundFile;
ISound* currentSound;
ISoundEngine* se;

bool init_audioPlayer()
{
	// Create a new audio device
	se = irrklang::createIrrKlangDevice();

	if(!se){
		fprintf( stderr, "Failed to initialize IrrKlangDevice\n" );
		return false;
	}

	// Set the volume to full
	se->setSoundVolume(1);
	return true;
}

bool playCurrentSound(bool loop)
{
	// Adds the file to the audio device to be played in the background
	currentSound = se->play2D(soundFile.c_str(), loop, false, true);
	return !currentSound;
}

void stopSounds()
{
	// Stop all audio
	se->stopAllSounds();
}

void loopSound(string fileName)
{
	// Play sound and loop it
	soundFile = fileName;
	playCurrentSound(true);
}

void playSound(string fileName)
{
	// Set the file name and play it immediately
	soundFile = fileName;
	playCurrentSound(false);
}