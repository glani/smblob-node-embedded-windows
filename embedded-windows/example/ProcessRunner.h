#pragma once

#include <string>
#include <memory>
#include <iostream>

class ProcessRunner {
public:
    ProcessRunner();
    virtual ~ProcessRunner();

    void RunCommand(const std::string& command, char** argv = nullptr, int argc = 0);

    void StoreOutput(const std::string &result);

    void DisplayOutput();

    const std::string &getOutputStore() const;

private:
    std::string outputStore;
};

