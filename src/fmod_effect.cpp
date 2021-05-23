/* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "fmod_effect.hpp"
#include "fmod_sound_system.hpp"
#include <fmod_studio.hpp>

al::FMEffect::FMEffect(ISoundSystem &soundSys,const std::shared_ptr<FMOD::DSP> &dsp)
	: IEffect{soundSys},m_fmDsp(dsp)
{}
const std::shared_ptr<FMOD::DSP> &al::FMEffect::GetFMODDsp() const {return const_cast<al::FMEffect*>(this)->GetFMODDsp();}
std::shared_ptr<FMOD::DSP> &al::FMEffect::GetFMODDsp() {return m_fmDsp;}

void al::FMEffect::SetProperties(al::EfxChorusProperties props)
{
	FMOD::DSP *dsp;
	al::check_result(static_cast<FMSoundSystem&>(m_soundSystem).GetFMODLowLevelSystem().createDSPByType(FMOD_DSP_TYPE_CHORUS,&dsp));
	m_fmDsp = std::shared_ptr<FMOD::DSP>(dsp,[](FMOD::DSP *dsp) {
		al::check_result(dsp->release());
	});
	dsp->setParameterFloat(FMOD_DSP_ECHO_DELAY,props.flDelay);
	dsp->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK,props.flFeedback);
	//dsp->setParameterFloat(FMOD_DSP_ECHO_DRYLEVEL,props.);
	//dsp->setParameterFloat(FMOD_DSP_ECHO_WETLEVEL,props.);
}

void al::FMEffect::SetProperties(al::EfxEaxReverbProperties props)
{
	// TODO
}

void al::FMEffect::SetProperties(al::EfxDistortionProperties props)
{
	FMOD::DSP *dsp;
	al::check_result(static_cast<FMSoundSystem&>(m_soundSystem).GetFMODLowLevelSystem().createDSPByType(FMOD_DSP_TYPE_DISTORTION,&dsp));
	m_fmDsp = std::shared_ptr<FMOD::DSP>(dsp,[](FMOD::DSP *dsp) {
		al::check_result(dsp->release());
	});
	dsp->setParameterFloat(FMOD_DSP_DISTORTION_LEVEL,props.flGain);
}
void al::FMEffect::SetProperties(al::EfxEchoProperties props)
{
	FMOD::DSP *dsp;
	al::check_result(static_cast<FMSoundSystem&>(m_soundSystem).GetFMODLowLevelSystem().createDSPByType(FMOD_DSP_TYPE_ECHO,&dsp));
	m_fmDsp = std::shared_ptr<FMOD::DSP>(dsp,[](FMOD::DSP *dsp) {
		al::check_result(dsp->release());
	});
	dsp->setParameterFloat(FMOD_DSP_ECHO_DELAY,props.flDelay);
	dsp->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK,props.flFeedback);
	//dsp->setParameterFloat(FMOD_DSP_ECHO_DRYLEVEL,props.);
	//dsp->setParameterFloat(FMOD_DSP_ECHO_WETLEVEL,props.);
}
void al::FMEffect::SetProperties(al::EfxFlangerProperties props)
{
	FMOD::DSP *dsp;
	al::check_result(static_cast<FMSoundSystem&>(m_soundSystem).GetFMODLowLevelSystem().createDSPByType(FMOD_DSP_TYPE_FLANGE,&dsp));
	m_fmDsp = std::shared_ptr<FMOD::DSP>(dsp,[](FMOD::DSP *dsp) {
		al::check_result(dsp->release());
	});
	//dsp->setParameterFloat(FMOD_DSP_FLANGE_MIX,props.); // FMOD TODO
	dsp->setParameterFloat(FMOD_DSP_FLANGE_DEPTH,props.flDepth);
	dsp->setParameterFloat(FMOD_DSP_FLANGE_RATE,props.flRate);
}
void al::FMEffect::SetProperties(al::EfxFrequencyShifterProperties props)
{
	// FMOD TODO
}
void al::FMEffect::SetProperties(al::EfxVocalMorpherProperties props)
{
	// FMOD TODO
}
void al::FMEffect::SetProperties(al::EfxPitchShifterProperties props)
{
	// FMOD TODO
}
void al::FMEffect::SetProperties(al::EfxRingModulatorProperties props)
{
	// FMOD TODO
}
void al::FMEffect::SetProperties(al::EfxAutoWahProperties props)
{
	// FMOD TODO
}
void al::FMEffect::SetProperties(al::EfxCompressor props)
{
	// FMOD TODO
}
void al::FMEffect::SetProperties(al::EfxEqualizer props)
{
	// FMOD TODO
}
