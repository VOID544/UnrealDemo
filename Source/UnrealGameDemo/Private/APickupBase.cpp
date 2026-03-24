// Fill out your copyright notice in the Description page of Project Settings.


#include "APickupBase.h"
// 为了判断 OtherActor 是否是玩家，这里包含 ACharacter 的头文件
#include "GameFramework/Character.h"
// 为了访问背包组件的 AddItem 函数，这里包含你的背包组件头文件
#include "MyInventoryComponent.h"

// Sets default values
APickupBase::APickupBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 如果你还没在构造函数中创建组件，可以按下面这样创建并挂接：
	// （如果已经在别处创建了 SphereComponent/MeshComponent，就不要重复创建）

	// 确保 SphereComponent 存在
	if (!SphereComponent)
	{
		// 创建球体碰撞组件，用于检测玩家是否进入拾取范围
		SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		// 把球体设为根组件
		RootComponent = SphereComponent;
		// 设置球体半径，数值可以按需求调整
		SphereComponent->InitSphereRadius(50.f);
		// 设置为只用于重叠检测
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SphereComponent->SetCollisionObjectType(ECC_WorldDynamic);
		SphereComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
		// 绑定重叠事件到我们的处理函数 OnSphereOverlap
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APickupBase::OnSphereOverlap);
	}

	// 确保 MeshComponent 存在
	if (!MeshComponent)
	{
		// 创建静态网格组件，用于显示拾取物模型
		MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
		// 将网格绑定到球体根组件上
		MeshComponent->SetupAttachment(SphereComponent);
		// 通常让网格不参与碰撞由球体负责
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

// Called when the game starts or when spawned
void APickupBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// ================= 重叠逻辑实现 =================
void APickupBase::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// 1. 基本安全检查：排除空指针和与自己重叠的情况
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	// 2. 判断重叠的是否是“玩家”：
	//    这里用 ACharacter 做通用判断，如果你有自定义玩家类，
	//    可以改成 AMyPlayerCharacter* 然后再 Cast。
	ACharacter* OverlapCharacter = Cast<ACharacter>(OtherActor);
	if (!OverlapCharacter)
	{
		// 不是角色（可能是别的物体），不处理
		return;
	}

	// 3. 从玩家身上查找我们的背包组件 UMyInventoryComponent
	//	FindComponentByClass 会返回玩家身上第一个找到的 UMyInventoryComponent，如果没有找到则返回 nullptr。
	UMyInventoryComponent* InventoryComp = OverlapCharacter->FindComponentByClass<UMyInventoryComponent>();
	if (!InventoryComp)
	{
		// 玩家身上没有挂背包组件，打印一条警告便于调试
		UE_LOG(LogTemp, Warning, TEXT("APickupBase::OnSphereOverlap: Player has no UMyInventoryComponent!"));
		return;
	}

	// 4. 调用背包组件的 AddItem 函数
	InventoryComp->AddItem(TEXT("生命药水"), 2);

	// 5. 拾取完成后销毁自己，让场景里不再有这个拾取物
	Destroy();
}
