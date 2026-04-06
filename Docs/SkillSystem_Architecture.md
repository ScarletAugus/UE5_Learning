# SkillMenu 插件 — 技能系统架构文档

> **引擎版本**：UE 5.7  
> **插件路径**：`Plugins/SkillMenu/`  
> **最后更新**：2026-04-06

---

## 1. 概述

SkillMenu 是一个 UE5 自定义插件，提供**数据驱动的组合技能系统**。它允许策划在编辑器中通过 DataAsset 定义原子技能和组合技能，并在运行时由执行器组件按照预设的步骤序列自动调度。

### 核心特性
- 📦 **数据驱动**：技能逻辑与数据分离，通过 DataAsset 配置技能参数
- 🔗 **组合技能**：多个原子技能可按顺序/条件组合成连招
- 🎮 **蓝图友好**：所有核心类均对蓝图暴露，支持蓝图继承和重写
- 🛠️ **编辑器扩展**：在主菜单栏添加「技能」下拉菜单，集成 Editor Utility Widget

---

## 2. 模块结构

插件包含两个模块：

```
Plugins/SkillMenu/
├── SkillMenu.uplugin              # 插件描述文件
├── Source/
│   ├── SkillRuntime/              # 运行时模块（随游戏打包）
│   │   ├── SkillRuntime.Build.cs
│   │   ├── Public/
│   │   │   ├── SkillRuntimeModule.h
│   │   │   ├── SkillTriggerType.h
│   │   │   ├── SkillDataAsset.h
│   │   │   ├── SkillStep.h
│   │   │   ├── ComboSkillDataAsset.h
│   │   │   ├── SkillBaseActor.h
│   │   │   └── ComboSkillExecutorComponent.h
│   │   └── Private/
│   │       ├── SkillRuntimeModule.cpp
│   │       ├── SkillBaseActor.cpp
│   │       └── ComboSkillExecutorComponent.cpp
│   │
│   └── SkillMenu/                 # 编辑器模块（仅编辑器可用）
│       ├── SkillMenu.Build.cs
│       ├── Public/
│       │   └── SkillMenuModule.h
│       └── Private/
│           └── SkillMenuModule.cpp
│
└── Content/                       # 插件蓝图资产（待创建）
```

### 2.1 SkillRuntime（运行时模块）

| 属性 | 值 |
|------|---|
| **类型** | Runtime |
| **加载阶段** | Default |
| **依赖模块** | Core, CoreUObject, Engine |
| **用途** | 技能数据定义 + 运行时执行逻辑 |

### 2.2 SkillMenu（编辑器模块）

| 属性 | 值 |
|------|---|
| **类型** | Editor |
| **加载阶段** | PostEngineInit |
| **依赖模块** | Core, SkillRuntime, CoreUObject, Engine, Slate, SlateCore, UnrealEd, LevelEditor, ToolMenus, Blutility, UMG, UMGEditor |
| **用途** | 编辑器菜单 + 技能编辑器 UI 窗口 |

---

## 3. 类图与关系

