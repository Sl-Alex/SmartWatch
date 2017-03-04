package ua.com.slalex.smcenter.data;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import java.util.ArrayList;

import ua.com.slalex.smcenter.data.FiltersContract.FilterEntry;


public class FiltersDbHelper extends SQLiteOpenHelper {

    private static final String DATABASE_NAME = "filters.db";
    private static final int DATABASE_VERSION = 1;

    private static final String where_complete =
            FilterEntry.COLUMN_FTYPE + "=? AND " +
            FilterEntry.COLUMN_FNAME + "=? AND " +
            FilterEntry.COLUMN_FTEXT + "=?";

    public FiltersDbHelper(Context context) {
        super(context, DATABASE_NAME, null, DATABASE_VERSION);
    }

    @Override
    public void onCreate(SQLiteDatabase sqLiteDatabase) {
        // Create tables
        String SQL_CREATE_FILTERS_TABLE = "CREATE TABLE " + FilterEntry.TABLE_NAME + " ("
                + FilterEntry._ID + " INTEGER PRIMARY KEY AUTOINCREMENT, "
                + FilterEntry.COLUMN_FTYPE + " INTEGER NOT NULL, "
                + FilterEntry.COLUMN_FNAME + " TEXT NOT NULL, "
                + FilterEntry.COLUMN_FTEXT + " TEXT NOT NULL );";
        // Запускаем создание таблицы
        sqLiteDatabase.execSQL(SQL_CREATE_FILTERS_TABLE);
    }

    @Override
    public void onUpgrade(SQLiteDatabase sqLiteDatabase, int i, int i1) {

    }

    public int getNumberOfFilters(int filterType) {
        SQLiteDatabase db = this.getReadableDatabase();

        // Query number of filters by type
        Cursor cursor = db.query(
        FilterEntry.TABLE_NAME,
        null,
        FilterEntry.COLUMN_FTYPE + " = " + filterType,
        null,
        null,
        null,
        null);

        return cursor.getCount();
    }

    public ArrayList<FilterRecord> getFilters(int filterType) {

        ArrayList<FilterRecord> array = new ArrayList<>();

        SQLiteDatabase db = this.getReadableDatabase();
        String[] columns = {FilterEntry.COLUMN_FTYPE, FilterEntry.COLUMN_FNAME, FilterEntry.COLUMN_FTEXT};

        // Query number of filters by type
        Cursor cursor = db.query(
                FilterEntry.TABLE_NAME,
                columns,
                FilterEntry.COLUMN_FTYPE + " = " + filterType,
                null,
                null,
                null,
                null);

        try {
            int count = cursor.getCount();
            // Get columns indices
            int typeColumnIndex = cursor.getColumnIndex(FilterEntry.COLUMN_FTYPE);
            int nameColumnIndex = cursor.getColumnIndex(FilterEntry.COLUMN_FNAME);
            int textColumnIndex = cursor.getColumnIndex(FilterEntry.COLUMN_FTEXT);

            // Read all
            while (cursor.moveToNext()){
                // Get values using indices
                FilterRecord record = new FilterRecord();
                record.fType = cursor.getInt(typeColumnIndex);
                record.fName = cursor.getString(nameColumnIndex);
                record.fText = cursor.getString(textColumnIndex);
                array.add(record);
            }
        } finally {
            // Всегда закрываем курсор после чтения
            cursor.close();
        }

        return array;
    }

    public Boolean existsFilter(FilterRecord record)
    {
        SQLiteDatabase db = this.getReadableDatabase();

        String[] data = {
                String.valueOf(record.fType),
                record.fName,
                record.fText
        };

        Cursor cursor = db.query(
                FilterEntry.TABLE_NAME,
                new String[]{FilterEntry.COLUMN_FTYPE},
                where_complete,
                data,
                null,
                null,
                null);

        int count = cursor.getCount();
        return (count == 1);
    }

    public Boolean addUpdateFilter(FilterRecord previous, FilterRecord record) {
        Boolean exists;
        if (previous == null)
            exists = false;
        else
            exists = existsFilter(previous);
        SQLiteDatabase db = this.getWritableDatabase();
        ContentValues newValues = new ContentValues();
        newValues.put(FilterEntry.COLUMN_FTYPE, record.fType);
        newValues.put(FilterEntry.COLUMN_FNAME, record.fName);
        newValues.put(FilterEntry.COLUMN_FTEXT, record.fText);

        // Insert row
        long rowId;
        if (exists)
        {
            String[] oldValues = {
                    String.valueOf(previous.fType),
                    previous.fName,
                    previous.fText
            };
            rowId = db.update(
                    FilterEntry.TABLE_NAME,
                    newValues,
                    where_complete,
                    oldValues);
        }
        else
        {
            rowId = db.insert(FilterEntry.TABLE_NAME, null, newValues);
        }

        return (rowId != -1);
    }

    public void deleteFilter(FilterRecord record) {
        SQLiteDatabase db = this.getWritableDatabase();
        String where = FilterEntry.COLUMN_FTYPE + "=? AND " +
                FilterEntry.COLUMN_FNAME + "=? AND " +
                FilterEntry.COLUMN_FTEXT + "=?";
        String[] data = new String[]{String.valueOf(record.fType),
                record.fName,
                record.fText};

        db.delete(
                FilterEntry.TABLE_NAME,
                where,
                data
        );
    }
}
