#include "titantables.h"


client::Client::Client(std::string ip, int port, std::string user, std::string pass)
{
    init(ip, port, user, pass);
}

client::Client::Client(int team, std::string user, std::string pass)
{
    std::stringstream compile;
    compile << "roborio-";
    compile << team;
    compile << "-frc.local";
    std::string getip = compile.str();
    getip = this->resolveDNS(getip);
    init(getip, this->defaultport, user, pass);
}

client::Client::~Client()
{
    delete this->con;
}

void client::Client::useLatestTable()
{
    sql::ResultSet *res;
    sql::Statement *stmt

    stmt = this->con->createStatement();
    std::string toexec = "SELECT table_name, create_time, table_comment FROM INFORMATION_SCHEMA.TABLES WHERE table_schema = '" +
                         this->defaultdb + "' ORDER BY CREATE_TIME DESC" + this->term;

    res = stmt->executeQuery(toexec);
    if(res->rowsCount() == 0)
    {
        this->createBlank(stmt);
    }

    int num = 1;
    while(res->next())
    {
        std::string name = res->getString("table_name");
        std::string comment = res->getString("table_comment");
        if(num == 1 && (0 == comment.compare(this->useComment)))
        {
            this->useTable = name;
        }
        else
        {
            if(!name.compare(this->defaultLatest))
            {
                stmt->execute("RENAME TABLE " + this->defaultLatest + " TO older" + this->term);
                stmt->execute("ALTER TABLE " + this->defaultLatest + " COMMENT = 'not_using'");
            }
        }
        num += 1;
    }
    delete res;
    delete stmt;
}

void client::Client::addInt(std::string name)
{
    this->addBase(name, "INT");
}

void client::Client::setInt(std::string name, int value)
{
    this->updateBase(name, std::to_string(value));
}

int client::Client::getInt(std::string name, int defaultvalue)
{
    sql::Statement *stmt;
    sql::ResultSet *res;
    int toret = defaultvalue;
    try
    {
        stmt = this->con->createStatement();
        res = stmt->executeQuery(this->retrieveBase(name));
        if(res->next())
        {
            toret = res->getInt(1);
        }
    }
    catch(sql::SQLException& pullerr) {}
    delete stmt;
    delete res;
    return toret;
}

void client::Client::addIntArray(std::string name)
{
    this->addBase(name, "LONGTEXT");
}

void client::Client::setIntArray(std::string name, std::vector<int> values)
{
    std::string serialized;
    this->serializeArr(values, serialized);
    this->updateBase(name, "'" + serialized + "'");
}

std::vector<int> client::Client::getIntArray(std::string name, std::vector<int> defaultvalues)
{
    sql::Statement *stmt;
    sql::ResultSet *res;
    std::vector<int> toret = defaultvalues;
    try
    {
        stmt = this->con->createStatement();
        res = stmt->executeQuery(this->retrieveBase(name));
        if(res->next())
        {
            std::string serialized = res->getString(1);
            std::vector<std::string> decompile = this->deserializeArr(serialized); //String to array

            std::vector<int> return_arr;
            for(std::string current: decompile)
            {
                return_arr.push_back(std::stoi(current));
            }
            toret = return_arr;
        }
    }
    catch(sql::SQLException& pullerr)
    {}
    catch(std::exception& err) {}
    delete stmt;
    delete res;
    return toret;
}

void client::Client::addString(std::string name)
{
    this->addBase(name, "TINYTEXT"); //255 chars max
}

void client::Client::setString(std::string name, std::string value)
{
    this->updateBase(name, "'" + value + "'");
}

std::string client::Client::getString(std::string name, std::string defaultvalue)
{
    sql::Statement *stmt;
    sql::ResultSet *res;
    std::string toret = defaultvalue;
    try
    {
        stmt = this->con->createStatement();
        res = stmt->executeQuery(this->retrieveBase(name));
        if(res->next())
        {
            toret = res->getString(1);
        }
    }
    catch(sql::SQLException& pullerr)
    {}
    catch(std::exception& err) {}
    delete stmt;
    delete res;
    return toret;
}

