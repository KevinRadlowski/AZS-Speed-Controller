#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <windowsx.h>
#include <tlhelp32.h>
#include <dwmapi.h>
#include <string>
#include <filesystem>
#include <array>
#include <cmath>
#include <algorithm>
#include <commctrl.h>
#include <vector>
#pragma comment(lib,"dwmapi.lib")
struct Shared { volatile LONG speed100; volatile LONG active; };
static HANDLE mapHandle=nullptr;
static Shared* shared=nullptr;
static DWORD pid=0;
static HWND windowHandle=nullptr;
static int speed100=100;
static int custom100=500;
static bool locked=false, alwaysOnTop=true, extremeConfirmed=false;
static int language=0; // 0 auto, 1 English, 2 French, 3+ localized
static bool customSelected=false;
static const wchar_t* languageNames[]={L"Automatic (Windows)",L"English",L"Français",L"Polski",L"Čeština",L"Deutsch",L"Português (Brasil)",L"Español",L"Română",L"Türkçe",L"Русский",L"Українська",L"简体中文",L"日本語",L"한국어"};
static constexpr int languageCount=sizeof(languageNames)/sizeof(languageNames[0]);
static HWND settingsWindow=nullptr;
static int draggingSlider=0;
static bool compact=false;
static bool hidden=false; // Minimized to taskbar, not removed from taskbar
static int opacity=235;
static std::wstring iniPath;
static int savedX=80, savedY=80;
static bool hooked=false;
static HANDLE singletonMutex=nullptr;
static std::wstring status=L"Waiting for game...";
static constexpr int widths=448, heights=110;
static constexpr int compactWidth=200, compactHeight=54;

// Auxiliary panels: translated in the same 14-language order as localized[] below.
enum AuxText { AUX_0, AUX_1, AUX_2, AUX_3, AUX_4, AUX_5, AUX_6, AUX_7, AUX_8, AUX_9, AUX_10, AUX_11, AUX_12, AUX_13, AUX_14, AUX_15, AUX_16, AUX_17, AUX_18, AUX_COUNT };
static constexpr const wchar_t* auxLocalized[AUX_COUNT][14]={
  { L"Shortcuts", L"Raccourcis clavier", L"Skróty klawiszowe", L"Klávesové zkratky", L"Tastenkürzel", L"Atalhos de teclado", L"Atajos de teclado", L"Scurtături de tastatură", L"Klavye kısayolları", L"Горячие клавиши", L"Гарячі клавіші", L"键盘快捷键", L"キーボードショートカット", L"키보드 단축키" },
  { L"Speed profiles", L"Profils de vitesse", L"Profile prędkości", L"Profily rychlosti", L"Geschwindigkeitsprofile", L"Perfis de velocidade", L"Perfiles de velocidad", L"Profiluri de viteză", L"Hız profilleri", L"Профили скорости", L"Профілі швидкості", L"速度配置", L"速度プロファイル", L"속도 프로필" },
  { L"Click a row, then press a key combination. Del clears.", L"Cliquez sur une ligne, puis appuyez sur une combinaison. Suppr efface.", L"Kliknij wiersz i naciśnij skrót. Delete usuwa.", L"Klikněte na řádek a stiskněte klávesy. Delete smaže.", L"Zeile anklicken, Tastenkombination drücken. Entf löscht.", L"Clique numa linha e pressione teclas. Delete limpa.", L"Haz clic en una fila y pulsa teclas. Supr borra.", L"Apasă pe un rând, apoi tastele. Delete șterge.", L"Satıra tıklayın ve tuşlara basın. Delete siler.", L"Выберите строку и нажмите сочетание. Del удаляет.", L"Виберіть рядок і натисніть клавіші. Del видаляє.", L"点击一行并按下组合键。Delete 清除。", L"行を選びキーを押します。Delete で解除。", L"행을 선택하고 단축키를 누르세요. Delete로 삭제." },
  { L"Press shortcut...", L"Appuyez sur les touches...", L"Naciśnij klawisze...", L"Stiskněte klávesy...", L"Tasten drücken...", L"Pressione as teclas...", L"Pulsa las teclas...", L"Apasă tastele...", L"Tuşlara basın...", L"Нажмите сочетание...", L"Натисніть клавіші...", L"请按组合键...", L"キーを押してください...", L"단축키를 누르세요..." },
  { L"Not assigned", L"Non attribué", L"Nieprzypisany", L"Nepřiřazeno", L"Nicht belegt", L"Não atribuído", L"Sin asignar", L"Neatribuit", L"Atanmamış", L"Не назначено", L"Не призначено", L"未分配", L"未割り当て", L"할당되지 않음" },
  { L"Saved.", L"Enregistré.", L"Zapisano.", L"Uloženo.", L"Gespeichert.", L"Salvo.", L"Guardado.", L"Salvat.", L"Kaydedildi.", L"Сохранено.", L"Збережено.", L"已保存。", L"保存しました。", L"저장됨." },
  { L"Already assigned to another action.", L"Déjà attribué à une autre action.", L"Już przypisany do innej akcji.", L"Již přiřazeno jiné akci.", L"Bereits einer anderen Aktion zugewiesen.", L"Já atribuído a outra ação.", L"Ya asignado a otra acción.", L"Deja atribuit altei acțiuni.", L"Başka bir eyleme atanmış.", L"Уже назначено другому действию.", L"Уже призначено іншій дії.", L"已分配给其他操作。", L"別の操作に割り当て済みです。", L"다른 동작에 이미 할당됨." },
  { L"Windows rejected this shortcut (in use elsewhere).", L"Windows refuse ce raccourci (déjà utilisé).", L"Windows odrzucił skrót (zajęty).", L"Windows zkratku odmítl (používá se).", L"Windows lehnt Kürzel ab (bereits belegt).", L"Windows recusou o atalho (em uso).", L"Windows rechazó el atajo (en uso).", L"Windows a refuzat scurtătura (ocupată).", L"Windows kısayolu reddetti (kullanımda).", L"Windows отклонила сочетание (занято).", L"Windows відхилила комбінацію (зайнята).", L"Windows 拒绝了此快捷键（已占用）。", L"Windows がショートカットを拒否（使用中）。", L"Windows가 단축키를 거부함 (사용 중)." },
  { L"Shortcut unavailable: ", L"Raccourci indisponible : ", L"Skrót niedostępny: ", L"Zkratka není dostupná: ", L"Tastenkürzel nicht verfügbar: ", L"Atalho indisponível: ", L"Atajo no disponible: ", L"Scurtătură indisponibilă: ", L"Kısayol kullanılamıyor: ", L"Сочетание недоступно: ", L"Комбінація недоступна: ", L"快捷键不可用：", L"ショートカットを使用できません：", L"단축키 사용 불가: " },
  { L"Left-click to activate. Right-click to save current speed.", L"Clic gauche : activer. Clic droit : enregistrer la vitesse.", L"Lewy klik: aktywuj. Prawy: zapisz prędkość.", L"Levý klik: aktivovat. Pravý: uložit rychlost.", L"Linksklick: aktivieren. Rechtsklick: Tempo speichern.", L"Clique esquerdo: ativar. Direito: salvar velocidade.", L"Clic izquierdo: activar. Derecho: guardar velocidad.", L"Clic stânga: activează. Dreapta: salvează viteza.", L"Sol tık: etkinleştir. Sağ tık: hızı kaydet.", L"ЛКМ: применить. ПКМ: сохранить скорость.", L"ЛКМ: застосувати. ПКМ: зберегти швидкість.", L"左键：启用。右键：保存当前速度。", L"左クリック：適用。右クリック：速度を保存。", L"왼쪽 클릭: 적용. 오른쪽 클릭: 속도 저장." },
  { L"Speed x1", L"Vitesse x1", L"Prędkość x1", L"Rychlost x1", L"Tempo x1", L"Velocidade x1", L"Velocidad x1", L"Viteză x1", L"Hız x1", L"Скорость x1", L"Швидкість x1", L"速度 x1", L"速度 x1", L"속도 x1" },
  { L"Speed x2", L"Vitesse x2", L"Prędkość x2", L"Rychlost x2", L"Tempo x2", L"Velocidade x2", L"Velocidad x2", L"Viteză x2", L"Hız x2", L"Скорость x2", L"Швидкість x2", L"速度 x2", L"速度 x2", L"속도 x2" },
  { L"Speed x5", L"Vitesse x5", L"Prędkość x5", L"Rychlost x5", L"Tempo x5", L"Velocidade x5", L"Velocidad x5", L"Viteză x5", L"Hız x5", L"Скорость x5", L"Швидкість x5", L"速度 x5", L"速度 x5", L"속도 x5" },
  { L"Speed x10", L"Vitesse x10", L"Prędkość x10", L"Rychlost x10", L"Tempo x10", L"Velocidade x10", L"Velocidad x10", L"Viteză x10", L"Hız x10", L"Скорость x10", L"Швидкість x10", L"速度 x10", L"速度 x10", L"속도 x10" },
  { L"Custom speed", L"Vitesse personnalisée", L"Własna prędkość", L"Vlastní rychlost", L"Eigene Geschwindigkeit", L"Velocidade personalizada", L"Velocidad personalizada", L"Viteză personalizată", L"Özel hız", L"Своя скорость", L"Власна швидкість", L"自定义速度", L"カスタム速度", L"사용자 지정 속도" },
  { L"Compact mode", L"Mode compact", L"Tryb kompaktowy", L"Kompaktní režim", L"Kompaktmodus", L"Modo compacto", L"Modo compacto", L"Mod compact", L"Kompakt mod", L"Компактный режим", L"Компактний режим", L"紧凑模式", L"コンパクトモード", L"간단 모드" },
  { L"Show / hide", L"Afficher / masquer", L"Pokaż / ukryj", L"Zobrazit / skrýt", L"Ein-/ausblenden", L"Mostrar / ocultar", L"Mostrar / ocultar", L"Arată / ascunde", L"Göster / gizle", L"Показать / скрыть", L"Показати / сховати", L"显示 / 隐藏", L"表示 / 非表示", L"표시 / 숨기기" },
  { L"Settings", L"Paramètres", L"Ustawienia", L"Nastavení", L"Einstellungen", L"Configurações", L"Ajustes", L"Setări", L"Ayarlar", L"Настройки", L"Налаштування", L"设置", L"設定", L"설정" },
  { L"Profile ", L"Profil ", L"Profil ", L"Profil ", L"Profil ", L"Perfil ", L"Perfil ", L"Profil ", L"Profil ", L"Профиль ", L"Профіль ", L"配置 ", L"プロファイル ", L"프로필 " },
};
static int resolvedLanguage();
static const wchar_t* aux(AuxText id) { return auxLocalized[id][resolvedLanguage()]; }

