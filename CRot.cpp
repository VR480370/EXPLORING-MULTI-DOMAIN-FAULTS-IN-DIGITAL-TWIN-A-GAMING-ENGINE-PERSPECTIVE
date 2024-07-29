
#include "CRot.h"
#include "Kismet/GameplayStatics.h" // Include this for accessing game time
#include "Misc/FileHelper.h" // Include this for file operations
#include "Misc/Paths.h" // Include this for file paths

// Sets default values for this actor's properties
ACRot::ACRot()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create and attach scene component
    SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
    RootComponent = SceneComponent;

    // Create and attach mesh component
    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    ItemMesh->SetupAttachment(RootComponent);

    // Default rotation rate 
    RotationRate = FRotator(0.0f, 0.0f, 180.0f);

    // Default motor parameters 
    Resistance = 0.4f;
    Inductance = 0.05f;
    Kt = 1.0f;
    Kv = 1.25f;
    J = 0.5f;
    Friction = 0.001f;

    // Initialize motor state variables
    Current = 0.0f;
    AngularVelocity = 0.0f;

    // Initial applied voltage and load torque
    AppliedVoltage = 0.0f;
    LoadTorque = 0.0f;

    // Simulation time
    RealTimeSeconds = 0.0f;

    FilePath = FPaths::ProjectSavedDir() + TEXT("test.txt");
    FFileHelper::SaveStringToFile(TEXT("RealTimeSeconds \t DeltaTime \t Input Voltage \t Current \t Resistance \t Angular Velocity\n"), *FilePath);
}

// Called when the game starts or when spawned
void ACRot::BeginPlay()
{
    Super::BeginPlay();
    ACRot::StartDelayUpdate();
}

void ACRot::updateAfterDelay()
{
    if (this->valuesIndex >= 19)
    {
        this->valuesIndex = 0;
    }

    this->Resistance = this->ResistanceValues[this->valuesIndex];

    this->valuesIndex++;

}

void  ACRot::StartDelayUpdate()
{
    GetWorld()->GetTimerManager().SetTimer(this->TimerHandle, this, &ACRot::updateAfterDelay, 1.0f, true);
}

// Called every frame
void ACRot::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Calculating simulation time
    RealTimeSeconds += DeltaTime;

    // Desired step
    float step = 0.05;

    if (DeltaTime < step) {

        EulerDerivative(DeltaTime);

        // Update actor's rotation based on angular velocity
        FRotator DeltaRotation = RotationRate * DeltaTime * AngularVelocity;
        AddActorLocalRotation(DeltaRotation);

        // Save data to a text file
        FString TextData = FString::Printf(TEXT("%.3f \t %.3f \t %.3f \t %.3f \t %.3f \t %.3f\n"), RealTimeSeconds, DeltaTime, AppliedVoltage, Current, Resistance, AngularVelocity);
        FFileHelper::SaveStringToFile(TextData, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
    }
    else {
        int iters = int(DeltaTime / step);
        //int carry = int(DeltaTime % 0.05);
        for (int c = 0; c < iters; c++) {

            EulerDerivative(step);

            // Update actor's rotation based on angular velocity
            FRotator DeltaRotation = RotationRate * DeltaTime * AngularVelocity;
            AddActorLocalRotation(DeltaRotation);

            // Calculating simulation time
            RealTimeSeconds += step;

            // Save data to a text file
            FString TextData = FString::Printf(TEXT("%.3f \t %.3f \t %.3f \t %.3f \t %.3f \t %.3f\n"), RealTimeSeconds, (step * c), AppliedVoltage, Current, Resistance, AngularVelocity);
            FFileHelper::SaveStringToFile(TextData, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
        }
    }

    UE_LOG(LogClass, Log, TEXT("DeltaTime: %f \t RealTimeSeconds: %f \t Current: %f \t Resistance: %f \t Speed: %f"), DeltaTime, RealTimeSeconds, Current, Resistance, AngularVelocity);

    // Begin the input feed
    if (RealTimeSeconds > 1.0) {
        AppliedVoltage = 200.0;
    }

    // Stop the input feed
    if (RealTimeSeconds > 10.0) {
        AppliedVoltage = 0.0;
    }
}

void ACRot::EulerDerivative(float CustomDT) {
    // Calculate derivatives
    float dCurrent_dt = (AppliedVoltage - Kv * AngularVelocity - Resistance * Current) / Inductance;
    float dAngularVelocity_dt = (Kt * Current - Friction * AngularVelocity - LoadTorque) / J;

    // Euler's method to update state variables
    Current += dCurrent_dt * CustomDT;
    AngularVelocity += dAngularVelocity_dt * CustomDT;
}

void ACRot::SetVoltage(float Voltage)
{
    AppliedVoltage = Voltage;
}

void ACRot::SetLoadTorque(float Torque)
{
    LoadTorque = Torque;
}

