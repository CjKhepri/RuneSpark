//Copyright 2020 - 2025, Vault Break Studios Pty. Ltd.
//Strider - Part of the 'Vault Break Library' (VBL)
//Written by: Ken Claassen

#pragma once

#include "CoreMinimal.h"
#include "AnimNode_AccelerationWarp.h"
#include "AnimGraphNode_Base.h"
#include "AnimGraphNode_AccelerationWarp.generated.h"

/** An editor animation node for acceleration warping on characters */
UCLASS()
class STRIDEREDITOR_API UAnimGraphNode_AccelerationWarp : public UAnimGraphNode_Base
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Runtime")
	FAnimNode_AccelerationWarp AccelerationWarpNode;

public:
	UAnimGraphNode_AccelerationWarp();

	//~ Begin UEdGraphNode Interface.
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	//~ End UEdGraphNode Interface.

	//~ Begin UAnimGraphNode_Base Interface
	virtual FString GetNodeCategory() const override;
	//~ End UAnimGraphNode_Base Interface
};
