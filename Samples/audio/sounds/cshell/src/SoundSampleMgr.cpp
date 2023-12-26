//------------------------------------------------------------------------------//
//
// MODULE   : SoundSampleMgr.cpp
//
// PURPOSE  : Sound sample manager - allows access to a hardcoded number of sound samples
//
// CREATED  :
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//

#include "SoundSampleMgr.h"
#include "ClientUtil.h"
#include "clientinterfaces.h"

#include <iltsoundmgr.h>
#include <iclientshell.h>
#include <ltobjectcreate.h>
#include <iltcommon.h>



//-------------------------------------------------------------------------------------------------
// SoundSample
//-------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------
//  Name:       SoundSample
//
//  Purpose:	constructor
//
//----------------------------------------------------------------------------
SoundSample::SoundSample()
	: m_hSound(NULL),
	  m_hObject(NULL)
{
	// Initialize sound description
	PLAYSOUNDINFO_INIT(m_PlaySoundInfo);
	m_PlaySoundInfo.m_dwFlags |= PLAYSOUND_CTRL_VOL |  		// allows us to set the volume
								 PLAYSOUND_CTRL_PITCH | 	// allows us to set the pitch shift
								 PLAYSOUND_GETHANDLE;		// keeps the sound handle around even after the sound finishes
}



//----------------------------------------------------------------------------
//  Name:       ~SoundSample
//
//  Purpose:	destructor
//
//----------------------------------------------------------------------------
SoundSample::~SoundSample()
{
	Stop();
}



//----------------------------------------------------------------------------
//  Name:       SetName
//
//  Purpose:	Set the sample name
//
//----------------------------------------------------------------------------
void SoundSample::SetName(const char *pFileName)
{
	LTStrCpy(m_PlaySoundInfo.m_szSoundName, pFileName, _MAX_PATH - 1);
}



//----------------------------------------------------------------------------
//  Name:       GetSampleType
//
//  Purpose:	Gets the sample type
//
//----------------------------------------------------------------------------
SoundSample::ESampleType SoundSample::GetSampleType() const
{
	ESampleType Type;

	if (IsLocal())
	{
		Type = SOUNDSAMPLE_LOCAL;
	}
	else if (IsAmbient())
	{
		Type = SOUNDSAMPLE_AMBIENT;
	}
	else if (Is3D())
	{
		Type = SOUNDSAMPLE_3D;
	}

	return Type;
}



//----------------------------------------------------------------------------
//  Name:       SetSampleType
//
//  Purpose:	Sets the sample type
//
//----------------------------------------------------------------------------
void SoundSample::SetSampleType(ESampleType SampleType)
{
	switch (SampleType)
	{
		case SOUNDSAMPLE_LOCAL:
			m_PlaySoundInfo.m_dwFlags |= PLAYSOUND_LOCAL;
			m_PlaySoundInfo.m_dwFlags &= ~PLAYSOUND_AMBIENT;
			m_PlaySoundInfo.m_dwFlags &= ~PLAYSOUND_3D;
			break;
		case SOUNDSAMPLE_AMBIENT:
			m_PlaySoundInfo.m_dwFlags &= ~PLAYSOUND_LOCAL;
			m_PlaySoundInfo.m_dwFlags |= PLAYSOUND_AMBIENT;
			m_PlaySoundInfo.m_dwFlags &= ~PLAYSOUND_3D;
			break;
		case SOUNDSAMPLE_3D:
			m_PlaySoundInfo.m_dwFlags &= ~PLAYSOUND_LOCAL;
			m_PlaySoundInfo.m_dwFlags &= ~PLAYSOUND_AMBIENT;
			m_PlaySoundInfo.m_dwFlags |= PLAYSOUND_3D;
			break;
	}
}



//----------------------------------------------------------------------------
//  Name:       IncSampleType
//
//  Purpose:	Increment the sample type
//
//----------------------------------------------------------------------------
void SoundSample::IncSampleType(int Offset)
{
	int Index = static_cast<int>(GetSampleType());

	if (Offset > 0)
	{
		++Index;
	}
	else
	{
		--Index;
	}

	ClientUtil::WrapToRange(&Index, static_cast<int>(SOUNDSAMPLE_LOCAL), static_cast<int>(SOUNDSAMPLE_3D));
	SetSampleType(static_cast<ESampleType>(Index));
}



//----------------------------------------------------------------------------
//  Name:       SetLooped
//
//  Purpose:	Sets the sample loop type
//
//----------------------------------------------------------------------------
void SoundSample::SetLooped(bool Loop)
{
	if (Loop)
	{
		m_PlaySoundInfo.m_dwFlags |= PLAYSOUND_LOOP;
	}
	else
	{
		m_PlaySoundInfo.m_dwFlags &= ~PLAYSOUND_LOOP;
	}
}