```
┌─────────────────────────────────────────────────────────┐
│                    SkillRuntime 模块                      │
│                                                          │
│  ┌──────────────────┐    ┌─────────────────────────┐    │
│  │ ESkillTriggerType │    │   USkillDataAsset        │    │
│  │   (枚举)          │    │   (原子技能数据)          │    │
│  │ · Immediate       │    │ · SkillID               │    │
│  │ · AfterPrevious   │    │ · SkillName             │    │
│  │ · OnApex          │    │ · SkillActorClass       │    │
│  │ · OnLanding       │    │ · Cooldown              │    │
│  │ · AfterDelay      │    │ · SkillParameters       │    │
│  │ · OnHit           │    │ · GetParameter()        │    │
│  └──────────────────┘    └─────────────────────────┘    │
│           │                          │                    │
│           ▼                          ▼                    │
│  ┌─────────────────────────────────────────────┐         │
│  │              FSkillStep (结构体)              │         │
│  │ · StepName          步骤名称                  │         │
│  │ · SkillData         引用的原子技能             │         │
│  │ · TriggerType       触发条件                  │         │
│  │ · DelayTime         延迟时间                  │         │
│  │ · ParameterOverrides 参数覆盖                 │         │
│  └─────────────────────────────────────────────┘         │
│                          │                                │
│                          ▼                                │
│  ┌─────────────────────────────────────────────┐         │
│  │        UComboSkillDataAsset                  │         │
│  │        (组合技能数据)                          │         │
│  │ · ComboSkillID     唯一标识                   │         │
│  │ · ComboSkillName   显示名称                   │         │
│  │ · Steps            TArray<FSkillStep>         │         │
│  │ · TotalCooldown    整体冷却                   │         │
│  └─────────────────────────────────────────────┘         │
│                          │                                │
│                     被引用                                 │
│                          ▼                                │
│  ┌─────────────────────────────────────────────┐         │
│  │    UComboSkillExecutorComponent              │         │
│  │    (组合技能执行器组件)                        │         │
│  │ · ExecuteComboSkill()  开始执行               │         │
│  │ · CancelComboSkill()   取消执行               │         │
│  │ · OnComboSkillFinished 完成事件               │         │
│  │ · OnComboStepStarted   步骤开始事件            │         │
│  │ 内部逻辑：                                     │         │
│  │ · ExecuteStep() → SpawnAndExecuteSkill()      │         │
│  │ · AdvanceToNextStep() (根据触发条件调度)        │         │
│  │ · TickComponent() (监听最高点/落地)            │         │
│  └─────────────────────────────────────────────┘         │
│                     生成 ▼                                │
│  ┌─────────────────────────────────────────────┐         │
│  │         ASkillBaseActor (抽象基类)            │         │
│  │ · ExecuteSkill()       执行技能(蓝图可重写)    │         │
│  │ · FinishSkill()        结束技能               │         │
│  │ · GetFinalParameter()  获取参数(含覆盖)       │         │
│  │ · OnSkillFinished      完成事件(委托)         │         │
│  └─────────────────────────────────────────────┘         │
└──────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────────┐
│                    SkillMenu 模块                         │
│                                                          │
│  ┌─────────────────────────────────────────────┐         │
│  │         FSkillMenuModule                     │         │
│  │ · StartupModule()     注册延迟回调            │         │
│  │ · RegisterMenus()     注册「技能」下拉菜单     │         │
│  │ · OpenEditorUtilityWidget()  打开 EUW 面板   │         │
│  │                                              │         │
│  │ 菜单项：                                      │         │
│  │  ├─ 技能列表      (占位，开发中)               │         │
│  │  ├─ 添加技能      (占位，开发中)               │         │
│  │  ├─ 技能编辑器    → 打开 EUW_SkillEditor      │         │
│  │  └─ 技能设置      (占位，开发中)               │         │
│  └─────────────────────────────────────────────┘         │
│                                                          │
│  蓝图资产（需手动创建）：                                   │
│  └─ Content/Skills/Editor/EUW_SkillEditor               │
└──────────────────────────────────────────────────────────┘
```

---

## 4. 核心类详解

### 4.1 ESkillTriggerType（触发类型枚举）

**文件**：`SkillRuntime/Public/SkillTriggerType.h`

定义组合技能中每一步的触发条件：

| 枚举值 | 显示名 | 说明 |
|--------|--------|------|
| `Immediate` | 立即执行 | 与上一步同时开始 |
| `AfterPrevious` | 上一步结束后 | 等待上一步 FinishSkill() 后触发 |
| `OnApex` | 到达最高点 | 监听角色 Z 速度从正变负的瞬间 |
| `OnLanding` | 落地时 | 监听 CharacterMovement::IsMovingOnGround() |
| `AfterDelay` | 延迟执行 | 由 FSkillStep::DelayTime 控制延迟秒数 |
| `OnHit` | 命中时 | 需要技能蓝图主动调用（当前为自动推进） |

### 4.2 USkillDataAsset（原子技能数据）

**文件**：`SkillRuntime/Public/SkillDataAsset.h`

描述一个最小的技能单元。在内容浏览器中通过 `右键 → Miscellaneous → Data Asset → SkillDataAsset` 创建。

| 属性 | 类型 | 说明 |
|------|------|------|
| `SkillID` | FName | 唯一标识 |
| `SkillName` | FText | 显示名称 |
| `SkillDescription` | FText | 描述文本 |
| `SkillIcon` | TSoftObjectPtr\<UTexture2D\> | 技能图标 |
| `SkillActorClass` | TSoftClassPtr\<AActor\> | 技能蓝图类引用 |
| `Cooldown` | float | 冷却时间（秒） |
| `SkillParameters` | TMap\<FName, float\> | 通用参数键值对 |

