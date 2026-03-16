section '.idata' import data readable

    library kernel32, 'kernel32.dll', \
            ws2_32, 'ws2_32.dll', \
            user32, 'user32.dll'

    import kernel32, \
           CreateFile, 'CreateFileA', \
           ReadFile, 'ReadFile', \
           WriteFile, 'WriteFile', \
           CloseHandle, 'CloseHandle', \
           GetStdHandle, 'GetStdHandle', \
           WriteConsole, 'WriteConsoleA', \
           ExitProcess, 'ExitProcess', \
           GetFileSize, 'GetFileSize', \
           SetFilePointer, 'SetFilePointer', \
           GetCurrentDirectory, 'GetCurrentDirectoryA', \
           Sleep, 'Sleep'

    import ws2_32, \
           WSAStartup, 'WSAStartup', \
           socket, 'socket', \
           htons, 'htons', \
           bind, 'bind', \
           listen, 'listen', \
           accept, 'accept', \
           recv, 'recv', \
           send, 'send', \
           closesocket, 'closesocket'

    import user32, \
           MessageBox, 'MessageBoxA'
