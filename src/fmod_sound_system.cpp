/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fmod_sound_system.hpp"
#include "fmod_sound_buffer.hpp"
#include "fmod_sound_source.hpp"
#include "fmod_listener.hpp"
#include <fmod_studio.hpp>
#include <fmod_errors.h>
#include <fsys/filesystem.h>
#include <cstring>

void al::check_result(uint32_t r)
{
	if(r != FMOD_OK)
	{
		std::cout<<"[FMOD] Error: "<<std::string(FMOD_ErrorString(static_cast<FMOD_RESULT>(r)))<<std::endl;
		// throw std::runtime_error("FMOD error: " +std::string(FMOD_ErrorString(static_cast<FMOD_RESULT>(r))));
	}
}

std::shared_ptr<al::FMSoundSystem> al::FMSoundSystem::Create(const std::string &deviceName,float metersPerUnit)
{
	FMOD::Studio::System *system = nullptr;
	al::check_result(FMOD::Studio::System::create(&system));
	auto ptrSystem = std::shared_ptr<FMOD::Studio::System>(system,[](FMOD::Studio::System *system) {
		al::check_result(system->release());
	});

	FMOD::System *lowLevelSystem = nullptr;
	al::check_result(system->getCoreSystem(&lowLevelSystem));
	al::check_result(lowLevelSystem->setSoftwareFormat(0,FMOD_SPEAKERMODE_5POINT1,0));

	void *extraDriverData = nullptr;
	al::check_result(system->initialize(1'024,FMOD_STUDIO_INIT_NORMAL,FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED | FMOD_INIT_VOL0_BECOMES_VIRTUAL,extraDriverData));
	al::check_result(lowLevelSystem->setFileSystem(
		[](const char *name,uint32_t *fileSize,void **handle,void *userData) -> FMOD_RESULT {
			auto f = FileManager::OpenFile(name,"rb");
			if(f == nullptr)
				return FMOD_RESULT::FMOD_ERR_FILE_NOTFOUND;
			*fileSize = f->GetSize();
			*handle = new VFilePtr(f);
			return FMOD_RESULT::FMOD_OK;
		},[](void *handle,void *userData) -> FMOD_RESULT {
			delete static_cast<VFilePtr*>(handle);
			return FMOD_RESULT::FMOD_OK;
		},[](void *handle,void *buffer,uint32_t sizeBytes,uint32_t *bytesRead,void *userData) -> FMOD_RESULT {
			*bytesRead = (*static_cast<VFilePtr*>(handle))->Read(buffer,sizeBytes);
			if((*static_cast<VFilePtr*>(handle))->Eof())
				return FMOD_RESULT::FMOD_ERR_FILE_EOF;
			return FMOD_RESULT::FMOD_OK;
		},[](void *handle,uint32_t pos,void *userData) -> FMOD_RESULT {
			(*static_cast<VFilePtr*>(handle))->Seek(pos);
			return FMOD_RESULT::FMOD_OK;
		},nullptr,nullptr,-1
	));
	auto soundSys = std::shared_ptr<FMSoundSystem>(new FMSoundSystem(ptrSystem,*lowLevelSystem,metersPerUnit),[](FMSoundSystem *sys) {
		sys->OnRelease();
		delete sys;
	});
	soundSys->Initialize();
	return soundSys;
}

void al::FMSoundSystem::Update()
{
	ISoundSystem::Update();
	al::check_result(m_fmSystem->update());
}

std::shared_ptr<al::FMSoundSystem> al::FMSoundSystem::Create(float metersPerUnit) {return Create("",metersPerUnit);}
const FMOD::Studio::System &al::FMSoundSystem::GetFMODSystem() const {return const_cast<FMSoundSystem*>(this)->GetFMODSystem();}
FMOD::Studio::System &al::FMSoundSystem::GetFMODSystem() {return *m_fmSystem;}
const FMOD::System &al::FMSoundSystem::GetFMODLowLevelSystem() const {return const_cast<FMSoundSystem*>(this)->GetFMODLowLevelSystem();}
FMOD::System &al::FMSoundSystem::GetFMODLowLevelSystem() {return m_fmLowLevelSystem;}
al::FMSoundSystem::FMSoundSystem(const std::shared_ptr<FMOD::Studio::System> &fmSystem,FMOD::System &lowLevelSystem,float metersPerUnit)
	: ISoundSystem{metersPerUnit},m_fmSystem(fmSystem),m_fmLowLevelSystem(lowLevelSystem)
{
	lowLevelSystem.set3DSettings(1.f,1.f,1.f);
	// FMOD TODO
	//SetSpeedOfSound(340.29f /metersPerUnit);
}

void al::FMSoundSystem::OnRelease()
{
	ISoundSystem::OnRelease();
	m_fmSystem = nullptr;
}

std::unique_ptr<al::IListener> al::FMSoundSystem::CreateListener() {return std::unique_ptr<FMListener>{new FMListener{*this}};}

al::ISoundBuffer *al::FMSoundSystem::DoLoadSound(const std::string &normPath,bool bConvertToMono,bool bAsync)
{
	FMOD::Sound *sound = nullptr;
	FMOD_CREATESOUNDEXINFO exInfo {};
	memset(&exInfo,0,sizeof(exInfo));
	exInfo.cbsize = sizeof(exInfo);
	al::check_result(m_fmLowLevelSystem.createSound(normPath.c_str(),FMOD_DEFAULT,&exInfo,&sound));
	if(!sound)
		return nullptr;
	auto ptrSound = std::shared_ptr<FMOD::Sound>(sound,[](FMOD::Sound *sound) {
		al::check_result(sound->release());
	});
	auto buf = PSoundBuffer(new FMSoundBuffer(m_fmLowLevelSystem,ptrSound));
	if(buf->GetChannelConfig() == al::ChannelConfig::Mono || bConvertToMono == true)
		m_buffers[normPath].mono = buf;
	else
		m_buffers[normPath].stereo = buf;
	if(bConvertToMono == true)
		buf->SetTargetChannelConfig(al::ChannelConfig::Mono);
	return buf.get();
}

al::PSoundChannel al::FMSoundSystem::CreateChannel(ISoundBuffer &buffer)
{
	FMOD::Channel *channel;
	al::check_result(m_fmLowLevelSystem.playSound(static_cast<FMSoundBuffer&>(buffer).GetFMODSound(),nullptr,true,&channel));
	auto snd = std::make_shared<FMSoundChannel>(*this,buffer);
	if(snd == nullptr)
		return nullptr;
	snd->SetSource(channel);
	FMOD_MODE mode;
	al::check_result(channel->getMode(&mode));
	mode &= ~(FMOD_3D_HEADRELATIVE | FMOD_3D_WORLDRELATIVE | FMOD_3D | FMOD_2D);
	mode |= FMOD_3D_WORLDRELATIVE | FMOD_3D;

	channel->setMode(mode);
	return snd;
}
al::PSoundChannel al::FMSoundSystem::CreateChannel(Decoder &decoder) {return nullptr;}

al::PDecoder al::FMSoundSystem::CreateDecoder(const std::string &path,bool bConvertToMono)
{
	return nullptr;
}

std::vector<std::string> al::FMSoundSystem::GetHRTFNames() const
{
	return {}; // FMOD TODO
}

std::string al::FMSoundSystem::GetCurrentHRTF() const
{
	return ""; // FMOD TODO
}
bool al::FMSoundSystem::IsHRTFEnabled() const
{
	return false; // FMOD TODO
}

void al::FMSoundSystem::SetHRTF(uint32_t id)
{
	// FMOD TODO
}
void al::FMSoundSystem::DisableHRTF()
{
	// FMOD TODO
}

uint32_t al::FMSoundSystem::GetMaxAuxiliaryEffectsPerSource() const
{
	return 0u; // FMOD TODO
}

bool al::FMSoundSystem::IsSupported(ChannelConfig channels,SampleType type) const
{
	return true; // FMOD TODO
}

float al::FMSoundSystem::GetDopplerFactor() const {return m_dopplerFactor;}
void al::FMSoundSystem::SetDopplerFactor(float factor)
{
	m_dopplerFactor = factor;
	// FMOD TODO
}

float al::FMSoundSystem::GetSpeedOfSound() const {return m_speedOfSound;}
void al::FMSoundSystem::SetSpeedOfSound(float speed)
{
	m_speedOfSound = speed;
	// FMOD TODO
}

std::string al::FMSoundSystem::GetDeviceName() const
{
	return "";// FMOD TODO
}

void al::FMSoundSystem::PauseDeviceDSP()
{
	// FMOD TODO
}
void al::FMSoundSystem::ResumeDeviceDSP()
{
	// FMOD TODO
}

al::IAuxiliaryEffectSlot *al::FMSoundSystem::CreateAuxiliaryEffectSlot()
{
	return nullptr; // FMOD TODO
}

al::DistanceModel al::FMSoundSystem::GetDistanceModel() const {return m_distanceModel;}
void al::FMSoundSystem::SetDistanceModel(DistanceModel mdl)
{
	m_distanceModel = mdl;
	// FMOD TODO
}

al::PEffect al::FMSoundSystem::CreateEffect()
{
	return nullptr; // FMOD TODO
}
