#include "stdafx.h"
#include "XmlWrapperBadge.h"

#include <cctype>


bool IsNumber(const string& s)
{
	return !s.empty() && find_if(s.begin(), s.end(), [](char c) { return !isdigit(c); }) == s.end();
}

bool CXmlWrapper::Load(LPCTSTR lpszFile)
{
	CResFile file;
	if (file.Open(lpszFile, "rb"))
	{
		int len = file.GetLength();
		char *szBuffer = new char[len + 1];
		file.Read(szBuffer, len);
		szBuffer[len] = 0;

		m_path = lpszFile;

		pugi::xml_parse_status ret = m_doc.load_string(szBuffer).status;

		delete[] szBuffer;

		return ret == pugi::xml_parse_status::status_ok;
	}
	return false;
}

bool CXmlWrapper::Save(LPCTSTR lpszPath)
{
	std::string sPath;
	if (lpszPath)
		sPath = lpszPath;
	else
		sPath = m_path;

	return m_doc.save_file(sPath.c_str());
}

bool CXmlWrapper::RemoveAttribute(LPCTSTR lpszFileName, LPCTSTR attr)
{
	//To-Do: Iteration
	pugi::xml_node node = m_doc.child("node");
	pugi::xml_node param = node.child("param");
	param.remove_attribute(attr);

	return m_doc.save_file(lpszFileName);
}

CXmlNodeWrapper CXmlWrapper::AsNode()
{
	return m_doc.root();
}

CXmlNodeWrapper::CXmlNodeWrapper(const pugi::xml_node node)
{
	m_node = node;
}

CXmlNodeWrapper CXmlNodeWrapper::SelectNode(LPCTSTR lpszPath, bool &bFound) const
{
	pugi::xml_node node = m_node.child(lpszPath);
	bFound = !node.empty();
	return node;
}

vector<CXmlNodeWrapper> CXmlNodeWrapper::SelectNodes(LPCTSTR lpszPath) const
{
	auto range = m_node.children(lpszPath);
	vector<CXmlNodeWrapper> nodes;

	auto iter = range.begin();
	while (iter != range.end())
	{
		nodes.push_back(*iter);
		++iter;
	}

	return nodes;
}

DWORD CXmlNodeWrapper::GetHexVal(LPCTSTR lpszPath, DWORD dwDefault) const
{
	pugi::xml_attribute attrib = m_node.attribute(lpszPath);
	if (attrib.empty())
		return dwDefault;

	return strtoul(attrib.value(), nullptr, 16);
}

DWORD CXmlNodeWrapper::GetUIntVal(LPCTSTR lpszPath, DWORD dwDefault) const
{
	pugi::xml_attribute attrib = m_node.attribute(lpszPath);

	if (!IsNumber(attrib.value()))
	{
		auto iter = CScript::m_defines.find(attrib.value());
		if (iter != CScript::m_defines.end())
			return iter->second;

		//Error("%s - %s Path(%s) not defined!", __FUNCTION__, attrib.value(), lpszPath);

		return dwDefault;
	}

	return attrib.as_uint(dwDefault);
}

int CXmlNodeWrapper::GetIntVal(LPCTSTR lpszPath, int nDefault) const
{
	pugi::xml_attribute attrib = m_node.attribute(lpszPath);
	if (attrib.value() == "=")
	{
		attrib = NULL_ID;
		return attrib.as_int(nDefault);
	}

	return attrib.as_int(nDefault);
}

string CXmlNodeWrapper::GetStringVal(LPCTSTR lpszPath, LPCTSTR lpszDefault, bool bLookUp) const
{
	pugi::xml_attribute attrib = m_node.attribute(lpszPath);
	if (attrib.empty())
		return lpszDefault;

	string ret = attrib.value();

	if (bLookUp)
	{
		if (ret.compare(0, 4, "IDS_") == 0)
		{
			auto iter = CScript::m_mapString.find(ret);
			if (iter != CScript::m_mapString.end())
				return iter->second;

			Error("%s - %s not found!", __FUNCTION__, attrib.value());
		}
	}

	return ret;
}

float CXmlNodeWrapper::GetFloatVal(LPCTSTR lpszPath, float fDefault) const
{
	pugi::xml_attribute attrib = m_node.attribute(lpszPath);
	return attrib.as_float(fDefault);
}

bool CXmlNodeWrapper::GetBoolVal(LPCTSTR lpszPath, bool bDefault) const
{
	pugi::xml_attribute attrib = m_node.attribute(lpszPath);
	return attrib.as_bool(bDefault);
}

bool CXmlNodeWrapper::GetRect(CRect & rect) const
{
	bool bFound;
	CXmlNodeWrapper rectNode = SelectNode("Rect", bFound);

	if (bFound)
	{
		rect.left = rectNode.GetIntVal("Left");
		rect.top = rectNode.GetIntVal("Top");
		rect.right = rectNode.GetIntVal("Right");
		rect.bottom = rectNode.GetIntVal("Bottom");
	}

	return bFound;
}

bool CXmlNodeWrapper::GetVector(D3DXVECTOR3 &vec, LPCTSTR lpszChildName) const
{
	bool bFound;
	CXmlNodeWrapper vecNode = SelectNode(lpszChildName, bFound);

	if (bFound)
	{
		vec.x = vecNode.GetFloatVal("x");
		vec.y = vecNode.GetFloatVal("y");
		vec.z = vecNode.GetFloatVal("z");
	}
	return bFound;
}