//Copyright 2020 - 2025, Vault Break Studios Pty. Ltd.
//Strider - Part of the 'Vault Break Library' (VBL)
//Written by: Ken Claassen
#include "AnimGraphNode_StrideWarp.h"

#define LOCTEXT_NAMESPACE "StriderEditor"

UAnimGraphNode_StrideWarp::UAnimGraphNode_StrideWarp()
{
}

FText UAnimGraphNode_StrideWarp::GetTooltipText() const
{
	return LOCTEXT("StrideWarp", "Stride Warp");
}

FLinearColor UAnimGraphNode_StrideWarp::GetNodeTitleColor() const
{
	return FLinearColor(0.0f, 1.0f, 0.5f);
}

FText UAnimGraphNode_StrideWarp::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("StrideWarp", "Stride Warp");
}

FString UAnimGraphNode_StrideWarp::GetNodeCategory() const
{
	return TEXT("Animation Warping");
}

const FAnimNode_SkeletalControlBase* UAnimGraphNode_StrideWarp::GetNode() const
{
	return &StrideWarpNode;
}

#undef LOCTEXT_NAMESPACE