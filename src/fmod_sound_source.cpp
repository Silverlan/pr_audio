/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fmod_sound_source.hpp"
#include "fmod_sound_buffer.hpp"
#include "fmod_sound_system.hpp"
#include <alsound_coordinate_system.hpp>
#include <fmod_studio.hpp>

al::FMSoundChannel::FMSoundChannel(ISoundSystem &system,ISoundBuffer &buffer)
	: ISoundChannel(system,buffer)
{}
al::FMSoundChannel::FMSoundChannel(ISoundSystem &system,Decoder &decoder)
	: ISoundChannel(system,decoder)
{}
void al::FMSoundChannel::SetSource(FMOD::Channel *source)
{
	m_source = source;
}
void al::FMSoundChannel::Update()
{
	ISoundChannel::Update();
	if(m_source != nullptr)
		m_soundSourceData.offset = GetOffset();
}

void al::FMSoundChannel::SetFrameOffset(uint64_t offset)
{
	m_soundSourceData.offset = offset;
	if(m_source != nullptr)
		CheckResultAndUpdateValidity(m_source->setPosition(offset,FMOD_TIMEUNIT_PCM));
}
uint64_t al::FMSoundChannel::GetFrameOffset(uint64_t *latency) const
{
	if(m_source != nullptr)
	{
		uint32_t pos;
		if(CheckResultAndUpdateValidity(m_source->getPosition(&pos,FMOD_TIMEUNIT_PCM)))
			return pos;
	}
	return m_soundSourceData.offset;
}

void al::FMSoundChannel::Stop()
{
	if(m_source != nullptr)
		CheckResultAndUpdateValidity(m_source->stop());
	m_bSchedulePlay = false;
}

void al::FMSoundChannel::Pause()
{
	if(m_source != nullptr)
		CheckResultAndUpdateValidity(m_source->setPaused(true));
}

void al::FMSoundChannel::Play()
{
	InitializeChannel();
	m_soundSourceData.offset = 0ull;
	if(m_source != nullptr)
		CheckResultAndUpdateValidity(m_source->setPosition(0u,FMOD_TIMEUNIT_MS));
	if(m_source != nullptr)
		CheckResultAndUpdateValidity(m_source->setPaused(false));
}

void al::FMSoundChannel::Resume()
{
	if(m_source == nullptr)
	{
		auto offset = GetFrameOffset();
		Play();
		SetFrameOffset(offset);
		return;
	}
	if(m_source != nullptr)
		CheckResultAndUpdateValidity(m_source->setPaused(false));
}

bool al::FMSoundChannel::IsPlaying() const
{
	if(m_bSchedulePlay == true)
		return true;
	if(m_source != nullptr)
	{
		auto r = false;
		if(CheckResultAndUpdateValidity(m_source->isPlaying(&r)))
			return r;
	}
	return false;
}
bool al::FMSoundChannel::IsPaused() const
{
	return !IsPlaying();
}

void al::FMSoundChannel::SetPriority(uint32_t priority)
{
	m_soundSourceData.priority = priority;
	if(m_source != nullptr)
		CheckResultAndUpdateValidity(m_source->setPriority(priority));
}
uint32_t al::FMSoundChannel::GetPriority() const
{
	if(m_source != nullptr)
	{
		int32_t priority;
		if(CheckResultAndUpdateValidity(m_source->getPriority(&priority)))
			return priority;
	}
	return m_soundSourceData.priority;
}

void al::FMSoundChannel::SetLooping(bool bLoop)
{
	m_soundSourceData.looping = bLoop;
	if(m_source != nullptr)
	{
		FMOD_MODE mode;
		if(CheckResultAndUpdateValidity(m_source->getMode(&mode)))
		{
			mode &= ~(FMOD_LOOP_OFF | FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI);
			if(bLoop == false)
				mode |= FMOD_LOOP_OFF;
			else
				mode |= FMOD_LOOP_NORMAL;
			CheckResultAndUpdateValidity(m_source->setMode(mode));
		}
	}
}
bool al::FMSoundChannel::IsLooping() const
{
	if(m_source != nullptr)
	{
		FMOD_MODE mode;
		if(CheckResultAndUpdateValidity(m_source->getMode(&mode)))
			return !(mode &FMOD_LOOP_OFF) && (mode &(FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI)) != 0;
	}
	return m_soundSourceData.looping;
}

