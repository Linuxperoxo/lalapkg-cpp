#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <libconfig.h++>
#include <stdexcept>
#include <vector>
#include <iostream>

#include "color.h"
#include "config_file.h"

int load_config(const std::string &file, Config_file* conf_file){
  libconfig::Config conf_obj;

  try{
    conf_obj.readFile(file);

    std::vector<std::string> var_warning;
    std::vector<std::string> var_failed;

    std::string var_names[6] = {"sync", "source_dir", "pkg_dir", "root_dir", "common_flags", "jobs"};
    std::string* var_pts[6] = {&conf_file->sync, &conf_file->source_dir, &conf_file->pkg_dir, &conf_file->root_dir, &conf_file->common_flags, &conf_file->jobs};
    
    int i = 0;
    
    for(const auto& names : var_names){
      conf_obj.lookupValue(names, *var_pts[i]);
      if(var_pts[i]->empty()){
        if(names == "common_flags" || names == "jobs"){
          var_warning.push_back(names);
        } else {
          var_failed.push_back(names);
        }
      }
      i++;
    }

    if(!var_failed.empty()){
      std::cout << RED << "|==========VAR-FAILS=========|" << NC << std::endl;
      for(const auto& failed : var_failed){
        std::cerr << failed << "...   [ " << RED << "FAIL" << NC << " ]" << std::endl;  
      }
      std::cout << RED << "|============================|" << NC << std::endl;
      throw std::runtime_error("Error while trying to load some var: Check config file -> " RED + file + NC);
    }

    if(!var_warning.empty()){
      std::cout << YELLOW << "|==========VAR-WARNINGS=========|" << NC << std::endl;
      for(const auto& warning : var_warning){
        std::cerr << warning << "...    [ " << YELLOW << "WARN" << NC << " ]" << std::endl;
      }
      std::cout << YELLOW << "|===============================|" << NC << std::endl;

      std::string user_buffer;

      while(true){
        std::cerr << YELLOW << "WARNING: " << NC << "Do you want to continue even without setting these variables?" << GREEN << "Y" << NC << "/" << RED << "n" << NC << std::endl;
        getline(std::cin, user_buffer);

        if(user_buffer[1] != '\0'){
          continue;
        } else {
          std::cout << user_buffer << std::endl;
          switch(std::tolower(user_buffer[0])){
            case 'y':
              return EXIT_SUCCESS;
            break;

            case 'n':
              EXIT_FAILURE;
            break;

            case '\0':
              return EXIT_SUCCESS;
            break;
          }
        }
      }
    }
    return EXIT_SUCCESS;
  }

  catch(std::runtime_error &error){
    std::cerr << RED << "ERROR: " << NC << error.what() << std::endl;
    return EXIT_FAILURE;
  }

  catch(libconfig::ParseException &paex){
    std::cerr << RED << "ERROR: " << NC <<  "Parse error in build file -> " << GREEN << paex.getFile() << NC << " -> " << RED << paex.getError() << NC << " -> " << "line -> " << RED << paex.getLine() << NC << std::endl;
    return EXIT_FAILURE;
  }

   catch(libconfig::FileIOException &fioex){
    std::cerr << RED << "ERROR: " << NC << "I/O error reading build file -> " << RED << fioex.what() << NC << std::endl;
    return EXIT_FAILURE;
  }
}
