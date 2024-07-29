#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFilemanager.h"
#include "Kismet/GameplayStatics.h"
#include "CRot.generated.h"

UCLASS()
class IMPORTTRY_API ACRot : public AActor
{
    GENERATED_BODY()

public:
    ACRot();

    virtual void Tick(float DeltaTime) override;

    // Set applied voltage to the motor
    void SetVoltage(float Voltage);

    // Set load torque on the motor
    void SetLoadTorque(float Torque);

    void EulerDerivative(float CustomDT);

protected:
    virtual void BeginPlay() override;

private:
    // Motor parameters
    float Resistance; // Resistance of the motor
    float Inductance; // Inductance of the motor
    float Kt;         // Torque constant
    float Kv;         // Voltage constant
    float J;          // Moment of inertia
    float Friction;          // Viscous friction coefficient

    // Motor state variables
    float Current;    // Current flowing through the motor
    float AngularVelocity; // Angular velocity of the motor shaft

    // Applied voltage and load torque
    float AppliedVoltage;
    float LoadTorque;

    float RealTimeSeconds;

    // Mesh component representing the motor
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DCMOTOR", meta = (AllowPrivateAccess = "true"))
    USceneComponent* SceneComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DCMOTOR", meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* ItemMesh;

    // Rotation rate and axis
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DCMOTOR", meta = (AllowPrivateAccess = "true"))
    FRotator RotationRate;

    // Output file path
    FString FilePath;

    void updateAfterDelay();
    void  StartDelayUpdate();

    FTimerHandle TimerHandle;
    double ResistanceValues[19] = { 3.8, 4.2, 4.8, 4.9, 5.2, 5.4, 5.8, 6.2, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4, 0.4 };
    int valuesIndex = 0;
};






