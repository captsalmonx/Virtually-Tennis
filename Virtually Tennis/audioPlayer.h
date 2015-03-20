#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <iostream>
#include <string>
#include <windows.h>
#include <irrKlang.h>

using namespace std;
using namespace irrklang;

bool init_audioPlayer();
bool playCurrentSound();
void playSound(string fileName);

#endif