/*!
  \file
  \brief Файл конфигурации

  Данный файл содержит основную конфигурацию интерпретатора и функции
  для изменения вывода интерпретатора
 */

#ifndef CONFIG_HPP
#define CONFIG_HPP

namespace Config {

//! Имя программы
static const char* programName = "YALI";
//! Версия программы
static const char* programVersion = "0.0.1";

//! Количество загружаемых файлов
static const int filesCount = 2;
//! Файлы которые необходимо загрузить на этапе запуска интерпретатора
static const char* filesToLoad[] {
    "YALIlib/macros.scm",
    "YALIlib/funcs.scm",
//    "YALIlib/tests.scm",
};

//! Доступ к переменной отвечающей за вывод этапов развертки макросов
bool& showMacro();
//! Доступ к переменной отвечающей за вывод типа значения в интерактивном режиме
bool& showType();

}

#endif // CONFIG_HPP
