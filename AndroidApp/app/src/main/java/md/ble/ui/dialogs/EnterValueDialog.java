package md.ble.ui.dialogs;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.text.InputType;
import android.widget.EditText;

import md.apk.R;

public class EnterValueDialog extends AppDialog<EnterValueDialog.ValueEnterDialogListener> {
    public static final String TAG = EnterValueDialog.class.getSimpleName();

    public interface ValueEnterDialogListener {
        void ValueEntered(String value);
    }

    private EditText _input = null;

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState) {
        _input = new EditText(getActivity());
        _input.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_CLASS_NUMBER);

        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());
        builder.setTitle(R.string.enter_value)
                .setView(_input)
                .setPositiveButton(R.string.ok, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        getListener().ValueEntered(_input.getText().toString());
                    }
                })
                .setNegativeButton(R.string.cancel, new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dialog.cancel();
                    }
                });
        return builder.create();
    }
    @Override
    public void onDestroy() {
        _input = null;
        super.onDestroy();
    }

    public void SetDefaultValue(String defValue)
    {
        if (_input != null)
            _input.setText(defValue);

    }


}