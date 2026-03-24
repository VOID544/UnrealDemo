// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"

// 必要头文件：UUserWidget 的定义
#include "Blueprint/UserWidget.h"

// Enhanced Input 相关：用于绑定输入动作（EnhancedInputComponent）和注册 MappingContext（EnhancedInputLocalPlayerSubsystem）
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// InputAsset 类型定义（如果头文件路径不同你已在 .h 中前向声明，可在 .cpp 中包含以便使用 LoadSynchronous 等）
#include "InputAction.h"
#include "InputMappingContext.h"

// 重写 BeginPlay：这里只是示例的调试信息，展示 Controller 正常运行
void AMyPlayerController::BeginPlay()
{
	// 调用父类的 BeginPlay，保证父类初始化逻辑正常执行
	Super::BeginPlay();

	// 安全检查 GEngine 是否有效，然后打印一条屏幕消息用于调试
	if (GEngine)
	{
		// AddOnScreenDebugMessage 参数说明：
		// -1：消息键（-1 表示总是新建一条，不覆盖之前的），5.f：显示时间（秒），FColor::Red：颜色，文本内容。
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.f,
			FColor::Red,
			TEXT("MyPlayerController is Alive!")
		);
	}
}

// SetupInputComponent：在这里注册输入绑定、加载 MappingContext 并把 ToggleInventoryAction 绑定到方法
void AMyPlayerController::SetupInputComponent()
{
	// 先调用父类实现，确保 InputComponent 等基类资源已准备好
	Super::SetupInputComponent();

	// 把默认的 InputComponent 强转为 EnhancedInputComponent，
	// Enhanced Input 的绑定接口位于 UEnhancedInputComponent，只有在使用 Enhanced Input 插件时才有效。
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInput)
	{
		// 如果转换失败，说明当前 PlayerController 没有使用 Enhanced Input（或未正确初始化）
		UE_LOG(LogTemp, Warning, TEXT("AMyPlayerController: EnhancedInputComponent unavailable."));
		return;
	}

	// 通过软引用同步加载 ToggleInventoryAction（这是一个 UInputAction 资源）
	// LoadSynchronous 会在编辑时/运行时尝试加载资源，如果为空则表示在编辑器中没有指定它
	UInputAction* ToggleAction = ToggleInventoryAction.LoadSynchronous();
	if (!ToggleAction)
	{
		// 没有指定输入动作时提示警告并返回，不要继续绑定空动作
		UE_LOG(LogTemp, Warning, TEXT("AMyPlayerController: ToggleInventoryAction not set."));
		return;
	}

	// 注册 MappingContext 到本地玩家的 Enhanced Input 子系统：
	// 1) 获取本地玩家（LocalPlayer）
	// 2) 从 LocalPlayer 获取 UEnhancedInputLocalPlayerSubsystem
	// 3) 把 MappingContext 加入子系统（优先级 0）
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			// 通过软引用加载 MappingContext 资源（如果没有设置则输出警告）
			if (UInputMappingContext* Mapping = DefaultMappingContext.LoadSynchronous())
			{
				// AddMappingContext 的第二个参数是优先级（数值越大优先级越高）
				Subsystem->AddMappingContext(Mapping, 0);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("AMyPlayerController: DefaultMappingContext not set."));
			}
		}
	}

	// 最后把输入动作绑定到本类的 ToggleInventory 方法
	// ETriggerEvent::Started 表示在按下（开始触发）时调用（也可根据需要使用 Completed/Triggered 等）
	EnhancedInput->BindAction(ToggleAction, ETriggerEvent::Started, this, &AMyPlayerController::ToggleInventory);
}

// ToggleInventory：核心逻辑，控制背包 UI 的创建、显示/隐藏与输入模式切换
void AMyPlayerController::ToggleInventory()
{
	// 步骤 A：如果还没有创建背包 UI 的实例，则创建之
	if (!InventoryWidgetInstance)
	{
		// 确认在编辑器中已经指定了 InventoryWidgetClass，如果没有则记录警告并返回
		if (!InventoryWidgetClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("AMyPlayerController: InventoryWidgetClass not assigned."));
			return;
		}

		// CreateWidget<T> 用于创建 UUserWidget 的实例
		// 第一个参数 this（PlayerController）作为 Outer，第二个参数是类引用
		InventoryWidgetInstance = CreateWidget<UUserWidget>(this, InventoryWidgetClass);
		if (!InventoryWidgetInstance)
		{
			// 如果创建失败，记录警告并返回
			UE_LOG(LogTemp, Warning, TEXT("AMyPlayerController: Failed to create inventory widget."));
			return;
		}

		// 把 Widget 添加到视口（Viewport）以便渲染
		InventoryWidgetInstance->AddToViewport();

		// 初始默认设置为 Collapsed（折叠隐藏），这样创建时不会立刻显示
		InventoryWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
	}

	// 步骤 B：判断当前是否应该显示（如果当前不是 Visible 就显示）
	const bool bShouldShow = InventoryWidgetInstance->GetVisibility() != ESlateVisibility::Visible;

	// 使用 SetVisibility 切换显示/隐藏（Visible 或 Collapsed）
	InventoryWidgetInstance->SetVisibility(bShouldShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	// 步骤 C：根据显示与否切换输入模式与鼠标显示
	if (bShouldShow)
	{
		// 当 UI 显示时，我们需要：
		// - 允许玩家同时与游戏和 UI 交互（GameAndUI）
		// - 把焦点设置到刚创建的 Widget 上（确保键盘/焦点事件发送给该 Widget）
		// - 显示鼠标光标，方便点击 UI 元素
		FInputModeGameAndUI InputMode;
		// SetWidgetToFocus 需要一个 TSharedRef<SWidget>，通过 TakeWidget 获取底层 Slate Widget 并设置焦点
		InputMode.SetWidgetToFocus(InventoryWidgetInstance->TakeWidget());
		// 不在捕获期间隐藏光标（我们想看到光标）
		InputMode.SetHideCursorDuringCapture(false);
		SetInputMode(InputMode);
		SetShowMouseCursor(true);
	}
	else
	{
		// 当 UI 隐藏时，恢复到纯游戏模式（GameOnly），隐藏鼠标，以便继续控制角色
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		SetShowMouseCursor(false);
	}
}
