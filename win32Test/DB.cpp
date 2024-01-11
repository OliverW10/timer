#include "DB.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>


char* ReadSchemaFromFile() {
	char* buffer = 0;
	long length;
	FILE* f = fopen("schema.sql", "r");

	if (f)
	{
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		buffer = (char*)malloc(length);
		if (buffer)
		{
			fread(buffer, 1, length, f);
		}
		fclose(f);
	}
	
	return buffer; // will leak but i dont care
}


const char* DB_NAME = "sessionsDb.db";

int callback(void* outNumRows, int numColumns, char** columns, char** results){
	*((int*)outNumRows) += 1; 
	return 0;
}

int SaveSession(wchar_t* appName, double timeSeconds) {
	sqlite3* db;
	int wasOpenSuccessful = sqlite3_open(DB_NAME, &db);
	if (wasOpenSuccessful != SQLITE_OK) {
		// message box?
		return -1;
	}

	int numRows = 0;
	sqlite3_exec(db, "SELECT * FROM sqlite_master WHERE type = 'table';", &callback, &numRows, NULL);
	if (numRows == 0) {
		sqlite3_exec(db, ReadSchemaFromFile(), NULL, NULL, NULL);
	}

	

	sqlite3_exec(db, sqlite3_mprintf("INSERT INTO Session "), NULL, NULL, NULL);

	sqlite3_close(db);
}