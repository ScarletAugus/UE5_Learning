#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SkillStep.h"
#include "ComboSkillDataAsset.generated.h"

class UTexture2D;

/**
 * 组合技能数据资产 —— 由多个原子技能按顺序/条件组合而成
 * 
 * 例如："跳跃踏地攻击" = 跳跃(立即) → 踏地攻击(到最高点时)
 */
UCLASS(BlueprintType, Blueprintable)
class SKILLRUNTIME_API UComboSkillDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** 组合技能唯一标识 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComboSkill|Basic")
	FName ComboSkillID;

	/** 组合技能显示名称 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComboSkill|Basic")
	FText ComboSkillName;

	/** 组合技能描述 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComboSkill|Basic", meta = (MultiLine = true))
	FText ComboSkillDescription;

	/** 组合技能图标 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComboSkill|Basic")
	TSoftObjectPtr<UTexture2D> ComboSkillIcon;

	/** 技能步骤序列（核心！按顺序执行） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComboSkill|Steps")
	TArray<FSkillStep> Steps;

	/** 整体冷却时间（秒） */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComboSkill|Parameters", meta = (ClampMin = "0.0"))
	float TotalCooldown = 5.0f;

	/** 是否启用 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ComboSkill|Parameters")
	bool bIsEnabled = true;

	// --- UPrimaryDataAsset interface ---
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("ComboSkill", GetFName());
	}

public:
	/** 获取步骤数量 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "ComboSkill")
	int32 GetStepCount() const
	{
		return Steps.Num();
	}
};
