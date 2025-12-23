// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AlkaidAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"

AAlkaidAIController::AAlkaidAIController()
{
}

void AAlkaidAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	//서버에서 실행중인지 확인
	if (!HasAuthority())return;

	//BT 없으면 종료함
	//if (!BehaviorTreeAsset)return;

	if(InPawn) 
	{
		UE_LOG(LogTemp, Warning, TEXT("AI Controller is sontrolling."), *InPawn->GetName());
	}

	//BT실행
	RunBehaviorTree(BehaviorTreeAsset.Get());
}
