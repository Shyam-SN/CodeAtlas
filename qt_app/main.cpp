#include <QApplication>
#include <cstring>
#include <cstdlib>
#include "main_window.hpp"

// INSECURE FUNCTION INJECTED FOR SECURITY SCANNER TESTING
void insecure_func(const char* input) {
    char buffer[10];
    strcpy(buffer, input); // Vulnerability: Dangerous Function
    system(buffer);        // Vulnerability: Command Injection
    const char* api_key = "12345-ABCDE"; // Vulnerability: Hardcoded Secret
    (void)api_key;
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    codeatlas::MainWindow window;
    window.show();

    return app.exec();
}
