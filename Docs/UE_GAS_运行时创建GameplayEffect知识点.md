# UE GAS：运行时创建 GameplayEffect 知识点

本文以 `UAuraAttributeSet::Debuff()` 中动态创建 Debuff GameplayEffect 的代码为例，说明 Transient Package、GameplayEffect 堆叠、Magnitude 类型和 Modifier 数组。

本文档位于 `Docs`，不会参与 Unreal Build Tool 编译。

## 1. NewObject 与 GetTransientPackage

原代码：

```cpp
UGameplayEffect* Effect = NewObject<UGameplayEffect>(
	GetTransientPackage(),
	FName(DebuffName)
);
```

`NewObject<T>()` 用来在运行时创建 UObject。这里的两个参数分别是：

```text
GetTransientPackage()  -> Outer
FName(DebuffName)      -> 对象名称
```

### GetTransientPackage 的作用

`GetTransientPackage()` 返回引擎提供的临时顶层 Package。引擎头文件对它的定义是：用于临时保存永远不应该被保存的对象。

把动态 GameplayEffect 放进 Transient Package，表示：

- 它只是运行时临时对象，不对应 Content Browser 中的 `.uasset`。
- 它不会被保存进地图或资产包。
- 它拥有一个合法的 UObject Outer 和对象路径，便于 UObject 系统管理和调试。
- 游戏进程或当前运行会话结束后，不需要持久化它。

对象路径大致会类似：

```text
/Engine/Transient.DynamicDebuff_Damage_Fire
```

如果不需要自定义名称，`NewObject<T>()` 的默认 Outer 本身就是 `GetTransientPackage()`：

```cpp
UGameplayEffect* Effect = NewObject<UGameplayEffect>();
```

### Transient Package 不等于永久存活

把对象的 Outer 设为 Transient Package，主要表达对象的层级和“不保存”语义，不应把它当成永久保活手段。

UObject 是否会被垃圾回收，仍取决于 GC 能否从有效引用链找到它，例如：

- 被 `UPROPERTY()`/`TObjectPtr` 引用；
- 被其他参与 GC 的 UObject 或 USTRUCT 引用；
- 被显式加入 Root；
- 被有效的 GameplayEffect Spec/ActiveGameplayEffect 持有。

当前场景中，Effect 应用成功后，ActiveGameplayEffect 的 Spec 会保存它的 Definition 引用。不要为了保活而随意调用 `AddToRoot()`，否则还需要正确配对 `RemoveFromRoot()`。

### 指定固定名字的注意事项

UObject 的名字在同一个 Outer 下需要唯一。反复使用下面的同一个 Outer 和名字创建对象会产生名字冲突/对象重建风险：

```cpp
NewObject<UGameplayEffect>(
	GetTransientPackage(),
	FName(DebuffName)
);
```

如果每次都需要独立对象，可以让 UE 自动生成名字：

```cpp
UGameplayEffect* Effect = NewObject<UGameplayEffect>();
```

如果既需要可读名字又需要唯一名字，可以使用：

```cpp
const FName UniqueName = MakeUniqueObjectName(
	GetTransientPackage(),
	UGameplayEffect::StaticClass(),
	FName(DebuffName)
);

UGameplayEffect* Effect = NewObject<UGameplayEffect>(
	GetTransientPackage(),
	UniqueName
);
```

但是 GameplayEffect 的堆叠匹配还要求使用同一个 Effect Definition，不能只靠相同的字符串名称，详见下一节。

## 2. GameplayEffect 堆叠

原代码：

```cpp
Effect->StackingType =
	EGameplayEffectStackingType::AggregateBySource;

Effect->StackLimitCount = 1;
```

`StackingType` 决定同一种 GameplayEffect 重复应用时如何归并，`StackLimitCount` 决定归并后的最大层数。

### None

```cpp
Effect->StackingType = EGameplayEffectStackingType::None;
```

不进行堆叠。多次应用会被当成多个独立的 ActiveGameplayEffect：

```text
火焰 Debuff 第一次 -> ActiveEffect A
火焰 Debuff 第二次 -> ActiveEffect B
```

每个实例有自己的持续时间、周期计时和移除时机。此时 `StackLimitCount` 不参与限制。

### AggregateBySource

```cpp
Effect->StackingType =
	EGameplayEffectStackingType::AggregateBySource;
```

每个来源 ASC 分别维护自己的堆叠：

```text
法师 A -> 敌人：一组火焰 Debuff 堆叠
法师 B -> 敌人：另一组火焰 Debuff 堆叠
```

设置 `StackLimitCount = 1` 后：

