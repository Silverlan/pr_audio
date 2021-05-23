/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fmod_sound_buffer.hpp"
#include "fmod_sound_system.hpp"
#include <fmod_studio.hpp>

al::FMSoundBuffer::FMSoundBuffer(FMOD::System &system,const std::shared_ptr<FMOD::Sound> &sound)
	: m_fmSystem(system),m_fmSound(sound)
{}
const FMOD::Sound *al::FMSoundBuffer::GetFMODSound() const {return const_cast<al::FMSoundBuffer*>(this)->GetFMODSound();}
FMOD::Sound *al::FMSoundBuffer::GetFMODSound() {return m_fmSound.get();}

al::FMSoundBuffer::~FMSoundBuffer()
{
}

bool al::FMSoundBuffer::IsReady() const
{
	FMOD_OPENSTATE openState;
	uint32_t percentBuffered;
	bool starving,diskBusy;
	al::check_result(m_fmSound->getOpenState(&openState,&percentBuffered,&starving,&diskBusy));
	return openState != FMOD_OPENSTATE_LOADING && 
		openState != FMOD_OPENSTATE_ERROR && 
		openState != FMOD_OPENSTATE_CONNECTING; // TODO: What about FMOD_OPENSTATE_BUFFERING?
}

uint64_t al::FMSoundBuffer::GetLength() const
{
	if(IsReady() == false)
		return 0;
	auto length = 0u;
	al::check_result(m_fmSound->getLength(&length,FMOD_TIMEUNIT_PCM));
	return length;
}
uint32_t al::FMSoundBuffer::GetFrequency() const
{
	float frequency;
	int32_t priority;
	al::check_result(m_fmSound->getDefaults(&frequency,&priority));
	return static_cast<uint32_t>(frequency);
}
al::ChannelConfig al::FMSoundBuffer::GetChannelConfig() const
{
	int32_t channels;
	al::check_result(m_fmSound->getFormat(nullptr,nullptr,&channels,nullptr));
	return (channels >= 2) ? al::ChannelConfig::Stereo : al::ChannelConfig::Mono;
}
al::SampleType al::FMSoundBuffer::GetSampleType() const
{
	FMOD_SOUND_FORMAT format;
	al::check_result(m_fmSound->getFormat(nullptr,&format,nullptr,nullptr));
	switch(format)
	{
		case FMOD_SOUND_FORMAT_PCMFLOAT:
			return al::SampleType::Float32;
		case FMOD_SOUND_FORMAT_PCM8:
			return al::SampleType::UInt8;
		case FMOD_SOUND_FORMAT_PCM16:
			return al::SampleType::Int16;
		default:
			// FMOD TODO
			throw std::runtime_error("Unsupported sample type " +std::to_string(format));
			break;
	}
}
uint32_t al::FMSoundBuffer::GetSize() const
{
	// FMOD TODO
	return 0u;
}
void al::FMSoundBuffer::SetLoopFramePoints(uint32_t start,uint32_t end)
{
	al::check_result(m_fmSound->setLoopPoints(start,FMOD_TIMEUNIT_PCM,end,FMOD_TIMEUNIT_PCM));
}
void al::FMSoundBuffer::SetLoopTimePoints(float tStart,float tEnd)
{
	auto dur = GetDuration();
	auto start = 0u;
	auto end = 0u;
	if(dur > 0.f)
	{
		tStart /= dur;
		tEnd /= dur;

		auto l = GetLength();
		start = umath::round(tStart *l);
		end = umath::round(tEnd *l);
	}
	SetLoopFramePoints(start,end);
}
std::pair<uint64_t,uint64_t> al::FMSoundBuffer::GetLoopFramePoints() const
{
	uint32_t start,end;
	al::check_result(m_fmSound->getLoopPoints(&start,FMOD_TIMEUNIT_PCM,&end,FMOD_TIMEUNIT_PCM));
	return {start,end};
}

std::string al::FMSoundBuffer::GetName() const
{
	// FMOD TODO
	static std::string r = "";
	return r;
}
bool al::FMSoundBuffer::IsInUse() const
{
	// FMOD TODO
	return true;
}