void client::Client::addStringArray(std::string name)
{
    this->addBase(name, "LONGTEXT");
}

void client::Client::setStringArray(std::string name, std::vector<std::string> values)
{
    std::string serialized;
    size_t sizes = values.size();
    for(unsigned int ind = 0; ind < sizes; ind++)
        values[ind] = client::compression::compress(values[ind]);
    this->serializeArr(values, serialized);
    std::string updatequery = "UPDATE " + this->useTable + " SET " + name + "=" +
                              "? WHERE id=1";
    sql::PreparedStatement *stmt;
    try
    {
        stmt = this->con->prepareStatement(updatequery);
        stmt->setString(1, serialized);
        int rows = stmt->executeUpdate();
        if(rows > 0)
        {
            this->verboPrint(2, "Database updated with stringarray", true);
        }
    }
    catch(sql::SQLException& err)
    {}
    catch(std::exception& err) {}
    delete stmt;
}

std::vector<std::string> client::Client::getStringArray(std::string name,
        std::vector<std::string> defaultvalues)
{
    sql::Statement *stmt;
    sql::ResultSet *res;
    std::vector<std::string> toret = defaultvalues;
    try
    {
        stmt = this->con->createStatement();
        res = stmt->executeQuery(this->retrieveBase(name));
        if(res->next())
        {
            std::string serialized = res->getString(1);
            std::vector<std::string> decompile = this->deserializeArr(serialized); //String to array

            std::vector<std::string> return_arr;
            for(std::string current: decompile)
            {
                return_arr.push_back(client::compression::decompress(current));
            }
            toret = return_arr;
        }
    }
    catch(sql::SQLException& pullerr) {}
    catch(std::exception& err)
    {}
    delete res;
    delete stmt;
    return toret;
}

void client::Client::addDouble(std::string name)
{
    this->addBase(name, "DOUBLE");
}

void client::Client::setDouble(std::string name, double value)
{
    this->updateBase(name, std::to_string(value));
}

double client::Client::getDouble(std::string name, double defaultvalue)
{
    sql::Statement *stmt;
    sql::ResultSet *res;
    double toret = defaultvalue;
    try
    {
        stmt = this->con->createStatement();
        res = stmt->executeQuery(this->retrieveBase(name));
        if(res->next())
        {
            toret = res->getDouble(1);
        }
    }
    catch(sql::SQLException& pullerr)
    {}
    catch(std::exception& err) {}
    delete res;
    delete stmt;
    return toret;
}

void client::Client::addDoubleArray(std::string name)
{
    this->addBase(name, "LONGTEXT");
}

void client::Client::setDoubleArray(std::string name, std::vector<double> values)
{
    std::string serialized;
    this->serializeArr(values, serialized);
    this->updateBase(name, "'" + serialized + "'");
}

std::vector<double> client::Client::getDoubleArray(std::string name, std::vector<double> defaultvalues)
{
    sql::Statement *stmt;
    sql::ResultSet *res;
    std::vector<double> toret = defaultvalues;
    try
    {
        stmt = this->con->createStatement();
        res = stmt->executeQuery(this->retrieveBase(name));
        if(res->next())
        {
            std::string serialized = res->getString(1);
            std::vector<std::string> decompile = this->deserializeArr(serialized); //String to array

            std::vector<double> return_arr;
            for(std::string current: decompile)
            {
                return_arr.push_back(boost::lexical_cast<double>(current));
            }
            toret = return_arr;
        }
    }
    catch(sql::SQLException& pullerr)
    {}
    catch(std::exception& err) {}
    delete res;
    delete stmt;
    return toret;
}

void client::Client::addFile(std::string name)
{
    this->addBase(name, "BLOB");
}

