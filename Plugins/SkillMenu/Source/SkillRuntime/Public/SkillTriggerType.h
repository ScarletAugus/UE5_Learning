#pragma once

#include "CoreMinimal.h"
#include "SkillTriggerType.generated.h"

/**
 * 组合技能中，每一步的触发条件
 */
UENUM(BlueprintType)
enum class ESkillTriggerType : uint8
{
	/** 立即执行（与上一步同时开始） */
	Immediate		UMETA(DisplayName = "立即执行"),

	/** 上一步完成后执行 */
	AfterPrevious	UMETA(DisplayName = "上一步结束后"),

	/** 到达跳跃最高点时执行 */
	OnApex			UMETA(DisplayName = "到达最高点"),

	/** 落地时执行 */
	OnLanding		UMETA(DisplayName = "落地时"),

	/** 延迟指定时间后执行 */
	AfterDelay		UMETA(DisplayName = "延迟执行"),

	/** 命中目标时执行 */
	OnHit			UMETA(DisplayName = "命中时"),
};
