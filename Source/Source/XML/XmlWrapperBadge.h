#pragma once
#include "..\XML\pugixml.hpp"

class CXmlNodeWrapper
{
private:
	pugi::xml_node m_node;
public:
	CXmlNodeWrapper(const pugi::xml_node node);

	CXmlNodeWrapper SelectNode(LPCTSTR lpszPath, bool &bFound) const;
	vector<CXmlNodeWrapper> SelectNodes(LPCTSTR lpszPath) const;

	DWORD GetHexVal(LPCTSTR lpszPath, DWORD dwDefault = 0) const;
	DWORD GetUIntVal(LPCTSTR lpszPath, DWORD dwDefault = 0) const;
	int GetIntVal(LPCTSTR lpszPath, int nDefault = 0) const;
	string GetStringVal(LPCTSTR lpszPath, LPCTSTR lpszDefault = "", bool bLookUp = true) const;
	float GetFloatVal(LPCTSTR lpszPath, float fDefault = 0.0f) const;
	bool GetBoolVal(LPCTSTR lpszPath, bool bDefault = false) const;
	bool GetRect(CRect &rect) const;
	bool GetVector(D3DXVECTOR3 &vec, LPCTSTR lpszChildName = "Vector") const;
};

class CXmlWrapper
{
private:
	pugi::xml_document m_doc;
	std::string m_path;

public:
	bool Load(LPCTSTR lpszFileName);
	bool Save(LPCTSTR lpszPath = nullptr);
	bool RemoveAttribute(LPCTSTR lpszFileName, LPCTSTR attr);

	CXmlNodeWrapper AsNode();
};