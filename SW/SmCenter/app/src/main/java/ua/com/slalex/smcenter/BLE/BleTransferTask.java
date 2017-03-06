package ua.com.slalex.smcenter.BLE;

/**
 * Transfer task, contains all possible fields, depending on a task type
 */

public class BleTransferTask {
    public static final String BUNDLE_TAG = "transferTask";
    public static final int TASK_INVALID = 0;
    public static final int TASK_TIMESYNC = 1;
    public static final int TASK_SMS = 2;
    public static final int TASK_VERSION = 3;

    public static String toString(int taskType) {
        switch (taskType) {
            case TASK_INVALID:
                return "INVALID";
            case TASK_TIMESYNC:
                return "TIMESYNC";
            case TASK_SMS:
                return "SMS";
            case TASK_VERSION:
                return "VERSION";
            default:
                return "UNKNOWN";
        }
    }

    // Task type
    public int type;

    public String SmsSender;
    public String SmsText;
    public String version;
    public boolean status;
}
