#include <gtest/gtest.h>
#include <libsdlang.h>
#include <vector>

std::vector<SdlangToken> getTokens(const std::string& str)
{
	std::vector<SdlangToken> tokens;

	SdlangCharStream stream = {
		str.c_str(),
		str.length()
	};
	SdlangParser parser = { stream };

	SdlangError _1;
	SdlangCharSlice _2, _3;
	while (parser.front.type != SDLANG_TOKEN_TYPE_EOF)
	{
		sdlangParserNext(&parser, &_1, &_2, &_3);
		tokens.push_back(parser.front);
	}

	return tokens;
}

std::string toStr(SdlangCharSlice slice)
{
	return std::string(slice.ptr, slice.length);
}

TEST(ParserBasic, TagName)
{
	const std::string code = "abc";
	const auto tokens = getTokens(code);
	EXPECT_EQ(tokens.size(), 2);
	EXPECT_EQ(tokens[0].type, SDLANG_TOKEN_TYPE_TAG_NAME);
	EXPECT_EQ(tokens[1].type, SDLANG_TOKEN_TYPE_EOF);
	EXPECT_EQ(std::string(tokens[0].name.ptr), "abc");
}

TEST(ParserBasic, TagNameWithNamespace)
{
	const std::string code = "abc:onetwothree";
	const auto tokens = getTokens(code);
	EXPECT_EQ(tokens.size(), 2);
	EXPECT_EQ(tokens[0].type, SDLANG_TOKEN_TYPE_TAG_NAME);
	EXPECT_EQ(tokens[1].type, SDLANG_TOKEN_TYPE_EOF);
	EXPECT_EQ(toStr(tokens[0].nspace), "abc");
	EXPECT_EQ(toStr(tokens[0].name), "onetwothree");
}

TEST(ParserBasic, ChildStartEnd)
{
	const std::string code = "t {\n}";
	const auto tokens = getTokens(code);
	EXPECT_EQ(tokens.size(), 4);
	EXPECT_EQ(tokens[0].type, SDLANG_TOKEN_TYPE_TAG_NAME);
	EXPECT_EQ(tokens[1].type, SDLANG_TOKEN_TYPE_CHILDREN_START);
	EXPECT_EQ(tokens[2].type, SDLANG_TOKEN_TYPE_CHILDREN_END);
	EXPECT_EQ(tokens[3].type, SDLANG_TOKEN_TYPE_EOF);
}

TEST(ParserBasic, ValueString)
{
	const std::string code = "\"abc\"";
	const auto tokens = getTokens(code);
	EXPECT_EQ(tokens.size(), 3);
	EXPECT_EQ(tokens[0].type, SDLANG_TOKEN_TYPE_TAG_NAME);
	EXPECT_EQ(tokens[1].type, SDLANG_TOKEN_TYPE_VALUE_STRING);
	EXPECT_EQ(tokens[2].type, SDLANG_TOKEN_TYPE_EOF);
	EXPECT_EQ(toStr(tokens[1].stringValue), "abc");
}

TEST(ParserBasic, ValueStringNewLine)
{
	const std::string code = "\"ab\\\nc\"";
	const auto tokens = getTokens(code);
	EXPECT_EQ(tokens.size(), 3);
	EXPECT_EQ(tokens[0].type, SDLANG_TOKEN_TYPE_TAG_NAME);
	EXPECT_EQ(tokens[1].type, SDLANG_TOKEN_TYPE_VALUE_STRING);
	EXPECT_EQ(tokens[2].type, SDLANG_TOKEN_TYPE_EOF);
	EXPECT_EQ(toStr(tokens[1].stringValue), "ab\\\nc");
}

TEST(ParserBasic, ValueStringWYSIWYG)
{
	const std::string code = "`abc\n123`";
	const auto tokens = getTokens(code);
	EXPECT_EQ(tokens.size(), 3);
	EXPECT_EQ(tokens[0].type, SDLANG_TOKEN_TYPE_TAG_NAME);
	EXPECT_EQ(tokens[1].type, SDLANG_TOKEN_TYPE_VALUE_STRING);
	EXPECT_EQ(tokens[2].type, SDLANG_TOKEN_TYPE_EOF);
	EXPECT_EQ(toStr(tokens[1].stringValue), "abc\n123");
}

