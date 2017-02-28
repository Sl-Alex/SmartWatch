package ua.com.slalex.smcenter.BLE;

/**
 * Transfer task, contains all possible fields, depending on a task type
 */

public class BleTransferTask {
    public static final String BUNDLE_TAG = "transferTask";
    @SuppressWarnings("unused")
    public static final int TASK_INVALID = 0;
    public static final int TASK_TIMESYNC = 1;
    public static final int TASK_SMS = 2;
    public static final int TASK_VERSION = 3;

    // Task type
    public int type;

    public String SmsSender;
    public String SmsText;
}
