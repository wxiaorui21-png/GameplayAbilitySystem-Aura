# Unreal Engine：const 引用与 AddImpulse 知识点

本文记录 Aura 项目中 `SetImpulseDirection` 的 const 引用报错，以及 `UPrimitiveComponent::AddImpulse` 第三个参数的作用。

本文档位于 `Docs`，不会参与 Unreal Build Tool 编译。

## 1. const 对象的成员为什么不能传给非常量引用

相关调用位于 `UAuraAbilitySystemLibrary::ApplyDamageEffect()`：

```cpp
FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(
	const FDamageEffectParams& DamageEffectParams)
{
	// ...
	SetImpulseDirection(
		ContextHandle,
		DamageEffectParams.ImpulseDirection
	);
	// ...
}
```

`DamageEffectParams` 的类型是 `const FDamageEffectParams&`。通过这个常量引用访问其成员时，`DamageEffectParams.ImpulseDirection` 也只能作为常量使用。

如果 `SetImpulseDirection` 的第二个参数声明为非常量引用：

```cpp
static void SetImpulseDirection(
	FGameplayEffectContextHandle& ContextHandle,
	FVector& InImpulseDirection
);
```

就会发生 const 不匹配：`const FVector` 不能绑定到 `FVector&`，因为非常量引用允许函数修改传入对象，这会破坏调用方的 const 保证。

### 正确写法

`SetImpulseDirection` 只读取方向，不修改调用方传入的 `FVector`，因此应该使用常量引用，并且头文件声明与 cpp 定义必须一致：

```cpp
// AuraAbilitySystemLibrary.h
static void SetImpulseDirection(
	UPARAM(ref) FGameplayEffectContextHandle& ContextHandle,
	const FVector& InImpulseDirection
);
```

```cpp
// AuraAbilitySystemLibrary.cpp
void UAuraAbilitySystemLibrary::SetImpulseDirection(
	FGameplayEffectContextHandle& ContextHandle,
	const FVector& InImpulseDirection)
{
	if (FAuraGameplayEffectContext* AuraContext =
		static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		AuraContext->SetImpulseDirection(InImpulseDirection);
	}
}
```

### 选择参数类型的原则

- 函数需要修改调用方对象：使用 `T&`。
- 函数只读取较大的结构体或对象：通常使用 `const T&`，既避免复制，也禁止修改。
- 小型基础类型，如 `bool`、`int32`、`float`：通常直接按值传递。

不要使用 `const_cast` 绕过这个错误；本例的真实语义就是“只读”，应直接修正函数签名。

## 2. AddImpulse 第三个参数 bVelChange

角色死亡后，项目通过以下代码给物理网格添加瞬时冲击：

```cpp
GetMesh()->AddImpulse(ImpulseDir, NAME_None, true);
```

UE 5.6 中对应的函数签名是：

```cpp
virtual void AddImpulse(
	FVector Impulse,
	FName BoneName = NAME_None,
	bool bVelChange = false
);
```

三个参数分别表示：

1. `Impulse`：冲击的方向和强度。
2. `BoneName`：骨骼网格中接受冲击的刚体；`NAME_None` 表示根刚体。
3. `bVelChange`：是否把第一个参数直接解释为速度变化量。

### bVelChange = false

这是默认值。传入值按物理冲量处理，质量会影响最终速度变化：

```text
速度变化 ≈ Impulse / Mass
```

相同冲量作用于不同质量的刚体时，质量越大的刚体速度变化越小。这种方式更符合常规物理冲量语义。

### bVelChange = true

传入值被当作速度变化量，忽略刚体质量：

```text
速度变化 ≈ Impulse
```

相同数值作用于不同质量的刚体时，会产生近似相同的速度变化。它适合希望击飞表现稳定、不受 Physics Asset 中质量配置影响的游戏效果。

因此项目中的代码：

```cpp
GetMesh()->AddImpulse(ImpulseDir, NAME_None, true);
```

表示给 Mesh 的根刚体施加一次忽略质量的速度变化。

## 3. 当前死亡击飞流程

`AAuraCharacterBase::MulticastHandleDeath_Implementation()` 中先开启物理模拟，再添加冲量：

```cpp
GetMesh()->SetSimulatePhysics(true);
GetMesh()->SetEnableGravity(true);
GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
GetMesh()->AddImpulse(ImpulseDir, NAME_None, true);
```

这里的关键点是：

- `AddImpulse` 是一次性的瞬时冲击，不是每帧持续施加的力。
- 接受冲量的刚体必须正在模拟物理，否则不会产生预期的物理运动。
- `NAME_None` 只对根刚体施加冲量；若要从指定骨骼开始影响多个刚体，可根据需求研究 `AddImpulseToAllBodiesBelow()`。
- 使用 `true` 能让死亡击飞速度不依赖角色质量，但也会弱化轻重角色之间的物理差异。

武器使用了较小的冲击量：

```cpp
Weapon->AddImpulse(ImpulseDir * 0.1, NAME_None, true);
```

由于 `bVelChange` 也是 `true`，这里的 `0.1` 直接把武器的速度变化缩放为角色 Mesh 的十分之一，而不是通过武器质量间接缩放。