void client::Client::setFile(std::string name, std::istream &blob)
{
    std::string updatequery = "UPDATE " + this->useTable + " SET " + name + "=" +
                              "? WHERE id=1";
    sql::PreparedStatement *stmt;
    try
    {
        stmt = this->con->prepareStatement(updatequery);
        stmt->setBlob(1, &blob);
        int rows = stmt->executeUpdate();
        if(rows > 0)
        {
            this->verboPrint(2, "Database updated with file", true);
        }
    }
    catch(sql::SQLException& err) {}
    delete stmt;
}


std::istream* client::Client::getFile(std::string name)
{
    sql::Statement *stmt;
    sql::ResultSet *res;
    std::istream *is;
    try
    {
        stmt = this->con->createStatement();
        res = stmt->executeQuery(this->retrieveBase(name));
        if(res->next())
        {
            is = res->getBlob(1);
        }
    }
    catch(sql::SQLException& pullerr)
    {}
    catch(std::exception& err) {}

    delete stmt;
    delete res;
    return is;
}

void client::Client::updateBase(std::string name, std::string value, int num)
{
    sql::Statement *stmt;
    sql::ResultSet *res;
    try
    {
        stmt = this->con->createStatement();
        std::string numstr =  boost::lexical_cast<std::string>(num);
        std::string updatequery = "UPDATE " + this->useTable + " SET " + name + "=" +
                                  value + " WHERE id=";
        if(!this->tempTable)
        {
            std::string check_exists = "SELECT EXISTS(SELECT 1 FROM " + this->useTable + " WHERE id=" + numstr + ")";
            res = stmt->executeQuery(check_exists);
            bool exists = false;
            if(res->next())
            {
                exists = res->getBoolean(1);
            }

            if(exists) {
                ptime now = microsec_clock::universal_time();
                updatequery = "INSERT INTO " + this->useTable + "(STAMP, " + name + ")"
            } else {
                updatequery += numstr;
            }
        }
        else
        {
            updatequery += "1";
        }

        int rows = stmt->executeUpdate(updatequery);
        if(rows > 0)
        {
            this->verboPrint(3, "Database updated", true);
        }
    }
    catch(sql::SQLException& pullerr)
    {}
    catch(std::exception& err) {}

    delete res;
    delete stmt;
}

int std::string client::Client::retrieveBase(std::string name, int num)
{
    std::string getquery;
    if(num == -1)
    {
        getquery = "SELECT " + name + " FROM " + this->useTable +
                   " ORDER BY id DESC LIMIT 1"; //Get last row
    }
    else
    {
        getquery = "SELECT " + name + " FROM " + this->useTable + " WHERE id=" +
                   std::to_string(num);
    }
    return getquery;
}

void client::Client::addBase(std::string name, std::string type)
{
    std::string sel = "SELECT table_name FROM INFORMATION_SCHEMA.COLUMNS WHERE table_name = '"
                      + this->useTable + "' AND column_name = '" + name  + "'";

    std::string toexec_non = "ALTER TABLE " + this->useTable + " ADD COLUMN " + name +
                             " " + type;
    std::string toexec_exist = "ALTER TABLE " + this->useTable +
                               " ALTER [COLUMN] " + name + " " + type;

    sql::Statement *stmt;
    sql::ResultSet *res;

    try
    {
        stmt = this->con->createStatement();
        res = stmt->executeQuery(sel);
        int rows = res->rowsCount();
        if(rows == 0)
        {
            stmt->execute(toexec_non);
        }
        else
        {
            stmt->execute(toexec_exist);
        }
    }
    catch(sql::SQLException& pullerr) {}
    catch(std::exception& err) {}

    delete res;
    delete stmt;
}

void client::Client::test()
{
    std::vector<std::string> a;

    a.push_back("asd asd");
    a.push_back("jdj dj d");
    a.push_back("d 2 da");

    std::string serialized;
    this->serializeArr(a, serialized);
    std::cout << "SER: " << serialized << std::endl;

    a = this->deserializeArr(serialized);
    std::cout << "a: " << a.at(0) << std::endl;
    std::cout << "b: " << a.at(1) << std::endl;
    std::cout << "c: " << a.at(2) << std::endl;

}

