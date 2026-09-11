#pragma once
#include <clay.h>

void initUi();
void renderMainMenu();
void renderSettingsMenu();
Clay_ElementDeclaration getInnerConfig(bool hovered);

extern Font grandover[2];
extern Clay_ElementDeclaration rootConfig;
extern Clay_ElementDeclaration containerConfig;