void al::FMSoundChannel::SetPitch(float pitch)
{
	m_soundSourceData.pitch = pitch;
	if(m_source != nullptr)
		CheckResultAndUpdateValidity(m_source->setPitch(pitch));
}
float al::FMSoundChannel::GetPitch() const
{
	if(m_source != nullptr)
	{
		auto pitch = 0.f;
		if(CheckResultAndUpdateValidity(m_source->getPitch(&pitch)))
			return pitch;
	}
	return m_soundSourceData.pitch;
}

void al::FMSoundChannel::SetGain(float gain)
{
	m_soundSourceData.gain = gain;
	if(m_source != nullptr)
		CheckResultAndUpdateValidity(m_source->setVolume(gain));
}
float al::FMSoundChannel::GetGain() const
{
	if(m_source != nullptr)
	{
		auto gain = 0.f;
		if(CheckResultAndUpdateValidity(m_source->getVolume(&gain)))
			return gain;
	}
	return m_soundSourceData.gain;
}

void al::FMSoundChannel::SetDistanceRange(float refDist,float maxDist)
{
	refDist = umath::min(refDist,maxDist);
	auto refDistAudio = al::to_audio_distance(refDist);
	auto maxDistAudio = al::to_audio_distance(maxDist);
	if(maxDistAudio == std::numeric_limits<float>::infinity())
		maxDistAudio = std::numeric_limits<float>::max();
	m_soundSourceData.distanceRange = {refDist,maxDist};
	if(Is3D() && m_source != nullptr)
		CheckResultAndUpdateValidity(m_source->set3DMinMaxDistance(refDistAudio,maxDistAudio));
}

std::pair<float,float> al::FMSoundChannel::GetDistanceRange() const
{
	if(Is3D() && m_source != nullptr)
	{
		float minDist,maxDist;
		if(CheckResultAndUpdateValidity(m_source->get3DMinMaxDistance(&minDist,&maxDist)))
			return {al::to_game_distance(minDist),al::to_game_distance(maxDist)};
	}
	return m_soundSourceData.distanceRange;
}

void al::FMSoundChannel::SetPosition(const Vector3 &pos)
{
	auto posAudio = al::to_audio_position(pos);
	m_soundSourceData.position = pos;
	UpdateMode();
	if(Is3D() && m_source != nullptr)
	{
		auto fmPos = al::to_custom_vector<FMOD_VECTOR>(posAudio);
		CheckResultAndUpdateValidity(m_source->set3DAttributes(&fmPos,nullptr));
		return;
	}
}

Vector3 al::FMSoundChannel::GetPosition() const
{
	if(Is3D() && m_source != nullptr)
	{
		FMOD_VECTOR pos;
		if(CheckResultAndUpdateValidity(m_source->get3DAttributes(&pos,nullptr)))
			return al::to_game_position({pos.x,pos.y,pos.z});
	}
	return m_soundSourceData.position;
}

void al::FMSoundChannel::SetVelocity(const Vector3 &vel)
{
	auto velAudio = al::to_audio_position(vel);
	m_soundSourceData.velocity = vel;
	UpdateMode();
	if(Is3D() && m_source != nullptr)
	{
		auto fmVel = al::to_custom_vector<FMOD_VECTOR>(velAudio);
		CheckResultAndUpdateValidity(m_source->set3DAttributes(nullptr,&fmVel));
		return;
	}
}
Vector3 al::FMSoundChannel::GetVelocity() const
{
	if(Is3D() && m_source != nullptr)
	{
		FMOD_VECTOR vel;
		if(CheckResultAndUpdateValidity(m_source->get3DAttributes(nullptr,&vel)))
			return al::to_game_position({vel.x,vel.y,vel.z});
	}
	return m_soundSourceData.velocity;
}

void al::FMSoundChannel::SetDirection(const Vector3 &dir)
{
	auto dirAudio = al::to_audio_direction(dir);
	// FMOD TODO
}
Vector3 al::FMSoundChannel::GetDirection() const
{
	// FMOD TODO
	return {};
}

void al::FMSoundChannel::SetOrientation(const Vector3 &at,const Vector3 &up)
{
	auto atAudio = al::to_audio_direction(at);
	auto atUp = al::to_audio_direction(up);
	// FMOD TODO
	//FMOD_VECTOR orientation {at.x,at.y,at.z};
	//al::fmod::check_result(m_source->set3DConeOrientation(&orientation));
}
std::pair<Vector3,Vector3> al::FMSoundChannel::GetOrientation() const
{
	// FMOD TODO
	return {{},{}};
}