template<class T>
void client::Client::serializeArr(T &lists, std::string &ret)
{
    std::ostringstream packetize;
    for(unsigned int it = 0; it < lists.size(); it++)
        packetize << lists.at(it) << " ";
    ret = packetize.str();
}

std::vector<std::string> client::Client::deserializeArr(std::string value)
{
    std::vector<std::string> ret;
    std::string temp;
    std::stringstream strings(value);
    while(strings >> temp) ret.push_back(temp);
    return ret;
}

void client::Client::useTempTable()
{
    this->tempTable = true;
    std::string toexec =
        "SELECT table_name FROM INFORMATION_SCHEMA.TABLES WHERE table_rows < 2 AND table_schema = '"
        + this->defaultdb + "'";
    sql::Statement *stmt;
    sql::ResultSet *res;
    try
    {
        stmt = this->con->createStatement();
        res = stmt->executeQuery(toexec);
        if(res->rowsCount() > 0)
        {
            if(res->first())
            {
                std::string todrop = "DROP TABLE " + res->getString(1);
                stmt->execute(todrop);
            }
        }
    }
    catch(sql::SQLException& err)
    {
        this->verboPrint(1, "TitanTables: Failed deleting old temp table", true);
    }

    delete res;
    try
    {
        toexec = "CREATE TABLE " + this->tempName + " (id INT NOT NULL PRIMARY KEY AUTO_INCREMENT)";
        stmt->execute(toexec);
        toexec = "INSERT INTO " + this->tempName + " (id) VALUES (1)";
        stmt->execute(toexec);
    }
    catch(sql::SQLException& err)
    {
        this->verboPrint(1, "TitanTables: Failed creating temp table", true);
    }
    catch(std::exception& initfail)
    {
        this->verboPrint(1, "TitanTables: Failed creating temp table", true);
    }
    this->useTable = this->tempName;
    delete stmt;
}

void client::Client::finishTable(std::string new_name)
{
    sql::Statement *stmt;
    try
    {
        stmt = this->con->createStatement();
        if(!this->tempTable)
        {
            stmt->execute("RENAME TABLE " + this->useTable + " TO " + new_name);
            stmt->execute("ALTER TABLE" + this->useTable + " COMMENT = 'not_using'");
        }
        else
        {
            stmt->execute("DROP TABLE IF EXISTS " + this->tempName);
        }
    }
    catch(sql::SQLException& deletefail)
    {
        this->verboPrint(1, "TitanTables: Failed deleting/removing old table", true);
    }

    delete stmt;
}

void client::Client::createBlank(sql::Statement *stmt)
{
    try
    {
        std::string toexec = "CREATE TABLE " + this->defaultLatest + "(id INT NOT NULL PRIMARY KEY AUTO_INCREMENT) COMMENT = '" +
                             this->useComment + "'" + this->term;
        this->verboPrint(1, "No 'latest' table creating one", true);
        stmt->execute(toexec);
    }
    catch(sql::SQLException& err)
    {
        std::cerr << "ERR" << std::endl;
    }
    this->useTable = this->defaultLatest;
}

size_t client::Client::charCount(std::string &str, const char toc)
{
    return std::count(str.begin(), str.end(), toc);
}

