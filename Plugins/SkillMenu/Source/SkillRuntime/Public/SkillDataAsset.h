#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkillDataAsset.generated.h"

class UTexture2D;

/**
 * 原子技能数据资产 —— 描述一个最小的技能单元（如跳跃、踏地攻击）
 * 
 * 在内容浏览器中右键 → Miscellaneous → Data Asset → 选择 SkillDataAsset 来创建实例。
 */
UCLASS(BlueprintType, Blueprintable)
class SKILLRUNTIME_API USkillDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** 技能唯一标识 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Basic")
	FName SkillID;

	/** 技能显示名称 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Basic")
	FText SkillName;

	/** 技能描述 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Basic", meta = (MultiLine = true))
	FText SkillDescription;

	/** 技能图标 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Basic")
	TSoftObjectPtr<UTexture2D> SkillIcon;

	/** 技能蓝图类引用（指向继承自 AActor 或自定义技能基类的蓝图） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Execution")
	TSoftClassPtr<AActor> SkillActorClass;

	/** 技能冷却时间（秒） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Parameters", meta = (ClampMin = "0.0"))
	float Cooldown = 1.0f;

	/**
	 * 技能参数表 —— 通用键值对，蓝图中可按名称读取
	 * 例如: "JumpHeight" = 600.0, "Damage" = 50.0
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill|Parameters")
	TMap<FName, float> SkillParameters;

	// --- UPrimaryDataAsset interface ---
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Skill", GetFName());
	}

public:
	/** 蓝图辅助函数：获取参数值，找不到则返回默认值 */
	UFUNCTION(BlueprintCallable, Category = "Skill")
	float GetParameter(FName ParamName, float DefaultValue = 0.0f) const
	{
		const float* Found = SkillParameters.Find(ParamName);
		return Found ? *Found : DefaultValue;
	}

	/** 蓝图辅助函数：设置参数值 */
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void SetParameter(FName ParamName, float Value)
	{
		SkillParameters.Add(ParamName, Value);
	}
};
