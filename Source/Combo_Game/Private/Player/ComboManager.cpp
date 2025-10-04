// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ComboManager.h"

// Sets default values for this component's properties
UComboManager::UComboManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UComboManager::BeginPlay()
{
	Super::BeginPlay();
    // 创建UI管理器
    UIManager = NewObject<UUIManager>();
    
    // 设置输入
    SetupPlayerInputComponent();
	// ...
	
}


// Called every frame
void UComboManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UComboManager::SetupPlayerInputComponent()
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    // 获取或创建输入组件
    InputComponent = Owner->FindComponentByClass<UInputComponent>();
    if (!InputComponent)
    {
        InputComponent = NewObject<UInputComponent>(Owner);
        Owner->AddInstanceComponent(InputComponent);
        InputComponent->RegisterComponent();
    }

    // 绑定按键
    if (InputComponent)
    {
        InputComponent->BindAction("ToggleUI", IE_Pressed, this, &UComboManager::OnToggleUIKeyPressed);
        UE_LOG(LogTemp, Warning, TEXT("ComboManagerComponent: 输入绑定完成，按Tab键切换UI"));
    }
}

void UComboManager::OnToggleUIKeyPressed()
{
    ToggleUI();
}

void UComboManager::ToggleUI()
{
    if (bIsUIVisible)
    {
        HideUI();
    }
    else
    {
        ShowUI();
    }
}

void UComboManager::ShowUI()
{
    if (!UIManager) return;

    UIManager->CreateMainUI();
    bIsUIVisible = true;
    SwitchToUIMode();
    
    UE_LOG(LogTemp, Warning, TEXT("ComboManagerComponent: 显示UI"));
}

void UComboManager::HideUI()
{
    if (!UIManager) return;

    UIManager->CloseUI();
    bIsUIVisible = false;
    SwitchToGameMode();
    
    UE_LOG(LogTemp, Warning, TEXT("ComboManagerComponent: 隐藏UI"));
}

void UComboManager::SwitchToUIMode()
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    APlayerController* PC = Cast<APlayerController>(Owner);
    if (!PC)
    {
        // 如果Owner不是PlayerController，尝试获取
        if (APawn* Pawn = Cast<APawn>(Owner))
        {
            PC = Pawn->GetController<APlayerController>();
        }
    }

    if (PC)
    {
        PC->SetShowMouseCursor(true);
        FInputModeUIOnly InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(InputMode);
        
        UE_LOG(LogTemp, Warning, TEXT("切换到UI输入模式"));
    }
}

void UComboManager::SwitchToGameMode()
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    APlayerController* PC = Cast<APlayerController>(Owner);
    if (!PC)
    {
        if (APawn* Pawn = Cast<APawn>(Owner))
        {
            PC = Pawn->GetController<APlayerController>();
        }
    }

    if (PC)
    {
        PC->SetShowMouseCursor(false);
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        
        UE_LOG(LogTemp, Warning, TEXT("切换到游戏输入模式"));
    }
}


