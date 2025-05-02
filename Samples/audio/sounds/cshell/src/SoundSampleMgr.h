//------------------------------------------------------------------------------//
//
// MODULE   : SoundSampleMgr.h
//
// PURPOSE  : Sound sample manager - allows access to a hardcoded number of sound samples
//
// CREATED  :
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//


#ifndef __SOUNDSAMPLEMGR_H__
#define __SOUNDSAMPLEMGR_H__


#include <iltclient.h>
#include <ltbasedefs.h>

#include "commandids.h"

#include <vector>



class SoundSample
{
public:

	enum ESampleType
	{
		SOUNDSAMPLE_LOCAL,
		SOUNDSAMPLE_AMBIENT,
		SOUNDSAMPLE_3D
	};

public:

	SoundSample();
	~SoundSample();

	HLTSOUND			GetSoundHandle()						{ return m_hSound; }
	HLOCALOBJ			GetObjectHandle()						{ return m_hObject; }

	// sound model
	void				SetModel(HLOCALOBJ hObject)				{ m_hObject = hObject; }

	// sample name
	const char*			GetName() const							{ return m_PlaySoundInfo.m_szSoundName; }
	void				SetName(const char *pFileName);

	// sample type
	bool				IsLocal() const							{ return !IsAmbient() && !Is3D(); }
	bool				IsAmbient() const						{ return (m_PlaySoundInfo.m_dwFlags & PLAYSOUND_AMBIENT) == PLAYSOUND_AMBIENT; }
	bool				Is3D() const							{ return (m_PlaySoundInfo.m_dwFlags & PLAYSOUND_3D) == PLAYSOUND_3D; }
	ESampleType			GetSampleType() const;
	void 				SetSampleType(ESampleType SampleType);
	void				IncSampleType(int Offset);

	// loop
	bool				IsLooped() const						{ return (m_PlaySoundInfo.m_dwFlags & PLAYSOUND_LOOP) == PLAYSOUND_LOOP; }
	void				SetLooped(bool Loop);

	// streaming
	//bool				IsStreamed() const						{ return m_PlaySoundInfo.m_bStream; }
	//void 				SetStreamed(bool bStream)				{ m_PlaySoundInfo.m_bStream = bStream; }

	// priority
	unsigned char		GetPriority() const						{ return m_PlaySoundInfo.m_nPriority; }
	void				SetPriority(unsigned char Priority)		{ m_PlaySoundInfo.m_nPriority = Priority; }
	void				IncPriority(int Offset);

	// outer radius
	float				GetOuterRadius() const					{ return m_PlaySoundInfo.m_fOuterRadius; }
	void				SetOuterRadius(float OuterRadius)		{ m_PlaySoundInfo.m_fOuterRadius = OuterRadius; }
	void				IncOuterRadius(float Offset);

	// inner radius
	float				GetInnerRadius() const					{ return m_PlaySoundInfo.m_fInnerRadius; }
	void				SetInnerRadius(float InnerRadius)		{ m_PlaySoundInfo.m_fInnerRadius = InnerRadius; }
	void				IncInnerRadius(float Offset);

	// volume
	uint8				GetVolume() const						{ return m_PlaySoundInfo.m_nVolume; }
	void				SetVolume(uint8 Vol)					{ m_PlaySoundInfo.m_nVolume = Vol; }
	void				IncVolume(int Offset);

	// pitch shift
	float				GetPitchShift() const					{ return m_PlaySoundInfo.m_fPitchShift; }
	void				SetPitchShift(float PitchShift)			{ m_PlaySoundInfo.m_fPitchShift = PitchShift; }
	void				IncPitchShift(float Offset);

	// position
	const LTVector&		GetPosition() const						{ return m_PlaySoundInfo.m_vPosition; }
	void				SetPosition(const LTVector &Pos)		{ m_PlaySoundInfo.m_vPosition = Pos; }

	bool				IsPlaying();
	bool				Play();
	bool				Stop();

protected:

	PlaySoundInfo		m_PlaySoundInfo;	// sound parameters
	HLTSOUND			m_hSound;			// sound handle
	HLOCALOBJ			m_hObject;			// model object handle
};



class SoundSampleMgr
{
public:

	enum { NUM_SOUND_SAMPLES = 6 };

public:

	SoundSampleMgr();
	~SoundSampleMgr();

	LTRESULT		Init();
	LTRESULT		Term();

	LTRESULT		LoadSounds();

	SoundSample*	GetCurrentSample();

	// Apply a command to the current sample. "bUp" indicates the direction of change (up or down).
	void			ApplyCommand(EGuiCommand CommandID, bool bUp);

	bool			IsSampleObject( HOBJECT hObj );
	void			SetCurrentSample(HOBJECT hObj);
	void			PlayCurrentSample();

protected:

	void			CreateSample(const char *pFilename, const LTVector &Pos);
	HLOCALOBJ 		LoadModel(const char *pModelFilename, const std::vector<const char*> &Skins,
							  const LTVector &Pos, const LTVector &Scale, const LTRotation &Rot, const char *pAnimName);
	void			IncCurrentSample(int Offset);

	void			SetSelectedModel(bool bOn);

protected:

	typedef std::vector<SoundSample*> LTSoundSamples;
	LTSoundSamples	m_Samples;							// list of samples

	int 			m_CurrentSample;					// index to currently selected sample
};



#endif // __SOUNDSAMPLEMGR_H__
