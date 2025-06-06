// main.cpp
// Консольный тест на Qt для проверки четырёх дефектов:
// 1. REGISTER без пароля → должен вернуть ERROR (High)
// 2. LOGIN без пароля    → должен вернуть ERROR (High)
// 3. SAVE_DB: проверка, что пароль шифруется в users.cfg (Middle)
// 4. GET_KEY: сервер должен вернуть хеш (High)

#include <QCoreApplication>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QAbstractSocket>
#include <QFile>
#include <QTextStream>
#include <QThread>
#include <QDebug>
#include <iostream>

// Параметры сервера (IP и порт). Настройте под ваш запущенный сервер.
static constexpr const char* SERVER_IP = "127.0.0.1";
static constexpr quint16      SERVER_PORT = 8080;

//------------------------------------------------------------------------------
// sendCommand: отправляет одну строку «команда\n» на сервер, ждёт ответ (до '\n').
// Возвращает ответ (QString). Если не удалось соединиться/прочитать — возвращает строку, 
// начинающуюся с "ERROR:".
//------------------------------------------------------------------------------
QString sendCommand(const QString& command) {
    QTcpSocket socket;
    socket.connectToHost(QHostAddress(SERVER_IP), SERVER_PORT);
    if (!socket.waitForConnected(3000)) {
        return QStringLiteral("ERROR: Cannot connect to server (%1)").arg(socket.errorString());
    }

    // Убедимся, что в конце команды есть '\n'
    QString cmd = command;
    if (!cmd.endsWith('\n'))
        cmd.append('\n');

    QByteArray data = cmd.toUtf8();
    if (socket.write(data) == -1 || !socket.waitForBytesWritten(2000)) {
        socket.disconnectFromHost();
        return QStringLiteral("ERROR: Write timeout (%1)").arg(socket.errorString());
    }

    if (!socket.waitForReadyRead(3000)) {
        socket.disconnectFromHost();
        return QStringLiteral("ERROR: Read timeout or no response");
    }

    QByteArray line = socket.readLine().trimmed();
    QString response = QString::fromLocal8Bit(line);
    socket.disconnectFromHost();
    return response;
}

//------------------------------------------------------------------------------
// testRegisterIncomplete: посылаем "REGISTER onlyusername".
// Ожидаем, что сервер вернёт "ERROR: ..." (потому что нет пароля).
// Если сервер отвечает OK — значит дефект есть, тест провален.
//------------------------------------------------------------------------------
bool testRegisterIncomplete() {
    std::cout << "Test 1: REGISTER without password... ";
    QString resp = sendCommand("REGISTER onlyusername");
    // Ожидаем, что сервер вернёт что-то, начинающееся на "ERROR"
    if (resp.startsWith("ERROR")) {
        std::cout << "PASS\n";
        return true;
    }
    else {
        std::cout << "FAIL (got \"" << resp.toStdString() << "\")\n";
        return false;
    }
}

//------------------------------------------------------------------------------
// testLoginIncomplete: посылаем "LOGIN onlyusername".
// Ожидаем, что сервер вернёт "ERROR: ..." (потому что нет пароля).
//------------------------------------------------------------------------------
bool testLoginIncomplete() {
    std::cout << "Test 2: LOGIN without password... ";
    QString resp = sendCommand("LOGIN onlyusername");
    if (resp.startsWith("ERROR")) {
        std::cout << "PASS\n";
        return true;
    }
    else {
        std::cout << "FAIL (got \"" << resp.toStdString() << "\")\n";
        return false;
    }
}

//------------------------------------------------------------------------------
// testPasswordEncryption: сохраняем резервную копию users.cfg, затем REGISTER testUser testPass.
// Открываем users.cfg и проверяем, что пароль хешируется (т. е. в файле нет "testPass").
//------------------------------------------------------------------------------
bool testPasswordEncryption() {
    std::cout << "Test 3: Password encryption in users.cfg... ";

    // 1) Резервная копия users.cfg
    QByteArray backupData;
    {
        QFile fin("users.cfg");
        if (fin.open(QIODevice::ReadOnly)) {
            backupData = fin.readAll();
            fin.close();
        }
    }

    const QString testUser = "autoTestUser";
    const QString testPass = "simplePassword";

    // 2) Отправляем REGISTER testUser simplePassword
    QString respReg = sendCommand(QString("REGISTER %1 %2").arg(testUser, testPass));
    // Если сервер вернул "ERROR: Username already exists" — всё равно читаем файл.
    QThread::msleep(500); // даём серверу чуть-чуть времени записать файл

    // 3) Читаем users.cfg
    QFile file("users.cfg");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "FAIL (cannot open users.cfg)\n";
        // Восстанавливаем backup
        if (!backupData.isEmpty()) {
            QFile fout("users.cfg");
            if (fout.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                fout.write(backupData);
                fout.close();
            }
        }
        return false;
    }

    bool found = false;
    bool hashed = false;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.startsWith(testUser + " ")) {
            found = true;
            QString stored = line.section(' ', 1);
            if (stored != testPass) {
                // Если хранимое не равно plain-тексту, предполагаем, что это хеш
                hashed = true;
            }
            break;
        }
    }
    file.close();

    // 4) Восстанавливаем users.cfg из резервной копии
    if (!backupData.isEmpty()) {
        QFile fout("users.cfg");
        if (fout.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            fout.write(backupData);
            fout.close();
        }
    }

    if (!found) {
        std::cout << "FAIL (user not found in users.cfg)\n";
        return false;
    }
    if (hashed) {
        std::cout << "PASS\n";
        return true;
    }
    else {
        std::cout << "FAIL (password stored in plaintext)\n";
        return false;
    }
}

//------------------------------------------------------------------------------
// testGetKey: проверяем, что после STORE_KEY сервер отдаёт "RESULT: <hash>".
//------------------------------------------------------------------------------
bool testGetKey() {
    std::cout << "Test 4: GET_KEY returns hash... ";

    const QString halfKey = "halfKey123";
    const QString text = "SomeSecretText";

    // 1) STORE_KEY halfKey123 SomeSecretText -
    QString respStore = sendCommand(QString("STORE_KEY %1 %2 -").arg(halfKey, text));
    if (!respStore.startsWith("OK")) {
        std::cout << "FAIL (STORE_KEY returned \"" << respStore.toStdString() << "\")\n";
        return false;
    }
    QThread::msleep(200); // даём серверу записать результат

    // 2) GET_KEY halfKey123 SomeSecretText
    QString respGet = sendCommand(QString("GET_KEY %1 %2").arg(halfKey, text));
    // Ожидаем "RESULT: <hex_string>" (хеш SHA-512 имеет 128 hex-символов, но проверим хотя бы >=16)
    if (respGet.startsWith("RESULT: ")) {
        QString hashPart = respGet.section(' ', 1);
        bool okHex = (hashPart.length() >= 16);
        for (QChar c : hashPart) {
            if (!c.isLetterOrNumber()) { okHex = false; break; }
        }
        if (okHex) {
            std::cout << "PASS\n";
            return true;
        }
        else {
            std::cout << "FAIL (invalid hash format: \"" << hashPart.toStdString() << "\")\n";
            return false;
        }
    }
    else {
        std::cout << "FAIL (GET_KEY returned \"" << respGet.toStdString() << "\")\n";
        return false;
    }
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    int passed = 0;
    const int total = 4;

    if (testRegisterIncomplete()) passed++;
    if (testLoginIncomplete())    passed++;
    if (testPasswordEncryption()) passed++;
    if (testGetKey())             passed++;

    std::cout << "\nSummary: " << passed << " / " << total << " tests passed.\n";
    return 0;
}
