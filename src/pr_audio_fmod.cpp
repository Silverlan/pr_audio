/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "pr_audio_fmod.hpp"
#include "fmod_sound_system.hpp"



#ifdef __linux__
#define DLLEXPORT __attribute__((visibility("default")))
#else
#define DLLEXPORT __declspec(dllexport)
#endif

extern "C"
{
    DLLEXPORT bool initialize_audio_api(float metersPerUnit,std::shared_ptr<al::ISoundSystem> &outSoundSystem,std::string &errMsg)
	{
		outSoundSystem = al::FMSoundSystem::Create(metersPerUnit);
		return outSoundSystem != nullptr;
	}
};
