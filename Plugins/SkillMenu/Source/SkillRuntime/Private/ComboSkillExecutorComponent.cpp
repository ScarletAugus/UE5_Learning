#include "ComboSkillExecutorComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

UComboSkillExecutorComponent::UComboSkillExecutorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UComboSkillExecutorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UComboSkillExecutorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerPawn);
	if (!OwnerCharacter) return;

	UCharacterMovementComponent* MovementComp = OwnerCharacter->GetCharacterMovement();
	if (!MovementComp) return;

	const float CurrentVelocityZ = MovementComp->Velocity.Z;

	// 检测到达最高点：Z 速度从正变为 ≤ 0
	if (bWaitingForApex)
	{
		if (LastVelocityZ > 0.0f && CurrentVelocityZ <= 0.0f)
		{
			bWaitingForApex = false;
			if (CurrentComboSkill && CurrentComboSkill->Steps.IsValidIndex(PendingStepIndex))
			{
				ExecuteStep(PendingStepIndex);
			}
		}
	}

	// 检测落地
	if (bWaitingForLanding)
	{
		if (MovementComp->IsMovingOnGround())
		{
			bWaitingForLanding = false;
			if (CurrentComboSkill && CurrentComboSkill->Steps.IsValidIndex(PendingStepIndex))
			{
				ExecuteStep(PendingStepIndex);
			}
		}
	}

	LastVelocityZ = CurrentVelocityZ;

	// 如果不再需要监听，关闭 Tick
	if (!bWaitingForApex && !bWaitingForLanding && !bIsExecutingCombo)
	{
		SetComponentTickEnabled(false);
	}
}

void UComboSkillExecutorComponent::ExecuteComboSkill(UComboSkillDataAsset* ComboSkillData)
{
	if (!ComboSkillData || ComboSkillData->Steps.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ComboSkillExecutor: 无效的组合技能数据或步骤为空"));
		return;
	}

	if (bIsExecutingCombo)
	{
		CancelComboSkill();
	}

	CurrentComboSkill = ComboSkillData;
	CurrentStepIndex = -1;
	bIsExecutingCombo = true;
	bWaitingForApex = false;
	bWaitingForLanding = false;
	PendingStepIndex = -1;

	// 从第一步开始
	ExecuteStep(0);
}

void UComboSkillExecutorComponent::CancelComboSkill()
{
	if (CurrentSkillActor && CurrentSkillActor->bIsExecuting)
	{
		CurrentSkillActor->FinishSkill();
	}

	GetWorld()->GetTimerManager().ClearTimer(DelayTimerHandle);

	bIsExecutingCombo = false;
	bWaitingForApex = false;
	bWaitingForLanding = false;
	CurrentComboSkill = nullptr;
	CurrentStepIndex = -1;
	PendingStepIndex = -1;

	SetComponentTickEnabled(false);
}

void UComboSkillExecutorComponent::ExecuteStep(int32 StepIndex)
{
	if (!CurrentComboSkill || !CurrentComboSkill->Steps.IsValidIndex(StepIndex))
	{
		FinishComboSkill();
		return;
	}

	CurrentStepIndex = StepIndex;
	const FSkillStep& Step = CurrentComboSkill->Steps[StepIndex];

	// 加载原子技能数据
	USkillDataAsset* StepSkillData = Step.SkillData.LoadSynchronous();
	if (!StepSkillData)
	{
		UE_LOG(LogTemp, Warning, TEXT("ComboSkillExecutor: 步骤 %d 的技能数据无效"), StepIndex);
		AdvanceToNextStep();
		return;
	}

	// 广播步骤开始事件
	OnComboStepStarted.Broadcast(StepIndex, StepSkillData);

	// 生成并执行技能
	SpawnAndExecuteSkill(Step);
}

