#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ComboSkillDataAsset.h"
#include "SkillBaseActor.h"
#include "ComboSkillExecutorComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComboSkillFinished, UComboSkillDataAsset*, ComboSkillData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnComboStepStarted, int32, StepIndex, USkillDataAsset*, StepSkillData);

/**
 * 组合技能执行器组件
 * 
 * 挂载到角色上，负责按照 ComboSkillDataAsset 中定义的步骤序列依次执行技能。
 * 支持多种触发条件（立即、延迟、落地、最高点等）。
 */
UCLASS(ClassGroup=(Skill), meta=(BlueprintSpawnableComponent), Blueprintable)
class SKILLRUNTIME_API UComboSkillExecutorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UComboSkillExecutorComponent();

	// ========================
	// 属性
	// ========================

	/** 当前正在执行的组合技能数据 */
	UPROPERTY(BlueprintReadOnly, Category = "ComboSkill")
	UComboSkillDataAsset* CurrentComboSkill;

	/** 当前执行到第几步 */
	UPROPERTY(BlueprintReadOnly, Category = "ComboSkill")
	int32 CurrentStepIndex = -1;

	/** 当前步骤产生的技能 Actor */
	UPROPERTY(BlueprintReadOnly, Category = "ComboSkill")
	ASkillBaseActor* CurrentSkillActor;

	/** 是否正在执行组合技能 */
	UPROPERTY(BlueprintReadOnly, Category = "ComboSkill")
	bool bIsExecutingCombo = false;

	// ========================
	// 事件
	// ========================

	/** 组合技能全部完成时广播 */
	UPROPERTY(BlueprintAssignable, Category = "ComboSkill|Events")
	FOnComboSkillFinished OnComboSkillFinished;

	/** 某一步开始时广播 */
	UPROPERTY(BlueprintAssignable, Category = "ComboSkill|Events")
	FOnComboStepStarted OnComboStepStarted;

	// ========================
	// 函数
	// ========================

	/**
	 * 执行一个组合技能
	 * @param ComboSkillData  组合技能数据资产
	 */
	UFUNCTION(BlueprintCallable, Category = "ComboSkill")
	void ExecuteComboSkill(UComboSkillDataAsset* ComboSkillData);

	/** 取消当前正在执行的组合技能 */
	UFUNCTION(BlueprintCallable, Category = "ComboSkill")
	void CancelComboSkill();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/** 执行指定步骤 */
	void ExecuteStep(int32 StepIndex);

	/** 生成并执行一个技能 Actor */
	void SpawnAndExecuteSkill(const FSkillStep& Step);

	/** 当某个技能 Actor 完成时回调 */
	UFUNCTION()
	void OnStepSkillFinished(ASkillBaseActor* FinishedActor);

	/** 推进到下一步（检查后续步骤的触发条件） */
	void AdvanceToNextStep();

	/** 完成整个组合技能 */
	void FinishComboSkill();

	// --- 监听辅助 ---

	/** 用于 OnApex 检测：上一帧的 Z 速度 */
	float LastVelocityZ = 0.0f;

	/** 是否正在监听最高点 */
	bool bWaitingForApex = false;

	/** 是否正在监听落地 */
	bool bWaitingForLanding = false;

	/** 等待中的步骤索引 */
	int32 PendingStepIndex = -1;

	/** 延迟计时器句柄 */
	FTimerHandle DelayTimerHandle;
};
