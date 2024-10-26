#pragma once
//////////////////////////////////////////////////////////////////////
// The Force Engine System Library
// System functionality, such as timers and logging.
//////////////////////////////////////////////////////////////////////

#include "types.h"
#include <vector>
#include <string>

typedef std::vector<std::string> TokenList;

class TFE_Parser
{
public:
	TFE_Parser();
	~TFE_Parser();

	void init(const char* buffer, size_t len);

	// Enable block comments of the form /*...*/
	void enableBlockComments();

	// Enable : as a seperator but do not remove it.
	void enableColonSeperator();

	// Add a string representing a comment, such as ";" "#" "//"
	void addCommentString(const char* comment);

	// Convert resulting strings to upper case, defaults to false.
	void convertToUpperCase(bool enable);

	// Read the next non-comment/whitespace line.
	const char* readLine(size_t& bufferPos, bool skipLeadingWhitespace = false, bool commentOnlyAtBeginning = false);
	// Split a line into tokens using space, comma or equals as separators.
	// Note strings with spaces still work, they need to be closed in quotes, which are removed upon tokenizing.
	void tokenizeLine(const char* line, TokenList& tokens);

private:
	const char* m_buffer;
	size_t m_bufferLen;
	TokenList m_commentStrings;
	bool m_enableBlockComments;
	bool m_blockComment;
	bool m_enableColonSeperator;
	bool m_convertToUppercase;

private:
	bool isComment(const char* buffer);
};
