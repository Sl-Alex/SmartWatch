package ua.com.slalex.smcenter;
import java.util.ArrayList;

import android.app.Activity;
import android.content.Context;
import android.support.annotation.IntegerRes;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.BaseAdapter;
import android.widget.TextView;

import ua.com.slalex.smcenter.data.FiltersContract;
import ua.com.slalex.smcenter.data.FiltersDbHelper;
import ua.com.slalex.smcenter.data.FilterRecord;

public class FilterAdapter extends BaseAdapter {

    ArrayList<FilterRecord> item_list;
    Context context;

    public FilterAdapter(Context context, ArrayList<FilterRecord> list){
        super();
        this.context=context;
        this.item_list=list;
    }
    // Hold views of the ListView to improve its scrolling performance
    static class ViewHolder {
        public TextView filterSource;
        public TextView filterText;
    }

    public View getView(int position, View convertView, ViewGroup parent) {
        View rowView = convertView;

        LayoutInflater inflater = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        if (rowView == null) {
            rowView = inflater.inflate(R.layout.filter_layout, parent, false);
            ViewHolder viewHolder = new ViewHolder();
            viewHolder.filterSource = (TextView) rowView.findViewById(R.id.filterListSource);
            viewHolder.filterText = (TextView) rowView.findViewById(R.id.filterListText);
            rowView.setTag(viewHolder);
        }
        ViewHolder holder = (ViewHolder) rowView.getTag();
        holder.filterSource.setText(item_list.get(position).fName);
        holder.filterText.setText(item_list.get(position).fText);
        return rowView;
    }

    @Override
    public int getCount() {
        return item_list.size();
    }

    @Nullable
    @Override
    public Object getItem(int position) {
        return item_list.get(position);
    }

    @Override
    public long getItemId(int i) {
        return 0;
    }
}