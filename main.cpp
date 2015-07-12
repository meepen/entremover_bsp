#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define HEADER_LUMPS (64)

struct lump_t
{
	int lump_offset;
	int lump_length;
	int version;
	char fourCC[4];
};

struct dlumpheader_t
{
	int lumpOffset;
	int lumpID;
	int lumpVersion;
	int lumpLength;
	int mapRevision;
};

struct dheader_t
{
	int ident; // VBSP little-endian
	int version;
	lump_t lumps[HEADER_LUMPS];
	int mapRevision;
};

void quit(const char *msg)
{
	printf("%s\n\n", msg);
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
	dheader_t header;
	dlumpheader_t lumpheader;

	printf("%i\n", argc);
	if (argc != 4) quit("Needs three arguments (the file to parsify, the output file, the output lump name)");

	FILE *file;
	fopen_s(&file, argv[1], "rb+");

	if (file == 0) quit("File doesn't exist or something!");

	fseek(file, 0, SEEK_SET);

	if (1 != fread(&header, sizeof(header), 1, file)) quit("file invalid!");

	if (header.ident != 'PSBV') quit("file invalid! (2)");

	// header.lumps[0] entity lump

	lump_t &ents = header.lumps[0];

	fseek(file, ents.lump_offset, SEEK_SET);
	char *ent_lump = new char[ents.lump_length];
	if (ents.lump_length != fread(ent_lump, 1, ents.lump_length, file)) quit("header invalid!");

	FILE *output;
	fopen_s(&output, argv[2], "wb");
	if (output == 0) quit("File output (#2) is not available!");

	fseek(file, 0, SEEK_END);

	long len = ftell(file);

	char *complete_file = new char[len];

	fseek(file, 0, SEEK_SET);
	fread(complete_file, 1, len, file);

	char *find = complete_file + ents.lump_offset;
	for (int i = 0; i < ents.lump_length; i++)
	{
		if (*find++ == '}') break;
	}

	// scary maths inbound

	memset(find, 0, ents.lump_length - (find - (complete_file + ents.lump_offset)));
	((dheader_t *)complete_file)->lumps[0].lump_length = ents.lump_length - (find - (complete_file + ents.lump_offset));

	fwrite(complete_file, len, 1, output);

	FILE *out_lump;
	fopen_s(&out_lump, argv[3], "wb");
	if (out_lump == 0) quit("output lump (#3) is unavailable!");

	lumpheader.lumpID = 0;
	lumpheader.lumpLength = ents.lump_length;
	lumpheader.lumpOffset = sizeof(lumpheader);
	lumpheader.lumpVersion = ents.version;
	lumpheader.mapRevision = header.mapRevision;
	fwrite(&lumpheader, sizeof(lumpheader), 1, out_lump);
	fwrite(ent_lump, ents.lump_length, 1, out_lump);

	fclose(out_lump);
	fclose(file);
	fclose(output);

	delete[] ent_lump;
	delete[] complete_file;

	return 0;
}