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
#ifdef WINDOWS // i fucking hate windows with a burning passion
    HANDLE g_hChildStd_OUT_Rd = nullptr;
    HANDLE g_hChildStd_OUT_Wr = nullptr;
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = true;
    saAttr.lpSecurityDescriptor = nullptr;
    CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0);
    SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = g_hChildStd_OUT_Wr;
    si.hStdError = g_hChildStd_OUT_Wr;
    si.dwFlags |= STARTF_USESTDHANDLES;
    CreateProcess(nullptr, command.c_str(), nullptr, nullptr, true, 0, nullptr, nullptr, &si, &pi)) {
    CloseHandle(g_hChildStd_OUT_Wr);
    int bytesRead;
    char buffer[4096];
    std::string output;
    while (ReadFile(g_hChildStd_OUT_Rd, buffer, 4096, &bytesRead, nullptr) && bytesRead != 0) {
        output.append(buffer, bytesRead);
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return output;
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