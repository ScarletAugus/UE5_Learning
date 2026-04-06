#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkillDataAsset.h"
#include "SkillBaseActor.generated.h"

class ASkillBaseActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillFinished, ASkillBaseActor*, SkillActor);

/**
 * 技能基类 Actor
 * 
 * 所有蓝图技能（跳跃、踏地攻击等）都应继承此类。
 * 提供统一的 Execute / Finish 接口，组合技能执行器通过这些接口调度技能。
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class SKILLRUNTIME_API ASkillBaseActor : public AActor
{
	GENERATED_BODY()

public:
	ASkillBaseActor();

	// ========================
	// 属性
	// ========================

	/** 当前技能使用的数据资产 */
	UPROPERTY(BlueprintReadWrite, Category = "Skill")
	TObjectPtr<USkillDataAsset> SkillData;

	/** 技能执行者（通常是玩家角色） */
	UPROPERTY(BlueprintReadWrite, Category = "Skill")
	TObjectPtr<APawn> SkillOwner;

	/** 参数覆盖（由组合技能执行器传入） */
	UPROPERTY(BlueprintReadWrite, Category = "Skill")
	TMap<FName, float> ParameterOverrides;

	/** 技能是否正在执行中 */
	UPROPERTY(BlueprintReadOnly, Category = "Skill")
	bool bIsExecuting = false;

	// ========================
	// 事件
	// ========================

	/** 技能完成时广播（组合技能执行器监听此事件来触发下一步） */
	UPROPERTY(BlueprintAssignable, Category = "Skill|Events")
	FOnSkillFinished OnSkillFinished;

	// ========================
	// 函数
	// ========================

	/**
	 * 执行技能（蓝图中重写此函数来实现具体技能逻辑）
	 * @param InOwner       技能施放者
	 * @param InSkillData   技能数据
	 * @param InOverrides   参数覆盖表
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Skill")
	void ExecuteSkill(APawn* InOwner, USkillDataAsset* InSkillData, const TMap<FName, float>& InOverrides);
	virtual void ExecuteSkill_Implementation(APawn* InOwner, USkillDataAsset* InSkillData, const TMap<FName, float>& InOverrides);

	/**
	 * 结束技能 —— 技能逻辑完成时必须调用此函数！
	 * 会广播 OnSkillFinished 事件。
	 */
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void FinishSkill();

	/**
	 * 获取最终参数值（优先取覆盖值，否则取 SkillData 中的值）
	 */
	UFUNCTION(BlueprintCallable, Category = "Skill")
	float GetFinalParameter(FName ParamName, float DefaultValue = 0.0f) const;
};