TEST(ParserBasic, ValueInteger)
{
	const std::string code = "123";
	const auto tokens = getTokens(code);
	EXPECT_EQ(tokens.size(), 3);
	EXPECT_EQ(tokens[0].type, SDLANG_TOKEN_TYPE_TAG_NAME);
	EXPECT_EQ(tokens[1].type, SDLANG_TOKEN_TYPE_VALUE_INTEGER);
	EXPECT_EQ(tokens[2].type, SDLANG_TOKEN_TYPE_EOF);
	EXPECT_EQ(tokens[1].intValue, 123);
}

TEST(ParserBasic, ValueIntegerLong)
{
	const std::string code = "123L";
	const auto tokens = getTokens(code);
	EXPECT_EQ(tokens.size(), 3);
	EXPECT_EQ(tokens[0].type, SDLANG_TOKEN_TYPE_TAG_NAME);
	EXPECT_EQ(tokens[1].type, SDLANG_TOKEN_TYPE_VALUE_INTEGER);
	EXPECT_EQ(tokens[2].type, SDLANG_TOKEN_TYPE_EOF);
	EXPECT_EQ(tokens[1].intValue, 123);
}

TEST(ParserBasic, ValueIntegerNegative)
{
	const std::string code = "-123";
	const auto tokens = getTokens(code);
	EXPECT_EQ(tokens.size(), 3);
	EXPECT_EQ(tokens[0].type, SDLANG_TOKEN_TYPE_TAG_NAME);
	EXPECT_EQ(tokens[1].type, SDLANG_TOKEN_TYPE_VALUE_INTEGER);
	EXPECT_EQ(tokens[2].type, SDLANG_TOKEN_TYPE_EOF);
	EXPECT_EQ(tokens[1].intValue, -123);
}

TEST(ParserBasic, ValueFloating)
{
	const std::string code = "1.23";
	const auto tokens = getTokens(code);
	EXPECT_EQ(tokens.size(), 3);
	EXPECT_EQ(tokens[0].type, SDLANG_TOKEN_TYPE_TAG_NAME);
	EXPECT_EQ(tokens[1].type, SDLANG_TOKEN_TYPE_VALUE_FLOATING);
	EXPECT_EQ(tokens[2].type, SDLANG_TOKEN_TYPE_EOF);
	EXPECT_EQ(tokens[1].floatValue, 1.23);
}

TEST(ParserBasic, ValueFloatingNegative)
{
	const std::string code = "-1.23";
	const auto tokens = getTokens(code);
	EXPECT_EQ(tokens.size(), 3);
	EXPECT_EQ(tokens[0].type, SDLANG_TOKEN_TYPE_TAG_NAME);
	EXPECT_EQ(tokens[1].type, SDLANG_TOKEN_TYPE_VALUE_FLOATING);
	EXPECT_EQ(tokens[2].type, SDLANG_TOKEN_TYPE_EOF);
	EXPECT_EQ(tokens[1].floatValue, -1.23);
}

TEST(ParserBasic, ValueBoolean)
{
	const std::string code = "tag true false on off";
	const auto tokens = getTokens(code);
	EXPECT_EQ(tokens.size(), 6);
	EXPECT_EQ(tokens[0].type, SDLANG_TOKEN_TYPE_TAG_NAME);
	EXPECT_EQ(tokens[1].type, SDLANG_TOKEN_TYPE_VALUE_BOOLEAN);
	EXPECT_EQ(tokens[2].type, SDLANG_TOKEN_TYPE_VALUE_BOOLEAN);
	EXPECT_EQ(tokens[3].type, SDLANG_TOKEN_TYPE_VALUE_BOOLEAN);
	EXPECT_EQ(tokens[4].type, SDLANG_TOKEN_TYPE_VALUE_BOOLEAN);
	EXPECT_EQ(tokens[5].type, SDLANG_TOKEN_TYPE_EOF);
    EXPECT_TRUE(tokens[1].boolValue);
    EXPECT_FALSE(tokens[2].boolValue);
    EXPECT_TRUE(tokens[3].boolValue);
    EXPECT_FALSE(tokens[4].boolValue);
}

