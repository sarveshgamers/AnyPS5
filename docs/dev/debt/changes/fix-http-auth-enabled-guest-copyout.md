# libSceHttp

## add Functional

- [sceHttpGetAuthEnabled](../../../core/libs/prx/libSceHttp/Export.cpp) (libSceHttp) - returning `ERROR_INVALID_VALUE` for an unwritable non-null output pointer is a host-safety fallback; whether the console faults or returns an argument error on unmapped memory is unmeasured. The value is read under the state lock and written through `GuestCopyOut_nid_no_patch`, which preserves host `errno`.
