section '.idata' import data readable

    library kernel32, 'kernel32.dll', \
            ws2_32, 'ws2_32.dll', \
            user32, 'user32.dll', \
            shell32, 'shell32.dll', \
            shell32, 'shell32.dll'

    import kernel32, \
           CreateFile, 'CreateFileA', \
           ReadFile, 'ReadFile', \
           CloseHandle, 'CloseHandle', \
           GetStdHandle, 'GetStdHandle', \
           WriteConsole, 'WriteConsoleA', \
           ExitProcess, 'ExitProcess', \
           GetCurrentDirectory, 'GetCurrentDirectoryA', \
           SetFilePointer, 'SetFilePointer', \
           WriteFile, 'WriteFile', \
           GetTickCount, 'GetTickCount'

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

    import shell32, \
           ShellExecuteA, 'ShellExecuteA'

