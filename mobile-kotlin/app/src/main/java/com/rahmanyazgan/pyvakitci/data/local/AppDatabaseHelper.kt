package com.rahmanyazgan.pyvakitci.data.local

import android.content.ContentValues
import android.content.Context
import android.database.sqlite.SQLiteDatabase
import android.database.sqlite.SQLiteOpenHelper
import com.rahmanyazgan.pyvakitci.data.model.PrayerTimes

class AppDatabaseHelper(context: Context) :
    SQLiteOpenHelper(context, DATABASE_NAME, null, DATABASE_VERSION) {

    companion object {
        private const val DATABASE_NAME = "pyvakitci.db"
        private const val DATABASE_VERSION = 1

        private const val TABLE_PRAYERS = "prayer_times"
        private const val COL_ID = "id"
        private const val COL_DISTRICT_ID = "district_id"
        private const val COL_DATE = "date"
        private const val COL_HIJRI = "hijri_date"
        private const val COL_IMSAK = "imsak"
        private const val COL_GUNES = "gunes"
        private const val COL_OGLE = "ogle"
        private const val COL_IKINDI = "ikindi"
        private const val COL_AKSAM = "aksam"
        private const val COL_YATSI = "yatsi"
    }

    override fun onCreate(db: SQLiteDatabase) {
        val createSql = """
            CREATE TABLE $TABLE_PRAYERS (
                $COL_ID INTEGER PRIMARY KEY AUTOINCREMENT,
                $COL_DISTRICT_ID TEXT NOT NULL,
                $COL_DATE TEXT NOT NULL,
                $COL_HIJRI TEXT,
                $COL_IMSAK TEXT,
                $COL_GUNES TEXT,
                $COL_OGLE TEXT,
                $COL_IKINDI TEXT,
                $COL_AKSAM TEXT,
                $COL_YATSI TEXT,
                UNIQUE($COL_DISTRICT_ID, $COL_DATE) ON CONFLICT REPLACE
            )
        """.trimIndent()
        db.execSQL(createSql)
    }

    override fun onUpgrade(db: SQLiteDatabase, oldVersion: Int, newVersion: Int) {
        db.execSQL("DROP TABLE IF EXISTS $TABLE_PRAYERS")
        onCreate(db)
    }

    fun savePrayerTimes(districtId: String, list: List<PrayerTimes>) {
        val db = writableDatabase
        db.beginTransaction()
        try {
            for (p in list) {
                val cv = ContentValues().apply {
                    put(COL_DISTRICT_ID, districtId)
                    put(COL_DATE, p.date)
                    put(COL_HIJRI, p.hijriDate)
                    put(COL_IMSAK, p.imsak)
                    put(COL_GUNES, p.gunes)
                    put(COL_OGLE, p.ogle)
                    put(COL_IKINDI, p.ikindi)
                    put(COL_AKSAM, p.aksam)
                    put(COL_YATSI, p.yatsi)
                }
                db.insertWithOnConflict(TABLE_PRAYERS, null, cv, SQLiteDatabase.CONFLICT_REPLACE)
            }
            db.setTransactionSuccessful()
        } finally {
            db.endTransaction()
        }
    }

    fun getPrayerTimesForDate(districtId: String, date: String): PrayerTimes? {
        val db = readableDatabase
        val cursor = db.query(
            TABLE_PRAYERS,
            null,
            "$COL_DISTRICT_ID = ? AND $COL_DATE = ?",
            arrayOf(districtId, date),
            null, null, null
        )
        cursor.use {
            if (it.moveToFirst()) {
                return PrayerTimes(
                    date = it.getString(it.getColumnIndexOrThrow(COL_DATE)),
                    hijriDate = it.getString(it.getColumnIndexOrThrow(COL_HIJRI)) ?: "",
                    imsak = it.getString(it.getColumnIndexOrThrow(COL_IMSAK)) ?: "",
                    gunes = it.getString(it.getColumnIndexOrThrow(COL_GUNES)) ?: "",
                    ogle = it.getString(it.getColumnIndexOrThrow(COL_OGLE)) ?: "",
                    ikindi = it.getString(it.getColumnIndexOrThrow(COL_IKINDI)) ?: "",
                    aksam = it.getString(it.getColumnIndexOrThrow(COL_AKSAM)) ?: "",
                    yatsi = it.getString(it.getColumnIndexOrThrow(COL_YATSI)) ?: ""
                )
            }
        }
        return null
    }

    fun getRemainingStoredDaysCount(districtId: String, fromDate: String): Int {
        val db = readableDatabase
        val cursor = db.rawQuery(
            "SELECT COUNT(*) FROM $TABLE_PRAYERS WHERE $COL_DISTRICT_ID = ? AND $COL_DATE >= ?",
            arrayOf(districtId, fromDate)
        )
        cursor.use {
            if (it.moveToFirst()) {
                return it.getInt(0)
            }
        }
        return 0
    }

    fun getAllStoredDates(districtId: String): List<String> {
        val dates = mutableListOf<String>()
        val db = readableDatabase
        val cursor = db.query(
            TABLE_PRAYERS,
            arrayOf(COL_DATE),
            "$COL_DISTRICT_ID = ?",
            arrayOf(districtId),
            null, null, "$COL_DATE ASC"
        )
        cursor.use {
            while (it.moveToNext()) {
                dates.add(it.getString(0))
            }
        }
        return dates
    }
}

