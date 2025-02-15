Для сборки примеров необходимо не забыть прилинковать библиотеку WinSock2. 

Например, для пакета MinGW с компилятором GCC это может выглядеть так:
```
gcc client.cpp -l ws2_32 -o client.exe
gcc server.cpp -l ws2_32 -o server.exe
```
Эти команды необходимо выполнять из консоли "MinGW Command Prompt".

В случае использования компилятора `cl`, поставляющегося с Microsoft Visual C++, команды для сборки примеров из консоли будут выглядеть так:
```
cl client.cpp ws2_32.lib
cl server.cpp ws2_32.lib
```
Выполнять их нужно из консоли "Developer command prompt", либо открыть командный интерпретатор `cmd` и выполнить файл `vcvars.bat` (другие возможные варианты названия этого файла - `vcvars32.bat` или `vcvars64.bat`). Более подробная документация доступна на английском языке по [ссылке](https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line).


Для запуска примеров необходимо открыть два окна командного интерпретатора `cmd`. В одном из них запустить собранную программу `server.exe` и только после этого во втором окне запустить собранную программму `client.exe`. В случае запуска программ в другом порядке соединение между клиентом и сервером установлено не будет.
