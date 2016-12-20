package ua.com.slalex.smcenter.data;

/**
 * Created by Sl-Alex on 17.12.2016.
 */

import android.provider.BaseColumns;

public class FiltersContract {

    private FiltersContract() {

    }

    public static final class FilterEntry implements BaseColumns {

        public final static String TABLE_NAME = "filters";

        public final static String _ID = BaseColumns._ID;
        public final static String COLUMN_FTYPE = "ftype";
        public final static String COLUMN_FNAME = "fname";
        public final static String COLUMN_FTEXT = "ftext";

        public static final int FTYPE_WHITE = 1;
        public static final int FTYPE_BLACK = 2;


    }
}
