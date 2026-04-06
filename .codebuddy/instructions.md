# 项目笔记 - test (UE 工程)

## 基本信息
- **Git 地址**：https://github.com/ScarletAugus/UE5_Learning
- **引擎版本**：UE 5.7
- **项目名称**：test
- **UE 安装路径**：`D:\UE_5.7`

## 插件说明

### SkillMenu 插件
数据驱动的组合技能系统插件，包含两个模块：

- **SkillRuntime**（运行时）：定义技能数据结构和运行时执行逻辑
  - `USkillDataAsset` — 原子技能数据（ID、名称、参数、蓝图类引用）
  - `UComboSkillDataAsset` — 组合技能数据（步骤序列 + 触发条件）
  - `ASkillBaseActor` — 技能基类 Actor（蓝图继承并重写 ExecuteSkill）
  - `UComboSkillExecutorComponent` — 挂载到角色上的执行器组件，按步骤序列自动调度技能
  - `ESkillTriggerType` — 触发条件枚举（立即/上一步后/最高点/落地/延迟/命中）

- **SkillMenu**（编辑器）：在主菜单栏添加「技能」下拉菜单
  - 技能列表（开发中）
  - 添加技能（开发中）
  - 技能编辑器 → 打开 `Content/Skills/Editor/EUW_SkillEditor`
  - 技能设置（开发中）

> 📄 **详细架构文档**：[`Docs/SkillSystem_Architecture.md`](../Docs/SkillSystem_Architecture.md)


## 重要决策记录
- 技能系统采用数据驱动架构：DataAsset 定义数据，SkillBaseActor 蓝图实现逻辑
- 组合技能通过 ComboSkillExecutorComponent 自动调度，支持 6 种触发条件
- 编辑器 UI 使用 Editor Utility Widget 方案（UMG + 蓝图）

## 其他备注
- 编译命令：`D:\UE_5.7\Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe testEditor Win64 Development -Project=D:\ScarletAugus\test\test.uproject -WaitMutex`
- 编辑器运行时不能用 UBT 编译，需关闭编辑器或使用 Live Coding (Ctrl+Alt+F11)