//----------------------------------------------------------------------------
//  Name:       IncPriority
//
//  Purpose:	Increment the priority by the given offset
//
//----------------------------------------------------------------------------
void SoundSample::IncPriority(int Offset)
{
	int Priority = GetPriority();
	Priority += Offset;
	ClientUtil::WrapToRange(&Priority, 0, 255);

	SetPriority(static_cast<unsigned char>(Priority));
}



//----------------------------------------------------------------------------
//  Name:       IncOuterRadius
//
//  Purpose:	Increment the outer radius by the given offset
//
//----------------------------------------------------------------------------
void SoundSample::IncOuterRadius(float Offset)
{
	float Radius = GetOuterRadius();
	Radius += Offset;

	if (Radius < GetInnerRadius())
	{
		Radius = GetInnerRadius();
	}
	if (Radius < 1.0f)
	{
		Radius = 1.0f;
	}

	SetOuterRadius(Radius);
}



//----------------------------------------------------------------------------
//  Name:       IncInnerRadius
//
//  Purpose:	Increment the inner radius by the given offset
//
//----------------------------------------------------------------------------
void SoundSample::IncInnerRadius(float Offset)
{
	float Radius = GetInnerRadius();
	Radius += Offset;

	if (Radius > GetOuterRadius())
	{
		Radius = GetOuterRadius();
	}
	if (Radius < 1.0f)
	{
		Radius = 1.0f;
	}

	SetInnerRadius(Radius);
}



//----------------------------------------------------------------------------
//  Name:       IncVolume
//
//  Purpose:	Increment the volume
//
//----------------------------------------------------------------------------
void SoundSample::IncVolume(int Offset)
{
	int Vol = GetVolume();
	Vol += Offset;
	ClientUtil::WrapToRange(&Vol, 0, 100);
	SetVolume(static_cast<uint8>(Vol));
}



//----------------------------------------------------------------------------
//  Name:       IncPitchShift
//
//  Purpose:	Increment the pitch shift
//
//----------------------------------------------------------------------------
void SoundSample::IncPitchShift(float Offset)
{
	float Shift = GetPitchShift();
	Shift += Offset;
	SetPitchShift(Shift);
}



//----------------------------------------------------------------------------
//  Name:       Play
//
//  Purpose:	Plays the sample
//
//----------------------------------------------------------------------------
bool SoundSample::IsPlaying()
{
	if (m_hSound != NULL)
	{
		bool bDone = true;
		if (g_pLTCSoundMgr->IsSoundDone(m_hSound, bDone) == LT_OK)
		{
			return bDone == false;
		}
	}

	return false;
}



//----------------------------------------------------------------------------
//  Name:       Play
//
//  Purpose:	Plays the sample
//
//----------------------------------------------------------------------------
bool SoundSample::Play()
{
	return g_pLTCSoundMgr->PlaySound(&m_PlaySoundInfo, m_hSound) == LT_OK;
}



//----------------------------------------------------------------------------
//  Name:       Stop
//
//  Purpose:	Stops the sample
//
//----------------------------------------------------------------------------
bool SoundSample::Stop()
{
	if (m_hSound != NULL &&
		g_pLTCSoundMgr->KillSound(m_hSound) == LT_OK)
	{
		m_hSound = NULL;
		m_PlaySoundInfo.m_hSound = NULL;

		return true;
	}

	return false;
}



//-------------------------------------------------------------------------------------------------
// SoundSampleMgr
//-------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------
//  Name:       SoundSampleMgr
//
//  Purpose:	constructor
//
//----------------------------------------------------------------------------
SoundSampleMgr::SoundSampleMgr()
	: m_CurrentSample(-1)
{
}



//----------------------------------------------------------------------------
//  Name:       ~SoundSampleMgr
//
//  Purpose:	destructor
//
//----------------------------------------------------------------------------
SoundSampleMgr::~SoundSampleMgr()
{
	for (LTSoundSamples::iterator iterSample(m_Samples.begin()); iterSample != m_Samples.end(); ++iterSample)
	{
		delete *iterSample;
	}
	m_Samples.clear();
}



//----------------------------------------------------------------------------
//  Name:       Init
//
//  Purpose:	Initializes the sound manager
//
//----------------------------------------------------------------------------
LTRESULT SoundSampleMgr::Init()
{
	// Initialize sound
	InitSoundInfo sndinfo;
	sndinfo.Init();
	sndinfo.m_nNumSWVoices = NUM_SOUND_SAMPLES;
	sndinfo.m_nNum3DVoices = NUM_SOUND_SAMPLES;
	strcpy(sndinfo.m_sz3DProvider, "DirectSound Hardware");


	// Set distance factor lower than default so velocity is factored correctly because we are moving very fast in
	// this sample ( basically moving from point A to point B instantainously )
	sndinfo.m_fDistanceFactor = 0.0001f;

	//  Get our ILTClientSoundMgr pointer
	g_pLTCSoundMgr->InitSound(&sndinfo);

	return LT_OK;
}