TEST(ParserBasic, ValueDate)
{
	const std::string code = "1111/22/33";
	const auto tokens = getTokens(code);
	EXPECT_EQ(tokens.size(), 3);
	EXPECT_EQ(tokens[0].type, SDLANG_TOKEN_TYPE_TAG_NAME);
	EXPECT_EQ(tokens[1].type, SDLANG_TOKEN_TYPE_VALUE_DATE);
	EXPECT_EQ(tokens[2].type, SDLANG_TOKEN_TYPE_EOF);
	EXPECT_EQ(tokens[1].dateValue.year, 1111);
	EXPECT_EQ(tokens[1].dateValue.month, 22);
	EXPECT_EQ(tokens[1].dateValue.day, 33);
}

TEST(ParserBasic, ValueTimeSpan)
{
	const std::string code = "11:22:33";
	const auto tokens = getTokens(code);
	EXPECT_EQ(tokens.size(), 3);
	EXPECT_EQ(tokens[0].type, SDLANG_TOKEN_TYPE_TAG_NAME);
	EXPECT_EQ(tokens[1].type, SDLANG_TOKEN_TYPE_VALUE_TIMESPAN);
	EXPECT_EQ(tokens[2].type, SDLANG_TOKEN_TYPE_EOF);
	EXPECT_EQ(tokens[1].timeSpanValue.hours, 11);
	EXPECT_EQ(tokens[1].timeSpanValue.minutes, 22);
	EXPECT_EQ(tokens[1].timeSpanValue.seconds, 33);
}

TEST(ParserBasic, ValueTimeSpanDays)
{
	const std::string code = "44d:11:22:33";
	const auto tokens = getTokens(code);
	EXPECT_EQ(tokens.size(), 3);
	EXPECT_EQ(tokens[0].type, SDLANG_TOKEN_TYPE_TAG_NAME);
	EXPECT_EQ(tokens[1].type, SDLANG_TOKEN_TYPE_VALUE_TIMESPAN);
	EXPECT_EQ(tokens[2].type, SDLANG_TOKEN_TYPE_EOF);
	EXPECT_EQ(tokens[1].timeSpanValue.hours, 11);
	EXPECT_EQ(tokens[1].timeSpanValue.minutes, 22);
	EXPECT_EQ(tokens[1].timeSpanValue.seconds, 33);
	EXPECT_EQ(tokens[1].timeSpanValue.days, 44);
}

TEST(ParserBasic, ValueDateTime)
{
	const std::string code = "1111/22/33 11:22:33";
	const auto tokens = getTokens(code);
	EXPECT_EQ(tokens.size(), 3);
	EXPECT_EQ(tokens[0].type, SDLANG_TOKEN_TYPE_TAG_NAME);
	EXPECT_EQ(tokens[1].type, SDLANG_TOKEN_TYPE_VALUE_DATETIME);
	EXPECT_EQ(tokens[2].type, SDLANG_TOKEN_TYPE_EOF);
    EXPECT_EQ(tokens[1].dateTimeValue.date.year, 1111);
    EXPECT_EQ(tokens[1].dateTimeValue.date.month, 22);
    EXPECT_EQ(tokens[1].dateTimeValue.date.day, 33);
	EXPECT_EQ(tokens[1].dateTimeValue.time.hours, 11);
	EXPECT_EQ(tokens[1].dateTimeValue.time.minutes, 22);
	EXPECT_EQ(tokens[1].dateTimeValue.time.seconds, 33);
}

TEST(ParserBasic, ValueNull)
{
	const std::string code = "tag null";
	const auto tokens = getTokens(code);
	EXPECT_EQ(tokens.size(), 3);
	EXPECT_EQ(tokens[0].type, SDLANG_TOKEN_TYPE_TAG_NAME);
	EXPECT_EQ(tokens[1].type, SDLANG_TOKEN_TYPE_VALUE_NULL);
	EXPECT_EQ(tokens[2].type, SDLANG_TOKEN_TYPE_EOF);
}