void al::FMSoundChannel::SetConeAngles(float inner,float outer)
{
	m_soundSourceData.coneAngles = {inner,outer};
	UpdateMode();
	if(Is3D() && m_source != nullptr)
	{
		float t,volume;
		m_source->get3DConeSettings(&t,&t,&volume);
		CheckResultAndUpdateValidity(m_source->set3DConeSettings(inner,outer,volume));
	}
}
std::pair<float,float> al::FMSoundChannel::GetConeAngles() const
{
	if(Is3D() && m_source != nullptr)
	{
		float inner,outer;
		if(CheckResultAndUpdateValidity(m_source->get3DConeSettings(&inner,&outer,nullptr)))
			return {inner,outer};
	}
	return m_soundSourceData.coneAngles;
}

void al::FMSoundChannel::SetOuterConeGains(float gain,float gainHF)
{
	// FMOD TODO
}

std::pair<float,float> al::FMSoundChannel::GetOuterConeGains() const
{
	// FMOD TODO
	return {0.f,0.f};
}

float al::FMSoundChannel::GetOuterConeGain() const
{
	// FMOD TODO
	return 0.f;
}
float al::FMSoundChannel::GetOuterConeGainHF() const
{
	// FMOD TODO
	return 0.f;
}

void al::FMSoundChannel::SetRolloffFactors(float factor,float roomFactor)
{
	// FMOD TODO
}

std::pair<float,float> al::FMSoundChannel::GetRolloffFactors() const
{
	// FMOD TODO
	return {0.f,0.f};
}

float al::FMSoundChannel::GetRolloffFactor() const
{
	// FMOD TODO
	return 0.f;
}
float al::FMSoundChannel::GetRoomRolloffFactor() const
{
	// FMOD TODO
	return 0.f;
}

void al::FMSoundChannel::SetDopplerFactor(float factor)
{
	m_soundSourceData.dopplerFactor = factor;
	if(Is3D() && m_source != nullptr)
		CheckResultAndUpdateValidity(m_source->set3DDopplerLevel(factor));
}
float al::FMSoundChannel::GetDopplerFactor() const
{
	if(Is3D() && m_source != nullptr)
	{
		auto factor = 0.f;
		if(CheckResultAndUpdateValidity(m_source->get3DDopplerLevel(&factor)))
			return factor;
	}
	return m_soundSourceData.dopplerFactor;
}

void al::FMSoundChannel::SetRelative(bool bRelative)
{
	auto bRelativeOld = IsRelative();
	m_soundSourceData.relativeToListener = bRelative;
	UpdateMode();
	if(bRelative != bRelativeOld)
		CallCallbacks<void,bool>("OnRelativeChanged",bRelative);
}
bool al::FMSoundChannel::IsRelative() const
{
	return m_soundSourceData.relativeToListener;
}

void al::FMSoundChannel::SetRadius(float radius)
{
	auto radiusAudio = al::to_audio_distance(radius);
	SetMaxDistance(radiusAudio);
}
float al::FMSoundChannel::GetRadius() const
{
	return al::to_game_distance(GetMaxDistance());
}

void al::FMSoundChannel::SetStereoAngles(float leftAngle,float rightAngle)
{
	// FMOD TODO
}
std::pair<float,float> al::FMSoundChannel::GetStereoAngles() const
{
	// FMOD TODO
	return {0.f,0.f};
}

void al::FMSoundChannel::SetAirAbsorptionFactor(float factor)
{
	// FMOD TODO
}
float al::FMSoundChannel::GetAirAbsorptionFactor() const
{
	// FMOD TODO
	return 0.f;
}

void al::FMSoundChannel::SetGainAuto(bool directHF,bool send,bool sendHF)
{
	// FMOD TODO
}
std::tuple<bool,bool,bool> al::FMSoundChannel::GetGainAuto() const
{
	// FMOD TODO
	return {false,false,false};
}