- 法师 A 对同一目标重复施加时，A 的这一组最多保持 1 层。
- 法师 B 仍可以在该目标上维护自己的 1 层。
- 所以同一目标上可能同时存在 A 和 B 各自造成的 Debuff。

这适合“每个施法者只能在目标身上保留一份自己的 DOT”的规则。

### AggregateByTarget

```cpp
Effect->StackingType =
	EGameplayEffectStackingType::AggregateByTarget;
```

目标把所有来源施加的同一种 GameplayEffect 合并为一组：

```text
法师 A 的火焰 Debuff ┐
                    ├─> 目标身上的同一组堆叠
法师 B 的火焰 Debuff ┘
```

设置 `StackLimitCount = 1` 后，无论来自哪个施法者，该目标最多只有这一组的一层。

这适合“不关心是谁施加，同种状态全局只能存在一份”的规则。

### StackLimitCount = 1 不代表完全忽略再次应用

达到 1 层后再次应用，层数不会超过 1，但应用仍可能刷新其他状态。UE 5.6 的 GameplayEffect 默认值包括：

```cpp
StackDurationRefreshPolicy =
	EGameplayEffectStackingDurationPolicy::RefreshOnSuccessfulApplication;

StackPeriodResetPolicy =
	EGameplayEffectStackingPeriodPolicy::ResetOnSuccessfulApplication;
```

因此再次成功应用时通常会：

- 把持续时间重新刷新为完整时长；
- 重置到下一次周期 Tick 的等待时间；
- 保持 StackCount 为 1。

如果需要不同表现，可以显式配置：

```cpp
Effect->StackDurationRefreshPolicy =
	EGameplayEffectStackingDurationPolicy::NeverRefresh;

Effect->StackPeriodResetPolicy =
	EGameplayEffectStackingPeriodPolicy::NeverReset;
```

### 当前动态创建方式对堆叠的影响

UE 5.6 查找可堆叠 ActiveGameplayEffect 时会先判断：

```cpp
ActiveEffect.Spec.Def == NewSpec.Def
```

也就是说，堆叠要求两次应用共享同一个 `UGameplayEffect` Definition 指针：

- `AggregateBySource`：Definition 相同，并且 Source ASC 相同。
- `AggregateByTarget`：Definition 相同即可，不要求 Source ASC 相同。

如果每次调用 `Debuff()` 都创建一个全新的 `UGameplayEffect` 对象，即使两个对象配置和名称完全相同，它们的 Definition 指针不同，也不会互相堆叠。

若必须保证可靠堆叠，更稳妥的做法是：

- 使用一个 GameplayEffect 资产/类作为固定 Definition；或
- 按 Debuff 类型缓存并复用一个运行时创建的 Definition；
- 把每次变化的数据放入 Spec，例如 `DynamicGrantedTags` 和 SetByCaller Magnitude。

## 3. DurationMagnitude 为什么使用 FScalableFloat

原代码：

```cpp
Effect->DurationPolicy =
	EGameplayEffectDurationType::HasDuration;

Effect->DurationMagnitude = FScalableFloat(DebuffDuration);
```

### DurationMagnitude 的真实类型

`DurationMagnitude` 并不是 `float`，也不是直接声明为 `FScalableFloat`，它的真实类型是：

```cpp
FGameplayEffectModifierMagnitude DurationMagnitude;
```

`FGameplayEffectModifierMagnitude` 是一个“数值如何计算”的统一包装，可以表示多种计算方式：

- `ScalableFloat`：基础值，可按 GameplayEffect Level 通过曲线缩放；
- `AttributeBased`：根据捕获到的属性计算；
- `CustomCalculationClass`：通过自定义 MMC 计算；
- `SetByCaller`：创建 Spec 后由调用者传入。

它提供了接收 `FScalableFloat` 的构造函数：

```cpp
FGameplayEffectModifierMagnitude(
	const FScalableFloat& Value
);
```

所以这句代码：

```cpp
Effect->DurationMagnitude = FScalableFloat(DebuffDuration);
```

实际表达的是：创建一个以 `ScalableFloat` 为计算模式的 `FGameplayEffectModifierMagnitude`，再赋给 DurationMagnitude。

写得更完整等价于：

```cpp
Effect->DurationMagnitude =
	FGameplayEffectModifierMagnitude(
		FScalableFloat(DebuffDuration)
	);
```

### FScalableFloat 的意义

`FScalableFloat` 表示：

```text
最终值 = Value × Curve[Level]
```

如果没有设置 Curve，它就是一个固定值。当前写法：

```cpp
FScalableFloat(DebuffDuration)
```

只设置了 `Value`，没有设置曲线，因此最终持续时间就是 `DebuffDuration`。

