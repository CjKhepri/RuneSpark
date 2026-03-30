//Copyright 2020 - 2025, Vault Break Studios Pty. Ltd.
//Strider - Part of the 'Vault Break Library' (VBL)
//Written by: Ken Claassen

#pragma once

#include "CoreMinimal.h"
#include "AnimNode_SlopeWarp.h"
#include "AnimGraphNode_SkeletalControlBase.h"
#include "AnimGraphNode_SlopeWarp.generated.h"

/** An editor animation node for warping a character's legs along a slope */
UCLASS()
class STRIDEREDITOR_API UAnimGraphNode_SlopeWarp : public UAnimGraphNode_SkeletalControlBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Runtime")
	FAnimNode_SlopeWarp SlopeWarpNode;

public:
	UAnimGraphNode_SlopeWarp();

	// UEdGraphNode interface
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	// End of UEdGraphNode interface

protected:
	// UAnimGraphNode_Base interface
	virtual FString GetNodeCategory() const override;
	// End of UAnimGraphNode_Base interface

	// UAnimGraphNode_SkeletalControlBase interface
	virtual const FAnimNode_SkeletalControlBase* GetNode() const;
	// End of UAnimGraphNode_SkeletalControlBase interface
};