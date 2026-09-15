#ifndef __PATH_H
#define __PATH_H

//CString MakePath( LPCTSTR lpszFileName );
CString MakePath(LPCTSTR lpszDirName, LPCTSTR lpszFileName);
CString MakePath(LPCTSTR lpDir, DWORD dwLan, LPCTSTR lpFile);
void MakePath(CString& strFullPath, LPCTSTR lpszDirName, LPCTSTR lpszFileName);
void MakePath(TCHAR* lpszFullPath, LPCTSTR lpszDirName, LPCTSTR lpszFileName);
//void SetResourcePath();
//LPCTSTR GetResourcePath();

#ifdef __AES
#define DIR_MUSIC			    _T("Data\\Music\\")
#define DIR_SOUND			    _T("Data\\Music\\Sound\\")
#define DIR_ICON			    _T("Data\\Icon\\")
#define DIR_ITEM			    _T("Data\\Item\\")
#define DIR_CLIENT			    _T("Data\\Client\\")
#define DIR_DATA			    _T("Data\\")
#define DIR_CHAR			    _T("Data\\Char\\")
#define DIR_MODEL			    _T("Data\\Model\\")
#define DIR_SFX				    _T("Data\\Sfx\\")
#define DIR_SFXTEX			    _T("Data\\Sfx\\Texture\\")
#define DIR_THEME			    _T("Data\\Theme\\Default\\")
#define DIR_SHOPS			    _T("Data\\Theme\\Default\\Shops\\")
#define DIR_LOADMAP			    _T("Data\\Theme\\Default\\Teleporter\\")
#define DIR_MODEL			    _T("Data\\Model\\")
#define DIR_ASE				    _T("Data\\Model\\Ase\\")
#define DIR_MODELTEX		    _T("Data\\Model\\Texture\\")
#define DIR_MODELTEXMID		    _T("Data\\Model\\Texture\\")
#define DIR_MODELTEXLOW		    _T("Data\\Model\\Texture\\")
#define DIR_LOGO			    _T("Data\\Model\\Logo\\")
#define DIR_WEATHER			    _T("Data\\Weather\\")

#ifdef __BS_EFFECT_LUA
#define DIR_SFXLUA			    _T("Data\\Sfx\\Scripts\\")
#endif //__BS_EFFECT_LUA

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
#define DIR_THEME_ACHIEVEMENT   _T("Data\\Theme\\Default\\Achievement\\")
#endif // __AEGON_ACHIEVEMENT_SYSTEM

#ifdef __AEGON_CRAFTING_SYSTEM
#define DIR_THEME_CRAFTING   _T("Data\\Theme\\Default\\Crafting\\")
#endif // __AEGON_CRAFTING_SYSTEM

#ifdef __AEGON_ADVANCED_ANNOUNCEMENT
#define DIR_THEME_ANNOUNCEMENT   _T("Data\\Theme\\Default\\Announcement\\")
#endif // __AEGON_ADVANCED_ANNOUNCEMENT

#ifdef __AEGON_SEASON_PASS
#define DIR_THEME_SEASON_PASS   _T("Data\\Theme\\Default\\Seasonpass\\")
#endif // __AEGON_SEASON_PASS

#ifdef __AEGON_CUSTOM_FONTS
#define DIR_THEME_FONT   _T("Data\\Theme\\Default\\Fonts\\")
#endif // __AEGON_CUSTOM_FONTS

#ifdef __AEGON_APNG
#define DIR_THEME_APNG   _T("Data\\Theme\\Default\\apng\\")
#endif // __AEGON_APNG

#ifdef __AEGON_EMOTE_SYSTEM
#define DIR_THEME_EMOTE   _T("Data\\Theme\\Default\\emotes\\")
#endif //__AEGON_EMOTE_SYSTEM

#ifdef __AEGON_THEME_SWITCHER
#define DIR_THEME_SWITCHER   _T("Data\\Theme\\Default\\ThemeSwitcher\\")
#endif

#ifdef __AEGON_PVP_MINIGAMES
#define DIR_THEME_MINIGAMES   _T("Data\\Theme\\Default\\minigames\\")
#define DIR_THEME_MINIGAMES_UNO   _T("Data\\Theme\\Default\\minigames\\uno\\")
#endif

#ifdef __AEGON_MESSENGER
#define DIR_MESSENGER_LOG _T("Log\\Messenger\\")
#endif

#else // __AES
#define DIR_MUSIC               _T("Music\\")
#define DIR_SOUND               _T("Sound\\")
#define DIR_ICON                _T("Icon\\")
#define DIR_ITEM                _T("Item\\")
#define DIR_CLIENT              _T("Client\\")
#define DIR_SFX                 _T("Sfx\\")
#define DIR_SFXTEX              _T("Sfx\\Texture\\")
#define DIR_THEME               _T("Theme\\Default\\")
#define DIR_SHOPS               _T("Theme\\Default\\Shops\\")
#define DIR_LOADMAP             _T("Theme\\Default\\Teleporter\\")
#define DIR_MODEL			    _T("Model\\")
#define DIR_ASE				    _T("Model\\Ase\\")
#define DIR_MODELTEX		    _T("Model\\Texture\\")
#define DIR_MODELTEXMID		    _T("Model\\Texture\\")
#define DIR_MODELTEXLOW		    _T("Model\\Texture\\")
#define DIR_LOGO                _T("Model\\Logo\\")
#define DIR_WEATHER             _T("Weather\\")

#ifdef __BS_EFFECT_LUA
#define DIR_SFXLUA		        _T("Sfx\\Scripts\\")
#endif //__BS_EFFECT_LUA

#ifdef __AEGON_ACHIEVEMENT_SYSTEM
#define DIR_THEME_ACHIEVEMENT	_T("Theme\\Default\\Achievement\\")
#endif // __AEGON_ACHIEVEMENT_SYSTEM

#ifdef __AEGON_CRAFTING_SYSTEM
#define DIR_THEME_CRAFTING		_T("Theme\\Default\\Crafting\\")
#endif // __AEGON_CRAFTING_SYSTEM

#endif // __AES
#define DIR_TEXT                _T("Text\\")
#define DIR_DIALOG              _T("Dialog\\")
#define DIR_WORLD               _T("World\\")
#define DIR_WORLD_GUILDCOMBAT   _T("World\\WdGuildWar\\")
#define DIR_WORLDPLAY           _T("World\\Play\\")
#define DIR_WORLDTEX            _T("World\\Texture\\") 
#define DIR_WORLDTEXMID         _T("World\\Texture\\") 
#define DIR_WORLDTEXLOW         _T("World\\Texture\\") 
#define DIR_WORLDTILE           _T("World\\Tile\\")
#define DIR_TEXTURE             _T("Texture\\")
#define DIR_FONT                _T("Font\\")
#define DIR_EFFECT              _T("Effect\\")

#endif