// Actions and configurable global shortcuts (MOD_NOREPEAT is added on registration).
enum Action { SPEED_1, SPEED_2, SPEED_5, SPEED_10, SPEED_CUSTOM, TOGGLE_COMPACT, TOGGLE_VISIBLE, OPEN_SETTINGS, PROFILE_1, PROFILE_2, PROFILE_3, ACTION_COUNT };
static std::wstring actionName(int id) {
  if(id>=PROFILE_1 && id<=PROFILE_3) return std::wstring(aux(AUX_18))+std::to_wstring(id-PROFILE_1+1);
  return aux(static_cast<AuxText>(AUX_10+id));
}
struct Shortcut { UINT modifiers; UINT key; };
static Shortcut shortcuts[ACTION_COUNT]={{0,VK_F6},{0,VK_F7},{0,VK_F8},{0,VK_F9},{0,0},{0,VK_F10},{0,VK_F11},{0,0},{0,0},{0,0},{0,0}};
static int profileSpeeds[3]={200,500,1000};
static HWND shortcutsWindow=nullptr, profilesWindow=nullptr;
static int capturingAction=-1;
static std::wstring shortcutNotice;
static std::wstring shortcutText(Shortcut key) {
  if(!key.key)return aux(AUX_4);
  std::wstring out;
  if(key.modifiers&MOD_CONTROL)out+=L"Ctrl+";
  if(key.modifiers&MOD_ALT)out+=L"Alt+";
  if(key.modifiers&MOD_SHIFT)out+=L"Shift+";
  if(key.modifiers&MOD_WIN)out+=L"Win+";
  if(key.key>=VK_F1 && key.key<=VK_F24)out+=L"F"+std::to_wstring(key.key-VK_F1+1);
  else {wchar_t name[80]={};UINT scan=MapVirtualKeyW(key.key,MAPVK_VK_TO_VSC);if(GetKeyNameTextW(static_cast<LONG>(scan<<16),name,80))out+=name;else out+=L"VK "+std::to_wstring(key.key);}
  return out;
}
static bool shortcutRegistered[ACTION_COUNT]={};
static bool registerShortcuts(HWND hwnd) {
  for(int i=0;i<ACTION_COUNT;i++)UnregisterHotKey(hwnd,100+i);
  bool ok=true;
  for(int i=0;i<ACTION_COUNT;i++)shortcutRegistered[i]=false;
  for(int i=0;i<ACTION_COUNT;i++)if(shortcuts[i].key) {
    if(RegisterHotKey(hwnd,100+i,shortcuts[i].modifiers|MOD_NOREPEAT,shortcuts[i].key)) shortcutRegistered[i]=true;
    else {ok=false;shortcutNotice=std::wstring(aux(AUX_8))+actionName(i);}
  }
  return ok;
}
static void saveShortcuts() {
  for(int i=0;i<ACTION_COUNT;i++) {
    WritePrivateProfileStringW(L"Shortcuts",(L"Action"+std::to_wstring(i)).c_str(),(std::to_wstring(shortcuts[i].modifiers)+L","+std::to_wstring(shortcuts[i].key)).c_str(),iniPath.c_str());
  }
  for(int i=0;i<3;i++)WritePrivateProfileStringW(L"Profiles",(L"Speed"+std::to_wstring(i+1)).c_str(),std::to_wstring(profileSpeeds[i]).c_str(),iniPath.c_str());
}
static void loadShortcuts() {
  for(int i=0;i<ACTION_COUNT;i++) {
    wchar_t value[80]={};GetPrivateProfileStringW(L"Shortcuts",(L"Action"+std::to_wstring(i)).c_str(),L"",value,80,iniPath.c_str());
    unsigned m=0,k=0;if(swscanf_s(value,L"%u,%u",&m,&k)==2 && k<=255 && (m&~(MOD_ALT|MOD_CONTROL|MOD_SHIFT|MOD_WIN))==0)shortcuts[i]={m,k};
  }
  for(int i=0;i<3;i++)profileSpeeds[i]=(std::clamp)(static_cast<int>(GetPrivateProfileIntW(L"Profiles",(L"Speed"+std::to_wstring(i+1)).c_str(),profileSpeeds[i],iniPath.c_str())),100,2500);
}
static constexpr wchar_t const* localized[][14] = {
  {L"AZEROTH  /  SPEED CONTROL", L"AZEROTH / CONTRÔLE VITESSE", L"AZEROTH / KONTROLA PRĘDKOŚCI", L"AZEROTH / OVLÁDÁNÍ RYCHLOSTI", L"AZEROTH / GESCHWINDIGKEIT", L"AZEROTH / CONTROLE DE VELOCIDADE", L"AZEROTH / CONTROL DE VELOCIDAD", L"AZEROTH / CONTROLUL VITEZEI", L"AZEROTH / HIZ KONTROLÜ", L"AZEROTH / УПРАВЛЕНИЕ СКОРОСТЬЮ", L"AZEROTH / КЕРУВАННЯ ШВИДКІСТЮ", L"AZEROTH / 速度控制", L"AZEROTH / 速度コントロール", L"AZEROTH / 속도 조절"},
  {L"SETTINGS", L"PARAMÈTRES", L"USTAWIENIA", L"NASTAVENÍ", L"EINSTELLUNGEN", L"CONFIGURAÇÕES", L"AJUSTES", L"SETĂRI", L"AYARLAR", L"НАСТРОЙКИ", L"НАЛАШТУВАННЯ", L"设置", L"設定", L"설정"},
  {L"OVERLAY OPACITY", L"OPACITÉ DE LA FENÊTRE", L"PRZEZROCZYSTOŚĆ OKNA", L"PRŮHLEDNOST OKNA", L"FENSTERDECKKRAFT", L"OPACIDADE DA JANELA", L"OPACIDAD DE LA VENTANA", L"OPACITATEA FERESTREI", L"PENCERE SAYDAMLIĞI", L"НЕПРОЗРАЧНОСТЬ ОКНА", L"НЕПРОЗОРІСТЬ ВІКНА", L"窗口不透明度", L"ウィンドウの不透明度", L"창 불투명도"},
  {L"CUSTOM SPEED", L"VITESSE PERSONNALISÉE", L"WŁASNA PRĘDKOŚĆ", L"VLASTNÍ RYCHLOST", L"EIGENE GESCHWINDIGKEIT", L"VELOCIDADE PERSONALIZADA", L"VELOCIDAD PERSONALIZADA", L"VITEZĂ PERSONALIZATĂ", L"ÖZEL HIZ", L"СВОЯ СКОРОСТЬ", L"ВЛАСНА ШВИДКІСТЬ", L"自定义速度", L"カスタム速度", L"사용자 지정 속도"},
  {L"Experimental above x10 - use at your own risk", L"Au-delà de x10 : expérimental, à vos risques", L"Powyżej x10: eksperymentalne, na własne ryzyko", L"Nad x10: experimentální, na vlastní riziko", L"Über x10: experimentell, auf eigenes Risiko", L"Acima de x10: experimental, por sua conta e risco", L"Más de x10: experimental, bajo tu responsabilidad", L"Peste x10: experimental, pe propriul risc", L"x10 üzeri: deneyseldir, risk size aittir", L"Выше x10: экспериментально, на ваш риск", L"Понад x10: експериментально, на ваш ризик", L"超过 x10：实验功能，风险自负", L"x10超は試験的機能です。自己責任で使用", L"x10 초과: 실험 기능, 사용 시 주의"},
  {L"LANGUAGE", L"LANGUE", L"JĘZYK", L"JAZYK", L"SPRACHE", L"IDIOMA", L"IDIOMA", L"LIMBĂ", L"DİL", L"ЯЗЫК", L"МОВА", L"语言", L"言語", L"언어"},
  {L"LOCK OVERLAY POSITION", L"VERROUILLER LA POSITION", L"ZABLOKUJ POZYCJĘ", L"UZAMKNOUT POZICI", L"POSITION SPERREN", L"BLOQUEAR POSIÇÃO", L"BLOQUEAR POSICIÓN", L"BLOCHEAZĂ POZIȚIA", L"KONUMU KİLİTLE", L"ЗАКРЕПИТЬ ПОЛОЖЕНИЕ", L"ЗАКРІПИТИ ПОЛОЖЕННЯ", L"锁定窗口位置", L"位置を固定", L"위치 잠금"},
  {L"ALWAYS ON TOP", L"TOUJOURS AU PREMIER PLAN", L"ZAWSZE NA WIERZCHU", L"VŽDY NAHOŘE", L"IMMER IM VORDERGRUND", L"SEMPRE EM PRIMEIRO PLANO", L"SIEMPRE VISIBLE", L"MEREU DEASUPRA", L"HER ZAMAN ÜSTTE", L"ПОВЕРХ ВСЕХ ОКОН", L"ПОВЕРХ УСІХ ВІКОН", L"始终置顶", L"常に最前面", L"항상 위에 표시"},
  {L"Settings are saved automatically", L"Paramètres enregistrés automatiquement", L"Ustawienia zapisują się automatycznie", L"Nastavení se ukládá automaticky", L"Einstellungen werden automatisch gespeichert", L"Configurações salvas automaticamente", L"Los ajustes se guardan automáticamente", L"Setările sunt salvate automat", L"Ayarlar otomatik kaydedilir", L"Настройки сохраняются автоматически", L"Налаштування зберігаються автоматично", L"设置已自动保存", L"設定は自動保存されます", L"설정이 자동으로 저장됩니다"},
  {L"Normal speed (x1)", L"Vitesse normale (x1)", L"Normalna prędkość (x1)", L"Normální rychlost (x1)", L"Normale Geschwindigkeit (x1)", L"Velocidade normal (x1)", L"Velocidad normal (x1)", L"Viteză normală (x1)", L"Normal hız (x1)", L"Обычная скорость (x1)", L"Звичайна швидкість (x1)", L"正常速度 (x1)", L"通常速度 (x1)", L"일반 속도 (x1)"},
  {L"Settings...", L"Paramètres...", L"Ustawienia...", L"Nastavení...", L"Einstellungen...", L"Configurações...", L"Ajustes...", L"Setări...", L"Ayarlar...", L"Настройки...", L"Налаштування...", L"设置...", L"設定...", L"설정..."},
  {L"Expand (F10)", L"Agrandir (F10)", L"Rozwiń (F10)", L"Rozbalit (F10)", L"Erweitern (F10)", L"Expandir (F10)", L"Expandir (F10)", L"Extinde (F10)", L"Genişlet (F10)", L"Развернуть (F10)", L"Розгорнути (F10)", L"展开 (F10)", L"展開 (F10)", L"확장 (F10)"},
  {L"Compact (F10)", L"Compact (F10)", L"Kompaktowy (F10)", L"Kompaktní (F10)", L"Kompakt (F10)", L"Compacto (F10)", L"Compacto (F10)", L"Compact (F10)", L"Kompakt (F10)", L"Компактно (F10)", L"Компактно (F10)", L"紧凑模式 (F10)", L"コンパクト (F10)", L"간단히 (F10)"},
  {L"Minimize (F11)", L"Réduire (F11)", L"Minimalizuj (F11)", L"Minimalizovat (F11)", L"Minimieren (F11)", L"Minimizar (F11)", L"Minimizar (F11)", L"Minimizează (F11)", L"Küçült (F11)", L"Свернуть (F11)", L"Згорнути (F11)", L"最小化 (F11)", L"最小化 (F11)", L"최소화 (F11)"},
  {L"Exit and reset speed", L"Quitter et rétablir x1", L"Zamknij i przywróć x1", L"Ukončit a obnovit x1", L"Beenden und x1 wiederherstellen", L"Sair e restaurar x1", L"Salir y restaurar x1", L"Ieșire și revenire la x1", L"Çık ve x1 hızına dön", L"Выход и возврат к x1", L"Вийти та повернути x1", L"退出并恢复 x1", L"終了して x1 に戻す", L"종료하고 x1로 복원"},
  {L"Speeds above x10 are experimental and may crash or destabilize the game. Continue?", L"Les vitesses supérieures à x10 sont expérimentales et peuvent déstabiliser le jeu. Continuer ?", L"Prędkości powyżej x10 są eksperymentalne i mogą destabilizować grę. Kontynuować?", L"Rychlosti nad x10 jsou experimentální a mohou destabilizovat hru. Pokračovat?", L"Geschwindigkeiten über x10 sind experimentell und können das Spiel destabilisieren. Fortfahren?", L"Velocidades acima de x10 são experimentais e podem causar instabilidade no jogo. Continuar?", L"Las velocidades superiores a x10 son experimentales y pueden desestabilizar el juego. ¿Continuar?", L"Vitezele peste x10 sunt experimentale și pot destabiliza jocul. Continui?", L"x10 üzerindeki hızlar deneyseldir ve oyunu kararsız hale getirebilir. Devam edilsin mi?", L"Скорости выше x10 экспериментальны и могут нарушить стабильность игры. Продолжить?", L"Швидкості понад x10 експериментальні й можуть порушити стабільність гри. Продовжити?", L"超过 x10 的速度属于实验功能，可能导致游戏不稳定。继续吗？", L"x10を超える速度は試験的で、ゲームが不安定になる可能性があります。続行しますか？", L"x10을 초과하는 속도는 실험 기능이며 게임이 불안정해질 수 있습니다. 계속할까요?"},
  {L"Waiting for game...", L"En attente du jeu...", L"Oczekiwanie na grę...", L"Čekání na hru...", L"Warte auf Spiel...", L"Aguardando o jogo...", L"Esperando al juego...", L"Se așteaptă jocul...", L"Oyun bekleniyor...", L"Ожидание игры...", L"Очікування гри...", L"等待游戏...", L"ゲームを待機中...", L"게임 대기 중..."},
  {L"Connected", L"Connecté", L"Połączono", L"Připojeno", L"Verbunden", L"Conectado", L"Conectado", L"Conectat", L"Bağlandı", L"Подключено", L"Підключено", L"已连接", L"接続済み", L"연결됨"},
  {L"Connecting...", L"Connexion...", L"Łączenie...", L"Připojování...", L"Verbinde...", L"Conectando...", L"Conectando...", L"Se conectează...", L"Bağlanıyor...", L"Подключение...", L"Підключення...", L"连接中...", L"接続中...", L"연결 중..."},
  {L"Game not connected", L"Jeu non connecté", L"Gra niepołączona", L"Hra není připojena", L"Spiel nicht verbunden", L"Jogo não conectado", L"Juego desconectado", L"Joc neconectat", L"Oyun bağlı değil", L"Игра не подключена", L"Гру не підключено", L"游戏未连接", L"ゲーム未接続", L"게임 연결 안 됨"},
  {L"Missing AZSSpeedHook.dll", L"DLL AZSSpeedHook.dll introuvable", L"Brak pliku AZSSpeedHook.dll", L"Chybí AZSSpeedHook.dll", L"AZSSpeedHook.dll fehlt", L"AZSSpeedHook.dll não encontrada", L"Falta AZSSpeedHook.dll", L"Lipsește AZSSpeedHook.dll", L"AZSSpeedHook.dll bulunamadı", L"Не найдена AZSSpeedHook.dll", L"Не знайдено AZSSpeedHook.dll", L"缺少 AZSSpeedHook.dll", L"AZSSpeedHook.dll がありません", L"AZSSpeedHook.dll 파일 없음"},
  {L"Cannot access game process", L"Accès au processus refusé", L"Brak dostępu do procesu gry", L"Nelze získat přístup ke hře", L"Kein Zugriff auf Spielprozess", L"Sem acesso ao processo do jogo", L"No se puede acceder al proceso", L"Nu se poate accesa procesul", L"Oyun işlemine erişilemiyor", L"Нет доступа к процессу игры", L"Немає доступу до процесу гри", L"无法访问游戏进程", L"ゲームプロセスにアクセスできません", L"게임 프로세스에 접근할 수 없음"},
  {L"DLL load failed (check architecture/permissions)", L"Échec du chargement de la DLL", L"Nie udało się załadować DLL", L"Načtení DLL se nezdařilo", L"DLL konnte nicht geladen werden", L"Falha ao carregar DLL", L"Error al cargar DLL", L"Încărcarea DLL a eșuat", L"DLL yüklenemedi", L"Ошибка загрузки DLL", L"Помилка завантаження DLL", L"DLL 加载失败", L"DLLの読み込みに失敗", L"DLL 로드 실패"},
  {L"Waiting for AzerothSurvivors.exe", L"En attente d’AzerothSurvivors.exe", L"Oczekiwanie na AzerothSurvivors.exe", L"Čekání na AzerothSurvivors.exe", L"Warte auf AzerothSurvivors.exe", L"Aguardando AzerothSurvivors.exe", L"Esperando AzerothSurvivors.exe", L"Se așteaptă AzerothSurvivors.exe", L"AzerothSurvivors.exe bekleniyor", L"Ожидание AzerothSurvivors.exe", L"Очікування AzerothSurvivors.exe", L"等待 AzerothSurvivors.exe", L"AzerothSurvivors.exe を待機中", L"AzerothSurvivors.exe 대기 중"},
  {L"ONLINE", L"EN LIGNE", L"ONLINE", L"ONLINE", L"ONLINE", L"ONLINE", L"EN LÍNEA", L"ONLINE", L"ÇEVRİMİÇİ", L"В СЕТИ", L"У МЕРЕЖІ", L"在线", L"オンライン", L"온라인"},
  {L"OFFLINE", L"HORS LIGNE", L"OFFLINE", L"OFFLINE", L"OFFLINE", L"OFFLINE", L"SIN CONEXIÓN", L"OFFLINE", L"ÇEVRİMDIŞI", L"НЕ В СЕТИ", L"НЕ В МЕРЕЖІ", L"离线", L"オフライン", L"오프라인"},
  {L"Automatic (Windows)", L"Automatique (Windows)", L"Automatycznie (Windows)", L"Automaticky (Windows)", L"Automatisch (Windows)", L"Automático (Windows)", L"Automático (Windows)", L"Automat (Windows)", L"Otomatik (Windows)", L"Автоматически (Windows)", L"Автоматично (Windows)", L"自动（Windows）", L"自動（Windows）", L"자동 (Windows)"},
};
static int resolvedLanguage() {
  if(language>0) return language-1;
  switch(PRIMARYLANGID(GetUserDefaultUILanguage())) {
    case LANG_FRENCH: return 1;
    case LANG_POLISH: return 2;
    case LANG_CZECH: return 3;
    case LANG_GERMAN: return 4;
    case LANG_PORTUGUESE: return 5;
    case LANG_SPANISH: return 6;
    case LANG_ROMANIAN: return 7;
    case LANG_TURKISH: return 8;
    case LANG_RUSSIAN: return 9;
    case LANG_UKRAINIAN: return 10;
    case LANG_CHINESE: return 11;
    case LANG_JAPANESE: return 12;
    case LANG_KOREAN: return 13;
    default: return 0;
  }
}
static const wchar_t* loc(int key) { return localized[key][resolvedLanguage()]; }
static const wchar_t* tr(const wchar_t* en,const wchar_t* fr) {
  for(int i=0;i<27;i++) if(wcscmp(en,localized[i][0])==0) return loc(i);
  return resolvedLanguage()==1?fr:en;
}
static std::wstring speedText(int v) {return std::to_wstring(v/100)+(v%100?L".5":L"");}
// Utility windows are implemented above the renderer; declare dependencies here.
static void panelRect(HDC, RECT, COLORREF, COLORREF);
static void panelText(HDC, const std::wstring&, int, int, int, int, COLORREF, int=16, bool=false);
static void setSpeed(int);
static void saveSettings();
static void openSettings();
static void showUtility(HWND& target,const wchar_t* cls,const wchar_t* title,int h) {
  if(target){SetForegroundWindow(target);return;}
  RECT r{};GetWindowRect(settingsWindow?settingsWindow:windowHandle,&r);
  target=CreateWindowExW(WS_EX_TOOLWINDOW,cls,title,WS_POPUP|WS_SYSMENU,r.right+10,r.top,455,h,windowHandle,nullptr,(HINSTANCE)GetWindowLongPtrW(windowHandle,GWLP_HINSTANCE),nullptr);
  if(target){ShowWindow(target,SW_SHOW);UpdateWindow(target);SetForegroundWindow(target);}
}
static LRESULT CALLBACK shortcutProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
  switch(msg){
    case WM_ERASEBKGND:return 1;
    case WM_PAINT:{PAINTSTRUCT ps{};HDC dc=BeginPaint(hwnd,&ps);RECT r{};GetClientRect(hwnd,&r);panelRect(dc,r,RGB(17,19,24),RGB(177,127,50));
      panelText(dc,std::wstring(L"AZEROTH / ")+aux(AUX_0),18,8,395,34,RGB(241,192,102),19,true);
      panelText(dc,L"X",426,8,22,30,RGB(241,192,102),18,true);
      panelText(dc,aux(AUX_2),18,41,420,23,RGB(185,175,157),13);
      for(int i=0;i<ACTION_COUNT;i++){int y=70+i*34;panelRect(dc,{15,y,438,y+31},i==capturingAction?RGB(87,64,31):RGB(32,34,39),RGB(86,72,50));panelText(dc,actionName(i),23,y+3,210,25,RGB(225,210,180),15);panelText(dc,i==capturingAction?aux(AUX_3):shortcutText(shortcuts[i]),242,y+3,188,25,RGB(241,192,102),14,true);}
      panelText(dc,shortcutNotice,18,452,425,30,RGB(238,145,103),13);EndPaint(hwnd,&ps);return 0;}
    case WM_LBUTTONDOWN:{int x=GET_X_LPARAM(lp),y=GET_Y_LPARAM(lp);if(x>413 && y<42){DestroyWindow(hwnd);return 0;}if(y>=70 && y<70+ACTION_COUNT*34){capturingAction=(y-70)/34;shortcutNotice=L"";for(int k=0;k<ACTION_COUNT;k++)UnregisterHotKey(windowHandle,100+k);SetFocus(hwnd);InvalidateRect(hwnd,nullptr,FALSE);}return 0;}
    case WM_SYSKEYDOWN:case WM_KEYDOWN:{if(capturingAction<0)break;UINT vk=static_cast<UINT>(wp);if(vk==VK_CONTROL||vk==VK_LCONTROL||vk==VK_RCONTROL||vk==VK_MENU||vk==VK_LMENU||vk==VK_RMENU||vk==VK_SHIFT||vk==VK_LSHIFT||vk==VK_RSHIFT||vk==VK_LWIN||vk==VK_RWIN)return 0;
      if(vk==VK_ESCAPE){capturingAction=-1;registerShortcuts(windowHandle);InvalidateRect(hwnd,nullptr,FALSE);return 0;}
      UINT mod=0;if(GetKeyState(VK_CONTROL)&0x8000)mod|=MOD_CONTROL;if(GetKeyState(VK_MENU)&0x8000)mod|=MOD_ALT;if(GetKeyState(VK_SHIFT)&0x8000)mod|=MOD_SHIFT;if((GetKeyState(VK_LWIN)|GetKeyState(VK_RWIN))&0x8000)mod|=MOD_WIN;
      Shortcut candidate=vk==VK_DELETE?Shortcut{0,0}:Shortcut{mod,vk};bool duplicate=false;
      for(int i=0;i<ACTION_COUNT;i++)if(i!=capturingAction && candidate.key && shortcuts[i].key==candidate.key && shortcuts[i].modifiers==candidate.modifiers)duplicate=true;
      if(duplicate){shortcutNotice=aux(AUX_6);registerShortcuts(windowHandle);capturingAction=-1;}
      else {int selected=capturingAction;Shortcut old=shortcuts[selected];shortcuts[selected]=candidate;registerShortcuts(windowHandle);if(candidate.key && !shortcutRegistered[selected]){shortcuts[selected]=old;registerShortcuts(windowHandle);shortcutNotice=aux(AUX_7);}else {saveShortcuts();shortcutNotice=aux(AUX_5);capturingAction=-1;}}
      InvalidateRect(hwnd,nullptr,FALSE);return 0;}
    case WM_SYSKEYUP:case WM_KEYUP:return 0;
    case WM_CLOSE:DestroyWindow(hwnd);return 0;
    case WM_DESTROY:shortcutsWindow=nullptr;capturingAction=-1;if(windowHandle)registerShortcuts(windowHandle);return 0;
  }return DefWindowProcW(hwnd,msg,wp,lp);
}
static LRESULT CALLBACK profilesProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
  switch(msg){
    case WM_ERASEBKGND:return 1;
    case WM_PAINT:{PAINTSTRUCT ps{};HDC dc=BeginPaint(hwnd,&ps);RECT r{};GetClientRect(hwnd,&r);panelRect(dc,r,RGB(17,19,24),RGB(177,127,50));panelText(dc,std::wstring(L"AZEROTH / ")+aux(AUX_1),18,9,395,34,RGB(241,192,102),19,true);panelText(dc,L"X",426,8,22,30,RGB(241,192,102),18,true);
      panelText(dc,aux(AUX_9),18,45,420,30,RGB(180,170,150),13);
      for(int i=0;i<3;i++){int y=88+i*54;panelRect(dc,{18,y,435,y+46},RGB(37,34,30),RGB(174,124,50));panelText(dc,std::wstring(aux(AUX_18))+std::to_wstring(i+1),32,y+8,190,28,RGB(227,208,170),17);panelText(dc,L"x"+speedText(profileSpeeds[i]),290,y+8,120,28,RGB(241,192,102),19,true);}EndPaint(hwnd,&ps);return 0;}
    case WM_LBUTTONDOWN:case WM_RBUTTONDOWN:{int x=GET_X_LPARAM(lp),y=GET_Y_LPARAM(lp);if(x>413&&y<42){DestroyWindow(hwnd);return 0;}if(y>=88&&y<88+3*54){int i=(y-88)/54;if(msg==WM_RBUTTONDOWN){profileSpeeds[i]=speed100;saveShortcuts();}else{custom100=profileSpeeds[i];customSelected=true;setSpeed(custom100);saveSettings();if(settingsWindow)InvalidateRect(settingsWindow,nullptr,FALSE);}InvalidateRect(hwnd,nullptr,FALSE);}return 0;}
    case WM_CLOSE:DestroyWindow(hwnd);return 0;
    case WM_DESTROY:profilesWindow=nullptr;return 0;
  }return DefWindowProcW(hwnd,msg,wp,lp);
}
static LRESULT CALLBACK settingsProc(HWND,UINT,WPARAM,LPARAM);
static void saveSettings() {
  if(!windowHandle || iniPath.empty()) return;
  RECT r{};
  WINDOWPLACEMENT placement{};placement.length=sizeof(placement);
  if(IsIconic(windowHandle) && GetWindowPlacement(windowHandle,&placement)) r=placement.rcNormalPosition;
  else GetWindowRect(windowHandle,&r);
  WritePrivateProfileStringW(L"Overlay",L"X",std::to_wstring(r.left).c_str(),iniPath.c_str());
  WritePrivateProfileStringW(L"Overlay",L"Y",std::to_wstring(r.top).c_str(),iniPath.c_str());
  WritePrivateProfileStringW(L"Overlay",L"Compact",compact?L"1":L"0",iniPath.c_str());
  WritePrivateProfileStringW(L"Overlay",L"Opacity",std::to_wstring(opacity).c_str(),iniPath.c_str());
  WritePrivateProfileStringW(L"Overlay",L"CustomSpeed100",std::to_wstring(custom100).c_str(),iniPath.c_str());
  WritePrivateProfileStringW(L"Overlay",L"CustomSelected",customSelected?L"1":L"0",iniPath.c_str());
  WritePrivateProfileStringW(L"Overlay",L"Language",std::to_wstring(language).c_str(),iniPath.c_str());
  WritePrivateProfileStringW(L"Overlay",L"Locked",locked?L"1":L"0",iniPath.c_str());
  WritePrivateProfileStringW(L"Overlay",L"AlwaysOnTop",alwaysOnTop?L"1":L"0",iniPath.c_str());
}
static void resizeOverlay() {
  if(!windowHandle) return;
  SetWindowPos(windowHandle,alwaysOnTop?HWND_TOPMOST:HWND_NOTOPMOST,0,0,compact?compactWidth:widths,compact?compactHeight:heights,SWP_NOMOVE|SWP_NOACTIVATE);
  InvalidateRect(windowHandle,nullptr,TRUE);
}
// Refuse ambiguous matches instead of injecting into an arbitrary same-named process.
// QueryFullProcessImageNameW also confirms the name from the actual process image.
static DWORD findGame() {
  HANDLE snap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
  if(snap==INVALID_HANDLE_VALUE) return 0;
  PROCESSENTRY32W entry{sizeof(entry)};
  DWORD found=0;
  unsigned matches=0;
  if(Process32FirstW(snap,&entry)) do {
    if(_wcsicmp(entry.szExeFile,L"AzerothSurvivors.exe")!=0) continue;
    HANDLE candidate=OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,entry.th32ProcessID);
    if(!candidate) continue;
    wchar_t image[32768]={}; DWORD length=32768;
    if(QueryFullProcessImageNameW(candidate,0,image,&length) &&
       _wcsicmp(std::filesystem::path(image).filename().c_str(),L"AzerothSurvivors.exe")==0) {
      found=entry.th32ProcessID;
      ++matches;
    }
    CloseHandle(candidate);
  } while(Process32NextW(snap,&entry));
  CloseHandle(snap);
  if(matches>1) { status=L"Multiple game processes found; close extra copies"; return 0; }
  return matches==1 ? found : 0;
}
static bool connectMap(DWORD processId) {
  wchar_t name[96]; wsprintfW(name,L"Local\\AZSSpeed_%lu",processId);
  mapHandle=OpenFileMappingW(FILE_MAP_ALL_ACCESS,FALSE,name);
  if(!mapHandle) return false;
  shared=static_cast<Shared*>(MapViewOfFile(mapHandle,FILE_MAP_ALL_ACCESS,0,0,sizeof(Shared)));
  if(!shared) {CloseHandle(mapHandle);mapHandle=nullptr;return false;}
  return true;
}
static bool inject(DWORD processId) {
  wchar_t controllerPath[MAX_PATH] = {};
  if (!GetModuleFileNameW(nullptr, controllerPath, MAX_PATH)) { status=L"Cannot resolve controller location"; return false; }
  auto dll=(std::filesystem::path(controllerPath).parent_path() / L"AZSSpeedHook.dll").wstring();
  // Only use a regular 64-bit DLL from the controller directory.
  std::error_code fileError;
  if(!std::filesystem::is_regular_file(dll,fileError) || fileError) {status=L"Missing AZSSpeedHook.dll";return false;}
  // GetBinaryTypeW may reject DLL images. Inspect the PE headers instead.
  bool validDll = false;
  HANDLE dllFile = CreateFileW(dll.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (dllFile != INVALID_HANDLE_VALUE) {
    IMAGE_DOS_HEADER dos = {};
    DWORD bytesRead = 0;
    LARGE_INTEGER fileSize = {};
    if (GetFileSizeEx(dllFile, &fileSize) && fileSize.QuadPart >= 1024 &&
        ReadFile(dllFile, &dos, sizeof(dos), &bytesRead, nullptr) &&
        bytesRead == sizeof(dos) && dos.e_magic == IMAGE_DOS_SIGNATURE &&
        dos.e_lfanew >= static_cast<LONG>(sizeof(dos)) &&
        static_cast<LONGLONG>(dos.e_lfanew) + sizeof(IMAGE_NT_HEADERS64) <= fileSize.QuadPart) {
      LARGE_INTEGER offset = {};
      offset.QuadPart = dos.e_lfanew;
      IMAGE_NT_HEADERS64 nt = {};
      if (SetFilePointerEx(dllFile, offset, nullptr, FILE_BEGIN) &&
          ReadFile(dllFile, &nt, sizeof(nt), &bytesRead, nullptr) &&
          bytesRead == sizeof(nt)) {
        validDll = nt.Signature == IMAGE_NT_SIGNATURE &&
                   nt.FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64 &&
                   (nt.FileHeader.Characteristics & IMAGE_FILE_DLL) != 0 &&
                   nt.OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC;
      }
    }
    CloseHandle(dllFile);
  }
  if (!validDll) {
    status=L"AZSSpeedHook.dll is missing, unreadable or not a valid x64 DLL";
    return false;
  }
  HANDLE process=OpenProcess(PROCESS_CREATE_THREAD|PROCESS_QUERY_INFORMATION|PROCESS_VM_OPERATION|PROCESS_VM_WRITE|PROCESS_VM_READ,FALSE,processId);
  if(!process) {status=L"Cannot access game process";return false;}
  SIZE_T bytes=(dll.size()+1)*sizeof(wchar_t);
  LPVOID remote=VirtualAllocEx(process,nullptr,bytes,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
  bool ok=false;
  bool threadFinished=true;
  if(remote && WriteProcessMemory(process,remote,dll.c_str(),bytes,nullptr)) {
    HMODULE kernel=GetModuleHandleW(L"kernel32.dll");
    auto load=reinterpret_cast<LPTHREAD_START_ROUTINE>(GetProcAddress(kernel,"LoadLibraryW"));
    HANDLE thread=CreateRemoteThread(process,nullptr,0,load,remote,0,nullptr);
    if(thread) { DWORD wait=WaitForSingleObject(thread,5000); DWORD result=0;
      threadFinished=(wait==WAIT_OBJECT_0);
      if(threadFinished && GetExitCodeThread(thread,&result)) ok=(result!=0);
      CloseHandle(thread);
    }
  }
  // Do not free remote path if remote thread is still executing.
  if(remote && threadFinished) VirtualFreeEx(process,remote,0,MEM_RELEASE);
  CloseHandle(process);
  if(!ok) status=L"DLL load failed (check architecture/permissions)";
  return ok;
}
static void setSpeed(int value) {
  if(value>1000 && !extremeConfirmed) {
    int response=MessageBoxW(windowHandle,tr(L"Speeds above x10 are experimental and may crash or destabilize the game. Continue?",L"Les vitesses au-dessus de ×10 sont expérimentales et peuvent rendre le jeu instable. Continuer ?"),L"AZS Speed Controller",MB_YESNO|MB_ICONWARNING);
    if(response!=IDYES) return;
    extremeConfirmed=true;
  }
  if(!shared) {status=L"Game not connected";if(windowHandle) InvalidateRect(windowHandle,nullptr,TRUE);return;}
  speed100=value;
  InterlockedExchange(&shared->speed100,value);
  InvalidateRect(windowHandle,nullptr,TRUE);
}
static void disconnect() {
  if(shared) {InterlockedExchange(&shared->speed100,100);UnmapViewOfFile(shared);shared=nullptr;}
  if(mapHandle) {CloseHandle(mapHandle);mapHandle=nullptr;}
  pid=0; hooked=false;speed100=100;
}
static void refresh() {
  DWORD current=findGame();
  if(current!=pid) disconnect();
  if(!current) {if(status!=L"Multiple game processes found; close extra copies") status=L"Waiting for AzerothSurvivors.exe";InvalidateRect(windowHandle,nullptr,TRUE);return;}
  pid=current;
  if(!shared && connectMap(pid)) {hooked=true;status=L"Connected";setSpeed(speed100);}
  if(!shared && !hooked) {
    status=L"Connecting...";
    if(inject(pid)) { for(int i=0;i<12 && !shared;i++) {Sleep(50);connectMap(pid);} }
    if(shared) {hooked=true;status=L"Connected";setSpeed(100);}
  }
  InvalidateRect(windowHandle,nullptr,TRUE);
}
static void drawBorder(HDC dc, RECT rect, COLORREF color, int thickness) {
  HPEN pen=CreatePen(PS_SOLID,thickness,color);
  HGDIOBJ old=SelectObject(dc,pen);
  HGDIOBJ oldBrush=SelectObject(dc,GetStockObject(NULL_BRUSH));
  Rectangle(dc,rect.left,rect.top,rect.right,rect.bottom);
  SelectObject(dc,oldBrush);SelectObject(dc,old);DeleteObject(pen);
}
static void applyOpacity() {
  if(windowHandle) SetLayeredWindowAttributes(windowHandle,0,static_cast<BYTE>(opacity),LWA_ALPHA);
}
static void paint(HWND hwnd) {
  PAINTSTRUCT ps{};HDC dc=BeginPaint(hwnd,&ps);
  RECT rect{};GetClientRect(hwnd,&rect);
  const int w=rect.right,h=rect.bottom;
  HBRUSH bg=CreateSolidBrush(RGB(15,18,24));FillRect(dc,&rect,bg);DeleteObject(bg);
  RECT strip{2,2,w-2,5};HBRUSH gold=CreateSolidBrush(RGB(176,127,49));FillRect(dc,&strip,gold);DeleteObject(gold);
  RECT inner{4,7,w-4,h-4};drawBorder(dc,inner,RGB(79,64,43),1);
  drawBorder(dc,rect,RGB(171,123,50),1);
  SetBkMode(dc,TRANSPARENT);
  HFONT titleFont=CreateFontW(-17,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI Semibold");
  HFONT smallFont=CreateFontW(-13,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
  HGDIOBJ oldFont=SelectObject(dc,titleFont);
  SetTextColor(dc,RGB(237,193,111));
  RECT header{14,11,w-95,34};
  DrawTextW(dc,compact?L"AZEROTH  /  SPEED":tr(L"AZEROTH  /  SPEED CONTROL",L"AZEROTH  /  CONTROLE VITESSE"),-1,&header,DT_SINGLELINE|DT_END_ELLIPSIS);
  if(!compact) { SetTextColor(dc,RGB(181,146,91));RECT vr{w-116,35,w-16,55};DrawTextW(dc,L"v1.2.3",-1,&vr,DT_RIGHT|DT_SINGLELINE); }
  // Custom close button: always visible in both overlay sizes.
  if(!compact) {
    RECT cfg{w-57,11,w-34,29};
    drawBorder(dc,cfg,RGB(111,84,48),1);
    // Vector gear icon, drawn with GDI so no font glyph or asset is required.
    const int cx=(cfg.left+cfg.right)/2, cy=(cfg.top+cfg.bottom)/2;
    HPEN gearPen=CreatePen(PS_SOLID,2,RGB(237,193,111));
    HGDIOBJ previousPen=SelectObject(dc,gearPen);
    HGDIOBJ previousBrush=SelectObject(dc,GetStockObject(NULL_BRUSH));
    for(int i=0;i<8;++i) {
      const double angle=6.283185307179586*i/8.0;
      const int x1=cx+static_cast<int>(std::lround(5.0*std::cos(angle)));
      const int y1=cy+static_cast<int>(std::lround(5.0*std::sin(angle)));
      const int x2=cx+static_cast<int>(std::lround(8.0*std::cos(angle)));
      const int y2=cy+static_cast<int>(std::lround(8.0*std::sin(angle)));
      MoveToEx(dc,x1,y1,nullptr);LineTo(dc,x2,y2);
    }
    Ellipse(dc,cx-5,cy-5,cx+6,cy+6);
    Ellipse(dc,cx-2,cy-2,cx+3,cy+3);
    SelectObject(dc,previousBrush);SelectObject(dc,previousPen);
    DeleteObject(gearPen);
  }
  RECT closeButton{w-29,11,w-11,29};
  HBRUSH closeFill=CreateSolidBrush(RGB(75,30,30));
  FillRect(dc,&closeButton,closeFill);DeleteObject(closeFill);
  drawBorder(dc,closeButton,RGB(180,113,65),1);
  SetTextColor(dc,RGB(255,220,182));
  DrawTextW(dc,L"X",-1,&closeButton,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
  SelectObject(dc,smallFont);
  RECT stateRect{14,compact?33:36,w-14,compact?51:56};
  const std::wstring stateText=std::wstring(shared?loc(24):loc(25))+L"  "+std::wstring(tr(status.c_str(),status.c_str()))+L"  |  x"+speedText(speed100);
  SetTextColor(dc,shared?RGB(129,213,166):RGB(220,153,123));
  DrawTextW(dc,stateText.c_str(),-1,&stateRect,DT_SINGLELINE|DT_END_ELLIPSIS);
  if(!compact) {
    const int values[]={100,200,500,1000,custom100};
    for(int i=0;i<5;i++) {
      const int x=12+i*87;RECT button{x,63,x+78,98};
      const bool selected=(i==4)?customSelected:(!customSelected && values[i]==speed100);
      HBRUSH fill=CreateSolidBrush(selected?RGB(96,66,31):RGB(35,40,50));
      FillRect(dc,&button,fill);DeleteObject(fill);
      drawBorder(dc,button,selected?RGB(231,180,89):RGB(85,78,65),1);
      SetTextColor(dc,selected?RGB(255,221,157):RGB(207,214,226));
      std::wstring label=(i==4?L"*x":L"x")+speedText(values[i]);
      DrawTextW(dc,label.c_str(),-1,&button,DT_CENTER|DT_VCENTER|DT_SINGLELINE);
    }
  }
  SelectObject(dc,oldFont);DeleteObject(titleFont);DeleteObject(smallFont);
  EndPaint(hwnd,&ps);
}
static LRESULT CALLBACK wndProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
  switch(msg) {
    case WM_CREATE: SetTimer(hwnd,1,1500,nullptr);return 0;
    case WM_MOVE: if(windowHandle && !IsIconic(hwnd)) saveSettings();return 0;
    case WM_SIZE: if(wp==SIZE_RESTORED) {hidden=false;InvalidateRect(hwnd,nullptr,TRUE);} return 0;
    case WM_TIMER: refresh();return 0;
    case WM_HOTKEY: {
      int action=static_cast<int>(wp)-100;
      if(action>=SPEED_1 && action<=SPEED_10) {const int values[]={100,200,500,1000};customSelected=false;setSpeed(values[action]);}
      else if(action==SPEED_CUSTOM) {customSelected=true;setSpeed(custom100);}
      else if(action==TOGGLE_COMPACT) {compact=!compact;resizeOverlay();saveSettings();}
      else if(action==TOGGLE_VISIBLE) {
        if(IsIconic(hwnd)) {hidden=false;ShowWindow(hwnd,SW_RESTORE);SetWindowPos(hwnd,alwaysOnTop?HWND_TOPMOST:HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);}
        else {hidden=true;ShowWindow(hwnd,SW_MINIMIZE);}
      }
      else if(action==OPEN_SETTINGS)openSettings();
      else if(action>=PROFILE_1 && action<=PROFILE_3){customSelected=true;custom100=profileSpeeds[action-PROFILE_1];setSpeed(custom100);saveSettings();if(settingsWindow)InvalidateRect(settingsWindow,nullptr,FALSE);}
      return 0;
    }
    case WM_LBUTTONDOWN: {
      int x=GET_X_LPARAM(lp),y=GET_Y_LPARAM(lp);
      RECT client{};GetClientRect(hwnd,&client);
      if(x>=client.right-29 && x<client.right-11 && y>=11 && y<29) {SendMessageW(hwnd,WM_CLOSE,0,0);return 0;}
      if(!compact && x>=client.right-57 && x<client.right-34 && y>=11 && y<29) {openSettings();return 0;}
      if(!compact && y>=63 && y<=98 && x>=12 && x<448) {int idx=(x-12)/87;const int speeds[]={100,200,500,1000,custom100};if(idx<5) {customSelected=(idx==4);setSpeed(speeds[idx]);}}
      else if(compact && y>=30 && y<54 && x>=0 && x<200) {customSelected=false;setSpeed(100);}
      else if(!locked) {ReleaseCapture();SendMessageW(hwnd,WM_NCLBUTTONDOWN,HTCAPTION,0);}
      return 0;
    }
    case WM_RBUTTONUP: {
      POINT pt{};GetCursorPos(&pt);
      HMENU menu=CreatePopupMenu();
      AppendMenuW(menu,MF_STRING,1001,tr(L"Normal speed (x1)",L"Vitesse normale (x1)"));
      AppendMenuW(menu,MF_STRING,1005,tr(L"Settings...",L"Paramètres..."));
      AppendMenuW(menu,MF_STRING,1006,L"Keyboard shortcuts...");
      AppendMenuW(menu,MF_STRING,1007,L"Speed profiles...");
      AppendMenuW(menu,MF_STRING,1002,compact?tr(L"Expand (F10)",L"Agrandir (F10)"):tr(L"Compact (F10)",L"Compact (F10)"));
      AppendMenuW(menu,MF_STRING,1003,tr(L"Minimize (F11)",L"Réduire (F11)"));
      AppendMenuW(menu,MF_SEPARATOR,0,nullptr);
      AppendMenuW(menu,MF_STRING,1004,tr(L"Exit and reset speed",L"Quitter et remettre x1"));
      SetForegroundWindow(hwnd);
      int action=TrackPopupMenu(menu,TPM_RETURNCMD|TPM_RIGHTBUTTON,pt.x,pt.y,0,hwnd,nullptr);
      DestroyMenu(menu);
      if(action==1001) {customSelected=false;setSpeed(100);}
      if(action==1002) {compact=!compact;resizeOverlay();saveSettings();}
      if(action==1003) {hidden=true;ShowWindow(hwnd,SW_MINIMIZE);}
      if(action==1005) openSettings();
      if(action==1006) {openSettings();PostMessageW(settingsWindow,WM_APP+20,0,0);}
      if(action==1007) {openSettings();PostMessageW(settingsWindow,WM_APP+21,0,0);}
      if(action==1004) DestroyWindow(hwnd);
      return 0;
    }
    case WM_PAINT: paint(hwnd);return 0;
    case WM_QUERYENDSESSION: if(shared) InterlockedExchange(&shared->speed100,100);return TRUE;
    case WM_CLOSE: DestroyWindow(hwnd);return 0;
    case WM_DESTROY: if(shortcutsWindow)DestroyWindow(shortcutsWindow);if(profilesWindow)DestroyWindow(profilesWindow);if(settingsWindow) DestroyWindow(settingsWindow);saveSettings();disconnect();for(int k=0;k<ACTION_COUNT;k++)UnregisterHotKey(hwnd,100+k);if(singletonMutex){CloseHandle(singletonMutex);singletonMutex=nullptr;}PostQuitMessage(0);return 0;
  }
  return DefWindowProcW(hwnd,msg,wp,lp);
}
// Fully owner-drawn settings panel. No Windows themed controls.
static constexpr int settingsW=420, settingsH=482;
static void panelRect(HDC dc,RECT r,COLORREF fill,COLORREF edge) {
  HBRUSH brush=CreateSolidBrush(fill);FillRect(dc,&r,brush);DeleteObject(brush);
  drawBorder(dc,r,edge,1);
}
static void panelText(HDC dc,const std::wstring& str,int x,int y,int w,int h,COLORREF color,int size,bool bold) {
  HFONT font=CreateFontW(-size,0,0,0,bold?FW_SEMIBOLD:FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,L"Segoe UI");
  HGDIOBJ old=SelectObject(dc,font);SetTextColor(dc,color);SetBkMode(dc,TRANSPARENT);
  RECT r{x,y,x+w,y+h};DrawTextW(dc,str.c_str(),-1,&r,DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
  SelectObject(dc,old);DeleteObject(font);
}
static void panelSlider(HDC dc,int y,int val,int minV,int maxV) {
  const int left=25,right=395;
  RECT track{left,y+7,right,y+11};HBRUSH dark=CreateSolidBrush(RGB(65,60,54));FillRect(dc,&track,dark);DeleteObject(dark);
  int px=left+(right-left)*(val-minV)/(maxV-minV);
  RECT fill{left,y+7,px,y+11};HBRUSH gold=CreateSolidBrush(RGB(209,155,65));FillRect(dc,&fill,gold);DeleteObject(gold);
  RECT thumb{px-7,y+1,px+7,y+18};panelRect(dc,thumb,RGB(223,176,91),RGB(255,215,133));
}
static void paintSettings(HWND hwnd) {
  PAINTSTRUCT ps{};HDC dc=BeginPaint(hwnd,&ps);
  RECT area{};GetClientRect(hwnd,&area);HBRUSH bg=CreateSolidBrush(RGB(17,19,24));FillRect(dc,&area,bg);DeleteObject(bg);
  drawBorder(dc,area,RGB(177,127,50),1);
  RECT stripe{2,2,settingsW-2,6};HBRUSH gold=CreateSolidBrush(RGB(183,130,53));FillRect(dc,&stripe,gold);DeleteObject(gold);
  panelText(dc,L"AZEROTH  /  "+std::wstring(tr(L"SETTINGS",L"PARAMÈTRES")),22,15,340,30,RGB(239,192,105),20,true);
  panelRect(dc,{379,17,402,40},RGB(74,32,29),RGB(185,112,64));panelText(dc,L"X",385,18,18,20,RGB(255,215,175),15,true);
  panelText(dc,tr(L"OVERLAY OPACITY",L"OPACITÉ DE L'OVERLAY"),25,62,260,24,RGB(218,201,169),14,true);
  panelText(dc,std::to_wstring((opacity*100+127)/255)+L"%",344,62,55,24,RGB(241,190,101),17,true);
  panelSlider(dc,92,(opacity*100+127)/255,30,100);
  panelText(dc,tr(L"CUSTOM SPEED",L"VITESSE PERSONNALISÉE"),25,128,280,24,RGB(218,201,169),14,true);
  panelText(dc,L"x"+speedText(custom100),320,127,85,25,RGB(241,190,101),19,true);
  panelSlider(dc,159,custom100,100,2500);
  panelText(dc,L"x1",25,178,60,20,RGB(151,155,159),12);
  panelText(dc,L"x10",183,178,65,20,RGB(214,165,97),12);
  panelText(dc,L"x25",365,178,45,20,RGB(226,113,93),12);
  if(custom100>1000) panelText(dc,tr(L"Experimental above x10 - use at your own risk",L"Au-dessus de ×10 : expérimental, à vos risques"),25,202,374,23,RGB(228,147,102),13);
  panelText(dc,tr(L"LANGUAGE",L"LANGUE"),25,234,260,22,RGB(218,201,169),14,true);
  panelRect(dc,{25,260,395,295},RGB(32,34,39),RGB(109,84,49));
  panelText(dc,(language==0?loc(26):languageNames[language]),38,263,310,28,RGB(238,211,169),16);
  panelText(dc,L"▼",358,263,25,28,RGB(223,174,87),16,true);
  panelText(dc,tr(L"LOCK OVERLAY POSITION",L"VERROUILLER LA POSITION"),25,312,320,24,RGB(218,201,169),14);
  panelRect(dc,{363,313,394,338},locked?RGB(125,89,37):RGB(41,44,50),locked?RGB(231,174,80):RGB(91,88,79));
  if(locked) {RECT dot{373,319,385,331};HBRUSH b=CreateSolidBrush(RGB(255,225,153));HBRUSH old=(HBRUSH)SelectObject(dc,b);Ellipse(dc,dot.left,dot.top,dot.right,dot.bottom);SelectObject(dc,old);DeleteObject(b);}
  panelText(dc,tr(L"ALWAYS ON TOP",L"TOUJOURS AU PREMIER PLAN"),25,352,320,24,RGB(218,201,169),14);
  panelRect(dc,{363,353,394,378},alwaysOnTop?RGB(125,89,37):RGB(41,44,50),alwaysOnTop?RGB(231,174,80):RGB(91,88,79));
  if(alwaysOnTop) {RECT dot{373,359,385,371};HBRUSH b=CreateSolidBrush(RGB(255,225,153));HBRUSH old=(HBRUSH)SelectObject(dc,b);Ellipse(dc,dot.left,dot.top,dot.right,dot.bottom);SelectObject(dc,old);DeleteObject(b);}
  panelText(dc,tr(L"Settings are saved automatically",L"Paramètres enregistrés automatiquement"),25,390,290,22,RGB(140,139,131),12);
  panelText(dc,L"v1.2.3",342,390,65,22,RGB(204,155,77),12,true);
  panelRect(dc,{25,419,195,459},RGB(42,35,28),RGB(180,130,52));
  panelText(dc,aux(AUX_0),34,422,158,30,RGB(241,192,102),13,true);
  panelRect(dc,{206,419,395,459},RGB(42,35,28),RGB(180,130,52));
  panelText(dc,aux(AUX_1),218,422,168,30,RGB(241,192,102),13,true);
  EndPaint(hwnd,&ps);
}

static HWND languagePopup=nullptr;
static int languageScroll=0;
static constexpr int langRowH=31, langVisible=9, langPopupW=370;
static constexpr int langFooterH=26, langScrollW=15;
static LRESULT CALLBACK languageProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
  switch(msg) {
    case WM_ERASEBKGND:return 1;
    case WM_PAINT: {
      PAINTSTRUCT ps{};HDC dc=BeginPaint(hwnd,&ps);
      RECT all{};GetClientRect(hwnd,&all);panelRect(dc,all,RGB(20,23,29),RGB(199,145,62));
      for(int row=0;row<langVisible;row++) {
        int index=languageScroll+row;if(index>=languageCount)break;
        int top=4+row*langRowH;
        RECT r{4,top,langPopupW-langScrollW-5,top+langRowH-1};
        panelRect(dc,r,index==language?RGB(103,68,30):RGB(30,34,41),index==language?RGB(229,176,91):RGB(62,57,48));
        panelText(dc,languageNames[index],16,top+2,langPopupW-60,langRowH-4,RGB(242,214,164),15,index==language);
        if(index==language)panelText(dc,L"•",langPopupW-47,top+2,16,langRowH-4,RGB(255,201,102),16,true);
      }
      // Visible scrollbar and localized scrolling hint.
      const int trackTop=4, trackH=langVisible*langRowH;
      const int barX=langPopupW-langScrollW-2;
      panelRect(dc,{barX,trackTop,langPopupW-3,trackTop+trackH},RGB(35,37,42),RGB(75,67,54));
      const int thumbH=(std::max)(30,trackH*langVisible/languageCount);
      const int maxScroll=languageCount-langVisible;
      const int thumbY=trackTop+(trackH-thumbH)*languageScroll/(std::max)(1,maxScroll);
      panelRect(dc,{barX+2,thumbY,langPopupW-5,thumbY+thumbH},RGB(195,145,65),RGB(236,188,99));
      const wchar_t* hint=L"Mouse wheel / scroll";
      switch(resolvedLanguage()) {
        case 1: hint=L"Molette / défiler";break;
        case 2: hint=L"Kółko myszy / przewiń";break;
        case 3: hint=L"Kolečko myši / posun";break;
        case 4: hint=L"Mausrad / scrollen";break;
        case 5: hint=L"Roda do mouse / rolar";break;
        case 6: hint=L"Rueda del ratón / desplazar";break;
        case 7: hint=L"Rotița mouse-ului / derulare";break;
        case 8: hint=L"Fare tekerleği / kaydır";break;
        case 9: hint=L"Колесо мыши / прокрутка";break;
        case 10: hint=L"Коліщатко миші / прокрутка";break;
        case 11: hint=L"鼠标滚轮滚动";break;
        case 12: hint=L"マウスホイールでスクロール";break;
        case 13: hint=L"마우스 휠로 스크롤";break;
      }
      panelText(dc,std::wstring(L"↓  ")+hint,13,trackTop+trackH+2,langPopupW-22,langFooterH-4,RGB(197,157,94),13);
      EndPaint(hwnd,&ps);return 0;
    }
    case WM_MOUSEWHEEL: {
      int delta=GET_WHEEL_DELTA_WPARAM(wp);
      languageScroll=(std::clamp)(languageScroll-(delta>0?1:-1),0,languageCount-langVisible);
      InvalidateRect(hwnd,nullptr,FALSE);return 0;
    }
    case WM_LBUTTONDOWN: {
      const int mx=GET_X_LPARAM(lp), my=GET_Y_LPARAM(lp);
      if(mx>=langPopupW-langScrollW-3 && my>=4 && my<4+langVisible*langRowH) {
        const int trackH=langVisible*langRowH;
        const int thumbH=(std::max)(30,trackH*langVisible/languageCount);
        languageScroll=(std::clamp)((my-4-thumbH/2)*(languageCount-langVisible)/(std::max)(1,trackH-thumbH),0,languageCount-langVisible);
        InvalidateRect(hwnd,nullptr,FALSE);return 0;
      }
      if(my<4 || my>=4+langVisible*langRowH) return 0;
      int row=(my-4)/langRowH;
      int index=languageScroll+row;
      if(row>=0 && row<langVisible && index<languageCount) {
        language=index;
        shortcutNotice.clear(); // Drop a transient message rendered in the previous locale.
        saveSettings();
        // All auxiliary panels are owner-drawn. Their labels are resolved at paint time,
        // so invalidate every live window when the selected locale changes.
        for(HWND panel : {windowHandle, settingsWindow, shortcutsWindow, profilesWindow}) {
          if(panel && IsWindow(panel)) InvalidateRect(panel,nullptr,TRUE);
        }
        DestroyWindow(hwnd);
      }
      return 0;
    }
    case WM_KEYDOWN:if(wp==VK_ESCAPE){DestroyWindow(hwnd);return 0;}break;
    case WM_KILLFOCUS:DestroyWindow(hwnd);return 0;
    case WM_DESTROY:languagePopup=nullptr;return 0;
  }
  return DefWindowProcW(hwnd,msg,wp,lp);
}
static void toggleLanguagePopup(HWND parent) {
  if(languagePopup){DestroyWindow(languagePopup);return;}
  POINT pos{25,295};ClientToScreen(parent,&pos);
  RECT work{};SystemParametersInfoW(SPI_GETWORKAREA,0,&work,0);
  const int height=langVisible*langRowH+langFooterH+8;
  if(pos.y+height>work.bottom)pos.y-=height+35;
  languageScroll=(std::clamp)(language-3,0,languageCount-langVisible);
  languagePopup=CreateWindowExW(WS_EX_TOPMOST|WS_EX_TOOLWINDOW,L"AZSSpeedLanguages",L"Languages",WS_POPUP,pos.x,pos.y,langPopupW,height,parent,nullptr,(HINSTANCE)GetWindowLongPtrW(parent,GWLP_HINSTANCE),nullptr);
  if(languagePopup){ShowWindow(languagePopup,SW_SHOW);SetForegroundWindow(languagePopup);InvalidateRect(languagePopup,nullptr,TRUE);}
}

static void sliderChange(HWND hwnd,int x,int which) {
  x=(std::clamp)(x,25,395);
  if(which==1) {int percent=30+((x-25)*70+185)/370;opacity=(percent*255+50)/100;applyOpacity();}
  if(which==2) {int steps=((x-25)*48+185)/370;custom100=100+steps*50;if(customSelected && shared) {
    // Keep selected custom speed synchronized live; warning is confirmed once.
    if(custom100<=1000 || extremeConfirmed) setSpeed(custom100);
    else {customSelected=false;}
  }InvalidateRect(windowHandle,nullptr,TRUE);}
  saveSettings();InvalidateRect(hwnd,nullptr,FALSE);
}
static LRESULT CALLBACK settingsProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
  switch(msg) {
    case WM_APP+20:showUtility(shortcutsWindow,L"AZSSpeedShortcuts",L"Shortcuts",495);return 0;
    case WM_APP+21:showUtility(profilesWindow,L"AZSSpeedProfiles",L"Profiles",275);return 0;
    case WM_PAINT:paintSettings(hwnd);return 0;
    case WM_ERASEBKGND:return 1;
    case WM_LBUTTONDOWN: {
      int x=GET_X_LPARAM(lp),y=GET_Y_LPARAM(lp);
      if(x>=379 && y>=17 && y<40) {DestroyWindow(hwnd);return 0;}
      if(y>=84 && y<=117) {draggingSlider=1;SetCapture(hwnd);sliderChange(hwnd,x,1);return 0;}
      if(y>=151 && y<=179) {draggingSlider=2;SetCapture(hwnd);sliderChange(hwnd,x,2);return 0;}
      if(y>=419 && y<=459) {if(x<200)PostMessageW(hwnd,WM_APP+20,0,0);else PostMessageW(hwnd,WM_APP+21,0,0);return 0;}
      if(y>=260 && y<=295) {toggleLanguagePopup(hwnd);return 0;}
      if(y>=305 && y<=344) {locked=!locked;saveSettings();InvalidateRect(hwnd,nullptr,FALSE);return 0;}
      if(y>=345 && y<=385) {alwaysOnTop=!alwaysOnTop;SetWindowPos(windowHandle,alwaysOnTop?HWND_TOPMOST:HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);saveSettings();InvalidateRect(hwnd,nullptr,FALSE);return 0;}
      if(y<52) {ReleaseCapture();SendMessageW(hwnd,WM_NCLBUTTONDOWN,HTCAPTION,0);return 0;}
      return 0;
    }
    case WM_MOUSEMOVE:if(draggingSlider && (wp&MK_LBUTTON)) {sliderChange(hwnd,GET_X_LPARAM(lp),draggingSlider);return 0;}break;
    case WM_LBUTTONUP:if(draggingSlider) {sliderChange(hwnd,GET_X_LPARAM(lp),draggingSlider);draggingSlider=0;ReleaseCapture();return 0;}break;
    case WM_CAPTURECHANGED:draggingSlider=0;return 0;
    case WM_CLOSE:DestroyWindow(hwnd);return 0;
    case WM_DESTROY:if(languagePopup)DestroyWindow(languagePopup);settingsWindow=nullptr;draggingSlider=0;return 0;
  }
  return DefWindowProcW(hwnd,msg,wp,lp);
}
static void openSettings() {
  if(settingsWindow) {ShowWindow(settingsWindow,SW_RESTORE);SetForegroundWindow(settingsWindow);return;}
  RECT owner{};GetWindowRect(windowHandle,&owner);
  settingsWindow=CreateWindowExW(WS_EX_TOOLWINDOW,L"AZSSpeedSettings",L"AZS Speed - Settings",WS_POPUP|WS_SYSMENU,owner.left+20,owner.bottom+10,settingsW,settingsH,windowHandle,nullptr,(HINSTANCE)GetWindowLongPtrW(windowHandle,GWLP_HINSTANCE),nullptr);
  if(settingsWindow) {ShowWindow(settingsWindow,SW_SHOW);UpdateWindow(settingsWindow);}
}
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE,LPWSTR,int) {
  singletonMutex=CreateMutexW(nullptr,TRUE,L"Local\\AZSSpeedController_SingleInstance");
  if(!singletonMutex) return 1;
  if(GetLastError()==ERROR_ALREADY_EXISTS) {MessageBoxW(nullptr,L"AZS Speed Controller is already running.",L"AZS Speed",MB_OK|MB_ICONINFORMATION);CloseHandle(singletonMutex);singletonMutex=nullptr;return 0;}
  // Use the executable directory for the companion DLL, regardless of working directory.
  wchar_t exe[MAX_PATH];GetModuleFileNameW(nullptr,exe,MAX_PATH);
  auto exeDir=std::filesystem::path(exe).parent_path();
  SetCurrentDirectoryW(exeDir.c_str());
  iniPath=(exeDir/L"AZSSpeedController.ini").wstring();
  savedX=GetPrivateProfileIntW(L"Overlay",L"X",80,iniPath.c_str());
  savedY=GetPrivateProfileIntW(L"Overlay",L"Y",80,iniPath.c_str());
  compact=GetPrivateProfileIntW(L"Overlay",L"Compact",0,iniPath.c_str())!=0;
  opacity=(std::clamp)(static_cast<int>(GetPrivateProfileIntW(L"Overlay",L"Opacity",235,iniPath.c_str())),77,255);
  custom100=(std::clamp)(static_cast<int>(GetPrivateProfileIntW(L"Overlay",L"CustomSpeed100",500,iniPath.c_str())),100,2500);custom100=(custom100/50)*50;
  language=(std::clamp)(static_cast<int>(GetPrivateProfileIntW(L"Overlay",L"Language",0,iniPath.c_str())),0,languageCount-1);
  customSelected=GetPrivateProfileIntW(L"Overlay",L"CustomSelected",0,iniPath.c_str())!=0;
  locked=GetPrivateProfileIntW(L"Overlay",L"Locked",0,iniPath.c_str())!=0;
  alwaysOnTop=GetPrivateProfileIntW(L"Overlay",L"AlwaysOnTop",1,iniPath.c_str())!=0;
  loadShortcuts();
  // Keep positions on secondary monitors: use the monitor nearest the saved point.
  POINT savedPoint{savedX,savedY};
  HMONITOR monitor=MonitorFromPoint(savedPoint,MONITOR_DEFAULTTONEAREST);
  MONITORINFO monitorInfo{};monitorInfo.cbSize=sizeof(monitorInfo);
  RECT screen{};
  if(GetMonitorInfoW(monitor,&monitorInfo)) screen=monitorInfo.rcWork;
  else SystemParametersInfoW(SPI_GETWORKAREA,0,&screen,0);
  const int overlayWidth=compact?compactWidth:widths;
  const int overlayHeight=compact?compactHeight:heights;
  const int minX=static_cast<int>(screen.left),minY=static_cast<int>(screen.top);
  const int maxX=(std::max)(minX,static_cast<int>(screen.right)-overlayWidth);
  const int maxY=(std::max)(minY,static_cast<int>(screen.bottom)-overlayHeight);
  savedX=(std::clamp)(savedX,minX,maxX);
  savedY=(std::clamp)(savedY,minY,maxY);

  WNDCLASSW wc{};wc.lpfnWndProc=wndProc;wc.hInstance=instance;wc.lpszClassName=L"AZSSpeedOverlay";wc.hCursor=LoadCursorW(nullptr,MAKEINTRESOURCEW(32512));wc.hIcon=LoadIconW(instance,MAKEINTRESOURCEW(101));
  RegisterClassW(&wc);
  WNDCLASSW langClass{};langClass.lpfnWndProc=languageProc;langClass.hInstance=instance;langClass.lpszClassName=L"AZSSpeedLanguages";langClass.hCursor=LoadCursorW(nullptr,MAKEINTRESOURCEW(32512));RegisterClassW(&langClass);
  WNDCLASSW sw{};sw.lpfnWndProc=settingsProc;sw.hInstance=instance;sw.lpszClassName=L"AZSSpeedSettings";sw.hCursor=LoadCursorW(nullptr,MAKEINTRESOURCEW(32512));sw.hbrBackground=nullptr;RegisterClassW(&sw);
  WNDCLASSW kc{};kc.lpfnWndProc=shortcutProc;kc.hInstance=instance;kc.lpszClassName=L"AZSSpeedShortcuts";kc.hCursor=LoadCursorW(nullptr,MAKEINTRESOURCEW(32512));RegisterClassW(&kc);
  WNDCLASSW pc{};pc.lpfnWndProc=profilesProc;pc.hInstance=instance;pc.lpszClassName=L"AZSSpeedProfiles";pc.hCursor=LoadCursorW(nullptr,MAKEINTRESOURCEW(32512));RegisterClassW(&pc);
  windowHandle=CreateWindowExW((alwaysOnTop?WS_EX_TOPMOST:0)|WS_EX_APPWINDOW|WS_EX_LAYERED,L"AZSSpeedOverlay",L"AZS Speed Controller",WS_POPUP|WS_SYSMENU|WS_MINIMIZEBOX,savedX,savedY,compact?compactWidth:widths,compact?compactHeight:heights,nullptr,nullptr,instance,nullptr);
  if(!windowHandle) {CloseHandle(singletonMutex);singletonMutex=nullptr;return 1;}
  applyOpacity();
  registerShortcuts(windowHandle);
  ShowWindow(windowHandle,SW_SHOW);UpdateWindow(windowHandle);refresh();
  MSG message;while(GetMessageW(&message,nullptr,0,0)>0){TranslateMessage(&message);DispatchMessageW(&message);}return 0;
}