//----------------------------------------------------------------------------
//  Name:       Init
//
//  Purpose:	Initializes the sound manager
//
//----------------------------------------------------------------------------
LTRESULT SoundSampleMgr::Term()
{
	return LT_OK;
}



//----------------------------------------------------------------------------
//  Name:       LoadSounds
//
//  Purpose:	Loads the sound files and associated models
//
//----------------------------------------------------------------------------
LTRESULT SoundSampleMgr::LoadSounds()
{
	if (g_pLTCSoundMgr == NULL)
	{
		return LT_ERROR;
	}

	// Create the samples.
	CreateSample("sounds/Alarm_undwata.wav", 		LTVector(-400.0f, -150.0f,    0.0f));
	CreateSample("sounds/storm_3.WAV", 				LTVector( 400.0f, -150.0f,    0.0f));
	CreateSample("sounds/Dogdisturbance_3.WAV", 	LTVector(   0.0f, -150.0f,  400.0f));
	CreateSample("sounds/Radiochatter_1.WAV", 		LTVector(   0.0f, -150.0f, -400.0f));
	CreateSample("sounds/C03UnityBmp.wav", 			LTVector( 200.0f, -150.0f, -200.0f));

	m_CurrentSample = 0;
	SetSelectedModel(true);

	return LT_OK;
}



//----------------------------------------------------------------------------
//  Name:       CreateSample
//
//  Purpose:	Loads a sound file and model
//
//----------------------------------------------------------------------------
void SoundSampleMgr::CreateSample(const char *pFilename, const LTVector &Pos)
{
	SoundSample *pSample = new SoundSample;

	// Set the data members.
	pSample->SetName(pFilename);
	pSample->SetLooped(false);
	pSample->SetPriority(255);
	pSample->SetOuterRadius(250.0f);
	pSample->SetInnerRadius(100.0f);
	pSample->SetVolume(100);
	pSample->SetPosition(Pos);

	// Load the model for this sound.
	std::vector<char*> Skins;
	LTVector Scale(3.0f, 3.0f, 3.0f);
	LTRotation Rot;
	Skins.push_back("ModelTextures\\GenRadio.dtx");
	HLOCALOBJ hObject = LoadModel("Models\\GenRadio.ltb", Skins, Pos, Scale, Rot, 0);

	// Set the model.
	pSample->SetModel(hObject);

	// Add the new sample to the list.
	m_Samples.push_back(pSample);
}



//----------------------------------------------------------------------------
//  Name:       LoadModel
//
//  Purpose:	Loads a model using the given model parameters.
//
//----------------------------------------------------------------------------
HLOCALOBJ SoundSampleMgr::LoadModel(const char *pModelFilename, const std::vector<char*> &Skins,
							   		const LTVector &Pos, const LTVector &Scale, const LTRotation &Rot, const char *pAnimName)
{
	HLOCALOBJ hObject = NULL;

	ObjectCreateStruct ocs;
	ocs.Clear();

	ocs.m_ObjectType 	= OT_MODEL;
	ocs.m_Flags 		= FLAG_VISIBLE | FLAG_RAYHIT;
	strcpy(ocs.m_Filename, pModelFilename);

	unsigned NumSkins = Skins.size();
	if (NumSkins == 1)
	{
		strcpy(ocs.m_SkinName, Skins.front());
	}
	else
	{
		for (unsigned i = 0; i < MAX_MODEL_TEXTURES; ++i)
		{
			if (i < NumSkins)
			{
				strcpy(ocs.m_SkinNames[i], Skins[i]);
			}
			else
			{
				ocs.m_SkinNames[i][0] = '\0';
			}
		}
	}

	ocs.m_Pos 		= Pos;
	ocs.m_Scale 	= Scale;
	ocs.m_Rotation 	= Rot;

	// Create the client object.
	hObject = g_pLTClient->CreateObject(&ocs);

	// Start an animation.
	if (hObject != NULL && pAnimName != NULL)
	{
		HMODELANIM hAnim = g_pLTClient->GetAnimIndex(hObject, (char*)pAnimName);
		g_pLTClient->SetModelAnimation(hObject, hAnim);
		g_pLTClient->SetModelLooping(hObject, LTTRUE);
	}

	return hObject;
}



