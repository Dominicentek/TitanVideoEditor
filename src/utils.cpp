#include <vector>
#include <string>
#include <cstdio>
#ifdef WINDOWS
#include <windows.h>
#endif

std::vector<std::string> split_string(char delimiter, std::string input) {
    std::vector<std::string> tokens = {};
    std::string token = "";
    for (int i = 0; i < input.size(); i++) {
        char character = input[i];
        if (character == '\r') continue;
        if (character == delimiter) {
            tokens.push_back(token);
            token = "";
        }
        else token += character;
    }
    tokens.push_back(token);
    return tokens;
}


std::string exec_command(std::string command) {
    // popen opens up cmd window in windows smh, have to use this thing for windows
#ifdef WINDOWS // thanks integerbang
    HANDLE stdoutRead = INVALID_HANDLE_VALUE;
    HANDLE stdoutWrite = INVALID_HANDLE_VALUE;
    PROCESS_INFORMATION processInfo = {};
    char* cmd = strdup(command.c_str());
    std::string result;
    if (cmd) {
        SECURITY_ATTRIBUTES attributes = { .nLength = sizeof(SECURITY_ATTRIBUTES), .bInheritHandle = TRUE };
        if (CreatePipe(&stdoutRead, &stdoutWrite, &attributes, 0)) {
            if (SetHandleInformation(stdoutRead, HANDLE_FLAG_INHERIT, 0)) {
                STARTUPINFO startupInfo = { cb = sizeof(STARTUPINFO), .dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW, .wShowWindow = SW_HIDE, .hStdOutput = stdoutWrite, .hStdError = stdoutWrite };
                if (CreateProcessA(nullptr, cmd, nullptr, nullptr, true, 0, nullptr, nullptr, &startupInfo, &processInfo)) {
                    CloseHandle(stdoutWrite)
                    stdoutWrite = INVALID_HANDLE_VALUE;
                    CloseHandle(processInfo.hThread);
                    DWORD bytesRead;
                    char buffer[4096];
                    while (ReadFile(stdoutRead, buffer, 4096, &bytesRead, nullptr) && bytesRead > 0) {
                        result.append(buffer, bytesRead);
                    }
                    WaitForSingleObject(processInfo.hProcess, INFINITE);
                    CloseHandle(processInfo.hProcess);
                }
            }
            CloseHandle(stdoutRead);
            if (stdoutWrite != INVALID_HANDLE_VALUE) CloseHandle(stdoutWrite);
        }
        free(cmd);
    }
    return result;
#else // this is why unix better
    FILE* cmd = popen(command.c_str(), "r");
    char buffer[4096];
    std::string out = "";
    while (!feof(cmd)) {
        if (fgets(buffer, 4096, cmd) != nullptr) {
            out += buffer;
        }
    }
    pclose(cmd);
    return out;
#endif
}