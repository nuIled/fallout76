#pragma once
#include <string>
#include <vector>

void menuTheme(bool darkMode, float alpha);

void tooltips(const char* title, const char* description);

void renderMainTab();
void renderWepTab();
void renderMiscTab();
void renderTeleportTab();
void renderCfgTab();
void renderESPTab();
void renderAdminTab();
void renderMenu();
void checkTPHotkeys();
bool toggleswap(const char* label, bool& state);

extern bool bisformidvalid;
extern bool bDisplayoverlay;
extern bool bMainMenuVisible;
extern bool bnoclipenabled;
extern bool bnocliptoggle;
extern bool bnoclipon;
extern bool bfreecamtoggle;
extern bool bfreecamon;
extern bool bdarkmodeenabled;
extern bool fcharsizeloaded;
extern float fNoclipSpeedMult;
extern float fcharsigma;
extern float fcharfat;
extern float fcharthin;
extern float ftrans;
extern float fbgalpha;
extern float fwindowbordersize;
extern float fchildbordersize;
extern float ffontsize;
extern int bSelectedTab;
extern char cformidinput[16];
extern uint32_t uioriginalformid;
extern uint32_t uicurrentformid;