/* ---------------------------------------------------------------------------
 * Copyright © 2017 The trifecta Company Ltd.
 * Contact: http://www.trifecta.ru/licensing/
 * Author: BM
 *
 * This file is part of the Trifecta.
 *
 * Trifecta is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Trifecta is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 * ---------------------------------------------------------------------------
 */

#ifndef TLOGGER_H
#define TLOGGER_H

#include <QtWidgets/QApplication>
#include <QString>
#include <QtSql/QSqlError>
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cmath>
#include <fstream>
#include "tlogger_global.h"
#include "inireader.h"
#ifdef __linux__
    #include <sys/time.h>
#elif _WIN32
    #include <windows.h>
#endif


/**
 * @brief Класс TLogger
 * @details Предназначен для логгирования информации. Обеспечивает выходной
 * поток для отладки и должен использоваться всякий раз, когда необходимо
 * отслеживаемую информацию записать в файл, БД, или вывести в консоль. Класс
 * QDebug обеспечивает выходной поток для отладки информации. Имеет 5 уровней
 * логгирования, включающие в себя: Debug, Trace, Info, Warning и Error.
 * Каждый уровень логгирования обладает собственным цветом вывода в консоль.
 * Структура лога имеет следующий вид:
 * [dd.MM.yy HH:mm:ss.zzz] Текст сообщения
 *
 */


class TDebug;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

class TLOGGERSHARED_EXPORT TLogger {
public:
    static TLogger& get() {
        static TLogger instance;
        return instance;
    }

    void init(const std::string &fileName)
    {
        INIReader m_reader = INIReader(fileName);

        if (m_reader.ParseError() < 0) {
            std::cout << "Can't load "+fileName+"\n";
        }
        std::cout << "TLogger started with " << fileName << " config\n";

        m_dateFormat = m_reader.Get("tlogger","dateFormat","%d.%m.%y %H:%M:%S");
        m_fileName = m_reader.Get("tlogger","fileName","trifecta.log");
        m_colors.debug = m_reader.Get("tlogger","debugColor","36");
        m_colors.trace = m_reader.Get("tlogger","traceColor","35");
        m_colors.info = m_reader.Get("tlogger","infoColor","32");
        m_colors.warning = m_reader.Get("tlogger","warning","33");
        m_colors.error = m_reader.Get("tlogger","error","31");

        f.open(m_fileName, std::ios::app);
    }

    // Forbid copying and assigning
    TLogger(const TLogger& s) = delete;
    void operator=(const TLogger& s) = delete;

    void writeToFile(std::string msg)
    {
        f << msg << std::flush;
    }

    friend class TDebug;

private:
    TLogger()
    {
        qInstallMessageHandler(myMessageOutput);
    }
    ~TLogger(){ f.close();}
    std::string m_dateFormat;
    std::string m_fileName;
    struct
    {
        std::string debug;
        std::string trace;
        std::string info;
        std::string warning;
        std::string error;
    }m_colors;
    std::ofstream f;

};


class TLOGGERSHARED_EXPORT TDebug
{
public:
    // Конструкторы
    inline TDebug()
    {
        m_color = "\033[36m";
        m_space = true;
        m_dateFormat="[dd.MM.yy HH:mm:ss.zzz] ";
    }
    ~TDebug()
    {
        std::string curTimeString = curTimeC();
        std::string str = m_stream.str();
        size_t start_pos = 0;
        std::string from = "\n";
        std::string to = "";
        for(uint i=0;i<curTimeString.size();i++){to += ' ';}
        while((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.insert(start_pos+1, to);
            str.replace(start_pos+1, to.length(), to);
            start_pos += to.length();
        }
        TLogger::get().writeToFile(curTimeString+" "+str+"\n");
        std::cout << "\033[" << m_color << "m"<< curTimeString << str << "\033[0m" << std::endl;
    }

    // Методы
    inline std::string curTimeC()
    {
        int millisec;
#ifdef __linux__
        struct timeval tv;
        gettimeofday(&tv, NULL);
        millisec = lrint(tv.tv_usec/1000.0); // Round to nearest millisec
#elif _WIN32
        SYSTEMTIME st;
        GetSystemTime(&st);
        millisec = st.wMilliseconds;
#endif
        time_t current_time;
        struct tm * time_info;
        char timeString[24];
        time(&current_time);
        time_info = localtime(&current_time);
        std::string dateFormat = TLogger::get().m_dateFormat;
        strftime(timeString, 24, dateFormat.c_str(), time_info);
        std::stringstream ss;
        std::string zeroString;
        if(millisec < 100) zeroString = "0";
        if(millisec < 10) zeroString = "00";
        ss << '[' << timeString << '.' << zeroString << millisec << "] ";
        return ss.str();
    }
    inline TDebug& debug()   {m_color = TLogger::get().m_colors.debug; return *this;}
    inline TDebug& trace()   {m_color = TLogger::get().m_colors.trace; return *this;}
    inline TDebug& info()    {m_color = TLogger::get().m_colors.info; return *this;}
    inline TDebug& warning() {m_color = TLogger::get().m_colors.warning; return *this;}
    inline TDebug& error()   {m_color = TLogger::get().m_colors.error; return *this;}

    inline TDebug &space() { m_space = true; m_stream << ' '; return *this; }
    inline TDebug &nospace() { m_space = false; return *this; }
    inline TDebug &maybeSpace() { if (m_space) m_stream << ' '; return *this; }

    // Операторы перегрузки вывода
    TDebug& operator<<(const std::string& str) {m_stream << str; return maybeSpace();}
    TDebug& operator<<(const char* str) {m_stream << str; return maybeSpace();}
    TDebug& operator<<(char str) {m_stream << str; return maybeSpace();}
    TDebug& operator<<(const QString& str) {m_stream << str.toStdString();return maybeSpace();}
    TDebug& operator<<(int str) {m_stream << str;return maybeSpace();}
    TDebug& operator<<(QSqlError str) {m_stream << str.text().toStdString();return maybeSpace();}


    // TODO: Требуется сделать вывод лога в БД

private:
    std::ostringstream m_stream;
    std::string m_color;
    bool m_space;
    QString m_dateFormat;
};

#define TDEBUG TDebug().debug
#define TTRACE  TDebug().trace
#define TINFO  TDebug().info
#define TWARNING  TDebug().warning
#define TERROR  TDebug().error

#endif // TLOGGER_H
