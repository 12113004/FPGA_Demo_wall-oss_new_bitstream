uint32_t init(HANDLE h2cx_device, uint64_t base_addr, uint32_t size) {
    DWORD real_size;
    BYTE* write_data = allocate_buffer(size, 0);
    memset(write_data, 0, size);
    LARGE_INTEGER addr;
    addr.QuadPart = base_addr;
    // set file pointer to offset of target address within PCIe BAR
    if (INVALID_SET_FILE_POINTER == SetFilePointerEx(h2cx_device, addr, NULL, FILE_BEGIN)) {
        fprintf(stderr, "Error setting file pointer, win32 error code: %ld\n", GetLastError());
        CloseHandle(h2cx_device);
    }
    else{
        if (!WriteFile(h2cx_device, write_data, size, &real_size, NULL)){
            fprintf(stderr, "WriteFile to device %s failed with Win32 error code: %d\n","user_device_path", GetLastError());
            CloseHandle(h2cx_device);
            return 0;
        }
    }
    if (write_data)	_aligned_free(write_data);
    return real_size;
}
