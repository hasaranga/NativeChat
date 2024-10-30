
#pragma once

#pragma comment(lib,"sqlite3.lib")

#include "sqlite3/sqlite3.h"
#include <string.h>
#include <windows.h>

class VectorDB
{
    sqlite3* db;
public:
    VectorDB(const char* vecExtPath, const char* dbFilePath)
    {
        sqlite3_open(dbFilePath, &db);
        sqlite3_enable_load_extension(db, 1);
        if (sqlite3_load_extension(db, vecExtPath, 0, 0) != SQLITE_OK)
        {
            ::MessageBoxW(0, L"vector extension load failed!", L"Error", MB_ICONERROR);
        }
    }

    ~VectorDB()
    {
        sqlite3_close(db);
    }

    void CreateTables()
    {
        // distance_metric=cosine can be used. but we use default L2 in here.
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, "CREATE VIRTUAL TABLE IF NOT EXISTS vec_items USING vec0(embedding float[768])",
            -1, &stmt, NULL);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS meta_data (rowid INTEGER PRIMARY KEY, content TEXT NOT NULL);", NULL, NULL, NULL);
    }

    void ResetDB()
    {
        sqlite3_exec(db, "DROP TABLE meta_data", NULL, NULL, NULL);
        sqlite3_exec(db, "DROP TABLE vec_items", NULL, NULL, NULL);

        this->CreateTables();
    }

    // call before inserting rows
    void BeginInsert()
    {
        sqlite3_exec(db, "BEGIN", NULL, NULL, NULL);
    }

    // vector must be 768 float array
    void Insert(int rowid, float* vector, const char* metaData)
    {
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, "INSERT INTO vec_items(rowid, embedding) VALUES (?, ?)", -1, &stmt, NULL);
        sqlite3_bind_int64(stmt, 1, rowid);
        sqlite3_bind_blob(stmt, 2, vector, 768 * sizeof(float), SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        sqlite3_prepare_v2(db, "INSERT INTO meta_data(rowid, content) VALUES (?, ?)", -1, &stmt, NULL);
        sqlite3_bind_int(stmt, 1, rowid);
        sqlite3_bind_text(stmt, 2, metaData, (int)strlen(metaData), SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);   
    }

    // save inserted rows
    void EndInsert()
    {
        sqlite3_exec(db, "COMMIT", NULL, NULL, NULL);
    }

    int GetNextRowID()
    {
        int nextRowID = 1;
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db,
            "SELECT rowid FROM meta_data ORDER BY rowid DESC"
            , -1, &stmt, NULL);

        if (sqlite3_step(stmt) == SQLITE_ROW)
            nextRowID = sqlite3_column_int(stmt, 0) + 1;

        sqlite3_finalize(stmt);
        return nextRowID;
    }

    // returns combined meta data for the most matching 2 vectors. NULL if not found.
    // must free the returned string.
    char* Query(float* vector)
    {
        int res1RowID = -1;
        int res2RowID = -1;

        char* res1Text = NULL;
        char* res2Text = NULL;

        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db,
            "SELECT "
            "  rowid, "
            "  distance "
            "FROM vec_items "
            "WHERE embedding MATCH ?1 "
            "ORDER BY distance "
            "LIMIT 2 "
            , -1, &stmt, NULL);

        sqlite3_bind_blob(stmt, 1, vector, 768 * sizeof(float), SQLITE_TRANSIENT);

        // get the first record.
        if (sqlite3_step(stmt) == SQLITE_ROW)
            res1RowID = (int)sqlite3_column_int64(stmt, 0);

        // get the seconds record.
        if (sqlite3_step(stmt) == SQLITE_ROW)
            res2RowID = (int)sqlite3_column_int64(stmt, 0);

        sqlite3_finalize(stmt);

        // get meta data for first record
        if (res1RowID != -1)
        {
            sqlite3_prepare_v2(db,
                "SELECT "
                "  rowid, "
                "  content "
                "FROM meta_data "
                "WHERE rowid = ?1 "
                , -1, &stmt, NULL);

            sqlite3_bind_int(stmt, 1, res1RowID);

            if (sqlite3_step(stmt) == SQLITE_ROW)
                res1Text = _strdup((char*)sqlite3_column_text(stmt, 1));

            sqlite3_finalize(stmt);
        }

        // get meta data for second record
        if (res2RowID != -1)
        {
            sqlite3_prepare_v2(db,
                "SELECT "
                "  rowid, "
                "  content "
                "FROM meta_data "
                "WHERE rowid = ?1 "
                , -1, &stmt, NULL);

            sqlite3_bind_int(stmt, 1, res2RowID);

            if (sqlite3_step(stmt) == SQLITE_ROW)
                res2Text = _strdup((char*)sqlite3_column_text(stmt, 1));

            sqlite3_finalize(stmt);
        }

        if ((res1Text != NULL) && (res2Text != NULL))
        {
            // combine two strings.
            const size_t totalSize = strlen(res1Text) + strlen(res2Text) + 2;
            char* combinedStr = (char*)malloc(totalSize);
            strcpy_s(combinedStr, totalSize, res1Text);
            strcat_s(combinedStr, totalSize, " ");
            strcat_s(combinedStr, totalSize, res2Text);
            free(res1Text);
            free(res2Text);
            return combinedStr;
        }
        else
        {
            return res1Text;
        }
    }
};

