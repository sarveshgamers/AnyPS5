# libSceHttp

## add Functional

- [sceHttpGetLastErrno](../../../core/libs/prx/libSceHttp/Export.cpp) (libSceHttp) - returning `ERROR_INVALID_VALUE` for an unwritable non-null output pointer is a host-safety fallback; whether the console faults or returns an argument error on unmapped memory is unmeasured. The four-byte result goes through `GuestCopyOut_nid_no_patch`, which preserves host `errno`.
