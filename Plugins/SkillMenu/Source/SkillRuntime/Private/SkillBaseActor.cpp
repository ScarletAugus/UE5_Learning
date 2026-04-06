#include "SkillBaseActor.h"

ASkillBaseActor::ASkillBaseActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// 技能 Actor 默认不需要复制
	bReplicates = false;
}

void ASkillBaseActor::ExecuteSkill_Implementation(APawn* InOwner, USkillDataAsset* InSkillData, const TMap<FName, float>& InOverrides)
{
	SkillOwner = InOwner;
	SkillData = InSkillData;
	ParameterOverrides = InOverrides;
	bIsExecuting = true;

	// 子类蓝图应重写此函数来实现具体技能逻辑
	// 默认实现：直接结束
	FinishSkill();
}

void ASkillBaseActor::FinishSkill()
{
	bIsExecuting = false;
	OnSkillFinished.Broadcast(this);
}

float ASkillBaseActor::GetFinalParameter(FName ParamName, float DefaultValue) const
{
	// 优先从覆盖表中取值
	const float* OverrideValue = ParameterOverrides.Find(ParamName);
	if (OverrideValue)
	{
		return *OverrideValue;
	}

	// 其次从 SkillData 中取值
	if (SkillData)
	{
		return SkillData->GetParameter(ParamName, DefaultValue);
	}

	return DefaultValue;
}
