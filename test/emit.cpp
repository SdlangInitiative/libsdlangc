#include <gtest/gtest.h>
#include <stb_ds.h>
#include <libsdlang.h>
#include <vector>

// from parser_basic
std::string toStr(SdlangCharSlice slice);

TEST(Emit, TagWithValue)
{
	SdlangTag root = {};
	SdlangTag child = {};

	child.nspace = SDLANG_CHAR_SLICE("iam");
	child.name = SDLANG_CHAR_SLICE("monkeh");

	SdlangValue value;
	value.type = SDLANG_VALUE_TYPE_STRING;
	value.stringValue = SDLANG_CHAR_SLICE("Henlo!");

	arrput(child.values, value);
	arrput(root.children, child);

	char* slice;
	sdlangEmitToString(root, &slice);
	EXPECT_EQ(std::string(slice), "iam:monkeh `Henlo!` \n\n");
	free(slice);
}

TEST(Emit, TagWithAttribute)
{
	SdlangTag root = {};
	SdlangTag child = {};

	child.nspace = SDLANG_CHAR_SLICE("iam");
	child.name = SDLANG_CHAR_SLICE("monkeh");

	SdlangAttribute attrib;
	attrib.nspace = SDLANG_CHAR_SLICE("meta");
	attrib.name = SDLANG_CHAR_SLICE("species");
	attrib.value.type = SDLANG_VALUE_TYPE_INTEGER;
	attrib.value.intValue = 420;

	arrput(child.attributes, attrib);
	arrput(root.children, child);

	char* slice;
	sdlangEmitToString(root, &slice);
	EXPECT_EQ(std::string(slice), "iam:monkeh meta:species=420 \n\n");
	free(slice);
}

TEST(Emit, TagWithChildren)
{
	SdlangTag root = {};
	SdlangTag child = {};
	SdlangTag childChild = {};

	child.name = SDLANG_CHAR_SLICE("people");

	childChild.name = SDLANG_CHAR_SLICE("Bradley");
	arrput(child.children, childChild);

	childChild.name = SDLANG_CHAR_SLICE("Andy");
	arrput(child.children, childChild);

	arrput(root.children, child);

	char* slice;
	sdlangEmitToString(root, &slice);
	EXPECT_EQ(std::string(slice), "people {\n    Bradley \n    Andy \n}\n\n");
	free(slice);
}