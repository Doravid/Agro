#pragma once
#include <clay.h>

void initUi();
void renderMainMenu();
void renderSettingsMenu();
void renderUpgradesMenu();
Clay_ElementDeclaration getInnerConfig(bool hovered);

extern Clay_ElementDeclaration rootConfig;
extern Clay_ElementDeclaration containerConfig;