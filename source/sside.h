#ifndef HDR_SSIDE_H
#define HDR_SSIDE_H /* Предотвращает случайное двойное включение */

#include <stdio.h> /* Стандартные функции ввода-вывода */
#include <stdlib.h> /* Прототипы наиболее востребованных библиотечных
функций плюс константы EXIT_SUCCESS и EXIT_FAILURE */
#include <unistd.h> /* Прототипы многих системных вызовов, нужна для close() */
#include <errno.h> /* Объявление errno и определение констант ошибок */
#include <string.h> /* Наиболее используемые функции обработки строк */
#include <sys/wait.h>
#include <pthread.h>
/* CW ADDITIONS */
#include <sys/socket.h> // Для сокетов
#include <netinet/in.h> // Для AF_INET
#include <arpa/inet.h>
#include "lib/encrypt.h"
#endif