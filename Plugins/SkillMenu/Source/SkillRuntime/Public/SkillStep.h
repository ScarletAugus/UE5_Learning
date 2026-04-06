#pragma once

#include "CoreMinimal.h"
#include "SkillTriggerType.h"
#include "SkillDataAsset.h"
#include "SkillStep.generated.h"

/**
 * 组合技能中的一个步骤
 * 描述：在什么条件下，执行哪个原子技能，并可以覆盖哪些参数
 */
USTRUCT(BlueprintType)
struct SKILLRUNTIME_API FSkillStep
{
	GENERATED_BODY()

	/** 步骤名称（用于编辑器显示） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Step")
	FText StepName;

	/** 引用的原子技能数据 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Step")
	TSoftObjectPtr<USkillDataAsset> SkillData;

	/** 此步骤的触发条件 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Step")
	ESkillTriggerType TriggerType = ESkillTriggerType::AfterPrevious;

	/** 延迟时间（仅当 TriggerType == AfterDelay 时生效） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Step", meta = (ClampMin = "0.0"))
	float DelayTime = 0.0f;

	/** 参数覆盖：可覆盖原子技能中的参数值 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Step")
	TMap<FName, float> ParameterOverrides;
};
