package ua.com.slalex.smcenter.data;

import android.content.Context;
import android.util.SparseIntArray;

import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;

/**
 * Created by Sl-Alex on 02.03.2017.
 */

public class SmTextEncoder {
    Context mContext;
    //
    SparseIntArray mLutTable;

    static final int IDX_FW_FONT_SMALL = 2;
    static final int FONT_PARAMS_OFFSET = 4 + IDX_FW_FONT_SMALL * 8;

    // Location of the first symbol offset
    static final int FIRST_SYMBOL_OFFSET = 0x10;

    public SmTextEncoder(Context context)
    {
        mContext = context;
    }

    public long getEncoded(char symbol){
        return mLutTable.get(symbol);
    }

    long getUnsignedByte(byte[] arr, int idx)
    {
        return ((int)arr[idx]) & 0xFF;
    }

    long getUnsignedShort(byte[] arr, int idx)
    {
        long ret = 0;
        ret  = ((int)arr[idx + 1]) & 0xFF;
        ret <<= 8;
        ret += ((int)arr[idx]) & 0xFF;

        return ret;
    }

    long getUnsignedInt(byte[] arr, int idx)
    {
        long ret = 0;
        ret  = ((int)arr[idx + 3]) & 0xFF;
        ret <<= 8;
        ret += ((int)arr[idx + 2]) & 0xFF;
        ret <<= 8;
        ret += ((int)arr[idx + 1]) & 0xFF;
        ret <<= 8;
        ret += ((int)arr[idx]) & 0xFF;

        return ret;
    }

    public boolean loadTable()
    {
        InputStream is = null;
        byte[] data;
        try {
            is = mContext.getAssets().open("update.bin");
            data =new byte[is.available()];
            is.read(data);
            is.close();
        } catch (IOException e) {
            return false;
        }

        // data should be big enough
        if (data.length < FONT_PARAMS_OFFSET + 8)
            return false;

        // Ready to load a font
        if (getUnsignedInt(data, 0) <= IDX_FW_FONT_SMALL )
            return false;

        long font_offset = getUnsignedInt(data, FONT_PARAMS_OFFSET);
        long size = getUnsignedInt(data, FONT_PARAMS_OFFSET + 4);
        if (data.length < font_offset + 1)
            return false;

        // Seems to be too big
        if (size > 1024*1024)
            return false;

        long count = getUnsignedInt(data, (int) font_offset);
        mLutTable = new SparseIntArray();
        mLutTable.clear();
        for (int i = 0; i < count; i++)
        {
            long symbol = getUnsignedShort(data, (int) (font_offset + FIRST_SYMBOL_OFFSET + count * 4 + i * 2));
            mLutTable.put((int) symbol, i);
        }
        return true;
    }
}
