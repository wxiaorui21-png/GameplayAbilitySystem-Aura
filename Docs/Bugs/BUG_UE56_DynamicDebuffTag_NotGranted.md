# BUG：UE 5.6 动态 Debuff GameplayEffect 未授予目标 Tag

## 状态

- 类型：Gameplay Ability System / GameplayEffect
- 影响版本：当前项目（Unreal Engine 5.6）
- 影响功能：Debuff Tag、Debuff Niagara 特效激活
- 当前状态：已定位，待修复

## 问题现象

动态创建 Debuff GameplayEffect 时执行了：

```cpp
Effect->InheritableOwnedTagsContainer.AddTag(
	GameplayTags.DamageTypesToDebuff[DamageType]
);
```

但是 GameplayEffect 应用到目标后，目标 ASC 没有按预期获得 `Debuff.*` Tag，导致 `UDebuffNiagaraCompoent` 注册的 Gameplay Tag Event 没有触发，Niagara Debuff 特效不激活。

相关调用位置：

- `Source/Aura/Private/AbilitySystem/AuraAttributeSet.cpp`
- `Source/Aura/Private/Debuff/DebuffNiagaraCompoent.cpp`

## 根因

项目使用 Unreal Engine 5.6，而 `UGameplayEffect::InheritableOwnedTagsContainer` 从 UE 5.3 起已经废弃。

旧代码虽然调用了 `FInheritedTagContainer::AddTag()`，但 UE 5.6 在应用 GameplayEffect 时实际读取的是：

```cpp
Effect.Spec.Def->GetGrantedTags();
Effect.Spec.DynamicGrantedTags;
```

`GetGrantedTags()` 返回 GameplayEffect 的 `CachedGrantedTags`。当前 Effect 是通过下面的代码在运行时动态创建的：

```cpp
UGameplayEffect* Effect = NewObject<UGameplayEffect>(
	GetTransientPackage(),
	FName(DebuffName)
);
```

运行时修改废弃的 `InheritableOwnedTagsContainer`，不会自动把 Tag 重新汇总进 `CachedGrantedTags`。因此旧容器中看起来已经有 Tag，但目标 ASC 应用 Effect 时没有获得该 Tag。

## 推荐修复

Debuff Tag 根据 `DamageType` 在运行时决定，因此应将它添加到 `FGameplayEffectSpec::DynamicGrantedTags`。

```cpp
const FGameplayTag* DebuffTag =
	GameplayTags.DamageTypesToDebuff.Find(DamageType);

if (!DebuffTag)
{
	UE_LOG(
		LogTemp,
		Error,
		TEXT("DamageType 没有对应 Debuff Tag：%s"),
		*DamageType.ToString()
	);
	return;
}

FGameplayEffectSpec EffectSpec(Effect, ContextHandle, 1.f);
EffectSpec.DynamicGrantedTags.AddTag(*DebuffTag);

FAuraGameplayEffectContext* Context =
	static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get());

Context->SetDamageType(MakeShared<FGameplayTag>(DamageType));

Props.TargetASC->ApplyGameplayEffectSpecToSelf(EffectSpec);
```

同时删除旧写法：

```cpp
Effect->InheritableOwnedTagsContainer.AddTag(...);
```

## 为什么使用 DynamicGrantedTags

- Tag 是根据每次伤害的 `DamageType` 动态选择的。
- `DynamicGrantedTags` 属于具体的 GameplayEffect Spec，适合保存每次应用时才确定的数据。
- Effect 生效期间，目标 ASC 会持有该 Tag。
- Effect 到期或被移除时，ASC 会自动移除该 Tag。
- UE 5.6 会复制并同步 Spec 中的 `DynamicGrantedTags`。

如果 Tag 是 GameplayEffect 定义的固定配置，则应改用 `UTargetTagsGameplayEffectComponent`；当前动态 Debuff 场景不需要这样做。

## 同时修复内存泄漏

当前代码使用：

```cpp
FGameplayEffectSpec* EffectSpec =
	new FGameplayEffectSpec(Effect, ContextHandle, 1.f);
```

`ApplyGameplayEffectSpecToSelf()` 会复制 Spec，并不负责释放这个裸指针，因此该写法会泄漏内存。推荐修复代码使用栈对象：

```cpp
FGameplayEffectSpec EffectSpec(Effect, ContextHandle, 1.f);
Props.TargetASC->ApplyGameplayEffectSpecToSelf(EffectSpec);
```

## 关联问题：委托返回了副本

即使 Debuff Tag 已正确授予，如果组件是在 ASC 初始化完成前注册监听，Niagara 仍可能没有响应。

当前接口按值返回委托：

```cpp
virtual FOnDebuffGetASC GetOnDebuffGetASC() = 0;
virtual FOnDeath GetOnDeath() = 0;
```

`DebuffNiagaraCompoent.cpp` 对返回值调用 `AddLambda()` 或 `AddDynamic()` 时，监听器会被添加到临时委托副本，而不是角色持有的真实委托。之后角色调用 `Broadcast()`，该组件不会收到通知。

接口、声明和实现都应该返回引用：

```cpp
virtual FOnDebuffGetASC& GetOnDebuffGetASC() = 0;
virtual FOnDeath& GetOnDeath() = 0;
```

`AAuraCharacterBase` 中对应的声明和定义也必须同步改为引用。

Lambda 捕获组件的 `this` 时还应优先使用弱绑定：

```cpp
CombatInterface->GetOnDebuffGetASC().AddWeakLambda(
	this,
	[this](UAbilitySystemComponent* InASC)
	{
		if (!IsValid(InASC))
		{
			return;
		}

		InASC->RegisterGameplayTagEvent(
			DebuffTag,
			EGameplayTagEventType::NewOrRemoved
		).AddUObject(
			this,
			&UDebuffNiagaraCompoent::DebuffChangedEvent
		);
	}
);
```

## 验证步骤

1. 使用火焰伤害触发成功 Debuff。
2. 确认 `DamageType` 是有效的 `Damage.Fire`。
3. 确认映射结果是有效的 `Debuff.Burn`。
4. 应用 Effect 后检查：

```cpp
ensure(Props.TargetASC->HasMatchingGameplayTag(*DebuffTag));
```

5. 确认 `UDebuffNiagaraCompoent::DebuffChangedEvent()` 收到 `NewCount > 0`。
6. 确认 Niagara 激活。
7. 等待 Debuff 到期，确认回调收到 `NewCount == 0`，Niagara 随之关闭。

## 额外检查

- `DamageType` 必须有效且存在于 `DamageTypesToDebuff` 中。
- `DebuffDuration` 必须大于 `0`，否则 Tag 可能立即被移除。
- `DebuffFrequency` 应大于 `0`，以避免无效的周期配置。
- 修复委托 getter 后，需要保证所有头文件和 `.cpp` 定义的返回类型一致。
