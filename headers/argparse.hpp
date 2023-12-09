#pragma once

#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>

class ArgumentParser {
private:
    std::unordered_map<std::string, std::string> argumentDescriptions;
    std::unordered_map<std::string, std::string> arguments;
    std::unordered_map<std::string, bool>        optionalArguments;
    std::string                                  programName;

    bool validateArguments() const {
        for (const auto& desc : argumentDescriptions) {
            if (!optionalArguments.at(desc.first) && arguments.count(desc.first) == 0) {
                std::cerr << "Error: Argument " << desc.first << " is missing.\n";
                return false;
            }
        }
        return true;
    }

public:
    ArgumentParser() {
    }

    void addArgument(const std::string& name, const std::string& description, bool isOptional = false) {
        argumentDescriptions[name] = description;
        optionalArguments[name]    = isOptional;
    }

    bool parse(int argc, char* argv[]) {
        for (int i = 1; i < argc; i += 2) {
            std::string arg = argv[i];
            if (i + 1 < argc) {
                arguments[arg] = argv[i + 1];
            }
        }

        return validateArguments();
    }

    std::string getArgumentValue(const std::string& name) const {
        if (arguments.count(name) > 0) {
            return arguments.at(name);
        }
        return "";
    }

    bool argumentExists(const std::string& name) const {
        return arguments.count(name) > 0;
    }

    void printHelp() const {
        std::cout << "Usage: " << programName << " [options]\n";

        // Calculate the maximum argument name length for alignment
        size_t maxNameLength = 0;
        for (const auto& desc : argumentDescriptions) {
            maxNameLength = std::max(maxNameLength, desc.first.length());
        }

        // Print required argument descriptions
        for (const auto& desc : argumentDescriptions) {
            if (!optionalArguments.at(desc.first)) {
                std::cout << "  " << std::left << std::setw(maxNameLength + 5) << desc.first << desc.second << std::endl;
            }
        }

        // Print optional argument descriptions
        for (const auto& desc : argumentDescriptions) {
            if (optionalArguments.at(desc.first)) {
                std::cout << "  " << std::left << std::setw(maxNameLength + 5) << desc.first << desc.second << " (Optional)" << std::endl;
            }
        }
    }

    void setProgramName(const std::string& name) {
        programName = name;
    }
};