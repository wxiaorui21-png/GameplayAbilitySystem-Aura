# BUG：UE 5.6 AI 行走时 LaunchCharacter 水平击退被覆盖

## 状态

- 类型：AI / Behavior Tree / CharacterMovement / GAS
- 影响版本：当前项目（Unreal Engine 5.6）
- 影响功能：非致命伤害、受击反应、击退效果
- 当前状态：已修复

## 问题现象

投射物命中敌人并造成非致命伤害时，通过 `LaunchCharacter()` 施加一个同时包含水平和垂直分量的击退速度：

```cpp
Props.TargetCharacter->LaunchCharacter(
	UAuraAbilitySystemLibrary::GetKnockImpulseDirection(
		Props.EffectContextHandle
	),
	true,
	true
);
```

实际表现不稳定：

- 敌人没有行走或已经进入 HitReact 状态时，可以看到完整击退。
- 敌人正在执行 Behavior Tree 的 `Move To` 时，通常只能看到 Z 方向击飞，XY 水平击退不明显或完全消失。
- 部分攻击完全没有击退，容易与水平速度被覆盖的问题混淆。

相关源码：

- `Source/Aura/Private/Actor/AuraProjectile.cpp`
- `Source/Aura/Private/AbilitySystem/AuraAttributeSet.cpp`
- `Source/Aura/Private/Character/AuraEnemy.cpp`
- `Source/Aura/Private/AuraAbilityTypes.cpp`

## 击退数据流程

### 1. GameplayAbility 填充伤害参数

`UAuraDamageGameplayAbility::MakeDamageEffectParamsFromDefaultClass()` 把蓝图中配置的击退强度和击退概率写入 `FDamageEffectParams`：

```cpp
DamageEffectParams.KnockImpulseMagniture =
	KnockImpulseMagniture;

DamageEffectParams.KnockChance = KnockChance;
```

项目中属性使用 `EditDefaultsOnly`，因此实际运行值应以对应 GameplayAbility 蓝图保存的值为准，不一定等于 C++ 默认值。

### 2. Projectile 计算击退向量

命中时先判断击退概率，成功后把方向抬高 45 度：

```cpp
const bool bIsKnock =
	FMath::FRandRange(0.f, 100.f) <
	DamageEffectParams.KnockChance;

if (bIsKnock)
{
	FRotator Rotation = GetActorRotation();
	Rotation.Pitch = 45.f;

	const FVector Direction = Rotation.Vector();

	DamageEffectParams.KnockImpulseDirection =
		Direction *
		DamageEffectParams.KnockImpulseMagniture;
}
else
{
	DamageEffectParams.KnockImpulseDirection =
		FVector::ZeroVector;
}
```

所以击退向量同时包含 XY 和 Z 分量。如果只剩 Z 表现，说明向量计算本身不一定有错，水平移动可能在后续 CharacterMovement 或 AI 寻路阶段被覆盖。

### 3. EffectContext 传递向量

`ApplyDamageEffect()` 把击退向量写入自定义 `FAuraGameplayEffectContext`，AttributeSet 再从伤害 Context 中读取。

自定义 NetSerialize 使用 bit 15 标记 `KnockImpulseDirection`：

```cpp
RepBits |= 1 << 15;
```

`SerializeBits()` 的第二个参数是序列化的 bit 数量，不是最后一个 bit 的下标。为了包含下标 `0～15`，必须序列化 16 位：

```cpp
Ar.SerializeBits(&RepBits, 16);
```

写成 `15` 只会包含下标 `0～14`，导致联机序列化时 bit 15 丢失。

### 4. AttributeSet 只对非致命伤害执行击退

当前需求是只有非致命伤害触发 `LaunchCharacter()`：

```cpp
if (bFatal)
{
	CombatInterface->Die(
		UAuraAbilitySystemLibrary::GetImpulseDirection(
			Props.EffectContextHandle
		)
	);
}
else
{
	// 激活 HitReact，然后对存活角色执行 LaunchCharacter
}
```

致命伤害进入布娃娃死亡流程并使用 Mesh 物理冲量，不属于本 Bug。

## 根因

敌人行走时，Behavior Tree 的 `Move To` 和 PathFollowing 正在持续控制 CharacterMovement 的水平移动。

`LaunchCharacter()` 不会立即移动 Actor。它先把速度保存为 CharacterMovement 的待处理 Launch 速度，下一次 CharacterMovement Tick 才应用，并把移动模式切换为 `Falling`。

如果当前 `Move To` 没有及时结束，AI 已提交的寻路移动和水平速度会继续参与移动计算，造成以下表现：

```text
LaunchCharacter.XY  ─┐
                    ├─> 与 AI MoveTo/PathFollowing 水平移动竞争
AI Requested XY    ─┘

LaunchCharacter.Z  ───> 地面寻路通常不控制 Z，因此仍然可见
```

原来的 HitReact 回调只做了两件事：

```cpp
GetCharacterMovement()->MaxWalkSpeed =
	bHitReacting ? 0.f : BaseWalkSpeed;

Blackboard->SetValueAsBool(
	FName("HitReacting"),
	bHitReacting
);
```

这能阻止后续移动，但在当前执行时序下，不足以可靠清除正在运行的 `Move To` 和 CharacterMovement 中已有的水平速度。

## 最终修复

修复由行为树控制和 C++ 即时停止两部分组成。

### 1. Behavior Tree 配置

在包含 `Wait` 和 `Move To` 的移动 Sequence 上添加 Blackboard Decorator：

```text
Blackboard Key：HitReacting
Key Query：Is Not Set
Notify Observer：On Value Change
Observer Aborts：Self
```

