# clientserver

# Windows
Собирал в Windows 7, используя MinGW-W64-builds-4.3.5 i686 posix dwarf,
gcc 7.3.0, CMake 3.17.3.

Так как в задании говорится о возможности использования длины до 2^1008-1, то
я принял решение использовать библиотеку работы с длинными числами GNU GMP.

В сервере, возможно, Вам потребуется у себя изменить строки 
  include_directories(c:/mingw32/include)
  link_directories(c:/mingw32/lib)

# Linux
Собирал под Linux 5.0.0 i686, gcc 7.5.0 i686-linux-gnu posix,
cmake 3.10.2.

Клиент, получив данные от сервера, записывает их в файл data.txt в своей рабочей директории.

Сервер написан на С++14, клиенту достаточно С++11.

e-mail: ankhmesut@gmail.com