//----------------------------------------------------------------------------
//  Name:       GetCurrentSample
//
//  Purpose:	Returns the current sample
//
//----------------------------------------------------------------------------
SoundSample* SoundSampleMgr::GetCurrentSample()
{
	SoundSample *pSample = NULL;

	if (m_CurrentSample >= 0 && m_CurrentSample < static_cast<int>(m_Samples.size()))
	{
		pSample = m_Samples[m_CurrentSample];
	}

	return pSample;
}

//----------------------------------------------------------------------------
//  Name:       IsSampleObject
//
//  Purpose:	Given an object is this one of our samples
//
//----------------------------------------------------------------------------
bool SoundSampleMgr::IsSampleObject( HOBJECT hObj )
{
	int i = 0;

	while ( i < static_cast<int>(m_Samples.size()) )
	{
		if ( m_Samples[i]->GetObjectHandle() == hObj)
			return true;

		i++;
	}

	return false;

}

//----------------------------------------------------------------------------
//  Name:       SetCurrentSample
//
//  Purpose:	Set the current sample given its object
//
//----------------------------------------------------------------------------
void SoundSampleMgr::SetCurrentSample(HOBJECT hObj)
{
	int sample = 0;

	while ( sample < static_cast<int>(m_Samples.size()) )
	{
		if ( m_Samples[sample]->GetObjectHandle() == hObj)
		{
			// Reset the model color.
			SetSelectedModel(false);

			m_CurrentSample = sample;

			// Set the model color for the new selected model.
			SetSelectedModel(true);

			return;
		}

		sample++;
	}

}
//----------------------------------------------------------------------------
//  Name:		PlayCurrentSample
//
//  Purpose:	Play the current selected sample 
//
//----------------------------------------------------------------------------
void SoundSampleMgr::PlayCurrentSample()
{
	SoundSample *pSample = GetCurrentSample();
	if (pSample != NULL)
	{
		if (!pSample->IsPlaying())
		{
			pSample->Play();
		}
	}
}



//----------------------------------------------------------------------------
//  Name:       IncCurrentSample
//
//  Purpose:	Increment the current selected sample
//
//----------------------------------------------------------------------------
void SoundSampleMgr::IncCurrentSample(int Offset)
{
	// Reset the model color.
	SetSelectedModel(false);

	m_CurrentSample += Offset;
	ClientUtil::WrapToRange(&m_CurrentSample, 0, static_cast<int>(m_Samples.size() - 1));

	// Set the model color for the new selected model.
	SetSelectedModel(true);
}



//----------------------------------------------------------------------------
//  Name:       SetSelectedModel
//
//  Purpose:	Set the selected model attributes
//
//----------------------------------------------------------------------------
void SoundSampleMgr::SetSelectedModel(bool bOn)
{
	SoundSample *pSample = GetCurrentSample();
	if (pSample != NULL)
	{
		g_pLTCCommon->SetObjectFlags(GetCurrentSample()->GetObjectHandle(), OFT_Flags, (bOn ? FLAG_NOLIGHT : 0), FLAG_NOLIGHT);
	}
}



//----------------------------------------------------------------------------
//  Name:       ApplyCommand
//
//  Purpose:	Apply a command to the curent sample.
//              "bUp" indicates the direction of change (up or down).
//
//----------------------------------------------------------------------------
void SoundSampleMgr::ApplyCommand(EGuiCommand CommandID, bool bUp)
{
	SoundSample *pSample = GetCurrentSample();
	if (pSample != NULL)
	{
		switch (CommandID)
		{
			case GUICOMMAND_SAMPLE_NAME:
				IncCurrentSample(bUp ? 1 : -1);
				break;
			case GUICOMMAND_SAMPLE_TYPE:
				pSample->IncSampleType(bUp ? 1 : -1);
				break;
			case GUICOMMAND_PRIORITY:
				pSample->IncPriority(bUp ? 1 : -1);
				break;
			case GUICOMMAND_OUTER_RADIUS:
				pSample->IncOuterRadius(bUp ? 1.0f : -1.0f);
				break;
			case GUICOMMAND_INNER_RADIUS:
				pSample->IncInnerRadius(bUp ? 1.0f : -1.0f);
				break;
			case GUICOMMAND_VOLUME:
				pSample->IncVolume(bUp ? 1 : -1);
				break;
			case GUICOMMAND_PITCH_SHIFT:
				pSample->IncPitchShift(bUp ? 1.0f : -1.0f);
				break;
			case GUICOMMAND_POSITION:
				break;
			case GUICOMMAND_LOOPED:
				pSample->SetLooped(!pSample->IsLooped());
				break;
			case GUICOMMAND_PLAY:
				if (pSample->IsPlaying())
				{
					pSample->Stop();
				}
				else
				{
					pSample->Play();
				}
				break;
		}
	}
}