void UComboSkillExecutorComponent::SpawnAndExecuteSkill(const FSkillStep& Step)
{
	USkillDataAsset* StepSkillData = Step.SkillData.LoadSynchronous();
	if (!StepSkillData) return;

	// 获取技能蓝图类
	UClass* SkillClass = StepSkillData->SkillActorClass.LoadSynchronous();
	if (!SkillClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ComboSkillExecutor: 技能 '%s' 没有设置 SkillActorClass"), *StepSkillData->SkillName.ToString());
		AdvanceToNextStep();
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	// 在拥有者位置生成技能 Actor
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASkillBaseActor* SkillActor = GetWorld()->SpawnActor<ASkillBaseActor>(
		SkillClass,
		OwnerPawn->GetActorLocation(),
		OwnerPawn->GetActorRotation(),
		SpawnParams
	);

	if (!SkillActor)
	{
		UE_LOG(LogTemp, Error, TEXT("ComboSkillExecutor: 无法生成技能 Actor"));
		AdvanceToNextStep();
		return;
	}

	CurrentSkillActor = SkillActor;

	// 监听技能完成事件
	SkillActor->OnSkillFinished.AddDynamic(this, &UComboSkillExecutorComponent::OnStepSkillFinished);

	// 合并参数覆盖：步骤覆盖优先
	TMap<FName, float> MergedOverrides = Step.ParameterOverrides;

	// 执行技能
	SkillActor->ExecuteSkill(OwnerPawn, StepSkillData, MergedOverrides);
}

void UComboSkillExecutorComponent::OnStepSkillFinished(ASkillBaseActor* FinishedActor)
{
	if (FinishedActor)
	{
		FinishedActor->OnSkillFinished.RemoveDynamic(this, &UComboSkillExecutorComponent::OnStepSkillFinished);
		FinishedActor->Destroy();
	}

	AdvanceToNextStep();
}

void UComboSkillExecutorComponent::AdvanceToNextStep()
{
	int32 NextIndex = CurrentStepIndex + 1;

	if (!CurrentComboSkill || !CurrentComboSkill->Steps.IsValidIndex(NextIndex))
	{
		FinishComboSkill();
		return;
	}

	const FSkillStep& NextStep = CurrentComboSkill->Steps[NextIndex];

	switch (NextStep.TriggerType)
	{
	case ESkillTriggerType::Immediate:
		// 立即执行
		ExecuteStep(NextIndex);
		break;

	case ESkillTriggerType::AfterPrevious:
		// 上一步完成后执行（我们已经在 OnStepSkillFinished 中了，所以直接执行）
		ExecuteStep(NextIndex);
		break;

	case ESkillTriggerType::OnApex:
		// 监听最高点
		bWaitingForApex = true;
		PendingStepIndex = NextIndex;
		SetComponentTickEnabled(true);
		break;

	case ESkillTriggerType::OnLanding:
		// 监听落地
		bWaitingForLanding = true;
		PendingStepIndex = NextIndex;
		SetComponentTickEnabled(true);
		break;

	case ESkillTriggerType::AfterDelay:
		// 延迟执行
		{
			PendingStepIndex = NextIndex;
			float Delay = NextStep.DelayTime;
			GetWorld()->GetTimerManager().SetTimer(
				DelayTimerHandle,
				FTimerDelegate::CreateLambda([this, NextIndex]()
				{
					ExecuteStep(NextIndex);
				}),
				Delay,
				false
			);
		}
		break;

	case ESkillTriggerType::OnHit:
		// OnHit 需要由技能蓝图自行通知，这里先用 AfterPrevious 回退
		UE_LOG(LogTemp, Log, TEXT("ComboSkillExecutor: OnHit 触发条件需要在技能蓝图中主动调用，当前自动推进"));
		ExecuteStep(NextIndex);
		break;

	default:
		ExecuteStep(NextIndex);
		break;
	}
}

void UComboSkillExecutorComponent::FinishComboSkill()
{
	UComboSkillDataAsset* FinishedCombo = CurrentComboSkill;

	bIsExecutingCombo = false;
	bWaitingForApex = false;
	bWaitingForLanding = false;
	CurrentComboSkill = nullptr;
	CurrentStepIndex = -1;
	PendingStepIndex = -1;
	CurrentSkillActor = nullptr;

	SetComponentTickEnabled(false);

	if (FinishedCombo)
	{
		OnComboSkillFinished.Broadcast(FinishedCombo);
	}
}
