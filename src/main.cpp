
#include <cstdlib>

#include <iostream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include <sqlite3.h>

static int callback(__attribute__((unused)) void* unused_arg, int argc, char* argv[], char* column_names[]) {
    for (int i = 0; i < argc; ++i) {
        std::cout << column_names[i] << ": " << ((argv[i]) ? argv[i] : "NULL") << " "; 
    }

    std::cout << "\n";

    return EXIT_SUCCESS;
}

namespace Options = boost::program_options;

int main(int argc, char* argv[]) {
    size_t simulation_iterations { 0 };
    std::string database_name { };

    Options::options_description generic_options("Generic Options");
    generic_options.add_options()
        ("help", "Display this help message and exit")
        ("version", "Display version information and exit");

    Options::options_description data_configuration("Data Configuration");
    data_configuration.add_options()
        ("database-name", Options::value<std::string>(&database_name)->default_value("simulation.db"), "Database Sqlite3 filename");

    Options::options_description simulation_configuration("Simulation Configuration");
    simulation_configuration.add_options()
        ("iterations", Options::value<size_t>(&simulation_iterations)->default_value(250), "Number of iterations to simulate");

    Options::options_description visible_options("Program Options");
    visible_options
        .add(generic_options)
        .add(data_configuration)
        .add(simulation_configuration);

    Options::options_description hidden_options("Hidden Options");
    hidden_options.add_options()
        ("input-file", Options::value<std::vector<std::string>>(), "Input file(s)");

    Options::options_description cmdline_options;
    cmdline_options
        .add(generic_options)
        .add(data_configuration)
        .add(simulation_configuration)
        .add(hidden_options);

    Options::options_description config_file_options;
    config_file_options
        .add(simulation_configuration)
        .add(hidden_options);

    Options::positional_options_description positional_options;
    positional_options
        .add("input-file", -1);

    Options::variables_map variables_map;
    Options::store(
        Options::command_line_parser(argc, argv)
            .options(cmdline_options)
            .positional(positional_options)
            .run(),
        variables_map);
    Options::notify(variables_map);

    if (variables_map.count("help")) {
        std::cout << visible_options << "\n";
        return EXIT_SUCCESS;
    }

    if (variables_map.count("version")) {
        std::cerr << "<Version Information>\n";
        return EXIT_SUCCESS;
    }

    std::cout << "Database Name: " << database_name << "\n";
    std::cout << "Iterations: " << simulation_iterations << "\n";

    std::cout << "Simulating execution...\n";

    return EXIT_SUCCESS;

    sqlite3* db = nullptr;

    if (sqlite3_open_v2("simulation.db", &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr)) {
        std::cerr << "Failed to open Sqlite3 database.\n";
        sqlite3_close(db);
        return EXIT_FAILURE;
    }

    char* error_message = nullptr;

    if (sqlite3_exec(db, argv[1], callback, 0, &error_message) != SQLITE_OK) {
        std::cerr << "Error: " << error_message << "\n";
        sqlite3_free(error_message);
    }

    sqlite3_close(db);
}