bool al::FMSoundChannel::GetDirectGainHFAuto() const
{
	// FMOD TODO
	return false;
}
bool al::FMSoundChannel::GetSendGainAuto() const
{
	// FMOD TODO
	return false;
}
bool al::FMSoundChannel::GetSendGainHFAuto() const
{
	// FMOD TODO
	return false;
}
void al::FMSoundChannel::SetFMOD3DAttributesEffective(bool b)
{
	m_b3DAttributesEffective = b;
	UpdateMode();
}
void al::FMSoundChannel::UpdateMode()
{
	FMOD_MODE mode;
	if(CheckResultAndUpdateValidity(m_source->getMode(&mode)) == false)
		return;
	auto oldMode = mode;
	mode &= ~(FMOD_2D | FMOD_3D | FMOD_3D_HEADRELATIVE | FMOD_3D_WORLDRELATIVE);
	if(m_b3DAttributesEffective == false)
		mode |= FMOD_2D;
	else
	{
		if(IsRelative() == false)
			mode |= FMOD_3D | FMOD_3D_WORLDRELATIVE;
		else
		{
			auto coneAngles = GetConeAngles();
			if(uvec::length_sqr(m_soundSourceData.position) == 0.f && uvec::length_sqr(m_soundSourceData.velocity) == 0.f && m_soundSourceData.coneAngles.first >= 360.f && m_soundSourceData.coneAngles.second >= 360.f) // Note: UpdateMode() has to be called whenever one of these was changed
				mode |= FMOD_2D;
			else
				mode |= FMOD_3D | FMOD_3D_HEADRELATIVE;
		}
	}
	if(mode == oldMode)
		return;
	if((mode &FMOD_3D) == 0 || (oldMode &FMOD_3D) != 0) // No update required if new mode isn't 3D, or if old mode was already 3D
	{
		CheckResultAndUpdateValidity(m_source->setMode(mode));
		return;
	}
	// If this was previously a 2D sound, we have to re-set the 3D attributes
	// after the new mode has been applied
	auto distRange = GetDistanceRange();
	auto pos = GetPosition();
	auto vel = GetVelocity();
	auto coneAngles = GetConeAngles();
	auto dopplerFactor = GetDopplerFactor();
	if(CheckResultAndUpdateValidity(m_source->setMode(mode)) == false)
		return;
	SetDistanceRange(distRange.first,distRange.second);
	SetPosition(pos);
	SetVelocity(vel);
	SetConeAngles(coneAngles.first,coneAngles.second);
	SetDopplerFactor(dopplerFactor);
}
void al::FMSoundChannel::InvalidateSource() const {m_source = nullptr;}
bool al::FMSoundChannel::Is3D() const
{
	if(m_source != nullptr)
	{
		FMOD_MODE mode;
		if(CheckResultAndUpdateValidity(m_source->getMode(&mode)))
			return (mode &FMOD_3D) != 0;
	}
	return false;
}
bool al::FMSoundChannel::Is2D() const {return !Is3D();}
bool al::FMSoundChannel::InitializeChannel()
{
	if(m_source != nullptr || m_buffer.expired())
		return false;
	auto *sound = static_cast<FMSoundBuffer*>(m_buffer.lock().get())->GetFMODSound();
	if(sound == nullptr || CheckResultAndUpdateValidity(static_cast<FMSoundSystem&>(m_system).GetFMODLowLevelSystem().playSound(sound,nullptr,true,&m_source)) == false)
		return false;
	SetOffset(m_soundSourceData.offset);
	SetPriority(m_soundSourceData.priority);
	SetLooping(m_soundSourceData.looping);
	SetPitch(m_soundSourceData.pitch);
	SetGain(m_soundSourceData.gain);
	SetDistanceRange(m_soundSourceData.distanceRange.first,m_soundSourceData.distanceRange.second);
	SetPosition(m_soundSourceData.position);
	SetVelocity(m_soundSourceData.velocity);
	SetConeAngles(m_soundSourceData.coneAngles.first,m_soundSourceData.coneAngles.second);
	SetDopplerFactor(m_soundSourceData.dopplerFactor);
	SetRelative(m_soundSourceData.relativeToListener);
#if ALSYS_STEAM_AUDIO_SUPPORT_ENABLED == 1
	SetChannelGroup(GetChannelGroup());
#endif
	return true;
}
bool al::FMSoundChannel::CheckResultAndUpdateValidity(uint32_t result) const
{
	if(result == FMOD_ERR_INVALID_HANDLE || result == FMOD_ERR_CHANNEL_STOLEN)
	{
		InvalidateSource();
		return false;
	}
	al::check_result(result);
	return result == FMOD_OK;
}

void al::FMSoundChannel::SetGainRange(float minGain,float maxGain)
{
	m_soundSourceData.minGain = minGain;
	m_soundSourceData.maxGain = maxGain;
	SetGain(umath::clamp(GetGain(),minGain,maxGain));
}
std::pair<float,float> al::FMSoundChannel::GetGainRange() const
{
	return {m_soundSourceData.minGain,m_soundSourceData.maxGain};
}

float al::FMSoundChannel::GetMinGain() const
{
	return m_soundSourceData.minGain;
}

float al::FMSoundChannel::GetMaxGain() const
{
	return m_soundSourceData.maxGain;
}

const FMOD::Channel *al::FMSoundChannel::GetInternalSource() const {return const_cast<al::FMSoundChannel*>(this)->GetInternalSource();}
FMOD::Channel *al::FMSoundChannel::GetInternalSource() {return m_source;}
