#include "audioPlayer.h"

string soundFile;
ISound* currentSound;
ISoundEngine* se;

bool init_audioPlayer()
{
	se = irrklang::createIrrKlangDevice();

	if(!se){
		fprintf( stderr, "Failed to initialize IrrKlangDevice\n" );
		return false;
	}

	se->setSoundVolume(1);
	return true;
}

bool playCurrentSound()
{
	currentSound = se->play2D(soundFile.c_str(), false, false, true);
	return !currentSound;
}

void playSound(string fileName)
{
	soundFile = fileName;
	playCurrentSound();
}