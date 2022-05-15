#include "request_handler.hh"
#include "request_handler_factory.hh"
#include <boost/log/trivial.hpp>
#include <string>
namespace fs = boost::filesystem;
// Create a new RequestHandler object using the factory
RequestHandler* StaticHandlerFactory::create(std::string location, std::string request_url) {
    return new RequestHandlerStatic(this->root_file_path_, location.length());
}

RequestHandler* EchoHandlerFactory::create(std::string location, std::string request_url) {
    return new RequestHandlerEcho();
}

RequestHandler* NotFoundHandlerFactory::create(std::string location, std::string request_url) {
    return new RequestHandlerNotFound();
}

RequestHandler* APIHandlerFactory::create(std::string location, std::string request_url) {
    return new RequestHandlerAPI(this->root_file_path_, location.length(), &(this->entity_id_map), this->fs);
}

void APIHandlerFactory::Insert_ID(fs::directory_entry entry)
{
    std::set<int> currIDs;
    // go over all the file/directory from eneity path (eg. /api/Shoes)
    for (const fs::directory_entry & subentry : fs::directory_iterator(entry.path())){
        //for every file (eg. /api/Shoes/1)
        if (fs::is_regular_file(subentry.status())) {
            int id;
            //only allow interger filenames
            try {
                id = std::stoi(subentry.path().filename().string()); // convert id to int
                currIDs.insert(id);
                BOOST_LOG_TRIVIAL(info) << "CURD Init find Id: " << id << " for Entity: " << entry.path().filename().string();
            }
            catch(std::exception &err) { // path not ended with number
                BOOST_LOG_TRIVIAL(debug) << "CURD Init Ignore Id: " << subentry.path().filename().string() << " for Entity: " << entry.path().filename().string();
            }
        }
    }
    //insert entity with IDs to the map
    this->entity_id_map.insert(std::pair<std::string,std::set<int>>(entry.path().filename().string(), currIDs));
}
//scan file system, and put entity and ID into the entity_id_map if exists
//return false if root path does not exist
bool APIHandlerFactory::scanFS()
{
    
    fs::path rootpath(this->root_file_path_);
    //if root path (eg. /api) does not exist, create one
    if(!fs::exists(rootpath))
    {
        BOOST_LOG_TRIVIAL(info) << "CURD root path: " << this->root_file_path_ << " does not exist! Creating";
        fs::path rootFolder = this->root_file_path_;
        fs::create_directory(rootFolder);
        return false;
    }
    // go over all the file/directory from root path (eg. /api)
    for (const fs::directory_entry & entry : fs::directory_iterator(rootpath)){
        //for every directory (eg. /api/Shoes)
        if (fs::is_directory(entry.status())) {
            BOOST_LOG_TRIVIAL(info) << "CURD Init find Entity " << entry.path().filename().string();
            Insert_ID(entry);
        }
    }
    return true;
}