void client::Client::init(std::string ip, unsigned int port, std::string user, std::string pass)
{
#if !defined(BOOST_WINDOWS)
    int userid = getuid();
    if(userid != 0)
    {
        std::cerr << "WARNING: titantables would recommend root (guid 0)" << std::endl;
    }
#endif

    try
    {
        this->driver = get_driver_instance();
    }
    catch(sql::SQLException& drivererr)
    {
        std::cerr << "MYSQL driver error: " << drivererr.what() << std::endl;
        throw std::invalid_argument("Couldn't load driver");
    }
    std::stringstream compile;
    compile << "tcp://" << ip << ":" << port;
    std::string connection = compile.str();
    while(1)
    {
        try
        {
            this->con = this->driver->connect(connection, user, pass);
            break;
        }
        catch(sql::SQLException &err)
        {
            boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
            this->verboPrint(2, "Attempting to reconnect...", true);
        }
        catch(std::exception& err)
        {
            std::cerr << "TitanTables: Fatal couldn't connect" << err.what() << std::endl;
            throw std::invalid_argument("Couldn't connect");
        }
    }
    try
    {
        if(!this->con->isValid()) this->con->reconnect();
    }
    catch(sql::SQLException& err) {}

    try
    {
        bool setter = true;
        this->con->setClientOption("OPT_RECONNECT", &setter);
    }
    catch(sql::SQLException& err)
    {
        this->verboPrint(1, "TitanTables: Reconnect option failed", true);
    }

    sql::Statement *stmt;
    try
    {
        stmt = this->con->createStatement();
        stmt->execute("CREATE DATABASE IF NOT EXISTS " + this->defaultdb + this->term);
    }
    catch(sql::SQLException& err)
    {
        std::cout << "TitanTables: WARNING database creation failed (Possible errors may occur)" << std::endl;
    }
    this->con->setSchema(this->defaultdb);
    delete stmt;
}

void client::Client::verboPrint(int mins, const char* top, bool endliner)
{
    if(mins <= this->verbosity)
    {
        std::cout << top << ((char) (endliner) ? '\n' : char(0));
    }
}

std::string client::Client::resolveDNS(std::string name)
{
    std::string using_ip = name;
    try
    {
        boost::asio::io_service io_service;
        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::query query(name, "");
        for(boost::asio::ip::tcp::resolver::iterator i = resolver.resolve(query);
                i != boost::asio::ip::tcp::resolver::iterator();
                ++i)
        {
            boost::asio::ip::tcp::endpoint end = *i;
            if(end.address().is_v4())
            {
                using_ip =  end.address().to_string();
                break;
            }
        }
        this->verboPrint(1, "TitanTables DNS resolve: ", false);
        this->verboPrint(1, using_ip.c_str(), true);
    }
    catch(std::exception& iperr)
    {
        this->verboPrint(1, "TitanTables: DNS resolve error", true);
    }
    catch(boost::exception& iperr)
    {
        this->verboPrint(1, "TitanTables: DNS resolve error (boost input)", true);
    }
    return using_ip;
}

std::string client::Client::resolveHOST()
{
    std::string returnVal;
    char temp[512];

    if ((gethostname(temp,sizeof(temp))) == -1) throw std::invalid_argument("Wrong address!");

    const hostent* host_info = NULL;
    host_info = gethostbyname(temp);

    if (host_info)
    {
        const in_addr* address = (in_addr*)host_info->h_addr_list[0];
        memset(temp,char(0),sizeof(temp));
        strcpy(temp,inet_ntoa(*address));
    }
    else
        throw std::invalid_argument("Bad address!");

    returnVal = temp;
    this->verboPrint(1, "TitanTables HOST resolve: ", false);
    this->verboPrint(1, returnVal.c_str(), true);
    return returnVal;
}

std::string client::compression::compress(const std::string data)
{
    try
    {
        std::string space_seq = "(<!SPACE!>)";
        boost::regex re("( )|([^0-9a-zA-Z$#@*()&\\/?%-_<>!+={};:,.\\n\\r]+)");
        std::string fmt("(?1" + space_seq + ")(?2)");
        boost::regex_replace(data, re, fmt, boost::match_default | boost::format_all);
    }
    catch(boost::exception& regex) {}
    return data;
}

std::string client::compression::decompress(const std::string data)
{
    try
    {
        std::string space_seq = "(~@<SPACE>@~)";
        boost::regex re(space_seq);
        boost::regex_replace(data, re, " ");
    }
    catch(boost::exception& regex) {}
    return data;
}
