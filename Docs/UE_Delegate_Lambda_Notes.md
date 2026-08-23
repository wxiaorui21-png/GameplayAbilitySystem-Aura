# Unreal Engine：Lambda、WeakLambda、AddLambda 与 AddWeakLambda

本文档只是项目笔记，不在 `Source` 目录中，不会参与 Unreal Build Tool 编译。

## 1. Lambda 是什么

Lambda 是 C++ 的匿名函数：

```cpp
int32 BaseValue = 10;

auto AddToBase = [BaseValue](int32 Value)
{
	return BaseValue + Value;
};
```

`[]` 是捕获列表：

- `[]`：不捕获外部变量。
- `[Value]`：按值捕获，Lambda 内保存一份副本。
- `[&Value]`：按引用捕获，必须保证执行时 `Value` 仍然存在。
- `[this]`：捕获当前对象的裸 `this` 指针，不会自动检查 UObject 是否已销毁。
- `[WeakObject]`：常用于显式捕获 `TWeakObjectPtr`，执行前自行调用 `IsValid()`。

## 2. Lambda 与 WeakLambda 的区别

在 UE 委托语境下，通常所说的 `Lambda` 和 `WeakLambda` 指的是两种委托绑定方式，而不是两种 C++ Lambda 语法。

### 普通 Lambda

```cpp
Delegate.AddLambda([this]()
{
	DoSomething();
});
```

委托不会知道捕获的 `this` 指向一个 UObject，也不会自动检查该对象是否还有效。如果对象已经销毁但委托仍然广播，访问 `this` 可能导致悬空指针和崩溃。

### WeakLambda

```cpp
Delegate.AddWeakLambda(this, [this]()
{
	DoSomething();
});
```

第一个参数 `this` 是与绑定关联的 UObject。UE 以弱引用方式跟踪它：

- 不会因为委托绑定而阻止 UObject 被垃圾回收。
- 广播时如果该 UObject 已失效，Lambda 不会执行。
- 它只保护传给 `AddWeakLambda` 的这个 UObject；Lambda 中捕获的其他裸指针或引用仍需自行保证安全。

因此，当 Lambda 捕获 UObject 的 `this`，而委托的生命周期可能长于该对象时，优先使用 `AddWeakLambda(this, ...)`。

## 3. AddLambda 与 AddWeakLambda

两者都用于把 Lambda 添加到原生多播委托，并返回 `FDelegateHandle`。

| API | UObject 生命周期检查 | 常见用途 |
| --- | --- | --- |
| `AddLambda(Lambda)` | 没有 | 不捕获 UObject；或能严格保证绑定和捕获对象的生命周期 |
| `AddWeakLambda(UserObject, Lambda)` | 检查 `UserObject` | Lambda 捕获 UObject，委托可能比对象活得更久 |

```cpp
FDelegateHandle Handle = Delegate.AddLambda([](int32 NewValue)
{
	UE_LOG(LogTemp, Log, TEXT("Value: %d"), NewValue);
});

FDelegateHandle WeakHandle = Delegate.AddWeakLambda(this, [this](int32 NewValue)
{
	HandleValueChanged(NewValue);
});
```

如果需要主动解绑，应保存句柄并调用：

```cpp
Delegate.Remove(WeakHandle);
WeakHandle.Reset();
```

`AddWeakLambda` 能避免已销毁 UObject 被回调，但不等于不需要解绑。重复绑定、逻辑停用或持有大量失效项时，仍建议通过 `FDelegateHandle` 管理绑定。

## 4. CreateLambda 与 CreateWeakLambda

`Create...` 用于创建一个委托实例，`Add...` 用于向多播委托中增加一个绑定：

```cpp
FMyDelegate SingleDelegate = FMyDelegate::CreateLambda([]()
{
	// 单播委托回调
});

FMyMulticastDelegate MulticastDelegate;
MulticastDelegate.AddLambda([]()
{
	// 多播委托中的一个回调
});
```

对应的弱引用版本是 `CreateWeakLambda(UserObject, Lambda)` 和 `AddWeakLambda(UserObject, Lambda)`。

## 5. 当前 Debuff 组件的适配示例

你的组件是 UObject，因此弱绑定可以写成：

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

这个示例要求 `FOnDebuffGetASC` 传递 ASC 指针，而且接口返回真实委托的引用：

```cpp
class UAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(
	FOnDebuffGetASC,
	UAbilitySystemComponent*
);

virtual FOnDebuffGetASC& GetOnDebuffGetASC() = 0;
```

实现也需要返回引用：

```cpp
FOnDebuffGetASC& AAuraCharacterBase::GetOnDebuffGetASC()
{
	return DebuffGetASCDelegate;
}
```

当前源码中的以下形式返回的是副本：

```cpp
virtual FOnDebuffGetASC GetOnDebuffGetASC() = 0;
```

对这个返回值调用 `AddWeakLambda`，绑定只会加到临时副本中，之后无法收到 `DebuffGetASCDelegate.Broadcast(...)`。因此这里必须返回 `FOnDebuffGetASC&`。

## 6. 额外注意事项

