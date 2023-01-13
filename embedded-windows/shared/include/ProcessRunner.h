#pragma once

#include <string>
#include <memory>
#include <iostream>


enum // Define log instanceIds. Default is 0 and is omitted from this enum.
{
    SecondaryLog = 1
};

class ProcessRunner {
public:
    ProcessRunner();
    virtual ~ProcessRunner();

    void RunCommand(const std::string& command, char** argv = nullptr, int argc = 0);

    void DisplayOutput();

    const std::string &getOutput() const;
    const std::string &getError() const;

    int64_t getStatus() const;


private:

    void StoreStatus(int64_t status);

    void StoreError(const std::string &result);

    void StoreOutput(const std::string &result);

    int64_t status;

    std::string output;
    std::string error;
};

