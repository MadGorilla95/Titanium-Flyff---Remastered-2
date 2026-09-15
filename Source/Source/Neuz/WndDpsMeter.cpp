#include "stdafx.h"
#include "WndDpsMeter.h"
#include "CombatLog.h"
#include <functional>

#ifdef __DPS_METER
CWndDpsMeter::CWndDpsMeter() {
	SetDisplay(false);
}
CWndDpsMeter::~CWndDpsMeter() {

}
void CWndDpsMeter::InitDeviceObjects() {
	SetDisplay(false);

	// Setup Job Colors
	memset(dpsColor, 0, sizeof(DWORD) * MAX_JOB);
	dpsColor[JOB_VAGRANT] = 0xff331900;
	dpsColor[JOB_MERCENARY] = dpsColor[JOB_ASSIST] = dpsColor[JOB_ACROBAT] = dpsColor[JOB_MAGICIAN] = 0x96663300;
	dpsColor[JOB_BLADE] = dpsColor[JOB_BLADE_MASTER] = dpsColor[JOB_BLADE_HERO] = dpsColor[JOB_STORMBLADE_HERO] = 0x96ff8000;
	dpsColor[JOB_KNIGHT] = dpsColor[JOB_KNIGHT_MASTER] = dpsColor[JOB_KNIGHT_HERO] = dpsColor[JOB_LORDTEMPLER_HERO] = 0x96cc0000;
	dpsColor[JOB_RINGMASTER] = dpsColor[JOB_RINGMASTER_MASTER] = dpsColor[JOB_RINGMASTER_HERO] = dpsColor[JOB_FLORIST_HERO] = 0x9666cc00;
	dpsColor[JOB_BILLPOSTER] = dpsColor[JOB_BILLPOSTER_MASTER] = dpsColor[JOB_BILLPOSTER_HERO] = dpsColor[JOB_FORCEMASTER_HERO] = 0x96009900;
	dpsColor[JOB_RANGER] = dpsColor[JOB_RANGER_MASTER] = dpsColor[JOB_RANGER_HERO] = dpsColor[JOB_CRACKSHOOTER_HERO] = 0x96009999;
	dpsColor[JOB_JESTER] = dpsColor[JOB_JESTER_MASTER] = dpsColor[JOB_JESTER_HERO] = dpsColor[JOB_WINDLURKER_HERO] = 0x96004c99;
	dpsColor[JOB_ELEMENTOR] = dpsColor[JOB_ELEMENTOR_MASTER] = dpsColor[JOB_ELEMENTOR_HERO] = dpsColor[JOB_ELEMENTORLORD_HERO] = 0x96cc00cc;
	dpsColor[JOB_PSYCHIKEEPER] = dpsColor[JOB_PSYCHIKEEPER_MASTER] = dpsColor[JOB_PSYCHIKEEPER_HERO] = dpsColor[JOB_MENTALIST_HERO] = 0x96cc0066;

	lastToggleTick = 0;
}
void CWndDpsMeter::DeleteDeviceObjects() {

}

void CWndDpsMeter::Render(C2DRender* p2DRender, const int& nWindowHeight, const int& left)
{
	if (!bDisplay) return;
	if (!g_pPlayer) return;

	// Declare Total DPS Counter
	float totalDPS = 0.0f;

	// Setup Map of DPS Players
	dpsMap.clear();
	colorMap.clear();

	auto playerList = CombatLog::GetInstance()->GetObjects();
	for (const auto& entity : playerList) {
		CMover* mvr = prj.GetMover(entity.first);
		if (IsValidObj(mvr))
			if (mvr->IsPlayer()) {
				float dps = CombatLog::GetInstance()->GetPlayerDPS(entity.first);
				totalDPS += dps;
				// Put DPS into map
				dpsMap.insert(make_pair(
					CString(mvr->GetName()),
					dps
				));
				colorMap.insert(make_pair(
					CString(mvr->GetName()),
					dpsColor[mvr->GetJob()]
				));
			}
	}

	// Sort Map
	typedef std::function<bool(std::pair<CString, float>, std::pair<CString, float>)> Compare;

	Compare compFunc = [](std::pair<CString, float> i, std::pair<CString, float> o) {
		return i.second > o.second;
	};

	std::set<std::pair<CString, float>, Compare> sort(dpsMap.begin(), dpsMap.end(), compFunc);

	// Setup Style Variables
	int padding = 10;
	int lineHeight = 20;
	int barWidth = 240;

	int leftPos = left + padding;
	int topPos = nWindowHeight / 2 - 200;

	int meterHeight =
		40
		+ (dpsMap.size() * 20);

	// Setup Background Rendering
	CRect background;
	background.top = topPos - padding;
	background.bottom = topPos + padding + meterHeight;
	background.left = left;
	background.right = left + barWidth + 10;

	p2DRender->RenderFillRect(background, D3DCOLOR_ARGB(150, 0, 0, 0));

	// Show Total DPS
	CString strTotalDPS;
	strTotalDPS.Format("Total DPS: %.2f", totalDPS);
	p2DRender->TextOut(leftPos, topPos + padding, strTotalDPS, 0xffffffff, 0xff000000);

	// Iterate through Map of DPS Players
	auto it = sort.begin();
	for (int i = 0; i < sort.size(); i++) {
		// Setup DPS String
		CString strName, strDps;
		strName.Format("%s", it->first);
		strDps.Format("%0.2f", it->second);

		// Get Color for Player
		DWORD color = D3DCOLOR_ARGB(150, 200, 0, 0);
		auto colorIterator = colorMap.find(it->first);
		if (colorIterator != colorMap.end())
			color = colorIterator->second;


		// Show DPS Bar
		CRect dpsRect;
		dpsRect.left = left + padding;
		dpsRect.top = topPos + padding + (lineHeight * (i + 1));
		dpsRect.bottom = topPos + padding + (lineHeight * (i + 2)) - 2;
		dpsRect.right = left + (int)(barWidth * (1 + (it->second / totalDPS))) - barWidth;
		if (dpsRect.right > left + barWidth)
			dpsRect.right = left + barWidth;

		p2DRender->RenderFillRect(dpsRect, color);
		p2DRender->TextOut(
			leftPos,
			topPos + padding + (lineHeight * (i + 1)),
			strName,
			0xffffffff,
			0xff000000
		);
		p2DRender->TextOut(
			background.right - p2DRender->m_pFont->GetTextExtent(strDps).cx - 13,
			topPos + padding + (lineHeight * (i + 1)),
			strDps,
			0xffffffff,
			0xff000000
		);


		it++;
	}
}
#endif //__DPS_METER