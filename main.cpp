#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS
#define MAX_LINE 1024
#pragma warning(disable:4996)

#define STORE_FILE "D:\\ProgramingEnvironment\\UsefulExe\\CopyToClipboard\\CTCku.txt"

void SetClipLTY(const std::string& str)
{
	char* clipBoardData = (char*)malloc(str.length() + 1);
	if (clipBoardData == NULL) {
		printf("malloc failed!\n");
		return;
	}
	strcpy(clipBoardData, str.c_str());

	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, clipBoardData);
	CloseClipboard();
	free(clipBoardData);
}

void Usage() {
	printf("Usage:\n");
	printf("prog -a/-m/-d/--key\n");
	printf("-a: add a key-value map\n");
	printf("-m: modify the value of key\n");
	printf("-d: delete the key-value map\n");
	printf("--key: copy the value of this key to the clipboard\n");
}


std::string QueryMap(char* key)
{
	if (*key == '\0') {
		return "";
	}
	FILE* fp = fopen(STORE_FILE, "r");
	if (fp == NULL) {
		printf("open file %s failed!!!\n", STORE_FILE);
		return "";
	}
	char Line[MAX_LINE] = { 0 };
	while (fgets(Line, MAX_LINE, fp) != NULL) {
		Line[strlen(Line) - 1] = '\0';
		char* firstPos = strstr(Line, " ");
		if (firstPos != NULL) {
			*firstPos = '\0';
			if (strcmp(Line, key) == 0) {
				fclose(fp);
				return std::string(firstPos + 1);
			}
		}
	}
	fclose(fp);
	return "";
}

void AddMap(char* key, char* value)
{
	if (!QueryMap(key).empty()) {
		printf("the key already exists, please use -m to modify\n");
		return;
	}
	FILE* fp = fopen(STORE_FILE, "a");
	if (fp == NULL) {
		printf("open file %s failed!!!\n", STORE_FILE);
		return;
	}
	fprintf(fp, "%s %s\n", key, value);
	fclose(fp);
}

void ModifyMap(char* key, char* value)
{
	if (QueryMap(key).empty()) {
		AddMap(key, value);
		return;
	}
	FILE* src = fopen(STORE_FILE, "r");
	if (src == NULL) {
		printf("open file %s failed!!!\n", STORE_FILE);
		return;
	}
	FILE* dst = fopen(STORE_FILE".tmp", "w");
	if (dst == NULL) {
		printf("open file %s failed!!!\n", STORE_FILE".tmp");
		fclose(src);
		return;
	}
	char Line[MAX_LINE];
	while (fgets(Line, MAX_LINE, src)) {
		char* firstPos = strstr(Line, " ");
		if (firstPos != NULL) {
			*firstPos = '\0';
			if (strcmp(Line, key) == 0) {
				fprintf(dst, "%s %s\n", key, value);
			}
			else {
				*firstPos = ' ';
				fprintf(dst, "%s", Line);
			}
		}
	}
	fclose(src);
	fclose(dst);
	remove(STORE_FILE);
	rename(STORE_FILE".tmp", STORE_FILE);
}

void DelMap(char* key)
{
	if (QueryMap(key).empty()) {
		return;
	}
	FILE* src = fopen(STORE_FILE, "r");
	if (src == NULL) {
		printf("open file %s failed!!!\n", STORE_FILE);
		return;
	}
	FILE* dst = fopen(STORE_FILE".tmp", "w");
	if (dst == NULL) {
		printf("open file %s failed!!!\n", STORE_FILE".tmp");
		return;
	}
	char Line[MAX_LINE];
	while (fgets(Line, MAX_LINE, src)) {
		char* firstPos = strstr(Line, " ");
		if (firstPos != NULL) {
			*firstPos = '\0';
			if (strcmp(Line, key) != 0) {
				*firstPos = ' ';
				fprintf(dst, "%s", Line);
			}
		}
	}
	fclose(src);
	fclose(dst);
	remove(STORE_FILE);
	rename(STORE_FILE".tmp", STORE_FILE);
}

void ShowMapKeys()
{
	FILE* fp = fopen(STORE_FILE, "r");
	if (fp == NULL) {
		printf("open file %s failed!!!\n", STORE_FILE);
		return ;
	}
	char Line[MAX_LINE] = { 0 };
	while (fgets(Line, MAX_LINE, fp) != NULL) {
		char* firstPos = strstr(Line, " ");
		if (firstPos != NULL) {
			*firstPos = '\0';
			printf("%s\n", Line);
		}
	}
	fclose(fp);
}

int main(int argc, char **argv)
{
	if (argc < 2 || argv[1][0] != '-') {
		Usage();
		return 1;
	}
	if (argv[1][1] == 'a' && argv[1][2] == '\0' && argc > 3) {
		AddMap(argv[2], argv[3]);
	}
	if (argv[1][1] == 'm' && argv[1][2] == '\0' && argc > 3) {
		ModifyMap(argv[2], argv[3]);
	}
	if (argv[1][1] == 'd' && argv[1][2] == '\0' && argc > 2) {
		DelMap(argv[2]);
	}
	if (argv[1][1] == 'h' && argv[1][2] == '\0') {
		ShowMapKeys();
	}
	if (argv[1][1] == '-') {
		std::string tmp = QueryMap(&argv[1][2]);
		if (!tmp.empty()) {
			printf("%s\nsuccess copy to clipboard\n", tmp.c_str());
			SetClipLTY(tmp);
		}
	}
	return 0;
}