含义是：只有 `HitReacting == false` 时允许执行移动 Sequence；受击后该值变成 `true`，当前移动分支会被中止。

### 2. HitReact Tag 回调立即终止移动

最终生效的 `AAuraEnemy::HitReactTagChanged()`：

```cpp
void AAuraEnemy::HitReactTagChanged(
	const FGameplayTag CallbackTag,
	int32 NewCount)
{
	bHitReacting = NewCount > 0;

	GetCharacterMovement()->MaxWalkSpeed =
		bHitReacting ? 0.f : BaseWalkSpeed;

	if (AuraAIController &&
		AuraAIController->GetBlackboardComponent())
	{
		AuraAIController->GetBlackboardComponent()
			->SetValueAsBool(
				FName("HitReacting"),
				bHitReacting
			);

		if (bHitReacting)
		{
			// 中止 Behavior Tree 当前的 MoveTo
			AuraAIController->StopMovement();

			// 清除 CharacterMovement 已有的速度
			GetCharacterMovement()
				->StopMovementImmediately();
		}
	}
}
```

两个停止调用的职责不同：

- `AuraAIController->StopMovement()`：中止 AIController/PathFollowing 当前的寻路请求。
- `GetCharacterMovement()->StopMovementImmediately()`：清除 CharacterMovement 当前已有速度。

随后 `UAuraAttributeSet::HandleIncomingDamage()` 调用 `LaunchCharacter()`，新的击退速度不会再与旧的寻路速度竞争。

### 3. 保持正确执行顺序

非致命伤害分支先激活 HitReact，再执行 Launch：

```cpp
FGameplayTagContainer TagContainer;
TagContainer.AddTag(
	FAuraGameplayTags::Get().Effects_HitReact
);

Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);

const FVector KnockImpulse =
	UAuraAbilitySystemLibrary::GetKnockImpulseDirection(
		Props.EffectContextHandle
	);

if (!KnockImpulse.IsNearlyZero(1.f))
{
	Props.TargetCharacter->LaunchCharacter(
		KnockImpulse,
		true,
		true
	);
}
```

当 HitReact Ability 同步授予 `Effects.HitReact` Tag 时，执行顺序为：

```text
TryActivateAbilitiesByTag
        ↓
Effects.HitReact Tag 数量改变
        ↓
HitReactTagChanged(true)
        ↓
Blackboard 阻止移动分支
        ↓
StopMovement + StopMovementImmediately
        ↓
LaunchCharacter 写入新的击退速度
```

## 为什么问题看起来是随机的

项目本身包含击退概率：

```cpp
FMath::FRandRange(0.f, 100.f) < KnockChance
```

例如 `KnockChance = 60` 时，约 40% 的直接命中会把击退向量设为零，这些攻击按设计不会击退。

因此排查时存在两种不同的“没有击退”：

- 向量为零：击退概率判定失败，是预期行为。
- 向量有明显 XY/Z 分量，但只有 Z 可见：水平速度被 AI MoveTo/PathFollowing 覆盖，是本 Bug。

排查期间可把 GameplayAbility 蓝图中的 `KnockChance` 临时设为 `100`，避免概率干扰；修复确认后再恢复设计值。

## 验证步骤

1. 在 GameplayAbility 蓝图中临时把 `KnockChance` 设置为 `100`。
2. 让敌人处于静止状态并命中，确认出现完整 XY/Z 击退。
3. 让敌人正在执行 `Move To` 时命中，确认仍然出现完整 XY/Z 击退。
4. 在 Behavior Tree 调试器中选择该敌人，确认受击时 `HitReacting` 变成 `true`。
5. 确认移动 Sequence 因 `Observer Aborts: Self` 退出运行状态。
6. 确认 `StopMovement()` 中止当前寻路，击退期间不再立即恢复 MoveTo。
7. HitReact Tag 移除后，确认 `MaxWalkSpeed` 恢复为 `BaseWalkSpeed`，行为树重新进入正常移动分支。
8. 恢复实际 `KnockChance`，确认只有概率成功的攻击触发击退。
9. 联机测试中确认 `SerializeBits(&RepBits, 16)`，客户端也能获得完整击退 Context。

## 调试建议

出现类似问题时，同时记录击退数据、AI 状态和移动模式：

```cpp
UE_LOG(
	LogTemp,
	Warning,
	TEXT("Knock=%s HitReacting=%d Mode=%d Active=%d"),
	*KnockImpulse.ToString(),
	bHitReacting,
	static_cast<int32>(
		GetCharacterMovement()->MovementMode
	),
	GetCharacterMovement()->IsActive()
);
```

不要仅凭动画后仰判断是否发生了击退。`LaunchCharacter()` 移动的是 Character/Capsule，应结合 `GetActorLocation()`、CharacterMovement 速度和 Behavior Tree 运行节点判断。

## 经验总结

- `LaunchCharacter()` 通过 CharacterMovement 工作，不是物理 Mesh 冲量。
- AI 的 `Move To` 会持续影响水平移动，击退前需要正确中止或屏蔽寻路。
- 修改 `MaxWalkSpeed` 不等同于取消当前 PathFollowing 请求。
- Blackboard Decorator 负责阻止移动分支继续运行，`StopMovement()` 负责即时取消当前路径。
- `StopMovementImmediately()` 清除旧速度，之后再调用 `LaunchCharacter()` 写入新的击退速度。
- 行走时只有 Z 击退，通常应优先排查 AI/玩家输入对 XY 速度的持续覆盖。
- 概率判定、零向量、AI 速度覆盖是三个不同问题，应通过日志分别确认。
