uint32_t init(HANDLE h2cx_device, uint32_t size) {
    DWORD real_size;
    BYTE* write_data = allocate_buffer(size, 0);
    memset(write_data, 1, size);
    if (!WriteFile(h2cx_device, write_data, size, &real_size, NULL)){
        fprintf(stderr, "WriteFile to device %s failed with Win32 error code: %d\n","user_device_path", GetLastError());
        CloseHandle(h2cx_device);
    }
    return real_size;
}
