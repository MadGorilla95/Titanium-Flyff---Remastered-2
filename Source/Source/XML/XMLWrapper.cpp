#include "stdafx.h"
#include "XMLWrapper.h"

XMLWrapper::XMLWrapper(LPCTSTR szFile, LPCTSTR rootNode)
{
	CResFile file;
	if (file.Open(szFile, "rb"))
	{
		int len = file.GetLength();
		char *szBuffer = new char[len + 1];
		file.Read(szBuffer, len);
		szBuffer[len] = 0;

		pugi::xml_parse_status ret = _document.load_string(szBuffer).status;

		delete[] szBuffer;
	}

	_root = _document.child(rootNode);
	for (auto const& node : _root.children())
		_children.push_back(XMLNode(node));
}

const vector<XMLNode>XMLWrapper::GetSpecific(const char* name) {
	vector<XMLNode> returnVec;
	for (pugi::xml_node specific : _root.children(name)) {
		returnVec.push_back(XMLNode(specific));
	}
	const vector<XMLNode> copy = returnVec;
	return copy;
}

const XMLNode XMLWrapper::GetOne(const char* name) {
	CString search = name;
	for (auto node : _root.children())
	{
		if (strcmp(node.name(), search) == 0)
			return XMLNode(node);
	}
}


XMLWrapper::~XMLWrapper()
{
	_children.clear();
}

XMLNode::XMLNode(pugi::xml_node rootNode)
{
	_root = rootNode;
}

XMLNode::~XMLNode()
{
	_children.clear();
}

vector<XMLNode> XMLNode::GetChildren(const char* name)
{
	_children.clear();
	for (auto const& node : _root.children(name))
		_children.push_back(node);

	return _children;
}

XMLNode XMLNode::SelectChild(const char* name)
{
	return XMLNode(_root.child(name));
}

// Basic Types
int	XMLNode::GetInt(const char* attr, int defaultValue) const
{
	return _root.attribute(attr).as_int(defaultValue);
}
CString	XMLNode::GetString(const char* attr) const
{
	CString str;
	str.Format("%s", _root.attribute(attr).as_string());
	return str;
}
double XMLNode::GetFloat(const char* attr, float defaultValue) const
{
	return _root.attribute(attr).as_double(defaultValue);
}
vector<int> XMLNode::GetVector(bool& bFound)
{
	vector<XMLNode> ChildVector = GetChildren("Vector");
	if (ChildVector.size() == 0)
	{
		bFound = false;
		Error("XMLNode::GetVector - Invalid Call - No Child Element Found");
	}

	auto Vector = ChildVector[0];
	return vector<int>{Vector.GetInt("x"), Vector.GetInt("y"), Vector.GetInt("z")};
}
// Flyff Specific
DWORD XMLNode::GetID(const char* attr, bool& bFound, DWORD defaultValue) const
{
	bFound = false;

	auto Define = CScript::m_defines.find(_root.attribute(attr).value());
	if (Define != CScript::m_defines.end())
	{
		bFound = true;
		return Define->second;
	}

	return this->GetInt(attr,defaultValue);
}

/*
CString XMLNode::GetName(const char* attr, bool& bFound) const
{
	bFound = false;

	pugi::xml_attribute _attr = _root.attribute(attr);
	CString ret = _attr.value();

	auto iter = CScript::m_mapString.find(ret);
	if (iter != CScript::m_mapString.end())
	{
		bFound = true;
		return iter->second;
	}

	return ret;
}
*/

BOOL XMLNode::GetBOOL(const char* attr) {
	if (strcmp(this->GetString(attr), "true") == 0 || strcmp(this->GetString(attr), "TRUE") == 0 || this->GetInt(attr) == 1)
		return TRUE;

	return FALSE;
}

bool XMLNode::GetBool(const char* attr) {
	return _root.attribute(attr).as_bool(false);
}

DWORD XMLNode::GetDW(const char* attr, DWORD default) const {
	return (DWORD)(_root.attribute(attr).as_llong(default));
}

// Node Text Methods

int XMLNode::GetValue_Int(int defaultValue) const {
	return (int)(_root.text().as_int(defaultValue));
}

CString XMLNode::GetValue_String() const {
	CString retVal;
	retVal.Format("%s", _root.text().as_string());
	return retVal;
}

float XMLNode::GetValue_Float() const {
	return _root.text().as_float();
}