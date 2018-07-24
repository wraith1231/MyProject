#include "stdafx.h"
#include "ModelAnimationClip.h"

ModelAnimClip::ModelAnimClip()
	: defaltFrameRate(0.0)
{
}

ModelAnimClip::~ModelAnimClip()
{
	for (pair<wstring, ModelKeyframe *> temp : keyframeMap)
		SAFE_DELETE(temp.second);
}

void ModelAnimClip::Clone(void ** clone)
{
	ModelAnimClip* clip = new ModelAnimClip();
	clip->name = name;

	clip->totalFrame = totalFrame;
	clip->frameRate = frameRate;
	clip->defaltFrameRate = defaltFrameRate;

	for (pair<wstring, ModelKeyframe *> key : keyframeMap)
	{
		ModelKeyframe* temp = NULL;
		key.second->Clone((void **)&temp);

		clip->keyframeMap[temp->BoneName] = temp;
	}

	*clone = clip;
}