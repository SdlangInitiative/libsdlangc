#include <gtest/gtest.h>
#include <stb_ds.h>
#include <libsdlang.h>
#include <vector>

std::string toStr(SdlangCharSlice slice);

SdlangTag parse(const std::string& code)
{
	SdlangCharStream stream = { code.c_str(), code.size() };
	SdlangError error;
	SdlangCharSlice errorLine, errorSlice;
	SdlangTag tag = {};
	
	const bool parsed = sdlangParseCharStream(stream, &tag, &error, &errorLine, &errorSlice);
	EXPECT_TRUE(parsed) << std::string(error) << "\nLine: " << toStr(errorLine);

	return tag;
}

TEST(Ast, TagNoValues)
{
	std::string code = "tag\n";
	SdlangTag tag = parse(code);
	ASSERT_EQ(arrlen(tag.children), 1);
	EXPECT_EQ(toStr(tag.children[0].name), "tag");
}

TEST(Ast, Tags)
{
	std::string code = "tag \ndag";
	SdlangTag tag = parse(code);
	ASSERT_EQ(arrlen(tag.children), 2);
	EXPECT_EQ(toStr(tag.children[0].name), "tag");
	EXPECT_EQ(toStr(tag.children[1].name), "dag");
}

TEST(Ast, TagValues)
{
	std::string code = "tag 123 true";
	SdlangTag tag = parse(code);
	ASSERT_EQ(arrlen(tag.children[0].values), 2);
	EXPECT_EQ(tag.children[0].values[0].intValue, 123);
	EXPECT_TRUE(tag.children[0].values[0].boolValue);
}

TEST(Ast, TagAttribs)
{
	std::string code = "tag date=1111/22/33 meta:msg=`Henlo`";
	SdlangTag tag = parse(code);
	ASSERT_EQ(arrlen(tag.children[0].attributes), 2);
	EXPECT_EQ(toStr(tag.children[0].attributes[1].nspace), "meta");
	EXPECT_EQ(toStr(tag.children[0].attributes[1].name), "msg");
	EXPECT_EQ(toStr(tag.children[0].attributes[1].value.stringValue), "Henlo");
}

TEST(Ast, TagChildren)
{
    std::string code = "parent {\nchild 123\n}";
    SdlangTag tag = parse(code);
    ASSERT_EQ(arrlen(tag.children), 1);
    ASSERT_EQ(arrlen(tag.children[0].children), 1);
    EXPECT_EQ(toStr(tag.children[0].name), "parent");
    EXPECT_EQ(toStr(tag.children[0].children[0].name), "child");
    EXPECT_EQ(tag.children[0].children[0].values[0].intValue, 123);
}