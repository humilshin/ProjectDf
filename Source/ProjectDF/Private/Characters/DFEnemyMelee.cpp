#include "Characters/DFEnemyMelee.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/AIBlueprintHelperLibrary.h" // ✅ SimpleMoveToActor() 사용을 위한 include
#include "TimerManager.h"

ADFEnemyMelee::ADFEnemyMelee()
{
    PrimaryActorTick.bCanEverTick = false;

    AttackRangeComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("AttackSphere"));
    AttackRangeComp->SetupAttachment(GetRootComponent());
    AttackRangeComp->SetCapsuleSize(44.f, 60.f);
    AttackRangeComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    AttackRangeComp->SetCollisionObjectType(ECC_WorldDynamic);
    AttackRangeComp->SetCollisionResponseToAllChannels(ECR_Ignore);
    AttackRangeComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    AttackRangeComp->SetGenerateOverlapEvents(true);
}

void ADFEnemyMelee::BeginPlay()
{
    Super::BeginPlay();

    // 이동 속도 적용
    if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
    {
        MoveComp->MaxWalkSpeed = MoveSpeed;
    }
    
    // 히어로 캐시
    Hero = UGameplayStatics::GetPlayerPawn(this, 0);

    // 오버랩 이벤트 연결
    if (AttackRangeComp)
    {
        AttackRangeComp->OnComponentBeginOverlap.AddDynamic(this, &ADFEnemyMelee::OnAttackOverlapBegin);
        AttackRangeComp->OnComponentEndOverlap.AddDynamic(this, &ADFEnemyMelee::OnAttackOverlapEnd);
    }

    // 경로 갱신 타이머 시작
    GetWorldTimerManager().SetTimer(
        MoveUpdateHandle,
        this,
        &ADFEnemyMelee::UpdateMoveToHero,
        PathRefreshInterval,
        true,
        0.2f
    );
}

void ADFEnemyMelee::UpdateMoveToHero()
{

    if (!Hero)
        Hero = UGameplayStatics::GetPlayerPawn(this, 0);
    
    AController* MyController = GetController();
    if (!MyController || !Hero) return;

    // ✅ 한 줄로 경로 계산 및 MoveTo 실행
    UAIBlueprintHelperLibrary::SimpleMoveToActor(MyController, Hero);
}

void ADFEnemyMelee::OnAttackOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor != Hero) return;

    bOverlappingHero = true;

    if (!bDamageLoopActive)
    {
        bDamageLoopActive = true;
        TryApplyDamage();

        GetWorldTimerManager().SetTimer(
            DamageLoopHandle,
            this,
            &ADFEnemyMelee::TryApplyDamage,
            DamageInterval,
            true,
            DamageInterval
        );
    }
}

void ADFEnemyMelee::OnAttackOverlapEnd(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    if (!OtherActor || OtherActor != Hero) return;
    bOverlappingHero = false;

    if (bDamageLoopActive)
    {
        GetWorldTimerManager().ClearTimer(DamageLoopHandle);
        bDamageLoopActive = false;
    }
}

void ADFEnemyMelee::TryApplyDamage()
{
    if (!bOverlappingHero || !Hero) return;
    UGameplayStatics::ApplyDamage(
        Hero,
        Damage,
        GetController(),
        this,
        UDamageType::StaticClass()
    );
}
