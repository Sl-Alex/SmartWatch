package ua.com.slalex.smcenter;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.net.Uri;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v4.app.Fragment;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Spinner;
import android.widget.Toast;

import ua.com.slalex.smcenter.data.FiltersDbHelper;
import ua.com.slalex.smcenter.data.FilterRecord;

/**
 * A simple {@link Fragment} subclass.
 * Activities that contain this fragment must implement the
 * {@link FiltersFragment.OnFragmentInteractionListener} interface
 * to handle interaction events.
 * Use the {@link FiltersFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class FiltersFragment extends Fragment
        implements Dialog.OnClickListener,
        AdapterView.OnItemClickListener
{
    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";

    private static final int DIALOG_ACTION = 0;
    private static final int DIALOG_EDITOR = 1;

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    private View mView;
    private View mEditView;
    private int mSelected;
    private int mFilterType;
    private int mCurrentDialog; ///< Should be DIALOG_ACTION or DIALOG_EDITOR
    private FilterRecord mRecord = null;

    private AppSettings mSettings;

    private OnFragmentInteractionListener mListener;

    public FiltersFragment() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment FiltersFragment.
     */
    // TODO: Rename and change types and number of parameters
    public static FiltersFragment newInstance(String param1, String param2) {
        FiltersFragment fragment = new FiltersFragment();
        Bundle args = new Bundle();
        args.putString(ARG_PARAM1, param1);
        args.putString(ARG_PARAM2, param2);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            mParam1 = getArguments().getString(ARG_PARAM1);
            mParam2 = getArguments().getString(ARG_PARAM2);
        }
    }

    public void FilterSelected(View v)
    {

    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        mView = inflater.inflate(R.layout.fragment_filters, container, false);

        mSettings = new AppSettings(mView.getContext());
        return mView;
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        ListView listView=(ListView)mView.findViewById(R.id.filtersListview);
        listView.setOnItemClickListener(this);
        // Inflate header view
        ViewGroup headerView = (ViewGroup)getActivity().getLayoutInflater().inflate(R.layout.filter_header, listView,false);
        // Add header view to the ListView
        listView.addHeaderView(headerView);

        Spinner filterSpinner = (Spinner)mView.findViewById(R.id.filterSpinner);
        filterSpinner.setSelection(mSettings.getFilterType());
        filterSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                setFilterType(i);
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {
                Toast toast = Toast.makeText(mView.getContext(), "Nothing selected", Toast.LENGTH_LONG);
                toast.show();
            }
        });
    }

    // TODO: Rename method, update argument and hook method into UI event
    public void onButtonPressed(Uri uri) {
        if (mListener != null) {
            mListener.onFiltersFragmentInteraction(uri);
        }
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        if (context instanceof OnFragmentInteractionListener) {
            mListener = (OnFragmentInteractionListener) context;
        } else {
            throw new RuntimeException(context.toString()
                    + " must implement OnFragmentInteractionListener");
        }
    }

    @Override
    public void onDetach() {
        super.onDetach();
        mListener = null;
    }

    @Override
    public void onClick(DialogInterface dialogInterface, int selection) {
        if (mCurrentDialog == DIALOG_EDITOR) {
            FiltersDbHelper fdb = new FiltersDbHelper(mEditView.getContext());
            FilterRecord fr = new FilterRecord();
            fr.fType = mFilterType;
            fr.fName = ((EditText)mEditView.findViewById(R.id.filterDialogName)).getText().toString();
            fr.fText = ((EditText)mEditView.findViewById(R.id.filterDialogText)).getText().toString();
            fdb.addUpdateFilter(mRecord, fr);
            mRecord = null;
            updateFilters();
        }
        if (mCurrentDialog == DIALOG_ACTION)
        {
            switch (selection) {
                case 0: // Adding a filter
                case 1: // Editing a filter
                    AlertDialog.Builder ab = new AlertDialog.Builder(mView.getContext());
                    ab.setTitle("Filter editor");
                    ab.create();
                    LayoutInflater inflater = LayoutInflater.from(this.getActivity());
                    View editView = inflater.inflate(R.layout.filter_edit_dialog, null);
                    ab.setView(editView);
                    mEditView = editView;
                    ab.setPositiveButton("Save filter", this);
                    mCurrentDialog = DIALOG_EDITOR;
                    if (selection == 1) {
                        EditText source = (EditText) editView.findViewById(R.id.filterDialogName);
                        EditText text = (EditText) editView.findViewById(R.id.filterDialogText);
                        // Get content of the selected filter
                        ListView lv = (ListView) mView.findViewById(R.id.filtersListview);
                        FilterRecord record = ((FilterRecord) lv.getAdapter().getItem(mSelected));
                        source.setText(record.fName);
                        text.setText(record.fText);
                        if (mRecord == null)
                            mRecord = record;
                    }
                    ab.show();
                    break;
                case 2: // Deleting a filter
                    FiltersDbHelper helper = new FiltersDbHelper(mView.getContext());
                    ListView lv = (ListView) mView.findViewById(R.id.filtersListview);
                    FilterRecord record = ((FilterRecord) lv.getAdapter().getItem(mSelected));
                    helper.deleteFilter(record);
                    updateFilters();
                    break;

            }
        }
    }

    @Override
    public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
        String[] items;
        if (i == 0)
            items = getResources().getStringArray(R.array.add_filter);
        else
            items = getResources().getStringArray(R.array.add_edit_delete_filter);
        mSelected = i;
        AlertDialog.Builder ab = new AlertDialog.Builder(view.getContext());
        ab.setTitle(R.string.filter_actions);
        ab.setItems(items, this);
        mCurrentDialog = DIALOG_ACTION;
        ab.show();
    }

    /**
     * This interface must be implemented by activities that contain this
     * fragment to allow an interaction in this fragment to be communicated
     * to the activity and potentially other fragments contained in that
     * activity.
     * <p>
     * See the Android Training lesson <a href=
     * "http://developer.android.com/training/basics/fragments/communicating.html"
     * >Communicating with Other Fragments</a> for more information.
     */
    public interface OnFragmentInteractionListener {
        // TODO: Update argument type and name
        void onFiltersFragmentInteraction(Uri uri);
    }

    /// @brief BLABLABLA
    public void setFilterType(int type)
    {
        mFilterType = type;
        mSettings.setFilterType(type);

        switch (type)
        {
            case AppSettings.FT_NONE:
                mView.findViewById(R.id.filtersListview).setVisibility(View.GONE);
                break;
            case AppSettings.FT_BLACKLIST:
            case AppSettings.FT_WHITELIST:
                mView.findViewById(R.id.filtersListview).setVisibility(View.VISIBLE);
                updateFilters();
                break;
        }
    }

    public void updateFilters() {
        ListView listView=(ListView)mView.findViewById(R.id.filtersListview);
        // Get the string array defined in strings.xml file
        FiltersDbHelper frh = new FiltersDbHelper(mView.getContext());
        FilterAdapter adapter=new FilterAdapter(mView.getContext(),frh.getFilters(mFilterType));
        // Bind data to the ListView
        listView.setAdapter(adapter);
    }
}