- `AddLambda` 和 `AddWeakLambda` 适用于原生委托；动态多播委托通常使用 `AddDynamic`/`AddUniqueDynamic`，不能直接绑定 Lambda。
- `[&]` 捕获局部变量用于延迟回调通常很危险，因为回调执行时局部变量可能早已离开作用域。
- `AddWeakLambda(this, [this] { ... })` 中，第一个 `this` 负责 UE 的弱生命周期检查，捕获列表中的 `[this]` 则让 Lambda 能访问成员；两者用途不同。
- 如果只需调用一个 UObject 成员函数，`AddUObject(this, &Class::Function)` 通常更直接；需要在回调中组合多步逻辑时再使用 `AddWeakLambda`。

## 7. 动态多播委托与原生多播委托

当前 `CombatInterface.h` 中的两个声明分别属于原生多播委托和动态多播委托：

```cpp
DECLARE_MULTICAST_DELEGATE_OneParam(
	FOnDebuffGetASC,
	UAbilitySystemComponent*
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnDeath,
	AActor*,
	Character
);
```

第二个宏声明后也需要分号。即使部分宏展开形式可能让错误表现得不直观，也应统一写成 `);`。

### 核心区别

| 对比项 | `DECLARE_MULTICAST_DELEGATE` | `DECLARE_DYNAMIC_MULTICAST_DELEGATE` |
| --- | --- | --- |
| UE 反射系统 | 不参与 | 参与 |
| 蓝图中绑定事件 | 不支持 | 支持，可配合 `BlueprintAssignable` |
| 序列化 | 不支持反射序列化 | 支持 UE 动态委托序列化 |
| 回调函数要求 | 普通 C++ 函数即可 | 必须是匹配签名的 `UFUNCTION()` |
| Lambda | 支持 `AddLambda`、`AddWeakLambda` | 不支持直接绑定 Lambda |
| 常见绑定方式 | `AddUObject`、`AddWeakLambda`、`AddLambda` | `AddDynamic`、`AddUniqueDynamic` |
| 移除方式 | `Remove(DelegateHandle)`、`RemoveAll(Object)` | `RemoveDynamic(Object, Function)`、`RemoveAll(Object)` |
| 调用开销 | 较低，C++ 直接调用 | 较高，需要通过 UObject 反射和函数名调用 |
| 参数声明 | 只写参数类型 | 每个参数都必须写“类型 + 参数名” |

选择原则：

- 只在 C++ 内使用，并且需要 Lambda、较低开销或更灵活的绑定方式：使用原生委托。
- 需要在蓝图中分配事件，或者需要 UE 反射/序列化能力：使用动态委托。
- “动态”不是指异步、延迟执行或运行时才广播；它指委托接入了 UE 的反射系统。

### 原生多播委托示例：FOnDebuffGetASC

声明和成员：

```cpp
DECLARE_MULTICAST_DELEGATE_OneParam(
	FOnDebuffGetASC,
	UAbilitySystemComponent*
);

FOnDebuffGetASC OnDebuffGetASC;
```

绑定 UObject 成员函数：

```cpp
OnDebuffGetASC.AddUObject(
	this,
	&UMyObject::HandleDebuffASC
);
```

也可以绑定弱 Lambda：

```cpp
OnDebuffGetASC.AddWeakLambda(
	this,
	[this](UAbilitySystemComponent* ASC)
	{
		if (IsValid(ASC))
		{
			// 使用 ASC
		}
	}
);
```

广播：

```cpp
OnDebuffGetASC.Broadcast(AbilitySystemComponent);
```

### 动态多播委托示例：FOnDeath

声明后，可以把它作为 UObject/Actor 类的反射属性暴露给蓝图：

```cpp
UPROPERTY(BlueprintAssignable, Category = "Combat")
FOnDeath OnDeath;
```

接收函数必须是 `UFUNCTION()`，而且参数签名必须一致：

```cpp
UFUNCTION()
void HandleDeath(AActor* Character);
```

绑定：

```cpp
OnDeath.AddDynamic(this, &UMyObject::HandleDeath);
```

如果不允许同一个对象和函数被重复绑定，可以使用：

```cpp
OnDeath.AddUniqueDynamic(this, &UMyObject::HandleDeath);
```

广播和解绑：

```cpp
OnDeath.Broadcast(DeadCharacter);
OnDeath.RemoveDynamic(this, &UMyObject::HandleDeath);
```

`BlueprintAssignable` 通常用于动态多播委托，让蓝图通过“绑定事件”或“分配”节点添加监听者。它不是广播权限；C++ 仍然通过 `Broadcast()` 触发委托。

### 生命周期方面

动态委托保存 UObject 与 `UFUNCTION` 名称，并在执行时检查对象有效性，所以不会像捕获裸 `this` 的普通 Lambda 那样直接调用悬空对象。不过仍建议在逻辑结束时解绑，以避免重复绑定和无用的失效绑定。

原生委托的生命周期行为取决于绑定方式：

- `AddUObject`：弱引用跟踪 UObject，对象失效后不再执行。
- `AddWeakLambda`：弱引用检查指定 UObject，对象失效后不再执行。
- `AddLambda`：不理解 Lambda 捕获对象的生命周期。
- `AddRaw`：保存普通 C++ 对象裸指针，必须由开发者保证生命周期并及时解绑。
