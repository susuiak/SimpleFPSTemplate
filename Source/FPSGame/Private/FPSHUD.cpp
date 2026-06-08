// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"

AFPSHUD::AFPSHUD()
{
	// Set the crosshair texture (not normally recommended to link content through a hardcoded line like below)
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/UI/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;
}


void AFPSHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition(Center.X - (CrosshairTex->GetSurfaceWidth() * 0.5f), Center.Y - (CrosshairTex->GetSurfaceHeight() * 0.5f));

	// draw the crosshair
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->GetResource(), FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );

	//绘制子弹数量
	APawn* P = GetOwningPlayerController() ? GetOwningPlayerController()->GetPawn() : nullptr;
	AFPSCharacter* MyChar = P ? Cast<AFPSCharacter>(P) : nullptr;

	if (MyChar) {
		int32 Curr = MyChar->GetCurrentAmmo();
		int32 Reserve = MyChar->GetReserveAmmo();
		bool bReloading = MyChar->IsReloading();

		FString AmmoString = FString::Printf(TEXT("Ammo: %d / %d"), Curr, Reserve);
		if (bReloading)
		{
			AmmoString.Append(TEXT("  (Reloading...)"));
		}

		// Draw at bottom-left with small offset
		const float X = 50.0f;
		const float Y = Canvas->ClipY - 80.0f;
		
		FCanvasTextItem TextItem(FVector2D(X, Y), FText::FromString(AmmoString), GEngine->GetSmallFont(), FLinearColor::White);
		TextItem.EnableShadow(FLinearColor::Black);
		Canvas->DrawItem(TextItem);
	}
}
