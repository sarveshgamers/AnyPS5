# libSceHidControl

## add Functional

- [sceHidControlPswUpdateIsUpdateNeeded](../../../core/libs/prx/libSceHidControl/Export.cpp) (libSceHidControl) - returning `ErrorInvalidArgument` for an unwritable non-null result pointer is a host-safety fallback; whether the console faults or returns an argument error on unmapped memory is unmeasured. Both writes go through `StoreGuest`, which preserves host `errno`.