### 4.3 FSkillStep（技能步骤结构体）

**文件**：`SkillRuntime/Public/SkillStep.h`

描述组合技能中的一个步骤：在什么条件下执行哪个原子技能。

| 属性 | 类型 | 说明 |
|------|------|------|
| `StepName` | FText | 步骤名称 |
| `SkillData` | TSoftObjectPtr\<USkillDataAsset\> | 引用的原子技能 |
| `TriggerType` | ESkillTriggerType | 触发条件 |
| `DelayTime` | float | 延迟时间（仅 AfterDelay 生效） |
| `ParameterOverrides` | TMap\<FName, float\> | 参数覆盖 |

### 4.4 UComboSkillDataAsset（组合技能数据）

**文件**：`SkillRuntime/Public/ComboSkillDataAsset.h`

由多个原子技能按顺序/条件组合而成的复合技能。

| 属性 | 类型 | 说明 |
|------|------|------|
| `ComboSkillID` | FName | 唯一标识 |
| `ComboSkillName` | FText | 显示名称 |
| `Steps` | TArray\<FSkillStep\> | **核心：技能步骤序列** |
| `TotalCooldown` | float | 整体冷却时间 |
| `bIsEnabled` | bool | 是否启用 |

### 4.5 ASkillBaseActor（技能基类 Actor）

**文件**：`SkillRuntime/Public/SkillBaseActor.h` + `Private/SkillBaseActor.cpp`

所有蓝图技能的抽象基类。蓝图中继承此类并重写 `ExecuteSkill` 来实现具体技能逻辑。

| 函数 | 说明 |
|------|------|
| `ExecuteSkill(InOwner, InSkillData, InOverrides)` | 执行技能（BlueprintNativeEvent，蓝图可重写） |
| `FinishSkill()` | 结束技能，广播 OnSkillFinished |
| `GetFinalParameter(ParamName, DefaultValue)` | 获取参数（优先覆盖值 → SkillData 值 → 默认值） |

### 4.6 UComboSkillExecutorComponent（组合技能执行器）

**文件**：`SkillRuntime/Public/ComboSkillExecutorComponent.h` + `Private/ComboSkillExecutorComponent.cpp`

挂载到角色上的组件，负责按步骤序列调度技能。

**公开接口**：
| 函数/属性 | 说明 |
|-----------|------|
| `ExecuteComboSkill(ComboSkillData)` | 开始执行组合技能 |
| `CancelComboSkill()` | 取消当前组合技能 |
| `OnComboSkillFinished` | 委托：全部完成时广播 |
| `OnComboStepStarted` | 委托：某步开始时广播 |

**内部调度流程**：
```
ExecuteComboSkill()
    └→ ExecuteStep(0)
         ├→ SpawnAndExecuteSkill()  -- 生成 SkillBaseActor 并调用 ExecuteSkill
         │     └→ 监听 OnSkillFinished
         │           └→ OnStepSkillFinished()
         │                 └→ AdvanceToNextStep()
         │                       ├─ Immediate/AfterPrevious → ExecuteStep(next)
         │                       ├─ OnApex → 开启 Tick 监听 Z 速度
         │                       ├─ OnLanding → 开启 Tick 监听落地
         │                       ├─ AfterDelay → SetTimer
         │                       └─ 无更多步骤 → FinishComboSkill()
         └→ FinishComboSkill()  -- 广播 OnComboSkillFinished
```

### 4.7 FSkillMenuModule（编辑器菜单模块）

**文件**：`SkillMenu/Public/SkillMenuModule.h` + `Private/SkillMenuModule.cpp`

在 UE 编辑器主菜单栏注册「技能」下拉菜单，通过 `UToolMenus` API 实现。

| 菜单项 | 状态 | 功能 |
|--------|------|------|
| 技能列表 | 🚧 开发中 | 查看所有可用技能 |
| 添加技能 | 🚧 开发中 | 添加新技能 |
| 技能编辑器 | ✅ 已实现 | 打开 `EUW_SkillEditor` EditorUtilityWidget |
| 技能设置 | 🚧 开发中 | 配置技能系统参数 |

---

## 5. 数据流

