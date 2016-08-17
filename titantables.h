#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <istream>
#include <ostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <algorithm>
#include <netdb.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/range.hpp>
#include <boost/asio/ip/address_v4.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/regex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

using namespace boost::posix_time;
using namespace boost::gregorian;

namespace client
{
class Client
{
    unsigned short verbosity = 3;
    unsigned int defaultport = 3306;
    bool tempTable = false;
    std::string defaultdb = "robot";
    std::string defaultLatest = "latest";
    std::string useTable = "";
    std::string useComment = "in_use";
    std::string tempName = "temp_table";
    char term = ';';
    sql::Driver *driver;
    sql::Connection *con;

public:
    //!Constructor connects to sql database
    Client(std::string ip, int port, std::string user, std::string pass);

    //!Default connect to roborio
    Client(int team, std::string user, std::string pass);

    //!Deconstructor that deletes the sql connection
    ~Client(void);

    void useLatestTable();
    void useTempTable();

    void addInt(std::string name);
    void addString(std::string name);
    void addDouble(std::string name);
    void addFile(std::string name);
    void addIntArray(std::string name);
    void addStringArray(std::string name);
    void addDoubleArray(std::string name);

    void addBase(std::string name, std::string type);


    void setInt(std::string name, int value);
    void setString(std::string name, std::string value);
    void setDouble(std::string name, double value);
    void setFile(std::string name, std::istream &blob);
    void setIntArray(std::string name, std::vector<int> values);
    void setStringArray(std::string name, std::vector<std::string> values);
    void setDoubleArray(std::string name, std::vector<double> values);

    void updateBase(std::string name, std::string value, int num = 1);

    int getInt(std::string name, int defaultvalue = 0);
    std::string getString(std::string name, std::string defaultvalue = "");
    double getDouble(std::string name, double defaultvalue = 0.0);
    std::istream* getFile(std::string name);
    std::vector<int> getIntArray(std::string name, std::vector<int> defaultvalues = std::vector<int>());
    std::vector<std::string> getStringArray(std::string name, std::vector<std::string> defaultvalues = std::vector<std::string>());
    std::vector<double> getDoubleArray(std::string name, std::vector<double> defaultvalues = std::vector<double>());

    std::string retrieveBase(std::string name, int num = -1);

    void createBlank(sql::Statement *stmt);
    void finishTable(std::string new_name = "oldtable");
    std::string resolveDNS(std::string name);
    std::string resolveHOST();
    void test();
private:
    template<class T>
    void serializeArr(T &lists, std::string &ret);

    std::vector<std::string> deserializeArr(std::string value);

    size_t charCount(std::string &str, const char toc);
    void verboPrint(int mins, const char* top, bool endline);
    void init(std::string ip, unsigned int port, std::string user, std::string pass);
};

class compression
{
public:
    static std::string compress(const std::string data);
    static std::string decompress(const std::string data);
};
}

namespace server
{

class Server
{
    client::Client *client;
    unsigned int defaultport = 3306;

public:
    Server(std::string user , std::string pass, std::string ip = "127.0.0.1", int port = 3306);

};

}
