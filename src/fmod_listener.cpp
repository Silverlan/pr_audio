/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fmod_listener.hpp"
#include "fmod_sound_system.hpp"
#include <alsound_coordinate_system.hpp>
#include <fmod_studio.hpp>

al::FMListener::FMListener(al::ISoundSystem &system)
	: IListener{system}
{}
void al::FMListener::DoSetMetersPerUnit(float mu)
{
	m_metersPerUnit = mu;
}
void al::FMListener::SetGain(float gain)
{
	m_gain = gain;
}
void al::FMListener::SetPosition(const Vector3 &pos)
{
	auto posAudio = al::to_audio_position(pos);
	auto &fmodSys = static_cast<FMSoundSystem&>(m_soundSystem).GetFMODSystem();
	FMOD_3D_ATTRIBUTES attributes;
	al::check_result(fmodSys.getListenerAttributes(0,&attributes));
	attributes.position = {posAudio.x,posAudio.y,posAudio.z};
	al::check_result(fmodSys.setListenerAttributes(0,&attributes));
}
void al::FMListener::SetVelocity(const Vector3 &vel)
{
	auto velAudio = al::to_audio_position(vel);
	auto &fmodSys = static_cast<FMSoundSystem&>(m_soundSystem).GetFMODSystem();
	FMOD_3D_ATTRIBUTES attributes;
	al::check_result(fmodSys.getListenerAttributes(0,&attributes));
	attributes.velocity = {velAudio.x,velAudio.y,velAudio.z};
	al::check_result(fmodSys.setListenerAttributes(0,&attributes));
}
void al::FMListener::SetOrientation(const Vector3 &at,const Vector3 &up)
{
	auto atAudio = al::to_audio_direction(at);
	auto atUp = al::to_audio_direction(up);
	auto &fmodSys = static_cast<FMSoundSystem&>(m_soundSystem).GetFMODSystem();
	FMOD_3D_ATTRIBUTES attributes;
	al::check_result(fmodSys.getListenerAttributes(0,&attributes));
	attributes.forward = {atAudio.x,atAudio.y,atAudio.z};
	attributes.up = {atUp.x,atUp.y,atUp.z};
	al::check_result(fmodSys.setListenerAttributes(0,&attributes));
}
