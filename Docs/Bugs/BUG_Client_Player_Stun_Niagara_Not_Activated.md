# BUG：客户端玩家角色的眩晕 Niagara 特效未激活

## 状态

- 类型：网络复制 / Gameplay Ability System / Niagara
- 影响功能：玩家角色受到眩晕 Debuff 时的客户端视觉反馈
- 当前状态：已修复

## 现象

敌人受到眩晕 Debuff 时，客户端能够看到其眩晕 Niagara 特效；玩家角色受到眩晕时，客户端可能没有对应的特效。

`bIsStunned` 已在 `AAuraCharacterBase` 中声明为复制属性，并使用 `OnRep_Stunned` 作为复制回调。服务端在 `StunnedTagChanged()` 中根据 `Debuff.Stun` Tag 更新该变量。可是 `AAuraCharacterBase::OnRep_Stunned()` 是空实现，玩家角色此前也没有重写该回调来更新眩晕粒子。

## 原因

`OnRep_Stunned` 在客户端收到 `bIsStunned` 的新值时自动调用。它是玩家客户端获得眩晕状态的可靠同步入口。

此前该回调没有处理 `StunNiagaraComponent` 的状态。因此即使服务器已更新眩晕状态并将 `bIsStunned` 复制到玩家客户端，客户端的眩晕粒子也不会因为这次复制更新而被显式激活或关闭。

敌人与玩家的 ASC 所在位置和初始化流程不同：敌人的 ASC 在敌人 Actor 上，而玩家 ASC 在 PlayerState 上。不能只依赖两者都恰好通过同一种 ASC Tag 监听路径更新视觉效果；玩家角色应在自己的复制回调中处理其本地组件的表现。

## 本次解决方案

在 `AAuraCharacter::OnRep_Stunned()` 中，根据已经复制到客户端的 `bIsStunned` 显式同步眩晕表现：

- `bIsStunned == true`：添加本地输入阻塞 Loose Tags，并激活 `StunNiagaraComponent`。
- `bIsStunned == false`：移除这些 Loose Tags，并停用 `StunNiagaraComponent`。

这样服务器只需权威地更新眩晕状态；每个客户端在收到复制状态后，分别更新自己本地的输入限制和 Niagara 组件。

相关文件：

- `Source/Aura/Private/Character/AuraCharacter.cpp`
- `Source/Aura/Public/Character/AuraCharacterBase.h`
- `Source/Aura/Private/Character/AuraCharacterBase.cpp`

## 范围说明

本修复针对 `Debuff.Stun` 与 `StunNiagaraComponent`。燃烧特效使用 `Debuff.Burn` 和 `BurnNiagaraComponent`，若燃烧仍有客户端显示问题，需要单独检查其 Tag 事件注册与 ASC 初始化时序，不能将本修复视为燃烧问题的修复。

## 验证

1. 以客户端玩家角色作为眩晕目标。
2. 服务端施加 `Debuff.Stun`。
3. 确认客户端收到 `bIsStunned = true` 后出现眩晕 Niagara，且输入被阻塞。
4. 等待 Debuff 到期或移除 Debuff。
5. 确认客户端收到 `bIsStunned = false` 后粒子关闭，输入恢复。