```
策划在编辑器中操作：
                                              
  [1] 创建 USkillDataAsset         "跳跃技能"、"踏地攻击" 等原子技能
      ↓
  [2] 创建蓝图继承 ASkillBaseActor  在蓝图中实现具体技能逻辑
      ↓                            （播放动画、施加力、造成伤害等）
  [3] 创建 UComboSkillDataAsset     组合多个步骤 + 触发条件
      ↓                            例：跳跃(立即) → 踏地(到达最高点)
  [4] 角色蓝图添加 UComboSkillExecutorComponent
      ↓
  [5] 游戏中调用 ExecuteComboSkill() 触发连招
```

---

## 6. 文件清单

| # | 文件路径（相对于 Plugins/SkillMenu/） | 类型 | 说明 |
|---|--------------------------------------|------|------|
| 1 | `SkillMenu.uplugin` | 插件描述 | 定义两个模块 |
| 2 | `Source/SkillRuntime/SkillRuntime.Build.cs` | 构建脚本 | Runtime 模块依赖 |
| 3 | `Source/SkillRuntime/Public/SkillRuntimeModule.h` | 头文件 | 模块接口（空） |
| 4 | `Source/SkillRuntime/Private/SkillRuntimeModule.cpp` | 源文件 | 模块实现（空） |
| 5 | `Source/SkillRuntime/Public/SkillTriggerType.h` | 头文件 | 触发类型枚举 |
| 6 | `Source/SkillRuntime/Public/SkillDataAsset.h` | 头文件 | 原子技能数据 |
| 7 | `Source/SkillRuntime/Public/SkillStep.h` | 头文件 | 技能步骤结构体 |
| 8 | `Source/SkillRuntime/Public/ComboSkillDataAsset.h` | 头文件 | 组合技能数据 |
| 9 | `Source/SkillRuntime/Public/SkillBaseActor.h` | 头文件 | 技能基类 Actor |
| 10 | `Source/SkillRuntime/Private/SkillBaseActor.cpp` | 源文件 | 技能基类实现 |
| 11 | `Source/SkillRuntime/Public/ComboSkillExecutorComponent.h` | 头文件 | 组合技能执行器 |
| 12 | `Source/SkillRuntime/Private/ComboSkillExecutorComponent.cpp` | 源文件 | 执行器实现 |
| 13 | `Source/SkillMenu/SkillMenu.Build.cs` | 构建脚本 | Editor 模块依赖 |
| 14 | `Source/SkillMenu/Public/SkillMenuModule.h` | 头文件 | 编辑器菜单模块 |
| 15 | `Source/SkillMenu/Private/SkillMenuModule.cpp` | 源文件 | 菜单注册实现 |

---

## 7. 踩坑记录

### 7.1 ELevelComponentTickType → ELevelTick
- **问题**：`ComboSkillExecutorComponent.h` 中 `TickComponent` 使用了 `ELevelComponentTickType`
- **原因**：UE5 中正确的枚举类型是 `ELevelTick`，不是 `ELevelComponentTickType`
- **修复**：将 `.h` 和 `.cpp` 中的参数类型改为 `enum ELevelTick`

### 7.2 缺少 UMGEditor 模块依赖
- **问题**：编译报错 `fatal error C1083: 无法打开包括文件"WidgetBlueprint.h"`
- **原因**：`EditorUtilityWidgetBlueprint.h` 内部依赖 `WidgetBlueprint.h`，该头文件属于 `UMGEditor` 模块
- **修复**：在 `SkillMenu.Build.cs` 的 `PrivateDependencyModuleNames` 中添加 `"UMGEditor"`

### 7.3 菜单项点击无响应
- **问题**：菜单栏上的四个技能菜单项显示出来了但点击没有反应
- **原因**：使用了 `MainMenu->AddSection()` + `InitMenuEntry()` 的方式，菜单项被作为顶层 Section 平铺在菜单栏上，没有正确的下拉容器
- **修复**：改为 `AddSubMenu()` 创建真正的下拉子菜单，在子菜单回调中用 `AddMenuEntry()` 添加条目

---

## 8. 待开发功能

- [ ] **技能列表面板**：列出所有 SkillDataAsset / ComboSkillDataAsset
- [ ] **添加技能向导**：快速创建新的 DataAsset + SkillBaseActor 蓝图
- [ ] **技能编辑器 UI**：在 `EUW_SkillEditor` 中搭建可视化编辑界面
- [ ] **技能设置面板**：全局参数配置
- [ ] **OnHit 触发条件**：实现真正的命中检测回调机制
