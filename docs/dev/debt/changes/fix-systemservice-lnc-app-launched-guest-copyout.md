# libSceSystemService

## add Functional

- [sceLncUtilIsAppLaunched](../../../core/libs/prx/libSceSystemService/Export.cpp) (libSceSystemService) - returning `LNC_UTIL_ERROR_INVALID_ARGUMENT` for an unwritable non-null output pointer is a host-safety fallback; whether the console faults or returns an argument error on unmapped memory is unmeasured. The one-byte result goes through `GuestCopyOut_nid_no_patch`, which preserves host `errno`.