如果关联了曲线，就可以让持续时间随 GameplayEffect Level 变化。例如 Spec 使用 Level 5 时，系统会用 Level 5 对应的曲线值计算结果。

### 为什么不直接只用 float

GAS 需要让同一份 GameplayEffect 配置支持：

- 固定数值；
- 随等级成长；
- 根据属性变化；
- 自定义计算；
- 运行时由调用者设置。

因此 Duration 使用统一的 Magnitude 系统，而不是只能保存一个固定 `float`。

当前 `DebuffDuration` 已经是运行时计算出的最终值，所以使用无曲线的 `FScalableFloat` 是最简单的常量 Magnitude。

## 4. Effect->Modifiers[Index] 的作用

原代码：

```cpp
const int32 Index = Effect->Modifiers.Num();
Effect->Modifiers.Add(FGameplayModifierInfo());

FGameplayModifierInfo& ModifierInfo =
	Effect->Modifiers[Index];
```

`Effect->Modifiers` 的类型是：

```cpp
TArray<FGameplayModifierInfo> Modifiers;
```

每个 `FGameplayModifierInfo` 描述一条属性修改规则，主要包含：

```text
Attribute          修改哪个属性
ModifierOp         怎样修改：Add、Multiply、Divide、Override 等
ModifierMagnitude  修改多少
```

### 当前三行代码的执行过程

假设数组原来有 `N` 个元素：

1. `Num()` 返回 `N`，所以 `Index = N`。
2. `Add()` 在末尾添加一个默认的 Modifier。
3. 新元素的下标正好是 `N`。
4. `Modifiers[Index]` 返回这个新元素。
5. 使用引用 `ModifierInfo` 直接修改数组里的真实元素，而不是修改副本。

后续代码：

```cpp
ModifierInfo.ModifierMagnitude =
	FScalableFloat(DebuffDamage);

ModifierInfo.ModifierOp = EGameplayModOp::Additive;
ModifierInfo.Attribute = GetIncomingDamageAttribute();
```

表示每次 GameplayEffect 执行时：

```text
IncomingDamage += DebuffDamage
```

因为该 Effect 还设置了 `Period = DebuffFrequency`，所以这个 Modifier 会周期性地向 `IncomingDamage` 元属性写入 Debuff 伤害，随后由 AttributeSet 的伤害处理流程扣除生命值。

### 单独写 Effect->Modifiers[Index] 没有实际修改

下面这一句只取出元素并立即丢弃结果：

```cpp
Effect->Modifiers[Index];
```

真正有用的是把结果保存为引用，随后写入成员：

```cpp
FGameplayModifierInfo& ModifierInfo =
	Effect->Modifiers[Index];
```

如果忘记 `&`，就会得到副本：

```cpp
// 修改的是副本，不会写回 Modifiers 数组
FGameplayModifierInfo ModifierInfo =
	Effect->Modifiers[Index];
```

### 更简洁的写法

可以直接添加默认元素并取得引用：

```cpp
FGameplayModifierInfo& ModifierInfo =
	Effect->Modifiers.AddDefaulted_GetRef();

ModifierInfo.ModifierMagnitude =
	FScalableFloat(DebuffDamage);

ModifierInfo.ModifierOp = EGameplayModOp::Additive;
ModifierInfo.Attribute = GetIncomingDamageAttribute();
```

这种写法不需要手动计算 Index，意图也更清晰。

注意：如果取得 `ModifierInfo` 引用后继续向 `Modifiers` 添加元素，TArray 可能重新分配内存，使旧引用失效。应先配置完当前引用，再进行可能引发扩容的操作。

## 5. 当前 Debuff GameplayEffect 配置的整体含义

```cpp
Effect->DurationPolicy =
	EGameplayEffectDurationType::HasDuration;

Effect->Period = FScalableFloat(DebuffFrequency);
Effect->DurationMagnitude = FScalableFloat(DebuffDuration);

Effect->StackingType =
	EGameplayEffectStackingType::AggregateBySource;

Effect->StackLimitCount = 1;
```

整体含义是：

1. 创建一个存在 `DebuffDuration` 秒的持续型 GameplayEffect。
2. 每隔 `DebuffFrequency` 秒执行一次 Modifier。
3. 每次执行向 `IncomingDamage` 增加 `DebuffDamage`。
4. 同一个来源对同一个目标最多维持一层该 Effect。
5. 默认情况下，再次成功应用会刷新持续时间并重置周期计时。
6. 不同来源可以分别在目标身上维持自己的这一层 Effect。

还需要满足一个关键前提：重复应用必须共享同一个 GameplayEffect Definition，堆叠设置才会真正参与匹配。
