#include <gtest/gtest.h>
#include <stb_ds.h>
#include <libsdlang.h>
#include <vector>

// from parser_ast
SdlangTag parse(const std::string& code);
// from parser_basic
std::string toStr(SdlangCharSlice slice);

TEST(Helpers, GetAttribute)
{
	std::string code = "hello to=`the world`";
	SdlangTag tag = parse(code);
	SdlangAttribute* attrib = sdlangTagGetAttribute(tag.children[0], "to");
	EXPECT_NE(attrib, nullptr);
	attrib->value.stringValue.ptr = "Henlo";
	attrib->value.stringValue.length = 5;
	EXPECT_EQ(toStr(tag.children[0].attributes[0].value.stringValue), "Henlo");
	attrib = sdlangTagGetAttribute(tag.children[0], "I don't exist");
	EXPECT_EQ(attrib, nullptr);
}

TEST(Helpers, StringEscaper)
{
	std::string code = "tag \"abc\\\n123\\ndoe\\tray\"";
	SdlangTag tag = parse(code);
	SdlangValue str = tag.children[0].values[0];
	SdlangCharStream stream = {};
	ASSERT_TRUE(sdlangCharStreamFromValue(str, &stream));

	SdlangCharSlice slice;

	ASSERT_TRUE(sdlangCharStreamEscapeNext(&stream, &slice));
	EXPECT_EQ(toStr(slice), "abc");

	ASSERT_TRUE(sdlangCharStreamEscapeNext(&stream, &slice));
	EXPECT_EQ(toStr(slice), "123");

	ASSERT_TRUE(sdlangCharStreamEscapeNext(&stream, &slice));
	EXPECT_EQ(toStr(slice), "\n");

	ASSERT_TRUE(sdlangCharStreamEscapeNext(&stream, &slice));
	EXPECT_EQ(toStr(slice), "doe");

	ASSERT_TRUE(sdlangCharStreamEscapeNext(&stream, &slice));
	EXPECT_EQ(toStr(slice), "\t");

	ASSERT_TRUE(sdlangCharStreamEscapeNext(&stream, &slice));
	EXPECT_EQ(toStr(slice), "ray");

	ASSERT_FALSE(sdlangCharStreamEscapeNext(&stream, &slice));
}

TEST(Helpers, StringEscaperFull)
{
	std::string code = "tag \"abc\\\n123\\ndoe\\tray\"";
	SdlangTag tag = parse(code);
	SdlangValue str = tag.children[0].values[0];
	SdlangCharStream stream = {};
	ASSERT_TRUE(sdlangCharStreamFromValue(str, &stream));

	SdlangCharSlice slice = sdlangCharStreamEscapeFull(stream);
	EXPECT_EQ(toStr(slice), "abc123\ndoe\tray");
	free((void*)slice.ptr);
}