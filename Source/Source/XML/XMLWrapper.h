#pragma once
#include "../XML/pugixml.hpp"
#include <vector>

class XMLNode
{
public:
	XMLNode(pugi::xml_node rootNode);
	~XMLNode();

	// Basic Types
	int			GetInt(const char* attr, int defaultValue = NULL_ID) const;
	CString		GetString(const char* attr) const;
	double		GetFloat(const char* attr, float defaultValue = 1.0f) const;
	vector<int>	GetVector(bool& bFound);
	// Flyff Specific
	DWORD		GetID(const char* attr, bool& bFound, DWORD defaultValue = NULL_ID) const;
	//CString		GetName(const char* attr, bool& bFound) const;
	BOOL		GetBOOL(const char* attr);
	bool		GetBool(const char* attr);
	DWORD		GetDW(const char* attr, DWORD defaultValue = NULL_ID) const;
	// Node Texts
	int			GetValue_Int(int defaultValue = 0) const;
	CString		GetValue_String() const;
	float		GetValue_Float() const;
	// Dig Deeper
	vector<XMLNode> GetChildren(const char* name);
	XMLNode SelectChild(const char* name);

private:
	pugi::xml_node	_root;
	vector<XMLNode>	_children;
};

class XMLWrapper
{
public:
	XMLWrapper(LPCTSTR szFile, LPCTSTR rootNode);
	~XMLWrapper();

	vector<XMLNode>		Get() { return _children; }
	size_t				GetNum() { return _children.size(); }
	const vector<XMLNode>		GetSpecific(const char* name);
	const XMLNode		GetOne(const char* name);

private:
	pugi::xml_document	_document;
	pugi::xml_node		_root;
	vector<XMLNode>		_children;
};