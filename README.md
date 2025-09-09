# C++ Preprocessor (учебная реализация)

## Описание
Этот проект реализует упрощённый **препроцессор C++**, который обрабатывает директивы `#include` в исходных файлах.  
Код рекурсивно раскрывает заголовочные файлы, поддерживает поиск в пользовательских папках (`""`) и системных (`<>`),  
а также выводит ошибки при отсутствии подключаемого файла.  

Программа написана на **C++17** с использованием стандартных библиотек (`<filesystem>`, `<regex>`, `<fstream>`).

---

## Основные функции

- `PreprocessImpl`  
  Рекурсивная функция, которая построчно читает исходный файл, раскрывает `#include` и записывает результат в выходной поток.  
  Поддерживает:
  - `#include "file.h"` — поиск в текущей папке и заданных каталогах.
  - `#include <file.h>` — поиск только в include-директориях.

- `Preprocess`  
  Обёртка, открывающая входной/выходной файлы и вызывающая `PreprocessImpl`.

- `GetFileContents`  
  Читает файл полностью в `std::string`.

- `Test`  
  Создаёт тестовую структуру файлов и проверяет работу препроцессора на корректность и обработку ошибок.

---

## Пример работы

Пример
sources/a.cpp

  // this comment before include
  #include "dir1/b.h"
  // text between b.h and c.h
  #include "dir1/d.h"

  void SayHello() {
      std::cout << "hello, world!" << std::endl;
  #   include<dummy.txt>
  }
   
sources/dir1/b.h

  // text from b.h before include
  #include "subdir/c.h"
  // text from b.h after include
   
sources/dir1/subdir/c.h

  // text from c.h before include
  #include <std1.h>
  // text from c.h after include
   
sources/dir1/d.h

  // text from d.h before include
  #include "lib/std2.h"
  // text from d.h after include
   
sources/include1/std1.h

  // std1
   
sources/include2/lib/std2.h

  // std2
   
В качестве include-директорий поданы sources/include1 и sources/include2. Программа должна создать такой файл:

// this comment before include
// text from b.h before include
// text from c.h before include
// std1
// text from c.h after include
// text from b.h after include
// text between b.h and c.h
// text from d.h before include
// std2
// text from d.h after include

void SayHello() {
    std::cout << "hello, world!" << std::endl; 
В cout должен быть выведен текст:

unknown include file dummy.txt at file sources/a.cpp at line 8 
В некоторых операционных системах слеш может быть другим, это допустимо:

unknown include file dummy.txt at file sources\a.cpp at line 8 

---

##Сборка и запуск
Требования
-C++17
-g++/clang++ или MSVC

Запуск
-Открыть в MSVC
-Программа автоматически выполнит встроенный тест Test().

---

## Используемые технологии

-std::filesystem — работа с путями и файлами.
-std::regex — поиск директив #include.
-fstream — чтение и запись файлов.
-STL алгоритмы — distance, count_if, unique, sort.

---

## Особенности

-Поддержка поиска файлов в нескольких include-директориях.
-Сообщения об ошибках с указанием файла и строки.
-Рекурсивная обработка вложенных #include.
-Простая структура кода, ориентированная на обучение.
