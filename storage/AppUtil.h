/*
 * Copyright (c) [2004-2009] Novell, Inc.
 *
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail, you may
 * find current contact information at www.novell.com.
 */


#ifndef APP_UTIL_H
#define APP_UTIL_H

#include <libintl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sstream>
#include <locale>
#include <string>
#include <list>
#include <map>


namespace storage
{
    using std::string;
    using std::list;
    using std::map;


void createPath(const string& Path_Cv);
bool checkNormalFile(const string& Path_Cv);
bool checkDir(const string& Path_Cv);
bool getStatMode(const string& Path_Cv, mode_t& val );
bool setStatMode(const string& Path_Cv, mode_t val );

    list<string> glob(const string& path, int flags);

    struct StatVfs
    {
	unsigned long long sizeK;
	unsigned long long freeK;
    };

    bool getStatVfs(const string& path, StatVfs&);

    bool getMajorMinor(const string& device, unsigned long& major, unsigned long& minor);


string extractNthWord(int Num_iv, const string& Line_Cv, bool GetRest_bi = false);
std::list<string> splitString( const string& s, const string& delChars=" \t\n",
                          bool multipleDelim=true, bool skipEmpty=true,
			  const string& quotes="" );
std::map<string,string> makeMap( const std::list<string>& l,
                                 const string& delim = "=",
				 const string& removeSur = " \t\n" );

    string udevAppendPart(const string&, unsigned num);

    string udevEncode(const string&);
    string udevDecode(const string&);

    bool readlink(const string& path, string& buf);


    typedef map<string, list<string>> UdevMap;
    typedef map<string, string> RevUdevMap;

    UdevMap getUdevMap(const char* path);
    RevUdevMap getRevUdevMap(const char* path);

string normalizeDevice( const string& dev );
void normalizeDevice( string& dev );
string undevDevice( const string& dev );
void undevDevice( string& dev );
bool isNfsDev( const string& dev );

unsigned getMajorDevices(const char* driver);


template<class StreamType>
void classic(StreamType& stream)
{
    stream.imbue(std::locale::classic());
}


enum LogLevel { DEBUG, MILESTONE, WARNING, ERROR };

void createLogger(const string& name, const string& logpath, const string& logfile);

bool testLogLevel(LogLevel level);

std::ostringstream* logStreamOpen();

void logStreamClose(LogLevel level, const char* file, unsigned line,
		    const char* func, std::ostringstream*);

#define y2deb(op) y2log_op(storage::DEBUG, __FILE__, __LINE__, __FUNCTION__, op)
#define y2mil(op) y2log_op(storage::MILESTONE, __FILE__, __LINE__, __FUNCTION__, op)
#define y2war(op) y2log_op(storage::WARNING, __FILE__, __LINE__, __FUNCTION__, op)
#define y2err(op) y2log_op(storage::ERROR, __FILE__, __LINE__, __FUNCTION__, op)

#define y2log_op(level, file, line, func, op)				\
    do {								\
	if (storage::testLogLevel(level))				\
	{								\
	    std::ostringstream* __buf = storage::logStreamOpen();	\
	    *__buf << op;						\
	    storage::logStreamClose(level, file, line, func, __buf);	\
	}								\
    } while (0)


    string hostname();
    string datetime();


    class StopWatch
    {
    public:

	StopWatch();

	friend std::ostream& operator<<(std::ostream& s, const StopWatch& sw);

    protected:

	struct timeval start_tv;

    };


    struct Text
    {
	Text() : native(), text() {}
	Text(const string& native, const string& text) : native(native), text(text) {}

	void clear();

	const Text& operator+=(const Text& a);

	string native;
	string text;
    };


    Text sformat(const Text& format, ...);


    Text _(const char* msgid);
    Text _(const char* msgid, const char* msgid_plural, unsigned long int n);


extern const string app_ws;

}

#endif
